#include <cstdint>

#include "stride/parser/ast.h"
#include "stride/parser/langerror.h"
#include "stride/parser/listnode.h"
#include "stride/parser/streamnode.h"
#include "stride/parser/valuenode.h"
#include "gtest/gtest.h"

using namespace strd;

namespace {

// The value streamed by statement index of tree
std::shared_ptr<ValueNode> streamedValue(const ASTNode &tree, size_t index) {
  auto stream =
      std::static_pointer_cast<StreamNode>(tree->getChildren().at(index));
  auto left = stream->getLeft();
  EXPECT_TRUE(left->getNodeType() == AST::Int ||
              left->getNodeType() == AST::Real);
  return std::static_pointer_cast<ValueNode>(left);
}

bool hasLiteralError(const std::string &literal) {
  for (const auto &error : AST::getParseErrors()) {
    if (error.type == LangError::InvalidLiteral &&
        error.errorTokens.at(0) == literal) {
      return true;
    }
  }
  return false;
}

} // namespace

TEST(Literals, IntegerSuffixes) {
  auto tree =
      AST::parseFile(TESTS_SOURCE_DIR "literals/integer_suffixes.stride");
  ASSERT_NE(tree, nullptr);

  auto value = streamedValue(tree, 0);
  EXPECT_EQ(value->getNodeType(), AST::Int);
  EXPECT_EQ(value->getIntValue(), 255);
  EXPECT_EQ(value->getLiteralSuffix(), "u8");
  EXPECT_EQ(value->getLiteralType(), "UInt8");

  value = streamedValue(tree, 1);
  EXPECT_EQ(value->getIntValue(), -128);
  EXPECT_EQ(value->getLiteralType(), "Int8");

  EXPECT_EQ(streamedValue(tree, 2)->getIntValue(), 0x1F);
  EXPECT_EQ(streamedValue(tree, 3)->getIntValue(), 0x0F);
  EXPECT_EQ(streamedValue(tree, 4)->getIntValue(), 65535);
  EXPECT_EQ(streamedValue(tree, 4)->getLiteralType(), "UInt16");
  EXPECT_EQ(streamedValue(tree, 5)->getLiteralType(), "UInt");
  EXPECT_EQ(streamedValue(tree, 6)->getLiteralType(), "Size");

  value = streamedValue(tree, 7);
  EXPECT_EQ(static_cast<uint64_t>(value->getIntValue()), UINT64_MAX);
  EXPECT_EQ(value->toString(), "18446744073709551615");

  EXPECT_EQ(streamedValue(tree, 8)->getIntValue(), 9600000000LL);
}

TEST(Literals, FloatSuffixes) {
  auto tree = AST::parseFile(TESTS_SOURCE_DIR "literals/float_suffixes.stride");
  ASSERT_NE(tree, nullptr);

  auto value = streamedValue(tree, 0);
  EXPECT_EQ(value->getNodeType(), AST::Real);
  EXPECT_DOUBLE_EQ(value->getRealValue(), 0.5);
  EXPECT_EQ(value->getLiteralType(), "Float32");

  // A float suffix makes an integer a real
  value = streamedValue(tree, 1);
  EXPECT_EQ(value->getNodeType(), AST::Real);
  EXPECT_DOUBLE_EQ(value->getRealValue(), 2.0);
  EXPECT_EQ(value->getLiteralType(), "Float64");

  EXPECT_DOUBLE_EQ(streamedValue(tree, 2)->getRealValue(), 1e-3);
  EXPECT_DOUBLE_EQ(streamedValue(tree, 3)->getRealValue(), -1.5);
  EXPECT_EQ(streamedValue(tree, 3)->getLiteralType(), "Float64");
}

TEST(Literals, Untyped) {
  auto tree = AST::parseFile(TESTS_SOURCE_DIR "literals/untyped.stride");
  ASSERT_NE(tree, nullptr);

  EXPECT_EQ(streamedValue(tree, 0)->getIntValue(), 42);
  EXPECT_EQ(streamedValue(tree, 0)->getLiteralSuffix(), "");
  EXPECT_EQ(streamedValue(tree, 1)->getIntValue(), 1000);
  EXPECT_EQ(streamedValue(tree, 2)->getNodeType(), AST::Real);
  EXPECT_DOUBLE_EQ(streamedValue(tree, 2)->getRealValue(), 1000.0);
  EXPECT_EQ(streamedValue(tree, 3)->getIntValue(), 0x1f32);
  EXPECT_EQ(streamedValue(tree, 3)->getLiteralSuffix(), "");
  EXPECT_EQ(streamedValue(tree, 4)->getIntValue(), 0xFFFF);
}

TEST(Literals, InList) {
  auto tree = AST::parseFile(TESTS_SOURCE_DIR "literals/in_list.stride");
  ASSERT_NE(tree, nullptr);
  auto stream =
      std::static_pointer_cast<StreamNode>(tree->getChildren().at(0));
  auto list = stream->getLeft();
  ASSERT_EQ(list->getNodeType(), AST::List);
  ASSERT_EQ(list->getChildren().size(), 3u);
  auto literalType = [&](size_t index) {
    return std::static_pointer_cast<ValueNode>(list->getChildren().at(index))
        ->getLiteralType();
  };
  EXPECT_EQ(literalType(0), "UInt8");
  EXPECT_EQ(literalType(1), "");
  EXPECT_EQ(literalType(2), "Float32");
}

TEST(Literals, OutOfRange) {
  EXPECT_EQ(AST::parseFile(TESTS_SOURCE_DIR "literals/out_of_range.stride"),
            nullptr);
  EXPECT_TRUE(hasLiteralError("300u8"));
  EXPECT_TRUE(hasLiteralError("128i8"));
  EXPECT_TRUE(hasLiteralError("-129i8"));
  EXPECT_TRUE(hasLiteralError("-1u8"));
  EXPECT_TRUE(hasLiteralError("99999999999999999999u64"));
  EXPECT_TRUE(hasLiteralError("9223372036854775808i64"));
  // Only the six literals that don't fit
  EXPECT_EQ(AST::getParseErrors().size(), 6u);
}

TEST(Literals, InvalidSuffixes) {
  EXPECT_EQ(
      AST::parseFile(TESTS_SOURCE_DIR "literals/real_with_int_suffix.stride"),
      nullptr);
  EXPECT_EQ(AST::parseFile(TESTS_SOURCE_DIR "literals/unknown_width.stride"),
            nullptr);
}

TEST(Literals, ToText) {
  ValueNode byte((int64_t)255, "test", 1);
  byte.setLiteralSuffix("u8");
  EXPECT_EQ(byte.toText(0, 2, false), "255u8");

  ValueNode big((int64_t)-1, "test", 1);
  big.setLiteralSuffix("u64");
  EXPECT_EQ(big.toText(0, 2, false), "18446744073709551615u64");

  ValueNode single(0.5, "test", 1);
  single.setLiteralSuffix("f32");
  EXPECT_EQ(single.toText(0, 2, false), "0.500000f32");

  auto copy = std::static_pointer_cast<ValueNode>(byte.deepCopy());
  EXPECT_EQ(copy->getLiteralSuffix(), "u8");
}
