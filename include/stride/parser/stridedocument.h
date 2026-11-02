#ifndef STRIDEDOCUMENT_H
#define STRIDEDOCUMENT_H

#if __has_include("stride/parser/stridereader.h")
#include "stride/parser/stridereader.h"
#else
#include "stridereader.h"
#endif
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <fstream>

namespace strd {

enum class StrideType : uint8_t {
    None,
    Block,
    List,
    String,
    Int,
    Real,
    Bool
};

class StrideDocument;
struct StrideProperty;

/**
 * @brief Lightweight 16-byte value handle pointing into a StrideDocument.
 */
class StrideValue {
public:
    StrideValue() = default;
    StrideValue(const StrideDocument* doc, uint32_t index);

    // Type queries
    StrideType type() const;
    bool isBlock() const { return type() == StrideType::Block; }
    bool isList() const { return type() == StrideType::List; }
    bool isString() const { return type() == StrideType::String; }
    bool isInt() const { return type() == StrideType::Int; }
    bool isReal() const { return type() == StrideType::Real; }
    bool isNumber() const { return isInt() || isReal(); }
    bool isBool() const { return type() == StrideType::Bool; }
    bool isNone() const { return type() == StrideType::None; }
    explicit operator bool() const { return !isNone(); }

    // Block properties
    std::string_view typeName() const;
    std::string_view name() const;

    // Value accessors
    std::string_view asString() const;
    int64_t asInt() const;
    double asReal() const;
    double asDouble() const { return asReal(); }
    bool asBool() const;

    // Container size
    size_t size() const;
    bool empty() const { return size() == 0; }

    // Block property access
    bool hasProperty(std::string_view key) const;
    StrideValue operator[](std::string_view key) const;
    StrideValue get(std::string_view key, StrideValue defaultVal = {}) const;

    // List indexing
    StrideValue operator[](size_t index) const;

    // List iterator support
    class ListIterator {
    public:
        using value_type = StrideValue;
        using difference_type = std::ptrdiff_t;
        using pointer = const StrideValue*;
        using reference = StrideValue;
        using iterator_category = std::forward_iterator_tag;

        ListIterator(const StrideDocument* doc, uint32_t elementOffset, uint32_t remaining)
            : m_doc(doc), m_offset(elementOffset), m_remaining(remaining) {}

        StrideValue operator*() const;
        ListIterator& operator++() {
            if (m_remaining > 0) {
                m_offset++;
                m_remaining--;
            }
            return *this;
        }
        bool operator==(const ListIterator& other) const {
            return m_remaining == other.m_remaining && m_offset == other.m_offset;
        }
        bool operator!=(const ListIterator& other) const {
            return !(*this == other);
        }

    private:
        const StrideDocument* m_doc{nullptr};
        uint32_t m_offset{0};
        uint32_t m_remaining{0};
    };

    ListIterator begin() const;
    ListIterator end() const;

    // Property view helper for range-for loops
    class PropertyIterator {
    public:
        using value_type = StrideProperty;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        PropertyIterator(const StrideDocument* doc, uint32_t propOffset, uint32_t remaining)
            : m_doc(doc), m_offset(propOffset), m_remaining(remaining) {}

        StrideProperty operator*() const;
        PropertyIterator& operator++() {
            if (m_remaining > 0) {
                m_offset++;
                m_remaining--;
            }
            return *this;
        }
        bool operator==(const PropertyIterator& other) const {
            return m_remaining == other.m_remaining && m_offset == other.m_offset;
        }
        bool operator!=(const PropertyIterator& other) const {
            return !(*this == other);
        }

    private:
        const StrideDocument* m_doc{nullptr};
        uint32_t m_offset{0};
        uint32_t m_remaining{0};
    };

    class PropertyRange {
    public:
        PropertyRange(const StrideDocument* doc, uint32_t start, uint32_t count)
            : m_doc(doc), m_start(start), m_count(count) {}
        PropertyIterator begin() const { return PropertyIterator(m_doc, m_start, m_count); }
        PropertyIterator end() const { return PropertyIterator(m_doc, m_start + m_count, 0); }
    private:
        const StrideDocument* m_doc{nullptr};
        uint32_t m_start{0};
        uint32_t m_count{0};
    };

    PropertyRange properties() const;

private:
    const StrideDocument* m_doc{nullptr};
    uint32_t m_index{0};
};

struct StrideProperty {
    std::string_view key;
    StrideValue value;
};

/**
 * @brief High-performance contiguous DOM for Stride data documents.
 *
 * StrideDocument parses and indexes Stride data into flat memory vectors with zero-copy string views,
 * providing O(1) list indexing, sub-microsecond property lookups, and minimal peak memory overhead.
 */
class StrideDocument : public StrideHandler {
public:
    struct PropertyEntry {
        std::string_view key;
        uint32_t nodeIndex{0};
    };

    struct Node {
        StrideType type{StrideType::None};
        std::string_view typeName;
        std::string_view name;
        std::string_view strVal;
        int64_t intVal{0};
        double realVal{0.0};
        bool boolVal{false};
        uint32_t childStart{0};
        uint32_t childCount{0};
    };

