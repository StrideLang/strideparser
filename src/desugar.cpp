/*
    Stride is licensed under the terms of the 3-clause BSD license.

    Copyright (C) 2017. The Regents of the University of California.
    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

        Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

        Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

        Neither the name of the copyright holder nor the names of its
        contributors may be used to endorse or promote products derived from
        this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    Authors: Andres Cabrera and Joseph Tilbian
*/

#include "stride/parser/desugar.h"

#include <algorithm>

#include "stride/parser/arraynode.h"
#include "stride/parser/entitynode.h"
#include "stride/parser/langerror.h"
#include "stride/parser/listnode.h"
#include "stride/parser/memberaccessnode.h"
#include "stride/parser/propertynode.h"
#include "stride/parser/valuenode.h"

extern std::vector<strd::LangError> parseErrors;

namespace strd {

static std::string synthesizePortName(const std::string &entityName) {
  if (entityName.size() >= 4 &&
      entityName.compare(entityName.size() - 4, 4, "Port") == 0) {
    return entityName;
  }
  return entityName + "Port";
}

static ASTNode createAutoSizedEntityDecl(
    const std::shared_ptr<DeclarationNode> &decl, const std::string &portName,
    const std::string &filename, int line, int column) {
  std::string entityName = decl->getName();
  auto memberAccess = std::make_shared<MemberAccessNode>(
      portName, "size", filename.c_str(), line, column);
  auto idxList = std::make_shared<ListNode>(memberAccess, filename.c_str(), line, column);
  auto arrayNode = std::make_shared<ArrayNode>(entityName, idxList, filename.c_str(), line, column);

  auto propsList = std::make_shared<AST>();
  for (const auto &prop : decl->getProperties()) {
    propsList->addChild(prop);
  }

  auto newDecl = std::make_shared<DeclarationNode>(
      arrayNode, decl->getEntityType(), propsList, filename.c_str(), line, column);
  return newDecl;
}

static bool isDeclarationLike(const ASTNode &node) {
  return node->getNodeType() == AST::Declaration ||
         node->getNodeType() == AST::ArrayDeclaration;
}

static bool hasChildren(const ASTNode &node) {
  return node && !node->getChildren().empty();
}

namespace {
// Result of scanning the properties declared in a callable's body.
struct BodyScan {
  std::shared_ptr<PropertyNode> portsProp;
  std::shared_ptr<PropertyNode> codeProp;
  std::shared_ptr<PropertyNode> conflictingEntityOrStreamProp;
  std::vector<ASTNode> passthroughChildren;
};
} // namespace

static BodyScan scanBodyProperties(const ASTNode &bodyProperties) {
  BodyScan scan;
  if (!bodyProperties) {
    return scan;
  }
  for (const auto &child : bodyProperties->getChildren()) {
    if (!child) continue;
    if (child->getNodeType() != AST::Property) {
      scan.passthroughChildren.push_back(child);
      continue;
    }
    auto prop = std::static_pointer_cast<PropertyNode>(child);
    const std::string &pName = prop->getName();
    if (pName == "ports") {
      scan.portsProp = prop;
    } else if (pName == "code") {
      scan.codeProp = prop;
    } else {
      if ((pName == "entities" || pName == "streams") &&
          !scan.conflictingEntityOrStreamProp) {
        scan.conflictingEntityOrStreamProp = prop;
      }
      scan.passthroughChildren.push_back(prop);
    }
  }
  return scan;
}

static void reportCallableError(LangError::ErrorType type,
                                std::vector<std::string> tokens,
                                const PropertyNode &prop,
                                const char *filename, int line) {
  LangError err;
  err.type = type;
  err.errorTokens = std::move(tokens);
  err.filename = prop.getFilename().empty() ? filename : prop.getFilename();
  err.lineNumber = prop.getLine() > 0 ? prop.getLine() : line;
  err.columnNumber = prop.getColumn();
  parseErrors.push_back(err);
}

// Adds an entity to the entities list, replacing it by an auto-sized
// declaration when it is marked with _autoPortSize.
static void addPortEntity(const ASTNode &entity, const std::string &portName,
                          ListNode &entitiesList) {
  bool isAutoSized = entity->getCompilerProperty("_autoPortSize") != nullptr;
  if (isAutoSized && isDeclarationLike(entity)) {
    entitiesList.addChild(createAutoSizedEntityDecl(
        std::static_pointer_cast<DeclarationNode>(entity), portName,
        entity->getFilename(), entity->getLine(), entity->getColumn()));
  } else {
    entitiesList.addChild(entity);
  }
}

// Lowers header main inputs/outputs to port declarations plus entities.
static void lowerMainPorts(const ASTNode &items, const std::string &portType,
                           ListNode &portsList, ListNode &entitiesList) {
  if (!items) return;
  for (const auto &item : items->getChildren()) {
    if (!item || !isDeclarationLike(item)) continue;
    std::string eName = std::static_pointer_cast<DeclarationNode>(item)->getName();
    std::string pName = synthesizePortName(eName);

    auto portProps = std::make_shared<AST>();
    auto entityNode = std::make_shared<EntityNode>(
        eName, item->getFilename().c_str(), item->getLine(), item->getColumn());
    portProps->addChild(std::make_shared<PropertyNode>(
        "entity", entityNode, item->getFilename().c_str(), item->getLine(), item->getColumn()));
    portsList.addChild(std::make_shared<DeclarationNode>(
        pName, portType, portProps, item->getFilename().c_str(), item->getLine(), item->getColumn()));

    addPortEntity(item, pName, entitiesList);
  }
}

// Builds the signal declaration implied by a plain `name: value` property port.
static ASTNode makeDefaultSignalDecl(const std::string &eName,
                                     const ASTNode &value,
                                     const ASTNode &propItem) {
  auto defaultProp = std::make_shared<PropertyNode>(
      "default", value, propItem->getFilename().c_str(),
      propItem->getLine(), propItem->getColumn());
  auto props = std::make_shared<AST>();
  props->addChild(defaultProp);
  return std::make_shared<DeclarationNode>(
      eName, "signal", props, propItem->getFilename().c_str(),
      propItem->getLine(), propItem->getColumn());
}

static std::string capitalizeFirst(std::string name) {
  if (!name.empty() && name[0] >= 'a' && name[0] <= 'z') {
    name[0] = static_cast<char>(name[0] - 'a' + 'A');
  }
  return name;
}

// Lowers header property ports to port declarations plus entities.
static void lowerPropertyPorts(const ASTNode &propertyPorts,
                               ListNode &portsList, ListNode &entitiesList) {
  if (!propertyPorts) return;
  for (const auto &propItem : propertyPorts->getChildren()) {
    if (!propItem || propItem->getNodeType() != AST::Property) continue;
    auto propNode = std::static_pointer_cast<PropertyNode>(propItem);
    std::string propName = propNode->getName();
    ASTNode entityDeclNode = propNode->getValue();
    if (!entityDeclNode) continue;

    std::string eName;
    if (isDeclarationLike(entityDeclNode)) {
      eName = std::static_pointer_cast<DeclarationNode>(entityDeclNode)->getName();
    } else {
      eName = capitalizeFirst(propName);
      entityDeclNode = makeDefaultSignalDecl(eName, entityDeclNode, propItem);
    }
    std::string pName = synthesizePortName(eName);

    const char *file = propItem->getFilename().c_str();
    auto portProps = std::make_shared<AST>();
    auto nameVal = std::make_shared<ValueNode>(
        propName, file, propItem->getLine(), propItem->getColumn());
    portProps->addChild(std::make_shared<PropertyNode>(
        "name", nameVal, file, propItem->getLine(), propItem->getColumn()));
    auto entityNode = std::make_shared<EntityNode>(
        eName, file, propItem->getLine(), propItem->getColumn());
    portProps->addChild(std::make_shared<PropertyNode>(
        "entity", entityNode, file, propItem->getLine(), propItem->getColumn()));
    portsList.addChild(std::make_shared<DeclarationNode>(
        pName, "propertyInputPort", portProps, file, propItem->getLine(), propItem->getColumn()));

    addPortEntity(entityDeclNode, pName, entitiesList);
  }
}

// Splits the contents of a `code:` property into entities and streams.
static void partitionCode(const std::shared_ptr<PropertyNode> &codeProp,
                          ListNode &entitiesList, ListNode &streamsList) {
  if (!codeProp || !codeProp->getValue()) return;
  for (const auto &item : codeProp->getValue()->getChildren()) {
    if (!item) continue;
    if (isDeclarationLike(item)) {
      entitiesList.addChild(item);
    } else if (item->getNodeType() == AST::Stream) {
      streamsList.addChild(item);
    }
  }
}

DeclarationNode *desugarCallable(
    const std::string &callableType,
    const std::string &name,
    ASTNode mainInputs,
    ASTNode propertyPorts,
    ASTNode mainOutputs,
    ASTNode bodyProperties,
    const char *filename,
    int line,
    int column) {

  const bool hasHeaderPorts = hasChildren(mainInputs) ||
                              hasChildren(propertyPorts) ||
                              hasChildren(mainOutputs);

  BodyScan body = scanBodyProperties(bodyProperties);

  // Mutual-exclusivity validation 1: Header Ports vs. ports: property
  if (hasHeaderPorts && body.portsProp) {
    reportCallableError(LangError::HeaderPortsWithPortsProperty,
                        {callableType, name}, *body.portsProp, filename, line);
  }

  // Mutual-exclusivity validation 2: code: vs. entities:/streams:
  if (body.codeProp && body.conflictingEntityOrStreamProp) {
    reportCallableError(
        LangError::CodeWithEntitiesOrStreams,
        {callableType, name, body.conflictingEntityOrStreamProp->getName()},
        *body.conflictingEntityOrStreamProp, filename, line);
  }

  auto result = new DeclarationNode(name, callableType, nullptr,
                                    filename, line, column);

  auto portsList = std::make_shared<ListNode>(filename, line, column);
  auto entitiesList = std::make_shared<ListNode>(filename, line, column);
  auto streamsList = std::make_shared<ListNode>(filename, line, column);

  lowerMainPorts(mainInputs, "mainInputPort", *portsList, *entitiesList);
  lowerPropertyPorts(propertyPorts, *portsList, *entitiesList);
  lowerMainPorts(mainOutputs, "mainOutputPort", *portsList, *entitiesList);
  partitionCode(body.codeProp, *entitiesList, *streamsList);

  // Add ports property
  if (hasHeaderPorts) {
    result->addProperty(
        std::make_shared<PropertyNode>("ports", portsList, filename, line, column));
  } else if (body.portsProp) {
    result->addProperty(body.portsProp);
  }

  // Add entities property if we have entities from header or code, or if code: was specified
  if (body.codeProp || !entitiesList->getChildren().empty()) {
    result->addProperty(
        std::make_shared<PropertyNode>("entities", entitiesList, filename, line, column));
  }

  // Add streams property if we have streams from code, or if code: was specified
  if (body.codeProp || !streamsList->getChildren().empty()) {
    result->addProperty(
        std::make_shared<PropertyNode>("streams", streamsList, filename, line, column));
  }

  // Add remaining passthrough properties and children
  for (const auto &child : body.passthroughChildren) {
    if (child->getNodeType() == AST::Property) {
      result->addProperty(std::static_pointer_cast<PropertyNode>(child));
    } else {
      result->addChild(child);
    }
  }

  return result;
}

} // namespace strd
