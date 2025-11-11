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
  auto node = std::static_pointer_cast<StreamNode>(nodes.at(1));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_EQ(node->getLine(), 3);
  std::vector<ASTNode> streamParts = node->getChildren();
  EXPECT_TRUE(streamParts.size() == 2);
  ASTNode streamComp = streamParts.at(0);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
  EXPECT_EQ(streamComp->getLine(), 3);
  auto nameNode = std::static_pointer_cast<BlockNode>(streamComp);
  EXPECT_TRUE(nameNode->getName() == "Val1");
  EXPECT_TRUE(nameNode->getLine() == 3);
  streamComp = streamParts.at(1);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
  EXPECT_TRUE(streamComp->getLine() == 3);
  nameNode = std::static_pointer_cast<BlockNode>(streamComp);
  EXPECT_TRUE(nameNode->getName() == "Val2");
  EXPECT_TRUE(nameNode->getLine() == 3);

  // Func1() >> Func2() ;
  node = std::static_pointer_cast<StreamNode>(nodes.at(2));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 4);
  streamParts = node->getChildren();
  EXPECT_TRUE(streamParts.size() == 2);
  streamComp = streamParts.at(0);
  EXPECT_TRUE(streamComp->getLine() == 4);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  auto functionNode = std::static_pointer_cast<FunctionNode>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func1");
  EXPECT_TRUE(functionNode->getLine() == 4);
  streamComp = streamParts.at(1);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  EXPECT_TRUE(streamComp->getLine() == 4);
  functionNode = std::static_pointer_cast<FunctionNode>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func2");
  EXPECT_TRUE(functionNode->getLine() == 4);

  // Val1 >> Func1() >> Func2() >> Val2 ;
  node = std::static_pointer_cast<StreamNode>(nodes.at(3));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 6);
  streamComp = node->getLeft();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
  EXPECT_TRUE(streamComp->getLine() == 6);
  nameNode = std::static_pointer_cast<BlockNode>(streamComp);
  EXPECT_TRUE(nameNode->getName() == "Val1");
  EXPECT_TRUE(nameNode->getLine() == 6);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  node = std::static_pointer_cast<StreamNode>(node->getRight());
  streamComp = node->getLeft();
  EXPECT_TRUE(streamComp->getLine() == 6);
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  functionNode = std::static_pointer_cast<FunctionNode>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func1");
  EXPECT_TRUE(functionNode->getLine() == 6);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  node = std::static_pointer_cast<StreamNode>(node->getRight());
  EXPECT_TRUE(node->getLine() == 7);
  streamComp = node->getLeft();
  EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
  EXPECT_TRUE(streamComp->getLine() == 7);
  functionNode = std::static_pointer_cast<FunctionNode>(streamComp);
  EXPECT_TRUE(functionNode->getName() == "Func2");
  EXPECT_TRUE(functionNode->getLine() == 7);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Block);
  nameNode = std::static_pointer_cast<BlockNode>(node->getRight());
  EXPECT_TRUE(nameNode->getName() == "Val2");
  EXPECT_TRUE(nameNode->getLine() == 8);

  //    Bundle1[1] >> Bundle2[2];
  node = std::static_pointer_cast<StreamNode>(nodes.at(4));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 10);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  auto bundle = std::static_pointer_cast<BundleNode>(node->getLeft());
  EXPECT_TRUE(bundle->getName() == "Bundle1");
  EXPECT_TRUE(bundle->getLine() == 10);
  auto indexList = bundle->index();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  auto value =
      std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 10);
  EXPECT_TRUE(value->getIntValue() == 1);
  bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  EXPECT_TRUE(bundle->getName() == "Bundle2");
  EXPECT_TRUE(bundle->getLine() == 10);
  indexList = bundle->index();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 10);
  EXPECT_TRUE(value->getIntValue() == 2);

  //    Val1 * 3 >> Bundle[2];
  node = std::static_pointer_cast<StreamNode>(nodes.at(5));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Expression);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getLine() == 11);
  auto expression = std::static_pointer_cast<ExpressionNode>(node->getLeft());
  EXPECT_TRUE(expression->getExpressionType() == ExpressionNode::Multiply);
  EXPECT_TRUE(expression->getLeft()->getNodeType() == AST::Block);
  EXPECT_TRUE(expression->getLine() == 11);
  nameNode = std::static_pointer_cast<BlockNode>(expression->getLeft());
  EXPECT_TRUE(nameNode->getName() == "Val1");
  EXPECT_TRUE(nameNode->getLine() == 11);
  EXPECT_TRUE(expression->getRight()->getNodeType() == AST::Int);
  EXPECT_TRUE(std::static_pointer_cast<ValueNode>(expression->getRight())
                  ->getIntValue() == 3);
  bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  EXPECT_TRUE(bundle->getName() == "Bundle");
  EXPECT_TRUE(bundle->getLine() == 11);
  indexList = bundle->index();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 11);
  EXPECT_TRUE(value->getIntValue() == 2);

  //    Bundle1[1] * 0.5 >> Bundle2[2];
  node = std::static_pointer_cast<StreamNode>(nodes.at(6));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Expression);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getLine() == 12);
  expression = std::static_pointer_cast<ExpressionNode>(node->getLeft());
  EXPECT_TRUE(expression->getExpressionType() == ExpressionNode::Multiply);
  EXPECT_TRUE(expression->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(expression->getLine() == 12);
  bundle = std::static_pointer_cast<BundleNode>(expression->getLeft());
  EXPECT_TRUE(bundle->getName() == "Bundle1");
  EXPECT_TRUE(bundle->getLine() == 12);
  EXPECT_TRUE(expression->getRight()->getNodeType() == AST::Real);
  EXPECT_TRUE(std::static_pointer_cast<ValueNode>(expression->getRight())
                  ->getRealValue() == 0.5);
  bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  EXPECT_TRUE(bundle->getName() == "Bundle2");
  EXPECT_TRUE(bundle->getLine() == 12);
  indexList = bundle->index();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  value = std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 12);
  EXPECT_TRUE(value->getIntValue() == 2);

  //    BundleRange[1:2] >> BundleRange2[3:4];
  node = std::static_pointer_cast<StreamNode>(nodes.at(7));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getLine() == 14);
  bundle = std::static_pointer_cast<BundleNode>(node->getLeft());
  EXPECT_TRUE(bundle->getName() == "BundleRange");
  EXPECT_EQ(bundle->getLine(), 14);
  auto index = std::static_pointer_cast<ListNode>(bundle->index());
  EXPECT_TRUE(index->getNodeType() == AST::List);
  EXPECT_TRUE(index->getChildren().size() == 1);
  indexList = bundle->index();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  auto range =
      std::static_pointer_cast<RangeNode>(indexList->getChildren().at(0));
  EXPECT_TRUE(range->getNodeType() == AST::Range);
  EXPECT_TRUE(range->getLine() == 14);
  value = std::static_pointer_cast<ValueNode>(range->startIndex());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 1);
  value = std::static_pointer_cast<ValueNode>(range->endIndex());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 14);
  EXPECT_TRUE(value->getIntValue() == 2);

  bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  EXPECT_TRUE(bundle->getName() == "BundleRange2");
  EXPECT_TRUE(bundle->getLine() == 14);
  index = std::static_pointer_cast<ListNode>(bundle->index());
  EXPECT_TRUE(index->getNodeType() == AST::List);
  EXPECT_TRUE(index->getChildren().size() == 1);
  indexList = bundle->index();
  EXPECT_TRUE(indexList->getNodeType() == AST::List);
  EXPECT_TRUE(indexList->size() == 1);
  range = std::static_pointer_cast<RangeNode>(indexList->getChildren().at(0));
  EXPECT_TRUE(range->getNodeType() == AST::Range);
  EXPECT_TRUE(range->getLine() == 14);
  value = std::static_pointer_cast<ValueNode>(range->startIndex());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getIntValue() == 3);
  value = std::static_pointer_cast<ValueNode>(range->endIndex());
  EXPECT_TRUE(value->getNodeType() == AST::Int);
  EXPECT_TRUE(value->getLine() == 14);
  EXPECT_TRUE(value->getIntValue() == 4);

  //    AudioIn[1] >> level(gain: 1.5) >> AudioOut[1];
  node = std::static_pointer_cast<StreamNode>(nodes.at(8));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLine() == 16);
  node = std::static_pointer_cast<StreamNode>(node->getRight());
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Function);
  functionNode = std::static_pointer_cast<FunctionNode>(node->getLeft());
  EXPECT_TRUE(functionNode->getName() == "Level");
  std::vector<std::shared_ptr<PropertyNode>> properties =
      functionNode->getProperties();
  EXPECT_TRUE(properties.size() == 1);
  auto prop = properties[0];
  EXPECT_TRUE(prop->getName() == "gain");
  value = std::static_pointer_cast<ValueNode>(prop->getValue());
  EXPECT_TRUE(value->getNodeType() == AST::Real);
  EXPECT_TRUE(value->getRealValue() == 1.5);

  //    A[1:2,3,4] >> B[1,2,3:4] >> C[1,2:3,4] >> D[1,2,3,4];
  node = std::static_pointer_cast<StreamNode>(nodes.at(9));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  bundle = std::static_pointer_cast<BundleNode>(node->getLeft());
  EXPECT_TRUE(bundle->getName() == "A");
  EXPECT_TRUE(bundle->getLine() == 18);
  auto list = std::static_pointer_cast<ListNode>(bundle->index());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  EXPECT_TRUE(list->getLine() == 18);

  node = std::static_pointer_cast<StreamNode>(nodes.at(10));
  EXPECT_TRUE(node->getNodeType() == AST::Stream);
  EXPECT_TRUE(node->getLeft()->getNodeType() == AST::List);
  EXPECT_TRUE(node->getRight()->getNodeType() == AST::List);
  auto l = std::static_pointer_cast<ListNode>(node->getLeft());
  EXPECT_TRUE(l->size() == 2);
  std::vector<ASTNode> elements = l->getChildren();
  EXPECT_TRUE(elements.size() == 2);
  EXPECT_TRUE(elements.at(0)->getNodeType() == AST::Bundle);
  EXPECT_TRUE(elements.at(1)->getNodeType() == AST::Bundle);
  l = std::static_pointer_cast<ListNode>(node->getRight());
  EXPECT_TRUE(l->size() == 2);
  elements = l->getChildren();
  EXPECT_TRUE(elements.size() == 2);
  EXPECT_TRUE(elements.at(0)->getNodeType() == AST::Bundle);
  EXPECT_TRUE(elements.at(1)->getNodeType() == AST::Bundle);

  EXPECT_TRUE(l->getLine() == 20);
}

