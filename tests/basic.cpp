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

/*
TEST(Basic, ModuleDomains) {
    ASTNode tree;
    tree = AST::parseFile(BUILDPATH
                                   "/tests/data/12_modules_domains.stride");
    EXPECT_TRUE(tree != nullptr);
    CodeResolver resolver(tree, STRIDEROOT);
    resolver.process();
    CodeValidator validator(tree, CodeValidator::NO_RATE_VALIDATION);
    EXPECT_TRUE(validator.isValid());

    auto block =
        std::static_pointer_cast<DeclarationNode>(tree->getChildren()[1]);
    EXPECT_TRUE(block->getNodeType() == AST::Declaration);

    // Both the input and output blocks should be autodeclared to belong to the
    // output domain
    ListNode *blockList =
        static_cast<ListNode *>(block->getPropertyValue("blocks").get());
    EXPECT_TRUE(blockList->getNodeType() == AST::List);
    DeclarationNode *internalBlock =
        ASTQuery::findDeclarationByName(
            std::string("Input"), {{block, blockList->getChildren()}}, nullptr)
            .get();
    EXPECT_TRUE(internalBlock->getNodeType() == AST::Declaration);
    auto domainValue = std::static_pointer_cast<PortPropertyNode>(
        internalBlock->getPropertyValue("domain"));
    EXPECT_TRUE(domainValue);
    EXPECT_TRUE(domainValue->getNodeType() == AST::PortProperty);
    EXPECT_TRUE(domainValue->getName() == "OutputPort");
    EXPECT_TRUE(domainValue->getPortName() == "domain");

    auto reads = internalBlock->getCompilerProperty("reads")->getChildren();
    auto writes = internalBlock->getCompilerProperty("writes")->getChildren();

    EXPECT_TRUE(reads.size() == 1);
    EXPECT_TRUE(writes.size() == 0);

    auto readDomain = std::static_pointer_cast<PortPropertyNode>(reads.at(0));
    EXPECT_TRUE(readDomain->getNodeType() == AST::PortProperty);
    EXPECT_TRUE(readDomain->getPortName() == "domain");
    EXPECT_TRUE(readDomain->getName() == "OutputPort");

    internalBlock =
        ASTQuery::findDeclarationByName(
            std::string("Output"), {{block, blockList->getChildren()}}, nullptr)
            .get();
    EXPECT_TRUE(internalBlock->getNodeType() == AST::Declaration);
    domainValue = std::static_pointer_cast<PortPropertyNode>(
        internalBlock->getPropertyValue("domain"));
    EXPECT_TRUE(domainValue);
    EXPECT_TRUE(domainValue->getNodeType() == AST::PortProperty);
    EXPECT_TRUE(domainValue->getName() == "OutputPort");
    EXPECT_TRUE(domainValue->getPortName() == "domain");

    reads = internalBlock->getCompilerProperty("reads")->getChildren();
    writes = internalBlock->getCompilerProperty("writes")->getChildren();

    EXPECT_TRUE(reads.size() == 0);
    EXPECT_TRUE(writes.size() == 1);

    auto writeDomain = std::static_pointer_cast<PortPropertyNode>(writes.at(0));
    EXPECT_TRUE(writeDomain->getNodeType() == AST::PortProperty);
    EXPECT_TRUE(writeDomain->getPortName() == "domain");
    EXPECT_TRUE(writeDomain->getName() == "OutputPort");

    //    ListNode *streamList = static_cast<ListNode
    //    *>(block->getPropertyValue("streams").get()); ValueNode *constant =
    //    static_cast<ValueNode
    //    *>(streamList->getChildren()[0]->getChildren()[0].get());
    //    EXPECT_TRUE(constant->getNodeType() == AST::Int);
    //    EXPECT_TRUE(constant->getDomain());
    //    PortPropertyNode *domain = static_cast<PortPropertyNode
    //    *>(constant->getDomain().get()); EXPECT_TRUE(domain->getNodeType() ==
    //    AST::PortProperty); EXPECT_TRUE(domain->getPortName() == "domain");
    //    EXPECT_TRUE(domain->getName() == "OutputPort");
}

void ParserTest::testModules() {
    ASTNode tree;
    tree = AST::parseFile(TESTS_SOURCE_DIR "basic/11_modules.stride");
    EXPECT_TRUE(tree != nullptr);
    CodeResolver resolver(tree, STRIDEROOT);
    resolver.process();
    CodeValidator validator(tree, CodeValidator::NO_RATE_VALIDATION);
    EXPECT_TRUE(validator.isValid());

    auto moduleNode =
        std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(1));
    EXPECT_TRUE(moduleNode->getName() == "SizeTest");
    ListNode *blockList =
        static_cast<ListNode *>(moduleNode->getPropertyValue("blocks").get());
    EXPECT_TRUE(blockList->getNodeType() == AST::List);
    for (size_t i = 0; i < blockList->getChildren().size(); i++) {
        AST *member = blockList->getChildren().at(i).get();
        if (member->getNodeType() == AST::Declaration) {
            DeclarationNode *block = static_cast<DeclarationNode *>(member);
            if (block->getObjectType() == "constant") {
            }
        }
    }

    moduleNode =
        std::static_pointer_cast<DeclarationNode>(tree->getChildren().at(2));
    EXPECT_TRUE(moduleNode->getName() == "BlocksTest");
    blockList =
        static_cast<ListNode *>(moduleNode->getPropertyValue("blocks").get());
    EXPECT_TRUE(blockList->getNodeType() == AST::List);
    QStringList blockNames;
    blockNames << "Test";
    blockNames << "Input";
    blockNames << "Output";
    blockNames << "AutoDeclared";
    for (auto name : blockNames) {
        auto decl = ASTQuery::findDeclarationByName(
            name.toStdString(), {{moduleNode, blockList->getChildren()}},
nullptr); EXPECT_TRUE(decl);
    }
    // Check to make sure input and output domains have propagated correctly
    for (auto blockNode : blockList->getChildren()) {
        DeclarationNode *block = static_cast<DeclarationNode
*>(blockNode.get()); EXPECT_TRUE(block->getDomain());
        EXPECT_TRUE(block->getDomain()->getNodeType() == AST::PortProperty);
        auto domain = static_cast<PortPropertyNode *>(block->getDomain().get());
        if (block->getName() == "Input") {
            EXPECT_TRUE(domain->getPortName() == "domain");
            EXPECT_TRUE(domain->getName() == "OutputPort");
        } else {
            EXPECT_TRUE(domain->getPortName() == "domain");
            EXPECT_TRUE(domain->getName() == "OutputPort");
        }
    }
}*/

