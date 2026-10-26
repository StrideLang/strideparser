#include "gtest/gtest.h"

#include "stride/parser/arraynode.h"
#include "stride/parser/ast.h"
#include "stride/parser/declarationnode.h"
#include "stride/parser/entitynode.h"
#include "stride/parser/expressionnode.h"
#include "stride/parser/functionnode.h"
#include "stride/parser/generatorrangenode.h"
#include "stride/parser/importnode.h"
#include "stride/parser/packagenode.h"
#include "stride/parser/propertynode.h"
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
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
  std::vector<std::shared_ptr<PropertyNode>> props = block->getProperties();
  ListNode *list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  std::vector<ASTNode> members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (const ASTNode &member : members) {
    ValueNode *value = static_cast<ValueNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Int);
  }
  //    constant List_Real [4] {
  //            value: [ 16., 32.1, 64., 128. ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(2).get());
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
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
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
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
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode &member : members) {
    ValueNode *value = static_cast<ValueNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Switch);
  }

  // constant List_Mixed [4] {
  //     value: [ List_Integer[1], List_Real[2], List_Strings[3],
  //     List_Switches[4] ]
  // }
  block = static_cast<DeclarationNode *>(nodes.at(5).get());
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (const ASTNode &member : members) {
    EXPECT_TRUE(member->getNodeType() == AST::Array);
  }

  //    constant List_Namespaces [4] {
  //            value: [ ns.Name1, ns.Name2, ns.Name3, ns.Name4 ]
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(6).get());
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
  props = block->getProperties();
  list = static_cast<ListNode *>(props.at(0)->getValue().get());
  EXPECT_TRUE(list->getNodeType() == AST::List);
  members = list->getChildren();
  EXPECT_TRUE(members.size() == 4);
  for (ASTNode &member : members) {
    EntityNode *value = static_cast<EntityNode *>(member.get());
    EXPECT_TRUE(value->getNodeType() == AST::Entity);
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
  EXPECT_TRUE(internalBlock->getEntityType() == "blockType1");
  EXPECT_TRUE(internalBlock->getName() == "BlockName2");

  internalBlock = static_cast<DeclarationNode *>(members[1].get());
  EXPECT_TRUE(internalBlock->getNodeType() == AST::Declaration);
  EXPECT_TRUE(internalBlock->getEntityType() == "blockType2");
  EXPECT_TRUE(internalBlock->getName() == "BlockName3");

  //    constant IntegerList [3] {
  //            value: [[ 9, 8, 7 ] , [ 6, 5, 4 ] , [ 3, 2, 1 ] ]
  //            meta:	'List of lists'
  //    }
  block = static_cast<DeclarationNode *>(nodes.at(8).get());
  EXPECT_TRUE(block->getNodeType() == AST::ArrayDeclaration);
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
  {
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

  // Out >> [ 2,3,4,] >> Out;
  {
    StreamNode *stream = static_cast<StreamNode *>(nodes.at(11).get());
    EXPECT_TRUE(stream->getNodeType() == AST::Stream);
    list = static_cast<ListNode *>(stream->getLeft().get());
    EXPECT_TRUE(list->getNodeType() == AST::List);
    EXPECT_TRUE(list->getChildren().size() == 3);
    EXPECT_TRUE(list->getChildren()[0]->getNodeType() == AST::Int);
    EXPECT_TRUE(list->getChildren()[1]->getNodeType() == AST::Int);
    EXPECT_TRUE(list->getChildren()[2]->getNodeType() == AST::Int);
  }
}

TEST(Basic, NoneSwitch) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/06_basic_noneswitch.stride");
  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  EXPECT_TRUE(nodes.size() == 2);

  EXPECT_TRUE(nodes.at(0)->getNodeType() == AST::Declaration);
  DeclarationNode *block = static_cast<DeclarationNode *>(nodes.at(0).get());
  EXPECT_TRUE(block->getEntityType() == "object");
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

TEST(Basic, HexAndBinaryLiterals) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/22_hex_binary_literals.stride");

  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  // Node 0: Platform
  EXPECT_EQ(nodes.at(0)->getNodeType(), AST::Platform);

  // Node 1: 0x10 >> Out1 (0x10 == 16)
  auto stream1 = std::static_pointer_cast<StreamNode>(nodes.at(1));
  auto val1 = std::static_pointer_cast<ValueNode>(stream1->getLeft());
  EXPECT_EQ(val1->getNodeType(), AST::Int);
  EXPECT_EQ(val1->getIntValue(), 16);

  // Node 2: 0x1F >> Out2 (0x1F == 31)
  auto stream2 = std::static_pointer_cast<StreamNode>(nodes.at(2));
  auto val2 = std::static_pointer_cast<ValueNode>(stream2->getLeft());
  EXPECT_EQ(val2->getNodeType(), AST::Int);
  EXPECT_EQ(val2->getIntValue(), 31);

  // Node 3: 0X2A >> Out3 (0X2A == 42)
  auto stream3 = std::static_pointer_cast<StreamNode>(nodes.at(3));
  auto val3 = std::static_pointer_cast<ValueNode>(stream3->getLeft());
  EXPECT_EQ(val3->getNodeType(), AST::Int);
  EXPECT_EQ(val3->getIntValue(), 42);

  // Node 4: 0b1010 >> Out4 (0b1010 == 10)
  auto stream4 = std::static_pointer_cast<StreamNode>(nodes.at(4));
  auto val4 = std::static_pointer_cast<ValueNode>(stream4->getLeft());
  EXPECT_EQ(val4->getNodeType(), AST::Int);
  EXPECT_EQ(val4->getIntValue(), 10);

  // Node 5: 0b0 >> Out5 (0b0 == 0)
  auto stream5 = std::static_pointer_cast<StreamNode>(nodes.at(5));
  auto val5 = std::static_pointer_cast<ValueNode>(stream5->getLeft());
  EXPECT_EQ(val5->getNodeType(), AST::Int);
  EXPECT_EQ(val5->getIntValue(), 0);

  // Node 6: 0B11110000 >> Out6 (0B11110000 == 240)
  auto stream6 = std::static_pointer_cast<StreamNode>(nodes.at(6));
  auto val6 = std::static_pointer_cast<ValueNode>(stream6->getLeft());
  EXPECT_EQ(val6->getNodeType(), AST::Int);
  EXPECT_EQ(val6->getIntValue(), 240);

  // Node 7: HexBundle declaration
  auto hexBundleDecl = std::static_pointer_cast<DeclarationNode>(nodes.at(7));
  EXPECT_EQ(hexBundleDecl->getNodeType(), AST::ArrayDeclaration);
  auto hexProps = hexBundleDecl->getProperties();
  auto hexList = std::static_pointer_cast<ListNode>(hexProps.at(0)->getValue());
  auto hexMembers = hexList->getChildren();
  EXPECT_EQ(hexMembers.size(), 4);
  int64_t expectedHex[] = {1, 2, 4, 8};
  for (size_t i = 0; i < 4; ++i) {
    auto v = std::static_pointer_cast<ValueNode>(hexMembers[i]);
    EXPECT_EQ(v->getNodeType(), AST::Int);
    EXPECT_EQ(v->getIntValue(), expectedHex[i]);
  }

  // Node 8: BinBundle declaration
  auto binBundleDecl = std::static_pointer_cast<DeclarationNode>(nodes.at(8));
  EXPECT_EQ(binBundleDecl->getNodeType(), AST::ArrayDeclaration);
  auto binProps = binBundleDecl->getProperties();
  auto binList = std::static_pointer_cast<ListNode>(binProps.at(0)->getValue());
  auto binMembers = binList->getChildren();
  EXPECT_EQ(binMembers.size(), 4);
  int64_t expectedBin[] = {1, 2, 4, 8};
  for (size_t i = 0; i < 4; ++i) {
    auto v = std::static_pointer_cast<ValueNode>(binMembers[i]);
    EXPECT_EQ(v->getNodeType(), AST::Int);
    EXPECT_EQ(v->getIntValue(), expectedBin[i]);
  }

  // Node 9: HexBundle[0x02] >> Out7
  auto stream7 = std::static_pointer_cast<StreamNode>(nodes.at(9));
  auto bundle7 = std::static_pointer_cast<ArrayNode>(stream7->getLeft());
  auto idxList7 = bundle7->index();
  auto idxVal7 =
      std::static_pointer_cast<ValueNode>(idxList7->getChildren().at(0));
  EXPECT_EQ(idxVal7->getNodeType(), AST::Int);
  EXPECT_EQ(idxVal7->getIntValue(), 2);

  // Node 10: BinBundle[0b01] >> Out8
  auto stream8 = std::static_pointer_cast<StreamNode>(nodes.at(10));
  auto bundle8 = std::static_pointer_cast<ArrayNode>(stream8->getLeft());
  auto idxList8 = bundle8->index();
  auto idxVal8 =
      std::static_pointer_cast<ValueNode>(idxList8->getChildren().at(0));
  EXPECT_EQ(idxVal8->getNodeType(), AST::Int);
  EXPECT_EQ(idxVal8->getIntValue(), 1);
}

