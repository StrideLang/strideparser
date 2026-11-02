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

  const bool hasHeaderPorts =
      ((mainInputs && !mainInputs->getChildren().empty()) ||
       (propertyPorts && !propertyPorts->getChildren().empty()) ||
       (mainOutputs && !mainOutputs->getChildren().empty()));

  std::shared_ptr<PropertyNode> bodyPortsProp = nullptr;
  std::shared_ptr<PropertyNode> bodyCodeProp = nullptr;
  std::shared_ptr<PropertyNode> conflictingEntityOrStreamProp = nullptr;

  std::vector<ASTNode> passthroughChildren;

  if (bodyProperties) {
    for (const auto &child : bodyProperties->getChildren()) {
      if (child && child->getNodeType() == AST::Property) {
        auto prop = std::static_pointer_cast<PropertyNode>(child);
        const std::string &pName = prop->getName();
        if (pName == "ports") {
          bodyPortsProp = prop;
        } else if (pName == "code") {
          bodyCodeProp = prop;
        } else if (pName == "entities" || pName == "blocks" || pName == "streams") {
          if (!conflictingEntityOrStreamProp) {
            conflictingEntityOrStreamProp = prop;
          }
          passthroughChildren.push_back(prop);
        } else {
          passthroughChildren.push_back(prop);
        }
      } else if (child) {
        passthroughChildren.push_back(child);
      }
    }
  }

  // Mutual-exclusivity validation 1: Header Ports vs. ports: property
  if (hasHeaderPorts && bodyPortsProp) {
    LangError err;
    err.type = LangError::HeaderPortsWithPortsProperty;
    err.errorTokens = {callableType, name};
    err.filename = bodyPortsProp->getFilename().empty() ? filename : bodyPortsProp->getFilename();
    err.lineNumber = bodyPortsProp->getLine() > 0 ? bodyPortsProp->getLine() : line;
    err.columnNumber = bodyPortsProp->getColumn();
    parseErrors.push_back(err);
  }

  // Mutual-exclusivity validation 2: code: vs. entities:/blocks:/streams:
  if (bodyCodeProp && conflictingEntityOrStreamProp) {
    LangError err;
    err.type = LangError::CodeWithEntitiesOrStreams;
    err.errorTokens = {callableType, name, conflictingEntityOrStreamProp->getName()};
    err.filename = conflictingEntityOrStreamProp->getFilename().empty()
                       ? filename
                       : conflictingEntityOrStreamProp->getFilename();
    err.lineNumber = conflictingEntityOrStreamProp->getLine() > 0
                         ? conflictingEntityOrStreamProp->getLine()
                         : line;
    err.columnNumber = conflictingEntityOrStreamProp->getColumn();
    parseErrors.push_back(err);
  }

  auto result = new DeclarationNode(name, callableType, nullptr,
                                    filename, line, column);

  auto portsList = std::make_shared<ListNode>(filename, line, column);
  auto entitiesList = std::make_shared<ListNode>(filename, line, column);
  auto streamsList = std::make_shared<ListNode>(filename, line, column);

  // Lower header main inputs
  if (mainInputs) {
    for (const auto &item : mainInputs->getChildren()) {
      if (!item) continue;
      std::string eName;
      if (item->getNodeType() == AST::Declaration ||
          item->getNodeType() == AST::ArrayDeclaration) {
        eName = std::static_pointer_cast<DeclarationNode>(item)->getName();
      } else {
        continue;
      }
      std::string pName = synthesizePortName(eName);

      auto portProps = std::make_shared<AST>();
      auto entityNode = std::make_shared<EntityNode>(
          eName, item->getFilename().c_str(), item->getLine(), item->getColumn());
      portProps->addChild(std::make_shared<PropertyNode>(
          "entity", entityNode, item->getFilename().c_str(), item->getLine(), item->getColumn()));
      auto portDecl = std::make_shared<DeclarationNode>(
          pName, "mainInputPort", portProps, item->getFilename().c_str(), item->getLine(), item->getColumn());
      portsList->addChild(portDecl);

      bool isAutoSized = (item->getCompilerProperty("_autoPortSize") != nullptr);
      if (isAutoSized && item->getNodeType() == AST::Declaration) {
        entitiesList->addChild(createAutoSizedEntityDecl(
            std::static_pointer_cast<DeclarationNode>(item), pName,
            item->getFilename(), item->getLine(), item->getColumn()));
      } else {
        entitiesList->addChild(item);
      }
    }
  }

  // Lower header property ports
  if (propertyPorts) {
    for (const auto &propItem : propertyPorts->getChildren()) {
      if (!propItem || propItem->getNodeType() != AST::Property) continue;
      auto propNode = std::static_pointer_cast<PropertyNode>(propItem);
      std::string propName = propNode->getName();
      ASTNode entityDeclNode = propNode->getValue();
      if (!entityDeclNode) continue;

      std::string eName;
      if (entityDeclNode->getNodeType() == AST::Declaration ||
          entityDeclNode->getNodeType() == AST::ArrayDeclaration) {
        eName = std::static_pointer_cast<DeclarationNode>(entityDeclNode)->getName();
      } else {
        std::string capName = propName;
        if (!capName.empty() && capName[0] >= 'a' && capName[0] <= 'z') {
          capName[0] = static_cast<char>(capName[0] - 'a' + 'A');
        }
        eName = capName;
        auto defaultProp = std::make_shared<PropertyNode>(
            "default", entityDeclNode, propItem->getFilename().c_str(),
            propItem->getLine(), propItem->getColumn());
        auto props = std::make_shared<AST>();
        props->addChild(defaultProp);
        entityDeclNode = std::make_shared<DeclarationNode>(
            eName, "signal", props, propItem->getFilename().c_str(),
            propItem->getLine(), propItem->getColumn());
      }
      std::string pName = synthesizePortName(eName);

      auto portProps = std::make_shared<AST>();
      auto nameVal = std::make_shared<ValueNode>(
          propName, propItem->getFilename().c_str(), propItem->getLine(), propItem->getColumn());
      portProps->addChild(std::make_shared<PropertyNode>(
          "name", nameVal, propItem->getFilename().c_str(), propItem->getLine(), propItem->getColumn()));
      auto entityNode = std::make_shared<EntityNode>(
          eName, propItem->getFilename().c_str(), propItem->getLine(), propItem->getColumn());
      portProps->addChild(std::make_shared<PropertyNode>(
          "entity", entityNode, propItem->getFilename().c_str(), propItem->getLine(), propItem->getColumn()));
      auto portDecl = std::make_shared<DeclarationNode>(
          pName, "propertyInputPort", portProps, propItem->getFilename().c_str(), propItem->getLine(), propItem->getColumn());
      portsList->addChild(portDecl);

      bool isAutoSized = (entityDeclNode->getCompilerProperty("_autoPortSize") != nullptr);
      if (isAutoSized && entityDeclNode->getNodeType() == AST::Declaration) {
        entitiesList->addChild(createAutoSizedEntityDecl(
            std::static_pointer_cast<DeclarationNode>(entityDeclNode), pName,
            entityDeclNode->getFilename(), entityDeclNode->getLine(), entityDeclNode->getColumn()));
      } else {
        entitiesList->addChild(entityDeclNode);
      }
    }
  }

  // Lower header main outputs
  if (mainOutputs) {
    for (const auto &item : mainOutputs->getChildren()) {
      if (!item) continue;
      std::string eName;
      if (item->getNodeType() == AST::Declaration ||
          item->getNodeType() == AST::ArrayDeclaration) {
        eName = std::static_pointer_cast<DeclarationNode>(item)->getName();
      } else {
        continue;
      }
      std::string pName = synthesizePortName(eName);

      auto portProps = std::make_shared<AST>();
      auto entityNode = std::make_shared<EntityNode>(
          eName, item->getFilename().c_str(), item->getLine(), item->getColumn());
      portProps->addChild(std::make_shared<PropertyNode>(
          "entity", entityNode, item->getFilename().c_str(), item->getLine(), item->getColumn()));
      auto portDecl = std::make_shared<DeclarationNode>(
          pName, "mainOutputPort", portProps, item->getFilename().c_str(), item->getLine(), item->getColumn());
      portsList->addChild(portDecl);

      bool isAutoSized = (item->getCompilerProperty("_autoPortSize") != nullptr);
      if (isAutoSized && item->getNodeType() == AST::Declaration) {
        entitiesList->addChild(createAutoSizedEntityDecl(
            std::static_pointer_cast<DeclarationNode>(item), pName,
            item->getFilename(), item->getLine(), item->getColumn()));
      } else {
        entitiesList->addChild(item);
      }
    }
  }

  // Partition code: property
  if (bodyCodeProp && bodyCodeProp->getValue()) {
    for (const auto &item : bodyCodeProp->getValue()->getChildren()) {
      if (!item) continue;
      if (item->getNodeType() == AST::Declaration ||
          item->getNodeType() == AST::ArrayDeclaration) {
        entitiesList->addChild(item);
      } else if (item->getNodeType() == AST::Stream) {
        streamsList->addChild(item);
      }
    }
  }

  // Add ports property
  if (hasHeaderPorts) {
    result->addProperty(
        std::make_shared<PropertyNode>("ports", portsList, filename, line, column));
  } else if (bodyPortsProp) {
    result->addProperty(bodyPortsProp);
  }

  // Add entities property if we have entities from header or code
  if (!entitiesList->getChildren().empty()) {
    result->addProperty(
        std::make_shared<PropertyNode>("entities", entitiesList, filename, line, column));
  }

  // Add streams property if we have streams from code
  if (!streamsList->getChildren().empty()) {
    result->addProperty(
        std::make_shared<PropertyNode>("streams", streamsList, filename, line, column));
  }

  // Add remaining passthrough properties and children
  for (const auto &child : passthroughChildren) {
    if (child->getNodeType() == AST::Property) {
      result->addProperty(std::static_pointer_cast<PropertyNode>(child));
    } else {
      result->addChild(child);
    }
  }

  return result;
}

} // namespace strd
