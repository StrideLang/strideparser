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

TEST(Bundle, Indeces) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/07_bundle_indeces.stride");
  EXPECT_NE(tree, nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  EXPECT_TRUE(nodes.size() == 23);

  EXPECT_TRUE(nodes.at(0)->getNodeType() == AST::Declaration);
  DeclarationNode *block = static_cast<DeclarationNode *>(nodes.at(0).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_EQ(block->getLine(), 8);

  EXPECT_TRUE(nodes.at(1)->getNodeType() == AST::BundleDeclaration);
  block = static_cast<DeclarationNode *>(nodes.at(1).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 13);
  BundleNode *bundle = block->getBundle().get();
  EXPECT_TRUE(bundle->getName() == "SIZE");
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  EXPECT_TRUE(bundle->getLine() == 13);
  ListNode *indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  ValueNode *value =
      static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 4);
  EXPECT_TRUE(value->getLine() == 13);
  EXPECT_TRUE(block->getProperties().size() == 2);
  EXPECT_TRUE(block->getProperties().at(0)->getNodeType() == AST::Property);
  PropertyNode *property =
      static_cast<PropertyNode *>(block->getProperties().at(0).get());
  ListNode *listnode = static_cast<ListNode *>(property->getValue().get());
  EXPECT_TRUE(listnode->getNodeType() == AST::List);
  EXPECT_TRUE(listnode->getChildren().size() == 4);
  EXPECT_TRUE(listnode->getLine() == 14);
  value = static_cast<ValueNode *>(listnode->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 16);
  EXPECT_TRUE(value->getLine() == 14);
  value = static_cast<ValueNode *>(listnode->getChildren().at(1).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 32);
  EXPECT_TRUE(value->getLine() == 14);
  value = static_cast<ValueNode *>(listnode->getChildren().at(2).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 64);
  EXPECT_TRUE(value->getLine() == 14);
  value = static_cast<ValueNode *>(listnode->getChildren().at(3).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 128);
  EXPECT_TRUE(value->getLine() == 14);

  // constant Array_Parens [ ( CONST * 2 ) + 1 ] {}
  block = static_cast<DeclarationNode *>(nodes.at(6).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 23);
  bundle = block->getBundle().get();
  EXPECT_TRUE(bundle->getName() == "Array_Parens");
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  ExpressionNode *expr =
      static_cast<ExpressionNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(expr->getNodeType() == AST::Expression);
  EXPECT_TRUE(expr->getExpressionType() == ExpressionNode::Add);
  EXPECT_TRUE(expr->getLine() == 23);
  ExpressionNode *expr2 = static_cast<ExpressionNode *>(expr->getLeft().get());
  EXPECT_TRUE(expr2->getExpressionType() == ExpressionNode::Multiply);
  EXPECT_TRUE(expr2->getLeft()->getNodeType() == AST::Block);
  EXPECT_TRUE(expr2->getRight()->getNodeType() == AST::Int);
  EXPECT_TRUE(expr2->getLine() == 23);

  // constant Array_Expr [ SIZE [1] + SIZE [1 * 2] ] {}
  block = static_cast<DeclarationNode *>(nodes.at(8).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  EXPECT_TRUE(block->getLine() == 26);
  EXPECT_TRUE(block->getObjectType() == "constant");
  bundle = block->getBundle().get();
  EXPECT_TRUE(bundle->getName() == "Array_Expr");
  EXPECT_TRUE(bundle->getLine() == 26);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  expr = static_cast<ExpressionNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(expr->getNodeType() == AST::Expression);
  EXPECT_TRUE(expr->getExpressionType() == ExpressionNode::Add);
  EXPECT_TRUE(expr->getLine() == 26);
  bundle = static_cast<BundleNode *>(expr->getLeft().get());
  EXPECT_TRUE(bundle->getNodeType() == AST::Bundle);
  EXPECT_TRUE(bundle->getName() == "SIZE");
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 1);
  bundle = static_cast<BundleNode *>(expr->getRight().get());
  EXPECT_TRUE(bundle->getNodeType() == AST::Bundle);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  expr = static_cast<ExpressionNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(expr->getNodeType() == AST::Expression);
  EXPECT_TRUE(expr->getExpressionType() == ExpressionNode::Multiply);
  EXPECT_TRUE(expr->getLeft()->getNodeType() == AST::Int);
  EXPECT_TRUE(expr->getRight()->getNodeType() == AST::Int);
  EXPECT_TRUE(expr->getLine() == 26);

  // constant Array_Expr2 [ SIZE [1] / SIZE [1 - 2] ] {}
  block = static_cast<DeclarationNode *>(nodes.at(9).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 27);
  bundle = block->getBundle().get();
  EXPECT_TRUE(bundle->getName() == "Array_Expr2");
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  EXPECT_TRUE(bundle->getLine() == 27);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  expr = static_cast<ExpressionNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(expr->getNodeType() == AST::Expression);
  EXPECT_TRUE(expr->getExpressionType() == ExpressionNode::Divide);
  EXPECT_TRUE(expr->getLine() == 27);
  bundle = static_cast<BundleNode *>(expr->getLeft().get());
  EXPECT_TRUE(bundle->getNodeType() == AST::Bundle);
  EXPECT_TRUE(bundle->getName() == "SIZE");
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  EXPECT_TRUE(bundle->getLine() == 27);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 1);
  EXPECT_TRUE(value->getLine() == 27);
  bundle = static_cast<BundleNode *>(expr->getRight().get());
  EXPECT_TRUE(bundle->getNodeType() == AST::Bundle);
  EXPECT_TRUE(bundle->getLine() == 27);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  expr = static_cast<ExpressionNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(expr->getNodeType() == AST::Expression);
  EXPECT_TRUE(expr->getExpressionType() == ExpressionNode::Subtract);
  EXPECT_TRUE(expr->getLeft()->getNodeType() == AST::Int);
  EXPECT_TRUE(expr->getRight()->getNodeType() == AST::Int);
  EXPECT_TRUE(expr->getLine() == 27);
}