TEST(Basic, TrailingCommasInLists) {
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "basic/23_trailing_commas.stride");

  EXPECT_TRUE(tree != nullptr);
  std::vector<ASTNode> nodes = tree->getChildren();
  // Node 0: Platform
  EXPECT_EQ(nodes.at(0)->getNodeType(), AST::Platform);

  // Node 1: [ 1, 2, 3, ] >> Out1;
  auto stream1 = std::static_pointer_cast<StreamNode>(nodes.at(1));
  auto list1 = std::static_pointer_cast<ListNode>(stream1->getLeft());
  EXPECT_EQ(list1->getNodeType(), AST::List);
  EXPECT_EQ(list1->size(), 3);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(list1->getChildren()[0])
                ->getIntValue(),
            1);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(list1->getChildren()[1])
                ->getIntValue(),
            2);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(list1->getChildren()[2])
                ->getIntValue(),
            3);

  // Node 2: [ 42, ] >> Out2;
  auto stream2 = std::static_pointer_cast<StreamNode>(nodes.at(2));
  auto list2 = std::static_pointer_cast<ListNode>(stream2->getLeft());
  EXPECT_EQ(list2->getNodeType(), AST::List);
  EXPECT_EQ(list2->size(), 1);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(list2->getChildren()[0])
                ->getIntValue(),
            42);

  // Node 3: [ [ 1, 2, ], [ 3, 4, ], ] >> Out3;
  auto stream3 = std::static_pointer_cast<StreamNode>(nodes.at(3));
  auto list3 = std::static_pointer_cast<ListNode>(stream3->getLeft());
  EXPECT_EQ(list3->getNodeType(), AST::List);
  EXPECT_EQ(list3->size(), 2);
  auto subList1 = std::static_pointer_cast<ListNode>(list3->getChildren()[0]);
  auto subList2 = std::static_pointer_cast<ListNode>(list3->getChildren()[1]);
  EXPECT_EQ(subList1->size(), 2);
  EXPECT_EQ(subList2->size(), 2);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(subList1->getChildren()[0])
                ->getIntValue(),
            1);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(subList1->getChildren()[1])
                ->getIntValue(),
            2);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(subList2->getChildren()[0])
                ->getIntValue(),
            3);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(subList2->getChildren()[1])
                ->getIntValue(),
            4);

  // Node 4: signal MySignal[4, ] { default: 0 }
  auto signalDecl = std::static_pointer_cast<DeclarationNode>(nodes.at(4));
  EXPECT_EQ(signalDecl->getNodeType(), AST::ArrayDeclaration);
  EXPECT_EQ(signalDecl->getName(), "MySignal");

  // Node 5: module TestModule
  auto moduleDecl = std::static_pointer_cast<DeclarationNode>(nodes.at(5));
  EXPECT_EQ(moduleDecl->getNodeType(), AST::Declaration);
  EXPECT_EQ(moduleDecl->getName(), "TestModule");
  auto portsList =
      std::static_pointer_cast<ListNode>(moduleDecl->getPropertyValue("ports"));
  EXPECT_TRUE(portsList != nullptr);
  EXPECT_EQ(portsList->size(), 2);
  auto blocksList = std::static_pointer_cast<ListNode>(
      moduleDecl->getPropertyValue("blocks"));
  EXPECT_TRUE(blocksList != nullptr);
  EXPECT_EQ(blocksList->size(), 2);
  auto streamsList = std::static_pointer_cast<ListNode>(
      moduleDecl->getPropertyValue("streams"));
  EXPECT_TRUE(streamsList != nullptr);
  EXPECT_EQ(streamsList->size(), 1);

  // Node 6: MyBundle[1, 2, ] >> Out4;
  auto stream6 = std::static_pointer_cast<StreamNode>(nodes.at(6));
  auto bundle6 = std::static_pointer_cast<ArrayNode>(stream6->getLeft());
  EXPECT_EQ(bundle6->getName(), "MyBundle");
  auto idx6 = bundle6->index();
  EXPECT_EQ(idx6->size(), 2);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(idx6->getChildren()[0])
                ->getIntValue(),
            1);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(idx6->getChildren()[1])
                ->getIntValue(),
            2);
}

