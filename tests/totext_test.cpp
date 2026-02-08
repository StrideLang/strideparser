#include "stride/parser/ast.h"
#include "stride/parser/blocknode.h"
#include "stride/parser/declarationnode.h"
#include "stride/parser/expressionnode.h"
#include "stride/parser/functionnode.h"
#include "stride/parser/importnode.h"
#include "stride/parser/listnode.h"
#include "stride/parser/platformnode.h"
#include "stride/parser/portpropertynode.h"
#include "stride/parser/propertynode.h"
#include "stride/parser/rangenode.h"
#include "stride/parser/streamnode.h"
#include "stride/parser/valuenode.h"
#include "gtest/gtest.h"

using namespace strd;

TEST(ToTextTest, ValueNodes) {
  ValueNode iv((int64_t)42, "test", 1);
  EXPECT_EQ(iv.toText(0, 2, false), "42");

  ValueNode rv(3.1415, "test", 1);
  // to_string might have different precision but let's check prefix
  EXPECT_TRUE(rv.toText(0, 2, false).find("3.14150") == 0);

  ValueNode sv("hello", "test", 1);
  EXPECT_EQ(sv.toText(0, 2, false), "\"hello\"");

  ValueNode swon(true, "test", 1);
  EXPECT_EQ(swon.toText(0, 2, false), "on ");

  ValueNode swoff(false, "test", 1);
  EXPECT_EQ(swoff.toText(0, 2, false), "off ");
}

TEST(ToTextTest, BlockNode) {
  BlockNode block("MyBlock", "test", 1);
  EXPECT_EQ(block.toText(0, 2, false), "MyBlock");
}

TEST(ToTextTest, PropertyNode) {
  auto val = std::make_shared<ValueNode>((int64_t)10, "test", 1);
  PropertyNode prop("age", val, "test", 1);
  EXPECT_EQ(prop.toText(0, 2, false), "age: 10");
  EXPECT_EQ(prop.toText(2, 2, false), "  age: 10");
}

TEST(ToTextTest, DeclarationNode) {
  auto props = std::make_shared<ListNode>("test", 1);
  auto p1 = std::make_shared<PropertyNode>(
      "p1", std::make_shared<ValueNode>((int64_t)1, "test", 1), "test", 1);
  props->addChild(p1);

  DeclarationNode decl("myObj", "MyType", props, "test", 1);
  std::string expected = "MyType myObj {\n  p1: 1\n}";
  EXPECT_EQ(decl.toText(0, 2, false), expected);
}

TEST(ToTextTest, ListNode) {
  auto list = std::make_shared<ListNode>("test", 1);
  list->addChild(std::make_shared<ValueNode>((int64_t)1, "test", 1));
  EXPECT_EQ(list->toText(0, 2, false), "[ 1 ]");

  list->addChild(std::make_shared<ValueNode>((int64_t)2, "test", 1));
  std::string expected = "[ \n1, 2\n ]";
  EXPECT_EQ(list->toText(0, 2, false), expected);
}

TEST(ToTextTest, FunctionNode) {
  auto props = std::make_shared<ListNode>("test", 1);
  auto p1 = std::make_shared<PropertyNode>(
      "param", std::make_shared<ValueNode>((int64_t)5, "test", 1), "test", 1);
  props->addChild(p1);

  FunctionNode func("myFunc", props, "test", 1);
  EXPECT_EQ(func.toText(0, 2, false), "myFunc(param: 5 )");
}

TEST(ToTextTest, StreamNode) {
  auto n1 = std::make_shared<ValueNode>((int64_t)1, "test", 1);
  auto n2 = std::make_shared<ValueNode>((int64_t)2, "test", 1);
  auto n3 = std::make_shared<ValueNode>((int64_t)3, "test", 1);

  auto s2 = std::make_shared<StreamNode>(n2, n3, "test", 1);
  auto s1 = std::make_shared<StreamNode>(n1, s2, "test", 1);

  EXPECT_EQ(s1->toText(0, 2, false), "1 >> 2 >> 3;");
}

TEST(ToTextTest, BundleNode) {
  auto indexList = std::make_shared<ListNode>("test", 1);
  indexList->addChild(std::make_shared<ValueNode>((int64_t)5, "test", 1));
  BundleNode bundle("MyBundle", indexList, "test", 1);
  EXPECT_EQ(bundle.toText(0, 2, false), "MyBundle[ 5 ]");
}

TEST(ToTextTest, ExpressionNode) {
  auto n1 = std::make_shared<ValueNode>((int64_t)10, "test", 1);
  auto n2 = std::make_shared<ValueNode>((int64_t)5, "test", 1);
  ExpressionNode exp(ExpressionNode::Multiply, n1, n2, "test", 1);
  EXPECT_EQ(exp.toText(0, 2, false), "(10 * 5)");
}

TEST(ToTextTest, ImportNode) {
  ImportNode imp("MyFile", "test", 1, "Alias");
  EXPECT_EQ(imp.toText(0, 2, false), "import MyFile as Alias;");

  ImportNode imp2("OtherFile", "test", 1);
  EXPECT_EQ(imp2.toText(0, 2, false), "import OtherFile;");
}

TEST(ToTextTest, PortPropertyNode) {
  PortPropertyNode pp("MyBlock", "InputPort", "test", 1);
  EXPECT_EQ(pp.toText(0, 2, false), "MyBlock.InputPort");
}

TEST(ToTextTest, RangeNode) {
  auto start = std::make_shared<ValueNode>((int64_t)1, "test", 1);
  auto end = std::make_shared<ValueNode>((int64_t)10, "test", 1);
  RangeNode range(start, end, "test", 1);
  EXPECT_EQ(range.toText(0, 2, false), "1:10");
}

TEST(ToTextTest, SystemNode) {
  SystemNode sys("MyPlatform", 1, 2, "test", 1);
  EXPECT_EQ(sys.toText(0, 2, false), "use MyPlatform 1.2;");
}
