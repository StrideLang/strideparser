#include "stride/parser/stridedocument.h"
#include <fstream>

namespace strd {

// -----------------------------------------------------------------------------
// StrideValue
// -----------------------------------------------------------------------------

StrideValue::StrideValue(const StrideDocument* doc, uint32_t index)
    : m_doc(doc), m_index(index) {}

StrideType StrideValue::type() const {
    return m_doc ? m_doc->getNode(m_index).type : StrideType::None;
}

std::string_view StrideValue::typeName() const {
    return m_doc ? m_doc->getNode(m_index).typeName : std::string_view{};
}

std::string_view StrideValue::name() const {
    return m_doc ? m_doc->getNode(m_index).name : std::string_view{};
}

std::string_view StrideValue::asString() const {
    return m_doc ? m_doc->getNode(m_index).strVal : std::string_view{};
}

int64_t StrideValue::asInt() const {
    return m_doc ? m_doc->getNode(m_index).intVal : 0;
}

double StrideValue::asReal() const {
    return m_doc ? m_doc->getNode(m_index).realVal : 0.0;
}

bool StrideValue::asBool() const {
    return m_doc ? m_doc->getNode(m_index).boolVal : false;
}

size_t StrideValue::size() const {
    if (!m_doc) return 0;
    return m_doc->getNode(m_index).childCount;
}

bool StrideValue::hasProperty(std::string_view key) const {
    if (!m_doc || type() != StrideType::Block) return false;
    const auto& node = m_doc->getNode(m_index);
    for (uint32_t i = 0; i < node.childCount; ++i) {
        if (m_doc->getProperty(node.childStart + i).key == key) return true;
    }
    return false;
}

StrideValue StrideValue::operator[](std::string_view key) const {
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

StrideValue StrideValue::operator[](size_t index) const {
    if (!m_doc || type() != StrideType::List) return StrideValue();
    const auto& node = m_doc->getNode(m_index);
    if (index >= node.childCount) return StrideValue();
    uint32_t nodeIdx = m_doc->getListElement(node.childStart + static_cast<uint32_t>(index));
    return StrideValue(m_doc, nodeIdx);
}

StrideValue StrideValue::get(std::string_view key, StrideValue defaultVal) const {
    StrideValue v = (*this)[key];
    return v ? v : defaultVal;
}

StrideValue StrideValue::ListIterator::operator*() const {
    if (!m_doc) return StrideValue();
    uint32_t nodeIdx = m_doc->getListElement(m_offset);
    return StrideValue(m_doc, nodeIdx);
}

StrideValue::ListIterator StrideValue::begin() const {
    if (!m_doc || type() != StrideType::List) {
        return ListIterator(nullptr, 0, 0);
    }
    const auto& node = m_doc->getNode(m_index);
    return ListIterator(m_doc, node.childStart, node.childCount);
}

StrideValue::ListIterator StrideValue::end() const {
    if (!m_doc || type() != StrideType::List) {
        return ListIterator(nullptr, 0, 0);
    }
    const auto& node = m_doc->getNode(m_index);
    return ListIterator(m_doc, node.childStart + node.childCount, 0);
}

StrideProperty StrideValue::PropertyIterator::operator*() const {
    if (!m_doc) return StrideProperty{};
    const auto& prop = m_doc->getProperty(m_offset);
    return StrideProperty{prop.key, StrideValue(m_doc, prop.nodeIndex)};
}

StrideValue::PropertyRange StrideValue::properties() const {
    if (!m_doc || type() != StrideType::Block) {
        return PropertyRange(nullptr, 0, 0);
    }
    const auto& node = m_doc->getNode(m_index);
    return PropertyRange(m_doc, node.childStart, node.childCount);
}

// -----------------------------------------------------------------------------
// StrideDocument
// -----------------------------------------------------------------------------

bool StrideDocument::parse(std::string_view source) {
    clear();
    StrideReader reader(source);
    if (!reader.parse(*this)) {
        m_error = reader.getErrorMessage();
        return false;
    }
    return true;
}

bool StrideDocument::parseFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) {
        m_error = "Could not open file: " + path;
        return false;
    }
    m_ownedSource = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return parse(m_ownedSource);
}

void StrideDocument::clear() {
    m_nodes.clear();
    m_properties.clear();
    m_listElements.clear();
    m_rootIndices.clear();
    m_blockStack.clear();
    m_listStack.clear();
    m_currentPropKey = {};
    m_error.clear();
}

StrideValue StrideDocument::root(size_t index) const {
    if (index < m_rootIndices.size()) {
        return StrideValue(this, m_rootIndices[index]);
    }
    return StrideValue();
}

void StrideDocument::onBlockStart(std::string_view typeName, std::string_view name) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Block;
    node.typeName = typeName;
    node.name = name;
    m_nodes.push_back(node);

    attachValue(idx);
    m_blockStack.push_back({idx, {}});
}

void StrideDocument::onBlockEnd() {
    if (!m_blockStack.empty()) {
        auto& ctx = m_blockStack.back();
        uint32_t idx = ctx.nodeIndex;
        m_nodes[idx].childStart = static_cast<uint32_t>(m_properties.size());
        m_nodes[idx].childCount = static_cast<uint32_t>(ctx.properties.size());
        m_properties.insert(m_properties.end(), ctx.properties.begin(), ctx.properties.end());
        m_blockStack.pop_back();
    }
}

void StrideDocument::onProperty(std::string_view key) {
    m_currentPropKey = key;
}

void StrideDocument::onListStart() {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::List;
    m_nodes.push_back(node);

    attachValue(idx);
    m_listStack.push_back({idx, {}});
}

void StrideDocument::onListEnd() {
    if (!m_listStack.empty()) {
        auto& ctx = m_listStack.back();
        uint32_t idx = ctx.nodeIndex;
        m_nodes[idx].childStart = static_cast<uint32_t>(m_listElements.size());
        m_nodes[idx].childCount = static_cast<uint32_t>(ctx.elements.size());
        m_listElements.insert(m_listElements.end(), ctx.elements.begin(), ctx.elements.end());
        m_listStack.pop_back();
    }
}

void StrideDocument::onString(std::string_view val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::String;
    node.strVal = val;
    m_nodes.push_back(node);
    attachValue(idx);
}

void StrideDocument::onInt(int64_t val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Int;
    node.intVal = val;
    node.realVal = static_cast<double>(val);
    m_nodes.push_back(node);
    attachValue(idx);
}

void StrideDocument::onReal(double val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Real;
    node.realVal = val;
    node.intVal = static_cast<int64_t>(val);
    m_nodes.push_back(node);
    attachValue(idx);
}

void StrideDocument::onBool(bool val) {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::Bool;
    node.boolVal = val;
    m_nodes.push_back(node);
    attachValue(idx);
}

void StrideDocument::onNone() {
    uint32_t idx = static_cast<uint32_t>(m_nodes.size());
    Node node;
    node.type = StrideType::None;
    m_nodes.push_back(node);
    attachValue(idx);
}

void StrideDocument::onError(std::string_view errorMsg, size_t line, size_t col) {
    m_error = std::string(errorMsg);
}

void StrideDocument::attachValue(uint32_t nodeIndex) {
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
