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

#include "stride/parser/arraynode.h"
#include "stride/parser/ast.h"
#include "stride/parser/declarationnode.h"
#include "stride/parser/entitynode.h"
#include "stride/parser/langerror.h"
#include "stride/parser/listnode.h"
#include "stride/parser/memberaccessnode.h"
#include "stride/parser/propertynode.h"
#include "stride/parser/streamnode.h"
#include "stride/parser/valuenode.h"
#include "gtest/gtest.h"

using namespace strd;

TEST(CallableSyntaxTest, PropertyAliasing) {
  auto decl = std::make_shared<DeclarationNode>("Test", "module", nullptr,
                                                __FILE__, __LINE__);
  auto entitiesList = std::make_shared<ListNode>(__FILE__, __LINE__);
  auto sigDecl = std::make_shared<DeclarationNode>("Sig", "signal", nullptr,
                                                   __FILE__, __LINE__);
  entitiesList->addChild(sigDecl);

  decl->addProperty(std::make_shared<PropertyNode>("entities", entitiesList,
                                                   __FILE__, __LINE__));
  EXPECT_NE(decl->getPropertyValue("entities"), nullptr);
  EXPECT_NE(decl->getPropertyValue("blocks"), nullptr);
  EXPECT_EQ(decl->getPropertyValue("blocks"),
            decl->getPropertyValue("entities"));

  auto portDecl = std::make_shared<DeclarationNode>(
      "InPort", "mainInputPort", nullptr, __FILE__, __LINE__);
  auto entityNode = std::make_shared<EntityNode>("Sig", __FILE__, __LINE__);
  portDecl->addProperty(
      std::make_shared<PropertyNode>("entity", entityNode, __FILE__, __LINE__));
  EXPECT_NE(portDecl->getPropertyValue("entity"), nullptr);
  EXPECT_NE(portDecl->getPropertyValue("block"), nullptr);
  EXPECT_EQ(portDecl->getPropertyValue("block"),
            portDecl->getPropertyValue("entity"));
}

TEST(CallableSyntaxTest, CompactModuleDesugaring) {
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "callable_syntax/compact_module.stride");
  ASSERT_NE(tree, nullptr);
  ASSERT_EQ(AST::getParseErrors().size(), 0);

  // Child 0 is SystemNode, Child 1 is Module Declaration
  ASSERT_GE(tree->getChildren().size(), 2);
  auto modNode = tree->getChildren().at(1);
  ASSERT_EQ(modNode->getNodeType(), AST::Declaration);
  auto modDecl = std::static_pointer_cast<DeclarationNode>(modNode);
  EXPECT_EQ(modDecl->getEntityType(), "module");
  EXPECT_EQ(modDecl->getName(), "Amplifier");

  // Verify ports
  auto portsNode = modDecl->getPropertyValue("ports");
  ASSERT_NE(portsNode, nullptr);
  ASSERT_EQ(portsNode->getChildren().size(), 3);

  auto inPort =
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(0));
  EXPECT_EQ(inPort->getEntityType(), "mainInputPort");
  EXPECT_EQ(inPort->getName(), "InputPort");
  EXPECT_NE(inPort->getPropertyValue("entity"), nullptr);

  auto propPort =
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(1));
  EXPECT_EQ(propPort->getEntityType(), "propertyInputPort");
  EXPECT_EQ(propPort->getName(), "GainPort");
  EXPECT_NE(propPort->getPropertyValue("name"), nullptr);
  EXPECT_NE(propPort->getPropertyValue("entity"), nullptr);

  auto outPort =
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(2));
  EXPECT_EQ(outPort->getEntityType(), "mainOutputPort");
  EXPECT_EQ(outPort->getName(), "OutputPort");
  EXPECT_NE(outPort->getPropertyValue("entity"), nullptr);

  // Verify entities
  auto entitiesNode = modDecl->getPropertyValue("entities");
  ASSERT_NE(entitiesNode, nullptr);
  EXPECT_EQ(entitiesNode->getChildren().size(), 3);

  // Verify streams
  auto streamsNode = modDecl->getPropertyValue("streams");
  ASSERT_NE(streamsNode, nullptr);
  EXPECT_EQ(streamsNode->getChildren().size(), 1);
  EXPECT_EQ(streamsNode->getChildren().at(0)->getNodeType(), AST::Stream);
}