TEST(Basic, PackageDeclaration) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR "basic/24_package.stride");
  ASSERT_TRUE(tree != nullptr);

  std::vector<ASTNode> nodes = tree->getChildren();
  ASSERT_EQ(nodes.size(), 1);

  // Node 0: package Audio::Filters
  ASSERT_EQ(nodes.at(0)->getNodeType(), AST::Package);
  auto pkg = std::static_pointer_cast<PackageNode>(nodes.at(0));
  EXPECT_EQ(pkg->packageName(), "Filters");
  EXPECT_EQ(pkg->fullPackageName(), "Audio::Filters");
  const auto &segs = pkg->packageSegments();
  ASSERT_EQ(segs.size(), 2);
  EXPECT_EQ(segs[0], "Audio");
  EXPECT_EQ(segs[1], "Filters");
}

TEST(Basic, GeneratorSyntax) {
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR "basic/25_generator.stride");
  ASSERT_TRUE(tree != nullptr);

  std::vector<ASTNode> nodes = tree->getChildren();
  ASSERT_EQ(nodes.size(), 3);

  // Node 0: transition PlaceMarker[Index in 0:8]
  ASSERT_EQ(nodes.at(0)->getNodeType(), AST::Declaration);
  auto decl0 = std::static_pointer_cast<DeclarationNode>(nodes.at(0));
  EXPECT_EQ(decl0->getName(), "PlaceMarker");
  EXPECT_EQ(decl0->getEntityType(), "transition");
  auto isGen0 = decl0->getCompilerProperty("_isGenerator");
  ASSERT_NE(isGen0, nullptr);
  EXPECT_TRUE(std::static_pointer_cast<ValueNode>(isGen0)->getSwitchValue());
  auto var0 = decl0->getCompilerProperty("_generatorVar");
  ASSERT_NE(var0, nullptr);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(var0)->getStringValue(),
            "Index");
  auto range0 = decl0->getCompilerProperty("_generatorRange");
  ASSERT_NE(range0, nullptr);
  ASSERT_EQ(range0->getNodeType(), AST::GeneratorRange);
  auto r0 = std::static_pointer_cast<GeneratorRangeNode>(range0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(r0->startIndex())->getIntValue(), 0);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(r0->endIndex())->getIntValue(),
            8);

  // Node 1: transition [Index in 1..4] (anonymous generator transition)
  ASSERT_EQ(nodes.at(1)->getNodeType(), AST::Declaration);
  auto decl1 = std::static_pointer_cast<DeclarationNode>(nodes.at(1));
  EXPECT_EQ(decl1->getEntityType(), "transition");
  auto anon1 = decl1->getCompilerProperty("anonymous");
  ASSERT_NE(anon1, nullptr);
  EXPECT_TRUE(std::static_pointer_cast<ValueNode>(anon1)->getSwitchValue());
  auto isGen1 = decl1->getCompilerProperty("_isGenerator");
  ASSERT_NE(isGen1, nullptr);
  EXPECT_TRUE(std::static_pointer_cast<ValueNode>(isGen1)->getSwitchValue());
  auto var1 = decl1->getCompilerProperty("_generatorVar");
  ASSERT_NE(var1, nullptr);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(var1)->getStringValue(),
            "Index");
  auto range1 = decl1->getCompilerProperty("_generatorRange");
  ASSERT_NE(range1, nullptr);
  ASSERT_EQ(range1->getNodeType(), AST::GeneratorRange);
  auto r1 = std::static_pointer_cast<GeneratorRangeNode>(range1);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(r1->startIndex())->getIntValue(), 1);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(r1->endIndex())->getIntValue(),
            4);

  // Node 2: action DoSomething[Item in [A, B, C]]
  ASSERT_EQ(nodes.at(2)->getNodeType(), AST::Declaration);
  auto decl2 = std::static_pointer_cast<DeclarationNode>(nodes.at(2));
  EXPECT_EQ(decl2->getName(), "DoSomething");
  EXPECT_EQ(decl2->getEntityType(), "action");
  auto isGen2 = decl2->getCompilerProperty("_isGenerator");
  ASSERT_NE(isGen2, nullptr);
  EXPECT_TRUE(std::static_pointer_cast<ValueNode>(isGen2)->getSwitchValue());
  auto var2 = decl2->getCompilerProperty("_generatorVar");
  ASSERT_NE(var2, nullptr);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(var2)->getStringValue(),
            "Item");
  auto range2 = decl2->getCompilerProperty("_generatorRange");
  ASSERT_NE(range2, nullptr);
  ASSERT_EQ(range2->getNodeType(), AST::List);
  EXPECT_EQ(range2->getChildren().size(), 3);
}

