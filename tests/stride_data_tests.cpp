#include "gtest/gtest.h"
#include "stride/parser/stridereader.h"
#include "stride/parser/stridedocument.h"
#include <string>
#include <vector>

using namespace strd;

// -----------------------------------------------------------------------------
// StrideReader Unit Tests
// -----------------------------------------------------------------------------

class TestCaptureHandler : public StrideHandler {
public:
    std::vector<std::string> events;

    void onBlockStart(std::string_view typeName, std::string_view name) override {
        events.push_back("BLOCK_START:" + std::string(typeName) + ":" + std::string(name));
    }
    void onBlockEnd() override {
        events.push_back("BLOCK_END");
    }
    void onProperty(std::string_view key) override {
        events.push_back("PROP:" + std::string(key));
    }
    void onListStart() override {
        events.push_back("LIST_START");
    }
    void onListEnd() override {
        events.push_back("LIST_END");
    }
    void onString(std::string_view val) override {
        events.push_back("STR:" + std::string(val));
    }
    void onInt(int64_t val) override {
        events.push_back("INT:" + std::to_string(val));
    }
    void onReal(double val) override {
        events.push_back("REAL:" + std::to_string(val));
    }
    void onBool(bool val) override {
        events.push_back("BOOL:" + std::string(val ? "true" : "false"));
    }
    void onNone() override {
        events.push_back("NONE");
    }
    void onError(std::string_view errorMsg, size_t line, size_t col) override {
        events.push_back("ERROR:" + std::string(errorMsg));
    }
};

TEST(StrideReaderTest, BasicScalarsAndComments) {
    std::string text = R"(
        # A test configuration block
        gameConfig Settings {
            title: "Space Voyager"
            version: '2.0.1'
            maxPlayers: 16
            hexMask: 0x2A
            binMask: 0b1010
            gravity: -9.81
            hardcore: on
            fogOfWar: off
            optionalRule: none
        }
    )";

    TestCaptureHandler handler;
    StrideReader reader(text);
    EXPECT_TRUE(reader.parse(handler));
    EXPECT_FALSE(reader.hasError());

    EXPECT_EQ(handler.events[0], "BLOCK_START:gameConfig:Settings");
    EXPECT_EQ(handler.events[1], "PROP:title");
    EXPECT_EQ(handler.events[2], "STR:Space Voyager");
    EXPECT_EQ(handler.events[3], "PROP:version");
    EXPECT_EQ(handler.events[4], "STR:2.0.1");
    EXPECT_EQ(handler.events[5], "PROP:maxPlayers");
    EXPECT_EQ(handler.events[6], "INT:16");
    EXPECT_EQ(handler.events[7], "PROP:hexMask");
    EXPECT_EQ(handler.events[8], "INT:42");
    EXPECT_EQ(handler.events[9], "PROP:binMask");
    EXPECT_EQ(handler.events[10], "INT:10");
    EXPECT_EQ(handler.events[11], "PROP:gravity");
    EXPECT_EQ(handler.events[13], "PROP:hardcore");
    EXPECT_EQ(handler.events[14], "BOOL:true");
    EXPECT_EQ(handler.events[15], "PROP:fogOfWar");
    EXPECT_EQ(handler.events[16], "BOOL:false");
    EXPECT_EQ(handler.events[17], "PROP:optionalRule");
    EXPECT_EQ(handler.events[18], "NONE");
    EXPECT_EQ(handler.events.back(), "BLOCK_END");
}

TEST(StrideReaderTest, ListsAndTrailingCommas) {
    std::string text = R"(
        container Box {
            values: [ 10, 20, 30, ]
            emptyList: [ ]
            tags: [ "alpha", "beta" ]
        }
    )";

    TestCaptureHandler handler;
    StrideReader reader(text);
    EXPECT_TRUE(reader.parse(handler));
    EXPECT_FALSE(reader.hasError());
}

TEST(StrideReaderTest, SyntaxErrorDetection) {
    std::string unclosedBlock = "gameConfig MyConfig { title: 'Test'";
    TestCaptureHandler h1;
    StrideReader r1(unclosedBlock);
    EXPECT_FALSE(r1.parse(h1));
    EXPECT_TRUE(r1.hasError());

    std::string unclosedList = "gameConfig MyConfig { items: [ 1, 2, 3 }";
    TestCaptureHandler h2;
    StrideReader r2(unclosedList);
    EXPECT_FALSE(r2.parse(h2));
    EXPECT_TRUE(r2.hasError());
}

// -----------------------------------------------------------------------------
// StrideDocument Unit Tests
// -----------------------------------------------------------------------------