TEST(Stream, Iterator) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/04_basic_stream.stride");
  std::vector<ASTNode> nodes = tree->getChildren();

  // Val1 >> Val2 ;
  {
    auto node = std::static_pointer_cast<StreamNode>(nodes.at(1));
    EXPECT_TRUE(node->getNodeType() == AST::Stream);
    EXPECT_EQ(node->getLine(), 3);
    StreamNodeIterator it(node);
    auto streamComp = it.next();
    EXPECT_TRUE(streamComp);
    EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
    EXPECT_TRUE(std::static_pointer_cast<BlockNode>(streamComp)->getName() ==
                "Val1");
    EXPECT_EQ(streamComp->getLine(), 3);
    streamComp = it.next();
    EXPECT_TRUE(streamComp);
    EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
    EXPECT_TRUE(std::static_pointer_cast<BlockNode>(streamComp)->getName() ==
                "Val2");
    EXPECT_TRUE(streamComp->getLine() == 3);
    streamComp = it.next();
    EXPECT_FALSE(streamComp);
  }

  // Func1() >> Func2() ;
  {
    auto node = std::static_pointer_cast<StreamNode>(nodes.at(2));
    EXPECT_TRUE(node->getNodeType() == AST::Stream);
    EXPECT_TRUE(node->getLine() == 4);
    StreamNodeIterator it(node);
    auto streamComp = it.next();
    EXPECT_TRUE(streamComp);
    EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
    EXPECT_TRUE(std::static_pointer_cast<FunctionNode>(streamComp)->getName() ==
                "Func1");
    streamComp = it.next();
    EXPECT_TRUE(streamComp);
    EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
    EXPECT_TRUE(std::static_pointer_cast<FunctionNode>(streamComp)->getName() ==
                "Func2");
    streamComp = it.next();
    EXPECT_FALSE(streamComp);
  }

  // Val1 >> Func1() >> Func2() >> Val2 ;
  {
    auto node = std::static_pointer_cast<StreamNode>(nodes.at(3));
    EXPECT_TRUE(node->getNodeType() == AST::Stream);
    EXPECT_TRUE(node->getLine() == 6);
    StreamNodeIterator it(node);
    auto streamComp = it.next();
    EXPECT_TRUE(streamComp);
    EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
    EXPECT_TRUE(std::static_pointer_cast<BlockNode>(streamComp)->getName() ==
                "Val1");
    streamComp = it.next();
    node = std::static_pointer_cast<StreamNode>(streamComp);
    EXPECT_TRUE(streamComp->getLine() == 6);
    EXPECT_TRUE(streamComp->getNodeType() == AST::Function);
    auto functionNode = std::static_pointer_cast<FunctionNode>(streamComp);
    EXPECT_TRUE(functionNode->getName() == "Func1");
    EXPECT_TRUE(functionNode->getLine() == 6);
    streamComp = it.next();
    functionNode = std::static_pointer_cast<FunctionNode>(streamComp);
    EXPECT_TRUE(functionNode->getName() == "Func2");
    EXPECT_TRUE(functionNode->getLine() == 7);
    streamComp = it.next();
    EXPECT_TRUE(streamComp->getNodeType() == AST::Block);
    auto nameNode = std::static_pointer_cast<BlockNode>(streamComp);
    EXPECT_TRUE(nameNode->getName() == "Val2");
    EXPECT_TRUE(nameNode->getLine() == 8);
  }

  // //    Bundle1[1] >> Bundle2[2];
  // node = std::static_pointer_cast<StreamNode>(nodes.at(4));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLine() == 10);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  // auto bundle = std::static_pointer_cast<BundleNode>(node->getLeft());
  // EXPECT_TRUE(bundle->getName() == "Bundle1");
  // EXPECT_TRUE(bundle->getLine() == 10);
  // auto indexList = bundle->index();
  // EXPECT_TRUE(indexList->getNodeType() == AST::List);
  // EXPECT_TRUE(indexList->size() == 1);
  // auto value =
  //     std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getLine() == 10);
  // EXPECT_TRUE(value->getIntValue() == 1);
  // bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  // EXPECT_TRUE(bundle->getName() == "Bundle2");
  // EXPECT_TRUE(bundle->getLine() == 10);
  // indexList = bundle->index();
  // EXPECT_TRUE(indexList->getNodeType() == AST::List);
  // EXPECT_TRUE(indexList->size() == 1);
  // value =
  // std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getLine() == 10);
  // EXPECT_TRUE(value->getIntValue() == 2);

  // //    Val1 * 3 >> Bundle[2];
  // node = std::static_pointer_cast<StreamNode>(nodes.at(5));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Expression);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getLine() == 11);
  // auto expression =
  // std::static_pointer_cast<ExpressionNode>(node->getLeft());
  // EXPECT_TRUE(expression->getExpressionType() == ExpressionNode::Multiply);
  // EXPECT_TRUE(expression->getLeft()->getNodeType() == AST::Block);
  // EXPECT_TRUE(expression->getLine() == 11);
  // nameNode = std::static_pointer_cast<BlockNode>(expression->getLeft());
  // EXPECT_TRUE(nameNode->getName() == "Val1");
  // EXPECT_TRUE(nameNode->getLine() == 11);
  // EXPECT_TRUE(expression->getRight()->getNodeType() == AST::Int);
  // EXPECT_TRUE(std::static_pointer_cast<ValueNode>(expression->getRight())
  //                 ->getIntValue() == 3);
  // bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  // EXPECT_TRUE(bundle->getName() == "Bundle");
  // EXPECT_TRUE(bundle->getLine() == 11);
  // indexList = bundle->index();
  // EXPECT_TRUE(indexList->getNodeType() == AST::List);
  // EXPECT_TRUE(indexList->size() == 1);
  // value =
  // std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getLine() == 11);
  // EXPECT_TRUE(value->getIntValue() == 2);

  // //    Bundle1[1] * 0.5 >> Bundle2[2];
  // node = std::static_pointer_cast<StreamNode>(nodes.at(6));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Expression);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getLine() == 12);
  // expression = std::static_pointer_cast<ExpressionNode>(node->getLeft());
  // EXPECT_TRUE(expression->getExpressionType() == ExpressionNode::Multiply);
  // EXPECT_TRUE(expression->getLeft()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(expression->getLine() == 12);
  // bundle = std::static_pointer_cast<BundleNode>(expression->getLeft());
  // EXPECT_TRUE(bundle->getName() == "Bundle1");
  // EXPECT_TRUE(bundle->getLine() == 12);
  // EXPECT_TRUE(expression->getRight()->getNodeType() == AST::Real);
  // EXPECT_TRUE(std::static_pointer_cast<ValueNode>(expression->getRight())
  //                 ->getRealValue() == 0.5);
  // bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  // EXPECT_TRUE(bundle->getName() == "Bundle2");
  // EXPECT_TRUE(bundle->getLine() == 12);
  // indexList = bundle->index();
  // EXPECT_TRUE(indexList->getNodeType() == AST::List);
  // EXPECT_TRUE(indexList->size() == 1);
  // value =
  // std::static_pointer_cast<ValueNode>(indexList->getChildren().at(0));
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getLine() == 12);
  // EXPECT_TRUE(value->getIntValue() == 2);

  // //    BundleRange[1:2] >> BundleRange2[3:4];
  // node = std::static_pointer_cast<StreamNode>(nodes.at(7));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getLine() == 14);
  // bundle = std::static_pointer_cast<BundleNode>(node->getLeft());
  // EXPECT_TRUE(bundle->getName() == "BundleRange");
  // EXPECT_EQ(bundle->getLine(), 14);
  // auto index = std::static_pointer_cast<ListNode>(bundle->index());
  // EXPECT_TRUE(index->getNodeType() == AST::List);
  // EXPECT_TRUE(index->getChildren().size() == 1);
  // indexList = bundle->index();
  // EXPECT_TRUE(indexList->getNodeType() == AST::List);
  // EXPECT_TRUE(indexList->size() == 1);
  // auto range =
  //     std::static_pointer_cast<RangeNode>(indexList->getChildren().at(0));
  // EXPECT_TRUE(range->getNodeType() == AST::Range);
  // EXPECT_TRUE(range->getLine() == 14);
  // value = std::static_pointer_cast<ValueNode>(range->startIndex());
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getIntValue() == 1);
  // value = std::static_pointer_cast<ValueNode>(range->endIndex());
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getLine() == 14);
  // EXPECT_TRUE(value->getIntValue() == 2);

  // bundle = std::static_pointer_cast<BundleNode>(node->getRight());
  // EXPECT_TRUE(bundle->getName() == "BundleRange2");
  // EXPECT_TRUE(bundle->getLine() == 14);
  // index = std::static_pointer_cast<ListNode>(bundle->index());
  // EXPECT_TRUE(index->getNodeType() == AST::List);
  // EXPECT_TRUE(index->getChildren().size() == 1);
  // indexList = bundle->index();
  // EXPECT_TRUE(indexList->getNodeType() == AST::List);
  // EXPECT_TRUE(indexList->size() == 1);
  // range =
  // std::static_pointer_cast<RangeNode>(indexList->getChildren().at(0));
  // EXPECT_TRUE(range->getNodeType() == AST::Range);
  // EXPECT_TRUE(range->getLine() == 14);
  // value = std::static_pointer_cast<ValueNode>(range->startIndex());
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getIntValue() == 3);
  // value = std::static_pointer_cast<ValueNode>(range->endIndex());
  // EXPECT_TRUE(value->getNodeType() == AST::Int);
  // EXPECT_TRUE(value->getLine() == 14);
  // EXPECT_TRUE(value->getIntValue() == 4);

  // //    AudioIn[1] >> level(gain: 1.5) >> AudioOut[1];
  // node = std::static_pointer_cast<StreamNode>(nodes.at(8));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLine() == 16);
  // node = std::static_pointer_cast<StreamNode>(node->getRight());
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Function);
  // functionNode = std::static_pointer_cast<FunctionNode>(node->getLeft());
  // EXPECT_TRUE(functionNode->getName() == "Level");
  // std::vector<std::shared_ptr<PropertyNode>> properties =
  //     functionNode->getProperties();
  // EXPECT_TRUE(properties.size() == 1);
  // auto prop = properties[0];
  // EXPECT_TRUE(prop->getName() == "gain");
  // value = std::static_pointer_cast<ValueNode>(prop->getValue());
  // EXPECT_TRUE(value->getNodeType() == AST::Real);
  // EXPECT_TRUE(value->getRealValue() == 1.5);

  // //    A[1:2,3,4] >> B[1,2,3:4] >> C[1,2:3,4] >> D[1,2,3,4];
  // node = std::static_pointer_cast<StreamNode>(nodes.at(9));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::Stream);
  // bundle = std::static_pointer_cast<BundleNode>(node->getLeft());
  // EXPECT_TRUE(bundle->getName() == "A");
  // EXPECT_TRUE(bundle->getLine() == 18);
  // auto list = std::static_pointer_cast<ListNode>(bundle->index());
  // EXPECT_TRUE(list->getNodeType() == AST::List);
  // EXPECT_TRUE(list->getLine() == 18);

  // node = std::static_pointer_cast<StreamNode>(nodes.at(10));
  // EXPECT_TRUE(node->getNodeType() == AST::Stream);
  // EXPECT_TRUE(node->getLeft()->getNodeType() == AST::List);
  // EXPECT_TRUE(node->getRight()->getNodeType() == AST::List);
  // auto l = std::static_pointer_cast<ListNode>(node->getLeft());
  // EXPECT_TRUE(l->size() == 2);
  // std::vector<ASTNode> elements = l->getChildren();
  // EXPECT_TRUE(elements.size() == 2);
  // EXPECT_TRUE(elements.at(0)->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(elements.at(1)->getNodeType() == AST::Bundle);
  // l = std::static_pointer_cast<ListNode>(node->getRight());
  // EXPECT_TRUE(l->size() == 2);
  // elements = l->getChildren();
  // EXPECT_TRUE(elements.size() == 2);
  // EXPECT_TRUE(elements.at(0)->getNodeType() == AST::Bundle);
  // EXPECT_TRUE(elements.at(1)->getNodeType() == AST::Bundle);

  // EXPECT_TRUE(l->getLine() == 20);
}