TEST(Basic, GeneratorRangeUsage) {
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "basic/26_generator_usage.stride");
  ASSERT_NE(tree, nullptr);
  const auto &nodes = tree->getChildren();
  ASSERT_EQ(nodes.size(), 10);

  // 1. Used in streams
  // Node 0: [0..8] >> StreamTarget1;
  ASSERT_EQ(nodes.at(0)->getNodeType(), AST::Stream);
  auto stream0 = std::static_pointer_cast<StreamNode>(nodes.at(0));
  ASSERT_EQ(stream0->getLeft()->getNodeType(), AST::List);
  auto list0 = std::static_pointer_cast<ListNode>(stream0->getLeft());
  ASSERT_EQ(list0->getChildren().size(), 1);
  ASSERT_EQ(list0->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto gen0 =
      std::static_pointer_cast<GeneratorRangeNode>(list0->getChildren().at(0));
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen0->startIndex())->getIntValue(),
      0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen0->endIndex())->getIntValue(), 8);
  EXPECT_EQ(gen0->stepIndex(), nullptr);
  ASSERT_EQ(stream0->getRight()->getNodeType(), AST::Entity);
  EXPECT_EQ(
      std::static_pointer_cast<EntityNode>(stream0->getRight())->getName(),
      "StreamTarget1");

  // Node 1: [1, 2..5, 10] >> StreamTarget2;
  ASSERT_EQ(nodes.at(1)->getNodeType(), AST::Stream);
  auto stream1 = std::static_pointer_cast<StreamNode>(nodes.at(1));
  ASSERT_EQ(stream1->getLeft()->getNodeType(), AST::List);
  auto list1 = std::static_pointer_cast<ListNode>(stream1->getLeft());
  ASSERT_EQ(list1->getChildren().size(), 3);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(list1->getChildren().at(0))
                ->getIntValue(),
            1);
  ASSERT_EQ(list1->getChildren().at(1)->getNodeType(), AST::GeneratorRange);
  auto gen1 =
      std::static_pointer_cast<GeneratorRangeNode>(list1->getChildren().at(1));
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen1->startIndex())->getIntValue(),
      2);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen1->endIndex())->getIntValue(), 5);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(list1->getChildren().at(2))
                ->getIntValue(),
            10);
  ASSERT_EQ(stream1->getRight()->getNodeType(), AST::Entity);
  EXPECT_EQ(
      std::static_pointer_cast<EntityNode>(stream1->getRight())->getName(),
      "StreamTarget2");

  // Node 2: StreamSource1 >> [0..8];
  ASSERT_EQ(nodes.at(2)->getNodeType(), AST::Stream);
  auto stream2 = std::static_pointer_cast<StreamNode>(nodes.at(2));
  EXPECT_EQ(std::static_pointer_cast<EntityNode>(stream2->getLeft())->getName(),
            "StreamSource1");
  ASSERT_EQ(stream2->getRight()->getNodeType(), AST::List);
  auto list2 = std::static_pointer_cast<ListNode>(stream2->getRight());
  ASSERT_EQ(list2->getChildren().size(), 1);
  ASSERT_EQ(list2->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto gen2 =
      std::static_pointer_cast<GeneratorRangeNode>(list2->getChildren().at(0));
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen2->startIndex())->getIntValue(),
      0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen2->endIndex())->getIntValue(), 8);

  // Node 3: StreamSource2 >> [0..8] >> StreamTarget3;
  ASSERT_EQ(nodes.at(3)->getNodeType(), AST::Stream);
  auto stream3 = std::static_pointer_cast<StreamNode>(nodes.at(3));
  EXPECT_EQ(std::static_pointer_cast<EntityNode>(stream3->getLeft())->getName(),
            "StreamSource2");
  ASSERT_EQ(stream3->getRight()->getNodeType(), AST::Stream);
  auto subStream3 = std::static_pointer_cast<StreamNode>(stream3->getRight());
  ASSERT_EQ(subStream3->getLeft()->getNodeType(), AST::List);
  auto list3 = std::static_pointer_cast<ListNode>(subStream3->getLeft());
  ASSERT_EQ(list3->getChildren().size(), 1);
  ASSERT_EQ(list3->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto gen3 =
      std::static_pointer_cast<GeneratorRangeNode>(list3->getChildren().at(0));
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen3->startIndex())->getIntValue(),
      0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(gen3->endIndex())->getIntValue(), 8);
  EXPECT_EQ(
      std::static_pointer_cast<EntityNode>(subStream3->getRight())->getName(),
      "StreamTarget3");

  // 2. Passed to properties in functions
  // Node 4: FuncInList(range: [0..8]);
  ASSERT_EQ(nodes.at(4)->getNodeType(), AST::Stream);
  auto stream4 = std::static_pointer_cast<StreamNode>(nodes.at(4));
  ASSERT_EQ(stream4->getLeft()->getNodeType(), AST::Function);
  auto func4 = std::static_pointer_cast<FunctionNode>(stream4->getLeft());
  EXPECT_EQ(func4->getName(), "FuncInList");
  ASSERT_EQ(func4->getProperties().size(), 1);
  EXPECT_EQ(func4->getProperties().at(0)->getName(), "range");
  ASSERT_EQ(func4->getProperties().at(0)->getValue()->getNodeType(), AST::List);
  auto funcList4 = std::static_pointer_cast<ListNode>(
      func4->getProperties().at(0)->getValue());
  ASSERT_EQ(funcList4->getChildren().size(), 1);
  ASSERT_EQ(funcList4->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto funcGen4 = std::static_pointer_cast<GeneratorRangeNode>(
      funcList4->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(funcGen4->startIndex())
                ->getIntValue(),
            0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(funcGen4->endIndex())->getIntValue(),
      8);

  // Node 5: FuncMultiple(first: [0..4] second: [10..20]);
  ASSERT_EQ(nodes.at(5)->getNodeType(), AST::Stream);
  auto stream5 = std::static_pointer_cast<StreamNode>(nodes.at(5));
  ASSERT_EQ(stream5->getLeft()->getNodeType(), AST::Function);
  auto func5 = std::static_pointer_cast<FunctionNode>(stream5->getLeft());
  EXPECT_EQ(func5->getName(), "FuncMultiple");
  ASSERT_EQ(func5->getProperties().size(), 2);
  EXPECT_EQ(func5->getProperties().at(0)->getName(), "first");
  ASSERT_EQ(func5->getProperties().at(0)->getValue()->getNodeType(), AST::List);
  auto listFirst = std::static_pointer_cast<ListNode>(
      func5->getProperties().at(0)->getValue());
  ASSERT_EQ(listFirst->getChildren().size(), 1);
  ASSERT_EQ(listFirst->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto genFirst = std::static_pointer_cast<GeneratorRangeNode>(
      listFirst->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(genFirst->startIndex())
                ->getIntValue(),
            0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(genFirst->endIndex())->getIntValue(),
      4);
  EXPECT_EQ(func5->getProperties().at(1)->getName(), "second");
  ASSERT_EQ(func5->getProperties().at(1)->getValue()->getNodeType(), AST::List);
  auto listSecond = std::static_pointer_cast<ListNode>(
      func5->getProperties().at(1)->getValue());
  ASSERT_EQ(listSecond->getChildren().size(), 1);
  ASSERT_EQ(listSecond->getChildren().at(0)->getNodeType(),
            AST::GeneratorRange);
  auto genSecond = std::static_pointer_cast<GeneratorRangeNode>(
      listSecond->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(genSecond->startIndex())
                ->getIntValue(),
            10);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(genSecond->endIndex())->getIntValue(),
      20);

  // Node 6: StreamSource3 >> FuncInStream(range: [0..8]) >> StreamTarget4;
  ASSERT_EQ(nodes.at(6)->getNodeType(), AST::Stream);
  auto stream6 = std::static_pointer_cast<StreamNode>(nodes.at(6));
  EXPECT_EQ(std::static_pointer_cast<EntityNode>(stream6->getLeft())->getName(),
            "StreamSource3");
  ASSERT_EQ(stream6->getRight()->getNodeType(), AST::Stream);
  auto subStream6 = std::static_pointer_cast<StreamNode>(stream6->getRight());
  ASSERT_EQ(subStream6->getLeft()->getNodeType(), AST::Function);
  auto func6 = std::static_pointer_cast<FunctionNode>(subStream6->getLeft());
  EXPECT_EQ(func6->getName(), "FuncInStream");
  ASSERT_EQ(func6->getProperties().size(), 1);
  EXPECT_EQ(func6->getProperties().at(0)->getName(), "range");
  ASSERT_EQ(func6->getProperties().at(0)->getValue()->getNodeType(), AST::List);
  auto funcList6 = std::static_pointer_cast<ListNode>(
      func6->getProperties().at(0)->getValue());
  ASSERT_EQ(funcList6->getChildren().size(), 1);
  ASSERT_EQ(funcList6->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto funcGen6 = std::static_pointer_cast<GeneratorRangeNode>(
      funcList6->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(funcGen6->startIndex())
                ->getIntValue(),
            0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(funcGen6->endIndex())->getIntValue(),
      8);
  EXPECT_EQ(
      std::static_pointer_cast<EntityNode>(subStream6->getRight())->getName(),
      "StreamTarget4");

  // 3. Passed to properties in declarations
  // Node 7: constant ConstListRange { value: [0..8] }
  ASSERT_EQ(nodes.at(7)->getNodeType(), AST::Declaration);
  auto decl7 = std::static_pointer_cast<DeclarationNode>(nodes.at(7));
  EXPECT_EQ(decl7->getName(), "ConstListRange");
  EXPECT_EQ(decl7->getEntityType(), "constant");
  auto valProp7 = decl7->getPropertyValue("value");
  ASSERT_NE(valProp7, nullptr);
  ASSERT_EQ(valProp7->getNodeType(), AST::List);
  auto list7 = std::static_pointer_cast<ListNode>(valProp7);
  ASSERT_EQ(list7->getChildren().size(), 1);
  ASSERT_EQ(list7->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto declGen7 =
      std::static_pointer_cast<GeneratorRangeNode>(list7->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(declGen7->startIndex())
                ->getIntValue(),
            0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(declGen7->endIndex())->getIntValue(),
      8);

  // Node 8: module TestModule { range: [0..8], listRange: [0..8] }
  ASSERT_EQ(nodes.at(8)->getNodeType(), AST::Declaration);
  auto decl8 = std::static_pointer_cast<DeclarationNode>(nodes.at(8));
  EXPECT_EQ(decl8->getName(), "TestModule");
  EXPECT_EQ(decl8->getEntityType(), "module");
  auto propRange8 = decl8->getPropertyValue("range");
  ASSERT_NE(propRange8, nullptr);
  ASSERT_EQ(propRange8->getNodeType(), AST::List);
  auto listRange8 = std::static_pointer_cast<ListNode>(propRange8);
  ASSERT_EQ(listRange8->getChildren().size(), 1);
  ASSERT_EQ(listRange8->getChildren().at(0)->getNodeType(),
            AST::GeneratorRange);
  auto declGen8 = std::static_pointer_cast<GeneratorRangeNode>(
      listRange8->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(declGen8->startIndex())
                ->getIntValue(),
            0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(declGen8->endIndex())->getIntValue(),
      8);

  auto propList8 = decl8->getPropertyValue("listRange");
  ASSERT_NE(propList8, nullptr);
  ASSERT_EQ(propList8->getNodeType(), AST::List);
  auto list8 = std::static_pointer_cast<ListNode>(propList8);
  ASSERT_EQ(list8->getChildren().size(), 1);
  ASSERT_EQ(list8->getChildren().at(0)->getNodeType(), AST::GeneratorRange);

  // Node 9: signal TestSignal[9] { default: [0..8] }
  ASSERT_EQ(nodes.at(9)->getNodeType(), AST::ArrayDeclaration);
  auto decl9 = std::static_pointer_cast<DeclarationNode>(nodes.at(9));
  EXPECT_EQ(decl9->getName(), "TestSignal");
  EXPECT_EQ(decl9->getEntityType(), "signal");
  auto propDefault9 = decl9->getPropertyValue("default");
  ASSERT_NE(propDefault9, nullptr);
  ASSERT_EQ(propDefault9->getNodeType(), AST::List);
  auto list9 = std::static_pointer_cast<ListNode>(propDefault9);
  ASSERT_EQ(list9->getChildren().size(), 1);
  ASSERT_EQ(list9->getChildren().at(0)->getNodeType(), AST::GeneratorRange);
  auto declGen9 =
      std::static_pointer_cast<GeneratorRangeNode>(list9->getChildren().at(0));
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(declGen9->startIndex())
                ->getIntValue(),
            0);
  EXPECT_EQ(
      std::static_pointer_cast<ValueNode>(declGen9->endIndex())->getIntValue(),
      8);
}

TEST(Basic, SourceLocationAndDualPaths) {
  const char *sourceRel = "custom_rel/02_basic_blocks.stride";
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR "basic/02_basic_blocks.stride", sourceRel);
  ASSERT_TRUE(tree != nullptr);

  EXPECT_EQ(tree->getRelativeFilename(), sourceRel);
  EXPECT_FALSE(tree->getAbsoluteFilename().empty());

  const auto &nodes = tree->getChildren();
  ASSERT_GE(nodes.size(), 2);

  // Node 0: signal GuitarIn
  auto decl0 = nodes.at(0);
  EXPECT_EQ(decl0->getRelativeFilename(), sourceRel);
  EXPECT_FALSE(decl0->getAbsoluteFilename().empty());
  EXPECT_EQ(decl0->getLine(), 1);
  EXPECT_EQ(decl0->getColumn(), 1);

  // Check children have valid lines and columns
  for (const auto &child : decl0->getChildren()) {
    EXPECT_EQ(child->getRelativeFilename(), sourceRel);
    EXPECT_GT(child->getLine(), 1);
    EXPECT_GT(child->getColumn(), 0);
  }

  // Node 1: constant Integer (starts on line 6)
  auto decl1 = nodes.at(1);
  EXPECT_EQ(decl1->getRelativeFilename(), sourceRel);
  EXPECT_EQ(decl1->getLine(), 6);
  EXPECT_EQ(decl1->getColumn(), 1);

  // Deep copy preserves location metadata
  auto copy0 = decl0->deepCopy();
  EXPECT_EQ(copy0->getRelativeFilename(), decl0->getRelativeFilename());
  EXPECT_EQ(copy0->getAbsoluteFilename(), decl0->getAbsoluteFilename());
  EXPECT_EQ(copy0->getLine(), decl0->getLine());
  EXPECT_EQ(copy0->getColumn(), decl0->getColumn());
}