TEST(StrideDocumentTest, ParseSimpleConfigAndQueries) {
    std::string text = R"(
        gameConfig Config {
            gameTitle: "Starfleet Command"
            maxPlayers: 8
            tickRateHz: 60
            allowSpectators: on
            difficultyMultiplier: 1.25
            resolution: [ 2560, 1440 ]
        }
    )";

    StrideDocument doc;
    EXPECT_TRUE(doc.parse(text)) << doc.getError();
    EXPECT_FALSE(doc.hasError());
    EXPECT_EQ(doc.rootCount(), 1u);

    auto root = doc.root();
    EXPECT_TRUE(root.isBlock());
    EXPECT_EQ(root.typeName(), "gameConfig");
    EXPECT_EQ(root.name(), "Config");

    EXPECT_TRUE(root.hasProperty("gameTitle"));
    EXPECT_EQ(root["gameTitle"].asString(), "Starfleet Command");

    EXPECT_TRUE(root.hasProperty("maxPlayers"));
    EXPECT_EQ(root["maxPlayers"].asInt(), 8);

    EXPECT_TRUE(root.hasProperty("allowSpectators"));
    EXPECT_TRUE(root["allowSpectators"].asBool());

    EXPECT_TRUE(root.hasProperty("difficultyMultiplier"));
    EXPECT_DOUBLE_EQ(root["difficultyMultiplier"].asReal(), 1.25);

    auto res = root["resolution"];
    EXPECT_TRUE(res.isList());
    EXPECT_EQ(res.size(), 2u);
    EXPECT_EQ(res[0].asInt(), 2560);
    EXPECT_EQ(res[1].asInt(), 1440);

    // Non-existent property returns null/none value
    EXPECT_FALSE(root.hasProperty("nonExistent"));
    EXPECT_FALSE(root["nonExistent"]);
    EXPECT_EQ(root.get("nonExistent", root["maxPlayers"]).asInt(), 8);
}

TEST(StrideDocumentTest, NestedHierarchyTraversal) {
    std::string text = R"(
        levelSection RootLevel {
            depth: 0
            nested: levelSection Level_1 {
                depth: 1
                nested: levelSection Level_2 {
                    depth: 2
                }
            }
        }
    )";

    StrideDocument doc;
    EXPECT_TRUE(doc.parse(text)) << doc.getError();
    EXPECT_FALSE(doc.hasError());

    auto root = doc.root();
    EXPECT_EQ(root["depth"].asInt(), 0);
    EXPECT_EQ(root["nested"]["depth"].asInt(), 1);
    EXPECT_EQ(root["nested"]["nested"]["depth"].asInt(), 2);
}

TEST(StrideDocumentTest, ListOfBlocksAndIterators) {
    std::string text = R"(
        worldSector Sector_A {
            units: [
                gameUnit Warrior {
                    id: 101
                    role: "warrior"
                    health: 150
                    inventory: [ "sword", "shield" ]
                },
                gameUnit Mage {
                    id: 102
                    role: "mage"
                    health: 80
                    inventory: [ "staff" ]
                }
            ]
        }
    )";

    StrideDocument doc;
    EXPECT_TRUE(doc.parse(text)) << doc.getError();
    EXPECT_FALSE(doc.hasError());

    auto units = doc.root()["units"];
    EXPECT_TRUE(units.isList());
    EXPECT_EQ(units.size(), 2u);

    // Range-based for loop over list elements
    std::vector<int> ids;
    for (auto u : units) {
        EXPECT_TRUE(u.isBlock());
        ids.push_back(static_cast<int>(u["id"].asInt()));
    }
    EXPECT_EQ(ids, (std::vector<int>{101, 102}));

    // Array indexing and inner list checking
    auto warrior = units[0];
    EXPECT_EQ(warrior.typeName(), "gameUnit");
    EXPECT_EQ(warrior.name(), "Warrior");
    EXPECT_EQ(warrior["inventory"].size(), 2u);
    EXPECT_EQ(warrior["inventory"][0].asString(), "sword");
    EXPECT_EQ(warrior["inventory"][1].asString(), "shield");

    auto mage = units[1];
    EXPECT_EQ(mage.name(), "Mage");
    EXPECT_EQ(mage["inventory"].size(), 1u);
    EXPECT_EQ(mage["inventory"][0].asString(), "staff");
}

TEST(StrideDocumentTest, PropertyIteration) {
    std::string text = R"(
        dataRecord Item {
            id: 42
            name: "Excalibur"
            damage: 99.5
        }
    )";

    StrideDocument doc;
    EXPECT_TRUE(doc.parse(text)) << doc.getError();
    EXPECT_FALSE(doc.hasError());

    auto item = doc.root();
    std::vector<std::string> keys;
    for (auto [key, val] : item.properties()) {
        keys.push_back(std::string(key));
    }

    EXPECT_EQ(keys, (std::vector<std::string>{"id", "name", "damage"}));
}

TEST(StrideDocumentTest, MultiRootDocument) {
    std::string text = R"(
        gameConfig Config {
            title: "MultiRoot"
        }
        gameUnit Hero {
            name: "Arthur"
        }
    )";

    StrideDocument doc;
    EXPECT_TRUE(doc.parse(text)) << doc.getError();
    EXPECT_FALSE(doc.hasError());
    EXPECT_EQ(doc.rootCount(), 2u);

    EXPECT_EQ(doc.root(0).typeName(), "gameConfig");
    EXPECT_EQ(doc.root(0)["title"].asString(), "MultiRoot");

    EXPECT_EQ(doc.root(1).typeName(), "gameUnit");
    EXPECT_EQ(doc.root(1)["name"].asString(), "Arthur");
}
