#include "gtest/gtest.h"

#include "stride/parser/ast.h"
#include "stride/parser/blocknode.h"
#include "stride/parser/bundlenode.h"
#include "stride/parser/declarationnode.h"
#include "stride/parser/expressionnode.h"
#include "stride/parser/functionnode.h"
#include "stride/parser/importnode.h"
#include "stride/parser/platformnode.h"
#include "stride/parser/portpropertynode.h"
#include "stride/parser/rangenode.h"
#include "stride/parser/streamnode.h"
#include "stride/parser/valuenode.h"

using namespace strd;

TEST(BlockDeclaration, Declaration) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/02_basic_blocks.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  EXPECT_TRUE(nodes.size() == 5);
  AST *node = nodes.at(0).get();
  EXPECT_TRUE(node->getNodeType() == AST::Declaration);
  std::vector<std::shared_ptr<PropertyNode>> properties =
      static_cast<DeclarationNode *>(node)->getProperties();
  EXPECT_TRUE(properties.size() == 2);
  std::shared_ptr<PropertyNode> property = properties.at(0);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "rate");
  AST *propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::Block);
  EXPECT_TRUE(static_cast<BlockNode *>(propertyValue)->getName() ==
              "AudioRate");
  property = properties.at(1);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "meta");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::String);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getStringValue() ==
              "Guitar input.");

  node = nodes.at(1).get();
  EXPECT_TRUE(node->getNodeType() == AST::Declaration);
  properties = static_cast<DeclarationNode *>(node)->getProperties();
  EXPECT_TRUE(properties.size() == 2);
  property = properties.at(0);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::Int);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getIntValue() == 5);
  property = properties.at(1);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "meta");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::String);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getStringValue() ==
              "Integer Value.");

  // No properties
  node = nodes.at(2).get();
  EXPECT_TRUE(node->getNodeType() == AST::Declaration);
  properties = static_cast<DeclarationNode *>(node)->getProperties();
  EXPECT_TRUE(properties.size() == 0);

  // Property is an object
  node = nodes.at(3).get();
  EXPECT_TRUE(node->getNodeType() == AST::Declaration);
  properties = static_cast<DeclarationNode *>(node)->getProperties();
  EXPECT_TRUE(properties.size() == 2);
  property = properties.at(0);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::Declaration);
  DeclarationNode *object = static_cast<DeclarationNode *>(propertyValue);
  EXPECT_TRUE(object->getName() == "");
  EXPECT_TRUE(object->getObjectType() == "");
  std::vector<std::shared_ptr<PropertyNode>> objProperties =
      static_cast<DeclarationNode *>(object)->getProperties();
  EXPECT_TRUE(objProperties.size() == 2);
  property = objProperties.at(0);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "prop1");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::Int);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getIntValue() == 5);
  property = objProperties.at(1);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "prop2");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::String);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getStringValue() ==
              "hello");
  property = properties.at(1);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "meta");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::String);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getStringValue() ==
              "Block as Property");

  node = nodes.at(4).get();
  EXPECT_TRUE(node->getNodeType() == AST::Declaration);
  properties = static_cast<DeclarationNode *>(node)->getProperties();
  EXPECT_TRUE(properties.size() == 2);
  property = properties.at(0);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "process");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  ListNode *listnode = static_cast<ListNode *>(propertyValue);
  EXPECT_TRUE(listnode->getNodeType() == AST::List);
  EXPECT_TRUE(listnode->getChildren().size() == 1);
  StreamNode *streamNode =
      static_cast<StreamNode *>(listnode->getChildren().at(0).get());
  EXPECT_TRUE(streamNode->getChildren().size() == 2);
  EXPECT_TRUE(streamNode->getChildren().at(0)->getNodeType() == AST::Function);
  EXPECT_TRUE(streamNode->getChildren().at(1)->getNodeType() == AST::Bundle);
  property = properties.at(1);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_TRUE(property->getName() == "meta");
  propertyValue = property->getChildren().at(0).get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::String);
  EXPECT_TRUE(static_cast<ValueNode *>(propertyValue)->getStringValue() ==
              "Stream property");
}

TEST(BlockDeclaration, PortProperty) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/21_port_properties.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();

  AST *node = nodes.at(1).get();
  EXPECT_TRUE(node->getNodeType() == AST::Declaration);
  std::vector<std::shared_ptr<PropertyNode>> properties =
      static_cast<DeclarationNode *>(node)->getProperties();
  EXPECT_TRUE(properties.size() == 4);
  std::shared_ptr<PropertyNode> property = properties.at(2);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_EQ(property->getName(), "thirdport");
  auto propertyValueNode = property->getChildren();
  EXPECT_EQ(propertyValueNode.size(), 1);
  auto propValue =
      std::static_pointer_cast<PortPropertyNode>(propertyValueNode[0]);
  EXPECT_EQ(propValue->getNodeType(), AST::PortProperty);
  EXPECT_EQ(propValue->getName(), "");
  EXPECT_EQ(propValue->getPortName(), "firstport");

  property = properties.at(3);
  EXPECT_TRUE(property != nullptr && property->getChildren().size() == 1);
  EXPECT_EQ(property->getName(), "lastport");
  propertyValueNode = property->getChildren();
  EXPECT_EQ(propertyValueNode.size(), 1);
  propValue = std::static_pointer_cast<PortPropertyNode>(propertyValueNode[0]);
  EXPECT_EQ(propValue->getNodeType(), AST::PortProperty);
  EXPECT_EQ(propValue->getName(), "Block");
  EXPECT_EQ(propValue->getPortName(), "port");
}