TEST(Stream, Builder) {
  StreamNodeBuilder builder;
  builder.addNode(std::make_shared<BlockNode>("Name1", __FILE__, __LINE__));
  builder.addNode(std::make_shared<BlockNode>("Name2", __FILE__, __LINE__));
  builder.addNode(std::make_shared<BlockNode>("Name3", __FILE__, __LINE__));
  builder.addNode(std::make_shared<BlockNode>("Name4", __FILE__, __LINE__));
  builder.addNode(std::make_shared<BlockNode>("Name5", __FILE__, __LINE__));
  auto stream = builder.build();
  EXPECT_TRUE(stream->getLeft());
  EXPECT_EQ(std::static_pointer_cast<BlockNode>(stream->getLeft())->getName(),
            "Name1");
  stream = std::static_pointer_cast<StreamNode>(stream->getRight());
  EXPECT_EQ(std::static_pointer_cast<BlockNode>(stream->getLeft())->getName(),
            "Name2");
  stream = std::static_pointer_cast<StreamNode>(stream->getRight());
  EXPECT_EQ(std::static_pointer_cast<BlockNode>(stream->getLeft())->getName(),
            "Name3");
  stream = std::static_pointer_cast<StreamNode>(stream->getRight());
  EXPECT_EQ(std::static_pointer_cast<BlockNode>(stream->getLeft())->getName(),
            "Name4");
  auto node = std::static_pointer_cast<BlockNode>(stream->getRight());
  EXPECT_EQ(node->getName(), "Name5");
}
