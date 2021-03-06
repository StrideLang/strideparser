#include "gtest/gtest.h"

#include "ast.h"
#include "astfunctions.h"
#include "astquery.h"
#include "astvalidation.h"
#include "blocknode.h"
#include "bundlenode.h"
#include "declarationnode.h"
#include "expressionnode.h"
#include "functionnode.h"
#include "importnode.h"
#include "platformnode.h"
#include "rangenode.h"
#include "valuenode.h"

TEST(Schema, ObjectTypes) {
  auto strideroot = getenv("STRIDEROOT");
  EXPECT_NE(strideroot, nullptr);
  ASTNode tree;
  tree =
      ASTFunctions::parseFile(TESTS_SOURCE_DIR "schema/05_objectTypes.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);
  std::vector<LangError> errors;
  ASTValidation::validateTypes(tree, errors, {}, tree);

  EXPECT_EQ(errors.size(), 2);
}

TEST(Schema, IntLiterals) {
  auto strideroot = getenv("STRIDEROOT");
  EXPECT_NE(strideroot, nullptr);
  ASTNode tree;
  tree =
      ASTFunctions::parseFile(TESTS_SOURCE_DIR "schema/01_intliterals.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);
  std::vector<LangError> errors;
  ASTValidation::validateTypes(tree, errors, {}, tree);

  EXPECT_EQ(errors.size(), 10);
  EXPECT_EQ(errors[0].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[0].lineNumber, 66);
  EXPECT_EQ(errors[1].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[1].lineNumber, 67);
  EXPECT_EQ(errors[2].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[2].lineNumber, 68);
  EXPECT_EQ(errors[3].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[3].lineNumber, 72);
  EXPECT_EQ(errors[4].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[4].lineNumber, 73);
  EXPECT_EQ(errors[5].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[5].lineNumber, 74);
  EXPECT_EQ(errors[6].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[6].lineNumber, 78);
  EXPECT_EQ(errors[7].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[7].lineNumber, 79);
  EXPECT_EQ(errors[8].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[8].lineNumber, 80);
  EXPECT_EQ(errors[9].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[9].lineNumber, 81);
}

TEST(Schema, RealLiterals) {
  auto strideroot = getenv("STRIDEROOT");
  EXPECT_NE(strideroot, nullptr);
  ASTNode tree;
  tree =
      ASTFunctions::parseFile(TESTS_SOURCE_DIR "schema/02_realliterals.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);
  std::vector<LangError> errors;
  ASTValidation::validateTypes(tree, errors, {}, tree);

  EXPECT_EQ(errors.size(), 6);
  EXPECT_EQ(errors[0].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[0].lineNumber, 56);
  EXPECT_EQ(errors[1].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[1].lineNumber, 57);
  EXPECT_EQ(errors[2].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[2].lineNumber, 58);
  EXPECT_EQ(errors[3].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[3].lineNumber, 62);
  EXPECT_EQ(errors[4].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[4].lineNumber, 63);
  EXPECT_EQ(errors[5].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[5].lineNumber, 64);
}

TEST(Schema, StringLiterals) {
  auto strideroot = getenv("STRIDEROOT");
  EXPECT_NE(strideroot, nullptr);
  ASTNode tree;
  tree = ASTFunctions::parseFile(TESTS_SOURCE_DIR
                                 "schema/03_stringliterals.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);
  std::vector<LangError> errors;
  ASTValidation::validateTypes(tree, errors, {}, tree);

  EXPECT_EQ(errors.size(), 4);
  EXPECT_EQ(errors[0].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[0].lineNumber, 37);
  EXPECT_EQ(errors[1].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[1].lineNumber, 38);
  EXPECT_EQ(errors[2].type, LangError::InvalidPortType);
  EXPECT_EQ(errors[2].lineNumber, 42);
  EXPECT_EQ(errors[3].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[3].lineNumber, 43);
}

TEST(Schema, ConstrainedList) {
  auto strideroot = getenv("STRIDEROOT");
  EXPECT_NE(strideroot, nullptr);
  ASTNode tree;
  tree = ASTFunctions::parseFile(TESTS_SOURCE_DIR
                                 "schema/04_constrainedList.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);
  std::vector<LangError> errors;
  ASTValidation::validateTypes(tree, errors, {}, tree);

  EXPECT_EQ(errors.size(), 3);
  EXPECT_EQ(errors[0].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[0].lineNumber, 32);
  EXPECT_EQ(errors[1].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[1].lineNumber, 36);
  EXPECT_EQ(errors[2].type, LangError::ConstraintFail);
  EXPECT_EQ(errors[2].lineNumber, 40);
}