    StrideDocument() = default;

    /**
     * @brief Parses Stride data from a string_view. The string buffer must outlive the document.
     */
    bool parse(std::string_view source);

    /**
     * @brief Reads and parses a Stride data file into an internally owned memory buffer.
     */
    bool parseFile(const std::string& path);

    /**
     * @brief Clears all document contents.
     */
    void clear();

    /**
     * @brief Returns the root block value (index 0).
     */
    StrideValue root(size_t index = 0) const;

    /**
     * @brief Returns the total number of top-level root blocks parsed.
     */
    size_t rootCount() const { return m_rootIndices.size(); }

    bool hasError() const { return !m_error.empty(); }
    const std::string& getError() const { return m_error; }

    // Internal node access for StrideValue
    const Node& getNode(uint32_t index) const { return m_nodes[index]; }
    const PropertyEntry& getProperty(uint32_t index) const { return m_properties[index]; }
    uint32_t getListElement(uint32_t index) const { return m_listElements[index]; }

    // StrideHandler callbacks
    void onBlockStart(std::string_view typeName, std::string_view name) override;
    void onBlockEnd() override;
    void onProperty(std::string_view key) override;
    void onListStart() override;
    void onListEnd() override;
    void onString(std::string_view val) override;
    void onInt(int64_t val) override;
    void onReal(double val) override;
    void onBool(bool val) override;
    void onNone() override;
    void onError(std::string_view errorMsg, size_t line, size_t col) override;

private:
    struct BlockContext {
        uint32_t nodeIndex{0};
        std::vector<PropertyEntry> properties;
    };

    struct ListContext {
        uint32_t nodeIndex{0};
        std::vector<uint32_t> elements;
    };

    void attachValue(uint32_t nodeIndex);

    std::string m_ownedSource;
    std::vector<Node> m_nodes;
    std::vector<PropertyEntry> m_properties;
    std::vector<uint32_t> m_listElements;
    std::vector<uint32_t> m_rootIndices;
    std::vector<BlockContext> m_blockStack;
    std::vector<ListContext> m_listStack;
    std::string_view m_currentPropKey;
    std::string m_error;
};


// -----------------------------------------------------------------------------
// StrideValue & StrideDocument Inlined Implementation
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// StrideValue
// -----------------------------------------------------------------------------

inline StrideValue::StrideValue(const StrideDocument* doc, uint32_t index)
    : m_doc(doc), m_index(index) {}

inline StrideType StrideValue::type() const {
    return m_doc ? m_doc->getNode(m_index).type : StrideType::None;
}

inline std::string_view StrideValue::typeName() const {
    return m_doc ? m_doc->getNode(m_index).typeName : std::string_view{};
}

inline std::string_view StrideValue::name() const {
    return m_doc ? m_doc->getNode(m_index).name : std::string_view{};
}

inline std::string_view StrideValue::asString() const {
    return m_doc ? m_doc->getNode(m_index).strVal : std::string_view{};
}

inline int64_t StrideValue::asInt() const {
    return m_doc ? m_doc->getNode(m_index).intVal : 0;
}

inline double StrideValue::asReal() const {
    return m_doc ? m_doc->getNode(m_index).realVal : 0.0;
}

inline bool StrideValue::asBool() const {
    return m_doc ? m_doc->getNode(m_index).boolVal : false;
}

inline size_t StrideValue::size() const {
    if (!m_doc) return 0;
    return m_doc->getNode(m_index).childCount;
}

inline bool StrideValue::hasProperty(std::string_view key) const {
    if (!m_doc || type() != StrideType::Block) return false;
    const auto& node = m_doc->getNode(m_index);
    for (uint32_t i = 0; i < node.childCount; ++i) {
        if (m_doc->getProperty(node.childStart + i).key == key) return true;
    }
    return false;
}

inline StrideValue StrideValue::operator[](std::string_view key) const {
    if (!m_doc || type() != StrideType::Block) return StrideValue();
    const auto& node = m_doc->getNode(m_index);
    for (uint32_t i = 0; i < node.childCount; ++i) {
        const auto& prop = m_doc->getProperty(node.childStart + i);
        if (prop.key == key) {
            return StrideValue(m_doc, prop.nodeIndex);
        }
    }
    return StrideValue();
}

inline StrideValue StrideValue::operator[](size_t index) const {
    if (!m_doc || type() != StrideType::List) return StrideValue();
    const auto& node = m_doc->getNode(m_index);
    if (index >= node.childCount) return StrideValue();
    uint32_t nodeIdx = m_doc->getListElement(node.childStart + static_cast<uint32_t>(index));
    return StrideValue(m_doc, nodeIdx);
}

inline StrideValue StrideValue::get(std::string_view key, StrideValue defaultVal) const {
    StrideValue v = (*this)[key];
    return v ? v : defaultVal;
}

inline StrideValue StrideValue::ListIterator::operator*() const {
    if (!m_doc) return StrideValue();
    uint32_t nodeIdx = m_doc->getListElement(m_offset);
    return StrideValue(m_doc, nodeIdx);
}

inline StrideValue::ListIterator StrideValue::begin() const {
    if (!m_doc || type() != StrideType::List) {
        return ListIterator(nullptr, 0, 0);
    }
    const auto& node = m_doc->getNode(m_index);
    return ListIterator(m_doc, node.childStart, node.childCount);
}

inline StrideValue::ListIterator StrideValue::end() const {
    if (!m_doc || type() != StrideType::List) {
        return ListIterator(nullptr, 0, 0);
    }
    const auto& node = m_doc->getNode(m_index);
    return ListIterator(m_doc, node.childStart + node.childCount, 0);
}

inline StrideProperty StrideValue::PropertyIterator::operator*() const {
    if (!m_doc) return StrideProperty{};
    const auto& prop = m_doc->getProperty(m_offset);
    return StrideProperty{prop.key, StrideValue(m_doc, prop.nodeIndex)};
}

inline StrideValue::PropertyRange StrideValue::properties() const {
    if (!m_doc || type() != StrideType::Block) {
        return PropertyRange(nullptr, 0, 0);
    }
    const auto& node = m_doc->getNode(m_index);
    return PropertyRange(m_doc, node.childStart, node.childCount);
}

// -----------------------------------------------------------------------------
// StrideDocument
// -----------------------------------------------------------------------------

inline bool StrideDocument::parse(std::string_view source) {
    clear();
    StrideReader reader(source);
    if (!reader.parse(*this)) {
        m_error = reader.getErrorMessage();
        return false;
    }
    return true;
}

inline bool StrideDocument::parseFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) {
        m_error = "Could not open file: " + path;
        return false;
    }
    m_ownedSource = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return parse(m_ownedSource);
}