TEST(Bundle, Basic) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/03_basic_bundle.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  EXPECT_TRUE(nodes.size() == 8);
  DeclarationNode *block = static_cast<DeclarationNode *>(nodes.at(1).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 5);
  std::shared_ptr<BundleNode> bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "Integer");
  EXPECT_TRUE(bundle->getLine() == 5);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  ListNode *indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  ValueNode *valueNode =
      static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 3);
  EXPECT_TRUE(bundle->getLine() == 5);
  EXPECT_TRUE(block->getProperties().size() == 2);
  std::shared_ptr<PropertyNode> property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  AST *propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  ListNode *listnode = static_cast<ListNode *>(propertyValue);
  //    EXPECT_TRUE(listnode->getListType() == AST::Int);
  std::vector<ASTNode> listValues = listnode->getChildren();
  EXPECT_TRUE(listValues.size() == 3);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::Int);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::Expression);
  EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::Int);
  property = block->getProperties().at(1);
  EXPECT_TRUE(property->getName() == "meta");

  // Next Block - Float list
  block = static_cast<DeclarationNode *>(nodes.at(2).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 10);
  bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "Float");
  EXPECT_TRUE(bundle->getLine() == 10);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  valueNode = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 4);
  EXPECT_TRUE(block->getProperties().size() == 2);
  property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  listnode = static_cast<ListNode *>(propertyValue);
  //    EXPECT_TRUE(listnode->getListType() == AST::Real);
  listValues = listnode->getChildren();
  EXPECT_TRUE(listValues.size() == 4);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::Real);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::Expression);
  EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::Real);
  EXPECT_TRUE(listValues.at(3)->getNodeType() == AST::Real);
  EXPECT_TRUE(block->getChildren().at(1)->getNodeType() == AST::Property);
  property =
      std::static_pointer_cast<PropertyNode>(block->getProperties().at(1));
  EXPECT_TRUE(property->getName() == "meta");

  // Next Block - String list
  block = static_cast<DeclarationNode *>(nodes.at(3).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 15);
  bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "String");
  EXPECT_TRUE(bundle->getLine() == 15);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  valueNode = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 5);
  EXPECT_TRUE(block->getProperties().size() == 2);
  property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  listnode = static_cast<ListNode *>(propertyValue);
  EXPECT_TRUE(listnode->getListType() == AST::String);
  listValues = listnode->getChildren();
  EXPECT_TRUE(listValues.size() == 5);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::String);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::String);
  EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::String);
  EXPECT_TRUE(listValues.at(3)->getNodeType() == AST::String);
  EXPECT_TRUE(listValues.at(4)->getNodeType() == AST::String);
  property = block->getProperties().at(1);
  EXPECT_TRUE(property->getName() == "meta");

  // Next Block - UVar list
  block = static_cast<DeclarationNode *>(nodes.at(4).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 21);
  bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "UVar");
  EXPECT_TRUE(bundle->getLine() == 21);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  valueNode = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 7);
  EXPECT_TRUE(block->getProperties().size() == 2);
  property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  listValues = propertyValue->getChildren();
  EXPECT_TRUE(listValues.size() == 7);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::Block);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::Block);
  EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::Block);
  EXPECT_TRUE(listValues.at(3)->getNodeType() == AST::Block);
  EXPECT_TRUE(listValues.at(4)->getNodeType() == AST::Block);
  EXPECT_TRUE(listValues.at(5)->getNodeType() == AST::Block);
  EXPECT_TRUE(listValues.at(6)->getNodeType() == AST::Block);
  property = block->getProperties().at(1);
  EXPECT_TRUE(property->getName() == "meta");

  // Next Block - ArrayList list
  block = static_cast<DeclarationNode *>(nodes.at(5).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 26);
  bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "ArrayList");
  EXPECT_TRUE(bundle->getLine() == 26);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  valueNode = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 2);
  EXPECT_TRUE(block->getProperties().size() == 2);
  property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  listValues = propertyValue->getChildren();
  EXPECT_TRUE(listValues.size() == 2);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::Bundle);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::Bundle);
  property = block->getProperties().at(1);
  EXPECT_TRUE(property->getName() == "meta");

  // Next Block - BlockList list
  block = static_cast<DeclarationNode *>(nodes.at(6).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 31);
  bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "BlockList");
  EXPECT_TRUE(bundle->getLine() == 31);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  valueNode = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 3);
  EXPECT_TRUE(block->getProperties().size() == 2);
  property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  listValues = propertyValue->getChildren();
  EXPECT_TRUE(listValues.size() == 3);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::Declaration);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::Declaration);
  property =
      std::static_pointer_cast<PropertyNode>(block->getProperties().at(1));
  EXPECT_TRUE(property->getName() == "meta");

  // Next Block - BlockBundleList list
  block = static_cast<DeclarationNode *>(nodes.at(7).get());
  EXPECT_TRUE(block->getObjectType() == "constant");
  EXPECT_TRUE(block->getLine() == 36);
  bundle = block->getBundle();
  EXPECT_TRUE(bundle->getName() == "BlockBundleList");
  EXPECT_TRUE(bundle->getLine() == 36);
  EXPECT_TRUE(bundle->getChildren().size() == 1);
  indexList = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  valueNode = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  EXPECT_TRUE(valueNode->getIntValue() == 3);
  EXPECT_TRUE(block->getProperties().size() == 2);
  property = block->getProperties().at(0);
  EXPECT_TRUE(property->getName() == "value");
  propertyValue = property->getValue().get();
  EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  listValues = propertyValue->getChildren();
  EXPECT_TRUE(listValues.size() == 3);
  EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::BundleDeclaration);
  EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::BundleDeclaration);
  EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::BundleDeclaration);
  property = block->getProperties().at(1);
  EXPECT_TRUE(property->getName() == "meta");

  //    // Next Block - IntegerList list
  //    block = static_cast<DeclarationNode *>(nodes.at(7));
  //    EXPECT_TRUE(block->getObjectType() == "constant");
  //    EXPECT_TRUE(block->getLine() == 46);
  //    bundle = block->getBundle();
  //    EXPECT_TRUE(bundle->getName() == "IntegerList");
  //    EXPECT_TRUE(bundle->getLine() == 46);
  //    EXPECT_TRUE(bundle->getChildren().size() == 1);
  //    valueNode = static_cast<ValueNode *>(bundle->getChildren().at(0));
  //    EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  //    EXPECT_TRUE(valueNode->getIntValue() == 3);
  //    EXPECT_TRUE(block->getProperties().size() == 2);
  //    property = block->getProperties().at(0);
  //    EXPECT_TRUE(property->getName() == "value");
  //    propertyValue = property->getValue();
  //    EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  //    listValues = propertyValue->getChildren();
  //    EXPECT_TRUE(listValues.size() == 3);
  //    EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::List);
  //    EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::List);
  //    EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::List);
  //    property = block->getProperties().at(1);
  //    EXPECT_TRUE(property->getName() == "meta");

  //    // Next Block - IntegerList list
  //    block = static_cast<DeclarationNode *>(nodes.at(8));
  //    EXPECT_TRUE(block->getObjectType() == "constant");
  //    EXPECT_TRUE(block->getLine() == 52);
  //    bundle = block->getBundle();
  //    EXPECT_TRUE(bundle->getName() == "IntegerList");
  //    EXPECT_TRUE(bundle->getLine() == 53);
  //    EXPECT_TRUE(bundle->getChildren().size() == 1);
  //    valueNode = static_cast<ValueNode *>(bundle->getChildren().at(0));
  //    EXPECT_TRUE(valueNode->getNodeType() == AST::Int);
  //    EXPECT_TRUE(valueNode->getIntValue() == 3);
  //    EXPECT_TRUE(block->getProperties().size() == 2);
  //    property = block->getProperties().at(0);
  //    EXPECT_TRUE(property->getName() == "value");
  //    propertyValue = property->getValue();
  //    EXPECT_TRUE(propertyValue->getNodeType() == AST::List);
  //    listValues = propertyValue->getChildren();
  //    EXPECT_TRUE(listValues.size() == 3);
  //    EXPECT_TRUE(listValues.at(0)->getNodeType() == AST::List);
  //    EXPECT_TRUE(listValues.at(1)->getNodeType() == AST::List);
  //    EXPECT_TRUE(listValues.at(2)->getNodeType() == AST::List);
  //    property = block->getProperties().at(1);
  //    EXPECT_TRUE(property->getName() == "meta");
}