TEST(Basic, Lists) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/09_lists.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();

  //    constant List_Integer [4] {
  //            value: [ 16, 32, 64, 128 ]
  //    }
  DeclarationNode *block = static_cast<DeclarationNode *>(nodes.at(1).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  std::vector<std::shared_ptr<PropertyNode>> props = block->getProperties();
  ListNode *list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  std::vector<ASTNode> members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode member : members) {
    ValueNode *value = static_cast<ValueNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Int);
  }
  //    constant List_Real [4] {
  //            value: [ 16., 32.1, 64., 128. ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(2).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode member : members) {
    ValueNode *value = static_cast<ValueNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Real);
  }

  //    constant List_Strings [4] {
  //            value: [ '16', "32.1", '64', "128" ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(3).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode member : members) {
    ValueNode *value = static_cast<ValueNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::String);
  }

  //    constant List_Switches [4] {
  //            value: [ on, off, on, on ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(4).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode &member : members) {
    ValueNode *value = static_cast<ValueNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Switch);
  }

  //    constant List_Names [4] {
  //            value: [ Name1, Name2, Name3, Name4 ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(5).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode &member : members) {
    BlockNode *value = static_cast<BlockNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Bundle);
  }

  //    constant List_Namespaces [4] {
  //            value: [ ns.Name1, ns.Name2, ns.Name3, ns.Name4 ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(6).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode &member : members) {
    BlockNode *value = static_cast<BlockNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Block);
    EXPECT_TRUE(value->getScopeLevels() == 1);
    EXPECT_TRUE(value->getScopeAt(0) == "Ns");
  }

  //    block BlockName {
  //    property: [ blockType1 BlockName2 { property: "value" },
  //                blockType1 BlockName3 { value: 1.0 } ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(7).get());
  EXPECT_TRUE(block->getNodeType() == AST::Declaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 2);
  DeclarationNode *internalBlock =
      static_cast<DeclarationNode *>(members[0].get());
  EXPECT_TRUE(internalBlock->getNodeType() == AST::Declaration);
  EXPECT_TRUE(internalBlock->getObjectType() == "blockType1");
  EXPECT_TRUE(internalBlock->getName() == "BlockName2");

  internalBlock = static_cast<DeclarationNode *>(members[1].get());
  EXPECT_TRUE(internalBlock->getNodeType() == AST::Declaration);
  EXPECT_TRUE(internalBlock->getObjectType() == "blockType2");
  EXPECT_TRUE(internalBlock->getName() == "BlockName3");

  //    constant IntegerList [3] {
  //            value: [[ 9, 8, 7 ] , [ 6, 5, 4 ] , [ 3, 2, 1 ] ]
  //            meta:	'List of lists'
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(8).get());
  EXPECT_TRUE(block->getNodeType() == AST::BundleDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 3);
  EXPECT_TRUE(members.at(0)->getNodeType() == AST::List);
  EXPECT_TRUE(members.at(1)->getNodeType() == AST::List);
  EXPECT_TRUE(members.at(2)->getNodeType() == AST::List);

  //    [ In >> Out; OtherIn >> OtherOut;] >> [Out1, Out2];
  //    [ In >> Out; OtherIn >> OtherOut;] >> Out;
  //    Out >> [ In >> Out; OtherIn >> OtherOut;];

  StreamNode *stream = static_cast<StreamNode *>(nodes.at(9).get());
  EXPECT_TRUE(stream->getNodeType() == AST::Stream);
  list = static_cast<ListNode *>(stream->getLeft().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  EXPECT_TRUE(list->getChildren().size() == 2);
  EXPECT_TRUE(list->getChildren()[0]->getNodeType() == AST::Stream);
  EXPECT_TRUE(list->getChildren()[1]->getNodeType() == AST::Stream);
  //  stream = static_cast<StreamNode *>(nodes.at(10).get());
  //  list = static_cast<ListNode *>(stream->getLeft().get());
  //  EXPECT_TRUE(list->getNodeType() == AST::List);
  //  EXPECT_TRUE(list->getChildren().size() == 2);
  //  EXPECT_TRUE(list->getChildren()[0]->getNodeType() == AST::Stream);
  //  EXPECT_TRUE(list->getChildren()[1]->getNodeType() == AST::Stream);
  stream = static_cast<StreamNode *>(nodes.at(10).get());
  list = static_cast<ListNode *>(stream->getRight().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  EXPECT_TRUE(list->getChildren().size() == 2);
  EXPECT_TRUE(list->getChildren()[0]->getNodeType() == AST::Stream);
  EXPECT_TRUE(list->getChildren()[1]->getNodeType() == AST::Stream);
}

TEST(Basic, NoneSwitch) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/06_basic_noneswitch.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  EXPECT_TRUE(nodes.size() == 2);

  EXPECT_TRUE(nodes.at(0)->getNodeType() == AST::Declaration);
  DeclarationNode *block = static_cast<DeclarationNode *>(nodes.at(0).get());
  EXPECT_TRUE(block->getObjectType() == "object");
  std::vector<std::shared_ptr<PropertyNode>> properties =
      block->getProperties();
  EXPECT_TRUE(properties.size() == 3);
  EXPECT_TRUE(properties.at(0)->getName() == "prop1");
  ValueNode *value =
      static_cast<ValueNode *>(properties.at(0)->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Switch);
  EXPECT_TRUE(value->getSwitchValue() == true);
  EXPECT_TRUE(properties.at(1)->getName() == "prop2");
  value = static_cast<ValueNode *>(properties.at(1)->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Switch);
  EXPECT_TRUE(value->getSwitchValue() == false);
  EXPECT_TRUE(properties.at(2)->getName() == "prop3");
  value = static_cast<ValueNode *>(properties.at(2)->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::None);

  EXPECT_TRUE(nodes.at(1)->getNodeType() == AST::Stream);
  StreamNode *stream = static_cast<StreamNode *>(nodes.at(1).get());
  FunctionNode *func = static_cast<FunctionNode *>(stream->getLeft().get());
  EXPECT_TRUE(func->getNodeType() == AST::Function);
  std::vector<std::shared_ptr<PropertyNode>> funcProperties =
      func->getProperties();
  EXPECT_TRUE(funcProperties.size() == 3);
  EXPECT_TRUE(funcProperties.at(0)->getName() == "propf1");
  value = static_cast<ValueNode *>(funcProperties.at(0)->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Switch);
  EXPECT_TRUE(value->getSwitchValue() == true);
  EXPECT_TRUE(funcProperties.at(1)->getName() == "propf2");
  value = static_cast<ValueNode *>(funcProperties.at(1)->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::Switch);
  EXPECT_TRUE(value->getSwitchValue() == false);
  EXPECT_TRUE(funcProperties.at(2)->getName() == "propf3");
  value = static_cast<ValueNode *>(funcProperties.at(2)->getValue().get());
  EXPECT_TRUE(value->getNodeType() == AST::None);
}

