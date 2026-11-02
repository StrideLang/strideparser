# StrideDocument & StrideReader: High-Performance Data Ingestion

The Stride parser library provides two high-performance tiers for parsing and querying Stride data payloads (configurations, asset files, game saves, and network messages) without the overhead of the compiler semantic AST:

1. **`StrideReader`**: A zero-allocation, streaming SAX event parser achieving **150–350+ MB/s**.
2. **`StrideDocument`**: A contiguous, zero-copy Document Object Model (DOM) achieving **70–150+ MB/s** with sub-microsecond property lookups and $O(1)$ random-access array indexing.

---

## 1. When to Use Which Parser Tier?

| Tier | Output Representation | Heap Allocations | Throughput | Best For |
|---|---|---|---|---|
| **`AST::parseFile`** | Full polymorphic compiler AST (`ASTNode`, `EntityNode`, scopes, packages) | 1 per node (~300,000 for large datasets) | ~2–5 MB/s | Compiling `.stride` code to LLVM IR / JIT execution |
| **`StrideDocument`** | Contiguous flat DOM with `std::string_view` values | 1–3 buffer growths total | ~70–150 MB/s | Asset loading, game config, inspected structured data |
| **`StrideReader`** | Streaming SAX callbacks direct to consumer | **Zero heap allocations** | **150–350+ MB/s** | Direct deserialization into target C++ structs, streaming loaders |

---

## 2. Using `StrideDocument` (Zero-Copy DOM)

`StrideDocument` parses Stride data directly from a string or file into flat contiguous memory vectors. String literals and identifiers point directly into the source buffer without heap allocation.

### 2.1 Basic Usage

```cpp
#include "stride/parser/stridedocument.h"
#include <iostream>

using namespace strd;

int main() {
    std::string text = R"(
        gameConfig Config {
            gameTitle: "Starfleet Command"
            maxPlayers: 8
            allowSpectators: on
            difficultyMultiplier: 1.25
            resolution: [ 2560, 1440 ]
        }
    )";

    StrideDocument doc;
    if (!doc.parse(text)) {
        std::cerr << "Parse error: " << doc.getError() << "\n";
        return 1;
    }

    // Access root block
    StrideValue root = doc.root();
    std::cout << "Type: " << root.typeName() << ", Name: " << root.name() << "\n";

    // Direct property lookup by key
    std::string_view title = root["gameTitle"].asString();
    int64_t players        = root["maxPlayers"].asInt();
    bool spectators        = root["allowSpectators"].asBool();
    double difficulty      = root["difficultyMultiplier"].asReal();

    // List access and size
    StrideValue res = root["resolution"];
    std::cout << "Resolution: " << res[0].asInt() << "x" << res[1].asInt() << "\n";

    // Safe lookup with default fallback
    int64_t tickRate = root.get("tickRateHz", StrideValue{}).asInt(); // Returns 0 if missing

    return 0;
}
```

### 2.2 Loading Directly from Files

```cpp
StrideDocument doc;
if (!doc.parseFile("data/config.stride")) {
    std::cerr << "Failed to load: " << doc.getError() << "\n";
}
```

### 2.3 Iterating Lists and Properties

#### Iterating Lists:
```cpp
StrideValue units = root["units"];
for (StrideValue unit : units) {
    std::cout << "Unit: " << unit.name() 
              << ", Health: " << unit["health"].asInt() << "\n";
}
```

#### Iterating Properties:
```cpp
for (auto [key, val] : root.properties()) {
    std::cout << "Property: " << key << " (type " << static_cast<int>(val.type()) << ")\n";
}
```

### 2.4 Multi-Root Documents
Stride data files can contain multiple top-level blocks:

```cpp
for (size_t i = 0; i < doc.rootCount(); ++i) {
    StrideValue block = doc.root(i);
    std::cout << "Root #" << i << ": " << block.typeName() << " " << block.name() << "\n";
}
```

---

## 3. Using `StrideReader` (Streaming SAX Parser)

`StrideReader` performs single-pass scanning and tokenization with zero intermediate memory allocations, emitting callbacks directly to a `StrideHandler` interface.

### 3.1 Implementing a `StrideHandler`