inline void StrideDocument::clear() {
    m_nodes.clear();
    m_properties.clear();
    m_listElements.clear();
    m_rootIndices.clear();
    m_blockStack.clear();
    m_listStack.clear();
    m_currentPropKey = {};
    m_error.clear();
}

inline StrideValue StrideDocument::root(size_t index) const {
    if (index < m_rootIndices.size()) {
        return StrideValue(this, m_rootIndices[index]);
    }
    return StrideValue();
}

inline void StrideDocument::onBlockStart(std::string_view typeName, std::string_view name) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Block;
    node.typeName = typeName;
    node.name = name;
    m_nodes.push_back(node);

    attachValue(idx);
    m_blockStack.push_back({idx, {}});
}

inline void StrideDocument::onBlockEnd() {
    if (!m_blockStack.empty()) {
        auto& ctx = m_blockStack.back();
        uint32_t idx = ctx.nodeIndex;
        m_nodes[idx].childStart = static_cast<uint32_t>(m_properties.size());
        m_nodes[idx].childCount = static_cast<uint32_t>(ctx.properties.size());
        m_properties.insert(m_properties.end(), ctx.properties.begin(), ctx.properties.end());
        m_blockStack.pop_back();
    }
}

inline void StrideDocument::onProperty(std::string_view key) {
    m_currentPropKey = key;
}

inline void StrideDocument::onListStart() {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::List;
    m_nodes.push_back(node);

    attachValue(idx);
    m_listStack.push_back({idx, {}});
}

inline void StrideDocument::onListEnd() {
    if (!m_listStack.empty()) {
        auto& ctx = m_listStack.back();
        uint32_t idx = ctx.nodeIndex;
        m_nodes[idx].childStart = static_cast<uint32_t>(m_listElements.size());
        m_nodes[idx].childCount = static_cast<uint32_t>(ctx.elements.size());
        m_listElements.insert(m_listElements.end(), ctx.elements.begin(), ctx.elements.end());
        m_listStack.pop_back();
    }
}

inline void StrideDocument::onString(std::string_view val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::String;
    node.strVal = val;
    m_nodes.push_back(node);
    attachValue(idx);
}

inline void StrideDocument::onInt(int64_t val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Int;
    node.intVal = val;
    node.realVal = static_cast<double>(val);
    m_nodes.push_back(node);
    attachValue(idx);
}

inline void StrideDocument::onReal(double val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Real;
    node.realVal = val;
    node.intVal = static_cast<int64_t>(val);
    m_nodes.push_back(node);
    attachValue(idx);
}

inline void StrideDocument::onBool(bool val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Bool;
    node.boolVal = val;
    m_nodes.push_back(node);
    attachValue(idx);
}

inline void StrideDocument::onNone() {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::None;
    m_nodes.push_back(node);
    attachValue(idx);
}

inline void StrideDocument::onError(std::string_view errorMsg, size_t line, size_t col) {
    m_error = std::string(errorMsg);
}

inline void StrideDocument::attachValue(uint32_t nodeIndex) {
    if (!m_currentPropKey.empty()) {
        if (!m_blockStack.empty()) {
            m_blockStack.back().properties.push_back({m_currentPropKey, nodeIndex});
        }
        m_currentPropKey = {};
    } else if (!m_listStack.empty()) {
        m_listStack.back().elements.push_back(nodeIndex);
    } else {
        m_rootIndices.push_back(nodeIndex);
    }
}

} // namespace strd

#endif // STRIDEDOCUMENT_H