TEST(Basic, Header) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/01_header.stride");

  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  SystemNode *node = static_cast<SystemNode *>(nodes.at(0).get());
  EXPECT_TRUE(node->getNodeType() == AST::Platform);
  EXPECT_TRUE(node->platformName() == "PufferFish");
  EXPECT_TRUE(node->majorVersion() == 1);
  EXPECT_TRUE(node->minorVersion() == 1);
  EXPECT_TRUE(node->getChildren().size() == 0);
  EXPECT_EQ(node->getLine(), 1);

  node = static_cast<SystemNode *>(nodes.at(1).get());
  EXPECT_TRUE(node->getNodeType() == AST::Platform);
  EXPECT_TRUE(node->platformName() == "Gamma");
  EXPECT_TRUE(node->majorVersion() == -1);
  EXPECT_TRUE(node->minorVersion() == -1);
  EXPECT_TRUE(node->getChildren().size() == 0);
  EXPECT_TRUE(node->getLine() == 3);

  ImportNode *importnode = static_cast<ImportNode *>(nodes.at(2).get());
  EXPECT_TRUE(importnode->getNodeType() == AST::Import);
  EXPECT_TRUE(importnode->importName() == "File");
  EXPECT_TRUE(importnode->importAlias() == "");
  EXPECT_TRUE(importnode->getLine() == 5);

  importnode = static_cast<ImportNode *>(nodes.at(3).get());
  EXPECT_TRUE(importnode->getNodeType() == AST::Import);
  EXPECT_TRUE(importnode->importName() == "File");
  EXPECT_TRUE(importnode->importAlias() == "F");
  EXPECT_TRUE(importnode->getLine() == 6);
}
