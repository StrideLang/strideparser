#ifndef STRIDEDOCUMENT_H
#define STRIDEDOCUMENT_H

#include "stride/parser/stridereader.h"
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstddef>
#include <iterator>

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

} // namespace strd

#endif // STRIDEDOCUMENT_H