TEST(CallableSyntaxTest, CompactArraysAutoSizing) {
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "callable_syntax/compact_arrays.stride");
  ASSERT_NE(tree, nullptr);
  ASSERT_EQ(AST::getParseErrors().size(), 0);

  auto modDecl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
  EXPECT_EQ(modDecl->getName(), "SizedArrayMod");

  auto entitiesNode = modDecl->getPropertyValue("entities");
  ASSERT_NE(entitiesNode, nullptr);
  ASSERT_GE(entitiesNode->getChildren().size(), 1);

  // Frame should be an ArrayDeclaration auto-sized to FramePort.size
  auto frameDecl = std::static_pointer_cast<DeclarationNode>(
      entitiesNode->getChildren().at(0));
  EXPECT_EQ(frameDecl->getNodeType(), AST::ArrayDeclaration);
  EXPECT_EQ(frameDecl->getName(), "Frame");
  auto arrayNode = frameDecl->getArrayIndex();
  ASSERT_NE(arrayNode, nullptr);
  auto idxList = arrayNode->index();
  ASSERT_NE(idxList, nullptr);
  ASSERT_EQ(idxList->getChildren().size(), 1);
  EXPECT_EQ(idxList->getChildren().at(0)->getNodeType(), AST::MemberAccess);
  auto memberAccess =
      std::static_pointer_cast<MemberAccessNode>(idxList->getChildren().at(0));
  EXPECT_EQ(memberAccess->getEntity(), "FramePort");
  EXPECT_EQ(memberAccess->getPropertyName(), "size");
}

TEST(CallableSyntaxTest, CompactBundlePorts) {
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "callable_syntax/compact_bundle.stride");
  ASSERT_NE(tree, nullptr);
  ASSERT_EQ(AST::getParseErrors().size(), 0);

  auto modDecl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
  EXPECT_EQ(modDecl->getName(), "MultiPortMod");

  auto portsNode = modDecl->getPropertyValue("ports");
  ASSERT_NE(portsNode, nullptr);
  // 2 inputs + 2 outputs = 4 ports
  ASSERT_EQ(portsNode->getChildren().size(), 4);

  EXPECT_EQ(
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(0))
          ->getName(),
      "AudioInPort");
  EXPECT_EQ(
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(1))
          ->getName(),
      "SidechainInPort");
  EXPECT_EQ(
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(2))
          ->getName(),
      "AudioOutPort");
  EXPECT_EQ(
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(3))
          ->getName(),
      "GainReductionPort");
}

TEST(CallableSyntaxTest, CompactReaction) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR
                                "callable_syntax/compact_reaction.stride");
  ASSERT_NE(tree, nullptr);
  ASSERT_EQ(AST::getParseErrors().size(), 0);

  auto rxDecl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
  EXPECT_EQ(rxDecl->getEntityType(), "reaction");
  EXPECT_EQ(rxDecl->getName(), "ComputeEnvelope");

  auto portsNode = rxDecl->getPropertyValue("ports");
  ASSERT_NE(portsNode, nullptr);
  ASSERT_EQ(portsNode->getChildren().size(), 2);
  EXPECT_EQ(
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(0))
          ->getEntityType(),
      "propertyInputPort");
  EXPECT_EQ(
      std::static_pointer_cast<DeclarationNode>(portsNode->getChildren().at(1))
          ->getEntityType(),
      "mainOutputPort");
}

TEST(CallableSyntaxTest, CompactLoop) {
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "callable_syntax/compact_loop.stride");
  ASSERT_NE(tree, nullptr);
  ASSERT_EQ(AST::getParseErrors().size(), 0);

  auto loopDecl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
  EXPECT_EQ(loopDecl->getEntityType(), "loop");
  EXPECT_EQ(loopDecl->getName(), "SolveMakeup");

  EXPECT_NE(loopDecl->getPropertyValue("until"), nullptr);
  EXPECT_NE(loopDecl->getPropertyValue("maxIterations"), nullptr);

  auto portsNode = loopDecl->getPropertyValue("ports");
  ASSERT_NE(portsNode, nullptr);
  ASSERT_EQ(portsNode->getChildren().size(), 3);
}

TEST(CallableSyntaxTest, ComplexCompressor) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR
                                "callable_syntax/complex_compressor.stride");
  ASSERT_NE(tree, nullptr);
  ASSERT_EQ(AST::getParseErrors().size(), 0);

  auto compDecl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
  EXPECT_EQ(compDecl->getEntityType(), "module");
  EXPECT_EQ(compDecl->getName(), "SpectralCompressor");

  auto portsNode = compDecl->getPropertyValue("ports");
  ASSERT_NE(portsNode, nullptr);
  // 2 inputs + 5 property ports + 2 outputs = 9 ports
  EXPECT_EQ(portsNode->getChildren().size(), 9);

  auto entitiesNode = compDecl->getPropertyValue("entities");
  ASSERT_NE(entitiesNode, nullptr);
  EXPECT_GE(entitiesNode->getChildren().size(), 9);

  auto streamsNode = compDecl->getPropertyValue("streams");
  ASSERT_NE(streamsNode, nullptr);
  EXPECT_GE(streamsNode->getChildren().size(), 5);
}

