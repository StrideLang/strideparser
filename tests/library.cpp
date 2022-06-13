#include "gtest/gtest.h"

#include "stridelibrary.h"
#include "strideparser.h"

TEST(Library, Import) {
  StrideLibrary library;
  library.initializeLibrary("", {TESTS_SOURCE_DIR "library"});
  auto nodes = library.loadImport("Include", "Test");

  EXPECT_EQ(nodes.size(), 1);
  EXPECT_EQ(nodes[0]->getNodeType(), AST::Declaration);
  auto decl = std::static_pointer_cast<DeclarationNode>(nodes[0]);
  EXPECT_EQ(decl->getObjectType(), "module");
  EXPECT_EQ(decl->getName(), "LibTest");

  auto members = library.getLibraryMembers();

  EXPECT_EQ(members.size(), 1);
  EXPECT_TRUE(members.find("Test") != members.end());
  EXPECT_EQ(members["Test"][0], nodes[0]);
}