```cpp
#include "stride/parser/stridereader.h"
#include <iostream>

using namespace strd;

class GameDataLoader : public StrideHandler {
public:
    void onBlockStart(std::string_view typeName, std::string_view name) override {
        std::cout << "Begin Block: " << typeName << " " << name << "\n";
    }

    void onBlockEnd() override {
        std::cout << "End Block\n";
    }

    void onProperty(std::string_view key) override {
        std::cout << "  Prop Key: " << key << "\n";
    }

    void onString(std::string_view val) override {
        std::cout << "  String: " << val << "\n";
    }

    void onInt(int64_t val) override {
        std::cout << "  Integer: " << val << "\n";
    }

    void onReal(double val) override {
        std::cout << "  Real: " << val << "\n";
    }

    void onBool(bool val) override {
        std::cout << "  Boolean: " << (val ? "true" : "false") << "\n";
    }

    void onError(std::string_view msg, size_t line, size_t col) override {
        std::cerr << "Parse error at line " << line << ", col " << col << ": " << msg << "\n";
    }
};

int main() {
    std::string text = "gameUnit Unit_1 { id: 42 name: \"Hero\" }";
    GameDataLoader loader;
    StrideReader reader(text);
    
    if (reader.parse(loader)) {
        std::cout << "Parsing completed successfully!\n";
    }
    return 0;
}
```

### 3.2 Direct Struct Deserialization (Zero Intermediate Tree)

By implementing state tracking in a `StrideHandler`, data can be parsed directly into domain models (e.g. `std::vector<GameUnit>`) in a single pass without ever allocating an AST or DOM tree.

---

## 4. Supported Syntax

| Construct | Syntax Example | Notes |
|---|---|---|
| **Blocks** | `gameUnit Unit_1 { ... }` or `levelSection { ... }` | Supports named and anonymous blocks |
| **Properties** | `key: value` | Keys are unquoted identifiers |
| **Strings** | `"Hello"` or `'World'` | Double or single quotes with `\` escapes |
| **Integers** | `42`, `-10`, `0x2A` (hex), `0b1010` (binary) | Parsed via `std::from_chars` |
| **Reals** | `3.1415`, `-0.5`, `1.2e-3` | High precision floating-point |
| **Booleans** | `on`, `off`, `true`, `false` | Stride switches and standard booleans |
| **Lists** | `[ 1, 2, 3 ]` or `[ "a", "b", ]` | Trailing commas supported |
| **Comments** | `# This is a comment` | Ignored by scanner |

---

## 5. Standalone Header-Only Usage (Zero Dependencies)

Both `StrideReader` and `StrideDocument` are implemented as **100% standalone, header-only C++17 libraries**. They are completely decoupled from the rest of `strideparser`:

- **No AST / Semantic dependencies**: No `ASTNode`, `SymbolTable`, `CodeAnalysis`, `Scope`, or `LangError`.
- **No Lexer / Token dependencies**: `StrideReader` has its own lightweight internal single-pass tokenizer directly operating on `std::string_view`.
- **No Third-Party or LLVM dependencies**: Only standard C++17 library headers (`<string_view>`, `<charconv>`, `<vector>`, `<cstdint>`, `<fstream>`).
- **No Linking Required**: Class methods are defined inline, eliminating the need to compile or link `strideparser.lib` or any `.cpp` files.

### 5.1 Drop-In Options for External Projects

You can use them in any C++ project in either of two ways:

#### Option A: Umbrella Header (`stride_data.h`)
Copy `include/stride/parser/stride_data.h` into your project. Include it anywhere:
```cpp
#include "stride/parser/stride_data.h"
// Or simply #include "stride_data.h" if placed in your project's include root

int main() {
    strd::StrideDocument doc;
    doc.parse("gameConfig Config { maxPlayers: 8 }");
    std::cout << doc.root()["maxPlayers"].asInt() << "\n";
}
```

#### Option B: Individual Component Headers
Copy `stridereader.h` and/or `stridedocument.h` into your include directory:
```cpp
#include "stridereader.h"    // If you only need zero-allocation streaming SAX
#include "stridedocument.h"  // If you need the DOM tree and queries
```

Both headers use standard include guards and flexible `#if __has_include` checks, so they can be included across multiple translation units in large codebases without ODR violations or duplicate symbol errors.