// End positions and comments let tools edit source text without losing
// formatting or comments.
TEST(Basic, SourceRangesAndComments) {
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "basic/27_source_ranges.stride");
  ASSERT_TRUE(tree != nullptr);
  const auto &nodes = tree->getChildren();
  ASSERT_GE(nodes.size(), 2u);

  // signal In: lines 2 to 4, ending on the closing brace
  auto signalDecl = std::static_pointer_cast<DeclarationNode>(nodes.at(0));
  EXPECT_EQ(signalDecl->getLine(), 2);
  EXPECT_EQ(signalDecl->getEndLine(), 4);
  EXPECT_EQ(signalDecl->getEndColumn(), 1);
  auto defaultProp = signalDecl->getProperties().at(0);
  EXPECT_EQ(defaultProp->getLine(), 3);
  EXPECT_EQ(defaultProp->getEndLine(), 3);
  EXPECT_EQ(defaultProp->getEndColumn(), 14);

  // module M: lines 6 to 12; each stream ends on its own line
  auto moduleDecl = std::static_pointer_cast<DeclarationNode>(nodes.at(1));
  EXPECT_EQ(moduleDecl->getLine(), 6);
  EXPECT_EQ(moduleDecl->getEndLine(), 12);
  EXPECT_EQ(moduleDecl->getEndColumn(), 1);
  auto streams = moduleDecl->getPropertyValue("streams");
  ASSERT_NE(streams, nullptr);
  ASSERT_EQ(streams->getChildren().size(), 2u);
  EXPECT_EQ(streams->getChildren().at(0)->getLine(), 8);
  EXPECT_EQ(streams->getChildren().at(0)->getEndLine(), 8);
  EXPECT_GE(streams->getChildren().at(0)->getEndColumn(), 17);
  EXPECT_EQ(streams->getChildren().at(1)->getLine(), 10);
  EXPECT_EQ(streams->getChildren().at(1)->getEndLine(), 10);

  // A deep copy keeps the end position
  auto copy = signalDecl->deepCopy();
  EXPECT_EQ(copy->getEndLine(), 4);
  EXPECT_EQ(copy->getEndColumn(), 1);

  // Comments in source order, with the column of the '#'
  auto comments = AST::getParseComments();
  ASSERT_EQ(comments.size(), 4u);
  EXPECT_EQ(comments[0].line, 1);
  EXPECT_EQ(comments[0].column, 1);
  EXPECT_EQ(comments[0].text, " Leading comment");
  EXPECT_EQ(comments[1].line, 3);
  EXPECT_EQ(comments[1].column, 16);
  EXPECT_EQ(comments[1].text, " trailing comment");
  EXPECT_EQ(comments[2].line, 8);
  EXPECT_EQ(comments[2].column, 21);
  EXPECT_EQ(comments[2].text, " first stream");
  EXPECT_EQ(comments[3].line, 9);
  EXPECT_EQ(comments[3].column, 9);
  EXPECT_EQ(comments[3].text, " between streams");
}

