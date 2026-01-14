#include "gtest/gtest.h"

#include "stride/parser/ast.h"
#include "stride/parser/blocknode.h"
#include "stride/parser/bundlenode.h"
#include "stride/parser/declarationnode.h"
#include "stride/parser/expressionnode.h"
#include "stride/parser/functionnode.h"
#include "stride/parser/importnode.h"
#include "stride/parser/platformnode.h"
#include "stride/parser/rangenode.h"
#include "stride/parser/streamnode.h"
#include "stride/parser/valuenode.h"

using namespace strd;

TEST(Function, Basic) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/05_basic_functions.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();

  EXPECT_TRUE(nodes.at(0)->getNodeType() == AST::Stream);
  StreamNode *node = static_cast<StreamNode *>(nodes.at(0).get());
  FunctionNode *function = static_cast<FunctionNode *>(node->getLeft().get());
  EXPECT_TRUE(function->getNodeType() == AST::Function);
  EXPECT_TRUE(function->getName() == "Function1");
  std::vector<std::shared_ptr<PropertyNode>> properties =
      function->getProperties();
  PropertyNode *property = properties.at(0).get();
  EXPECT_TRUE(property->getName() == "propReal");
  ValueNode *value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Real);
  EXPECT_TRUE(value->getRealValue() == 1.1);
  property = properties.at(1).get();
  EXPECT_TRUE(property->getName() == "propInt");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 23);
  property = properties.at(2).get();
  EXPECT_TRUE(property->getName() == "propString");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::String);
  EXPECT_TRUE(value->getStringValue() == "hello");

  node = static_cast<StreamNode *>(node->getRight().get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);

  function = static_cast<FunctionNode *>(node->getLeft().get());
  EXPECT_TRUE(function->getNodeType() == AST::Function);
  EXPECT_TRUE(function->getName() == "Function2");
  properties = function->getProperties();
  property = properties.at(0).get();
  EXPECT_TRUE(property->getName() == "propReal");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Real);
  EXPECT_TRUE(value->getRealValue() == 1.2);
  property = properties.at(1).get();
  EXPECT_TRUE(property->getName() == "propInt");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 123);
  property = properties.at(2).get();
  EXPECT_TRUE(property->getName() == "propString");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::String);
  EXPECT_TRUE(value->getStringValue() == "function");

  function = static_cast<FunctionNode *>(node->getRight().get());
  EXPECT_TRUE(function->getNodeType() == AST::Function);
  EXPECT_TRUE(function->getName() == "Function3");
  properties = function->getProperties();
  property = properties.at(0).get();
  EXPECT_TRUE(property->getName() == "propReal");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Real);
  EXPECT_TRUE(value->getRealValue() == 1.3);
  property = properties.at(1).get();
  EXPECT_TRUE(property->getName() == "propInt");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 1123);
  property = properties.at(2).get();
  EXPECT_TRUE(property->getName() == "propString");
  value = static_cast<ValueNode *>(property->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::String);
  EXPECT_TRUE(value->getStringValue() == "lines");
}
