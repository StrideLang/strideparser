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

TEST(Stream, Basic) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/04_basic_stream.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();

  // Val1 >> Val2 ;
  StreamNode *node = static_cast<StreamNode *>(nodes.at(1).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_EQ(node->getLine(), 3);
  std::vector<ASTNode> streamParts = node->getChildren();
  EXPECT_TRUE(streamParts.size() == 2);
  AST *streamComp = streamParts.at(0).get();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
  EXPECT_EQ(streamComp->getLine(), 3);
  BlockNode *nameNode = static_cast<BlockNode *>(streamComp);
  EXPECT_TRUE(nameNode->getName() == "Val1");
  EXPECT_TRUE(nameNode->getLine() == 3);
  streamComp = streamParts.at(1).get();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
  EXPECT_TRUE(streamComp->getLine() == 3);
  nameNode = static_cast<BlockNode *>(streamComp);
  EXPECT_TRUE(nameNode->getName() == "Val2");
  EXPECT_TRUE(nameNode->getLine() == 3);

  // Func1() >> Func2() ;
  node = static_cast<StreamNode *>(nodes.at(2).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 4);
  streamParts = node->getChildren();
  EXPECT_TRUE(streamParts.size() == 2);
  streamComp = streamParts.at(0).get();
  EXPECT_TRUE(streamComp->getLine() == 4);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  FunctionNode *functionNode = static_cast<FunctionNode *>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func1");
  EXPECT_TRUE(functionNode->getLine() == 4);
  streamComp = streamParts.at(1).get();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  EXPECT_TRUE(streamComp->getLine() == 4);
  functionNode = static_cast<FunctionNode *>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func2");
  EXPECT_TRUE(functionNode->getLine() == 4);

  // Val1 >> Func1() >> Func2() >> Val2 ;
  node = static_cast<StreamNode *>(nodes.at(3).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 6);
  streamComp = node->getLeft().get();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
  EXPECT_TRUE(streamComp->getLine() == 6);
  nameNode = static_cast<BlockNode *>(streamComp);
  EXPECT_TRUE(nameNode->getName() == "Val1");
  EXPECT_TRUE(nameNode->getLine() == 6);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  node = static_cast<StreamNode *>(node->getRight().get());
  streamComp = node->getLeft().get();
  EXPECT_TRUE(streamComp->getLine() == 6);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  functionNode = static_cast<FunctionNode *>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func1");
  EXPECT_TRUE(functionNode->getLine() == 6);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  node = static_cast<StreamNode *>(node->getRight().get());
  EXPECT_TRUE(node->getLine() == 7);
  streamComp = node->getLeft().get();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  EXPECT_TRUE(streamComp->getLine() == 7);
  functionNode = static_cast<FunctionNode *>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func2");
  EXPECT_TRUE(functionNode->getLine() == 7);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Block);
  nameNode = static_cast<BlockNode *>(node->getRight().get());
  EXPECT_TRUE(nameNode->getName() == "Val2");
  EXPECT_TRUE(nameNode->getLine() == 8);

  //    Bundle1[1] >> Bundle2[2];
  node = static_cast<StreamNode *>(nodes.at(4).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 10);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  BundleNode *bundle = static_cast<BundleNode *>(node->getLeft().get());
  EXPECT_TRUE(bundle->getName() == "Bundle1");
  EXPECT_TRUE(bundle->getLine() == 10);
  ListNode *indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  ValueNode *value =
      static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 10);
  EXPECT_TRUE(value->getIntValue() == 1);
  bundle = static_cast<BundleNode *>(node->getRight().get());
  EXPECT_TRUE(bundle->getName() == "Bundle2");
  EXPECT_TRUE(bundle->getLine() == 10);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 10);
  EXPECT_TRUE(value->getIntValue() == 2);

  //    Val1 * 3 >> Bundle[2];
  node = static_cast<StreamNode *>(nodes.at(5).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Expression);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getLine() == 11);
  ExpressionNode *expression =
      static_cast<ExpressionNode *>(node->getLeft().get());
  EXPECT_TRUE(expression->getExpressionType() == ExpressionNode::Multiply);
  EXPECT_TRUE(expression->getLeft()->getNodeType() == AST::Block);
  EXPECT_TRUE(expression->getLine() == 11);
  nameNode = static_cast<BlockNode *>(expression->getLeft().get());
  EXPECT_TRUE(nameNode->getName() == "Val1");
  EXPECT_TRUE(nameNode->getLine() == 11);
  EXPECT_TRUE(expression->getRight()->getNodeType() == AST::Int);
  EXPECT_TRUE(
      static_cast<ValueNode *>(expression->getRight().get())->getIntValue() ==
      3);
  bundle = static_cast<BundleNode *>(node->getRight().get());
  EXPECT_TRUE(bundle->getName() == "Bundle");
  EXPECT_TRUE(bundle->getLine() == 11);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 11);
  EXPECT_TRUE(value->getIntValue() == 2);

  //    Bundle1[1] * 0.5 >> Bundle2[2];
  node = static_cast<StreamNode *>(nodes.at(6).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Expression);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getLine() == 12);
  expression = static_cast<ExpressionNode *>(node->getLeft().get());
  EXPECT_TRUE(expression->getExpressionType() == ExpressionNode::Multiply);
  EXPECT_TRUE(expression->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(expression->getLine() == 12);
  bundle = static_cast<BundleNode *>(expression->getLeft().get());
  EXPECT_TRUE(bundle->getName() == "Bundle1");
  EXPECT_TRUE(bundle->getLine() == 12);
  EXPECT_TRUE(expression->getRight()->getNodeType() == AST::Real);
  EXPECT_TRUE(
      static_cast<ValueNode *>(expression->getRight().get())->getRealValue() ==
      0.5);
  bundle = static_cast<BundleNode *>(node->getRight().get());
  EXPECT_TRUE(bundle->getName() == "Bundle2");
  EXPECT_TRUE(bundle->getLine() == 12);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = static_cast<ValueNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 12);
  EXPECT_TRUE(value->getIntValue() == 2);

  //    BundleRange[1:2] >> BundleRange2[3:4];
  node = static_cast<StreamNode *>(nodes.at(7).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getLine() == 14);
  bundle = static_cast<BundleNode *>(node->getLeft().get());
  EXPECT_TRUE(bundle->getName() == "BundleRange");
  EXPECT_EQ(bundle->getLine(), 14);
  ListNode *index = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(index->getNodeType() == AST::List);
  EXPECT_TRUE(index->getChildren().size() == 1);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  RangeNode *range =
      static_cast<RangeNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(range->getNodeType() == AST::Range);
  EXPECT_TRUE(range->getLine() == 14);
  value = static_cast<ValueNode *>(range->startIndex().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 1);
  value = static_cast<ValueNode *>(range->endIndex().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 14);
  EXPECT_TRUE(value->getIntValue() == 2);

  bundle = static_cast<BundleNode *>(node->getRight().get());
  EXPECT_TRUE(bundle->getName() == "BundleRange2");
  EXPECT_TRUE(bundle->getLine() == 14);
  index = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(index->getNodeType() == AST::List);
  EXPECT_TRUE(index->getChildren().size() == 1);
  indexList = bundle->index().get();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  range = static_cast<RangeNode *>(indexList->getChildren().at(0).get());
  EXPECT_TRUE(range->getNodeType() == AST::Range);
  EXPECT_TRUE(range->getLine() == 14);
  value = static_cast<ValueNode *>(range->startIndex().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 3);
  value = static_cast<ValueNode *>(range->endIndex().get());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 14);
  EXPECT_TRUE(value->getIntValue() == 4);

  //    AudioIn[1] >> level(gain: 1.5) >> AudioOut[1];
  node = static_cast<StreamNode *>(nodes.at(8).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 16);
  node = static_cast<StreamNode *>(node->getRight().get());
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Function);
  functionNode = static_cast<FunctionNode *>(node->getLeft().get());
  EXPECT_TRUE(functionNode->getName() == "Level");
  std::vector<std::shared_ptr<PropertyNode>> properties =
      functionNode->getProperties();
  EXPECT_TRUE(properties.size() == 1);
  PropertyNode *prop = properties[0].get();
  EXPECT_TRUE(prop->getName() == "gain");
  value = static_cast<ValueNode *>(prop->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Real);
  EXPECT_TRUE(value->getRealValue() == 1.5);

  //    A[1:2,3,4] >> B[1,2,3:4] >> C[1,2:3,4] >> D[1,2,3,4];
  node = static_cast<StreamNode *>(nodes.at(9).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  bundle = static_cast<BundleNode *>(node->getLeft().get());
  EXPECT_TRUE(bundle->getName() == "A");
  EXPECT_TRUE(bundle->getLine() == 18);
  ListNode *list = static_cast<ListNode *>(bundle->index().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  EXPECT_TRUE(list->getLine() == 18);

  node = static_cast<StreamNode *>(nodes.at(10).get());
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::List);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::List);
  ListNode *l = static_cast<ListNode *>(node->getLeft().get());
  EXPECT_TRUE(l->size() == 2);
  std::vector<ASTNode> elements = l->getChildren();
  EXPECT_TRUE(elements.size() == 2);
  EXPECT_TRUE(elements.at(0)->getNodeType() == AST::Bundle);
  EXPECT_TRUE(elements.at(1)->getNodeType() == AST::Bundle);
  l = static_cast<ListNode *>(node->getRight().get());
  EXPECT_TRUE(l->size() == 2);
  elements = l->getChildren();
  EXPECT_TRUE(elements.size() == 2);
  EXPECT_TRUE(elements.at(0)->getNodeType() == AST::Bundle);
  EXPECT_TRUE(elements.at(1)->getNodeType() == AST::Bundle);

  EXPECT_TRUE(l->getLine() == 20);
}