TEST(CallableSyntaxTest, ExclusivityHeaderAndPortsError) {
  ASTNode tree = AST::parseFile(
      TESTS_SOURCE_DIR "callable_syntax/error_header_and_ports.stride");
  auto errors = AST::getParseErrors();
  bool foundError = false;
  for (const auto &err : errors) {
    if (err.type == LangError::HeaderPortsWithPortsProperty) {
      foundError = true;
      EXPECT_EQ(err.errorTokens.at(0), "module");
      EXPECT_EQ(err.errorTokens.at(1), "BadMod");
    }
  }
  EXPECT_TRUE(foundError);
}

TEST(CallableSyntaxTest, ExclusivityCodeAndEntitiesError) {
  ASTNode tree = AST::parseFile(
      TESTS_SOURCE_DIR "callable_syntax/error_code_and_entities.stride");
  auto errors = AST::getParseErrors();
  bool foundError = false;
  for (const auto &err : errors) {
    if (err.type == LangError::CodeWithEntitiesOrStreams) {
      foundError = true;
      EXPECT_EQ(err.errorTokens.at(0), "module");
      EXPECT_EQ(err.errorTokens.at(1), "BadMod");
    }
  }
  EXPECT_TRUE(foundError);
}

TEST(CallableSyntaxTest, GenericCallableSyntaxParsing) {
  ASTNode tree = AST::parseFile(
      TESTS_SOURCE_DIR "callable_syntax/error_reaction_main_input.stride");
  auto errors = AST::getParseErrors();
  EXPECT_TRUE(errors.empty());
  ASSERT_TRUE(tree != nullptr);
}

TEST(CallableSyntaxTest, ArbitraryCallableType) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR
                                "callable_syntax/arbitrary_callable.stride");
  auto errors = AST::getParseErrors();
  EXPECT_TRUE(errors.empty());
  ASSERT_TRUE(tree != nullptr);

  auto decl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
  ASSERT_TRUE(decl != nullptr);
  EXPECT_EQ(decl->getName(), "MyFilter");
  EXPECT_EQ(decl->getEntityType(), "customFilter");
  EXPECT_TRUE(decl->getPropertyValue("ports") != nullptr);
  EXPECT_TRUE(decl->getPropertyValue("entities") != nullptr);
  EXPECT_TRUE(decl->getPropertyValue("streams") != nullptr);
}

TEST(CallableSyntaxTest, AnonymousReactionInStream) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR
                                "callable_syntax/anonymous_reaction.stride");
  auto errors = AST::getParseErrors();
  EXPECT_TRUE(errors.empty());
  ASSERT_TRUE(tree != nullptr);
}

TEST(CallableSyntaxTest, AnonymousReactionWithCodeProperty) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR
                                "callable_syntax/anonymous_reaction_code.stride");
  auto errors = AST::getParseErrors();
  EXPECT_TRUE(errors.empty());
  ASSERT_TRUE(tree != nullptr);

  auto moduleDecl =
      std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(0));
  ASSERT_TRUE(moduleDecl != nullptr);

  auto streamsProp = moduleDecl->getPropertyValue("streams");
  ASSERT_TRUE(streamsProp != nullptr);
  auto stream =
      std::static_pointer_cast<StreamNode>(streamsProp->getChildren().at(0));
  ASSERT_TRUE(stream != nullptr);

  auto right = stream->getRight();
  ASSERT_TRUE(right != nullptr);
  while (right->getNodeType() == AST::Stream) {
    right = std::static_pointer_cast<StreamNode>(right)->getRight();
  }
  ASSERT_EQ(right->getNodeType(), AST::Declaration);
  auto rxDecl = std::static_pointer_cast<DeclarationNode>(right);
  EXPECT_EQ(rxDecl->getEntityType(), "reaction");
  EXPECT_TRUE(rxDecl->getCompilerProperty("anonymous") != nullptr);

  auto entitiesProp = rxDecl->getPropertyValue("entities");
  ASSERT_TRUE(entitiesProp != nullptr);
  ASSERT_EQ(entitiesProp->getChildren().size(), 1);
  auto entityChild =
      std::static_pointer_cast<DeclarationNode>(entitiesProp->getChildren().at(0));
  EXPECT_EQ(entityChild->getName(), "Temp");

  auto streamsChild = rxDecl->getPropertyValue("streams");
  ASSERT_TRUE(streamsChild != nullptr);
  ASSERT_EQ(streamsChild->getChildren().size(), 1);
}
