/*
    Stride is licensed under the terms of the 3-clause BSD license.

    Copyright (C) 2017. The Regents of the University of California.
    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

        Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

        Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

        Neither the name of the copyright holder nor the names of its
        contributors may be used to endorse or promote products derived from
        this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    Authors: Andres Cabrera and Joseph Tilbian
*/

#include <cassert>
#include <iomanip>
#include <sstream>

#include "stride/parser/listnode.h"
#include "stride/parser/valuenode.h"

using namespace std;
using namespace strd;

ValueNode::ValueNode(const char *filename, int line)
    : AST(AST::None, filename, line) {
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::ValueNode(int64_t value, const char *filename, int line)
    : AST(AST::Int, filename, line) {
  m_value = value;
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::ValueNode(float value, const char *filename, int line)
    : AST(AST::Real, filename, line) {
  m_value = double(value);
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::ValueNode(double value, const char *filename, int line)
    : AST(AST::Real, filename, line) {
  m_value = value;
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::ValueNode(const char *value, const char *filename, int line)
    : AST(AST::String, filename, line) {
  m_value = string(value); // Assign const char* to std::string in variant
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::ValueNode(string value, const char *filename, int line)
    : AST(AST::String, filename, line) {
  m_value = value;
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::ValueNode(bool value, const char *filename, int line)
    : AST(AST::Switch, filename, line) {
  m_value = value;
  m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

ValueNode::~ValueNode() {}

int64_t ValueNode::getIntValue() const {
  assert(getNodeType() == AST::Int);
  return std::get<int64_t>(m_value);
}

double ValueNode::getRealValue() const {
  assert(getNodeType() == AST::Real);
  return std::get<double>(m_value);
}

double ValueNode::toReal() const {
  if (getNodeType() == AST::Real) {
    return std::get<double>(m_value);
  } else if (getNodeType() == AST::Int) {
    return (double)std::get<int64_t>(m_value);
  }
  return 0;
}

string ValueNode::getStringValue() const {
  assert(getNodeType() == AST::String);
  return std::get<string>(m_value);
}

string ValueNode::toString() const {
  if (getNodeType() == AST::Real) {
    stringstream s;
    s << std::setprecision(16) << std::get<double>(m_value);
    return s.str();
  } else if (getNodeType() == AST::Int) {
    return std::to_string(std::get<int64_t>(m_value));
  } else if (getNodeType() == AST::String) {
    return std::get<string>(m_value);
  } else if (getNodeType() == AST::Switch) {
    if (std::get<bool>(m_value)) {
      return "On";
    } else {
      return "Off";
    }
  }
  return "";
}

bool ValueNode::getSwitchValue() const {
  assert(getNodeType() == AST::Switch);
  return std::get<bool>(m_value);
}

string ValueNode::toText(int indentOffset, int indentSize, bool newLine) const {
  (void)indentOffset;
  (void)indentSize;
  string outText;
  if (getNodeType() == AST::Int) {
    outText += std::to_string(std::get<int64_t>(m_value));
  } else if (getNodeType() == AST::Real) {
    outText += std::to_string(std::get<double>(m_value));
  } else if (getNodeType() == AST::String) {
    outText += "\"" + std::get<string>(m_value) + "\"";
  } else if (getNodeType() == AST::Switch) {
    outText += (std::get<bool>(m_value) ? "on " : "off ");
  }
  if (newLine) {
    outText += "\n";
  }
  return outText;
}

ASTNode ValueNode::deepCopy() {
  // TODO newNode->m_CompilerProperties = this->m_CompilerProperties;
  if (getNodeType() == AST::Int) {
    return std::make_shared<ValueNode>(std::get<int64_t>(m_value),
                                       m_filename.c_str(), getLine());
  } else if (getNodeType() == AST::Real) {
    return std::make_shared<ValueNode>(std::get<double>(m_value),
                                       m_filename.c_str(), getLine());
  } else if (getNodeType() == AST::String) {
    return std::make_shared<ValueNode>(std::get<string>(m_value),
                                       m_filename.c_str(), getLine());
  } else if (getNodeType() == AST::Switch) {
    return std::make_shared<ValueNode>(std::get<bool>(m_value),
                                       m_filename.c_str(), getLine());
  } else if (getNodeType() == AST::None) {
    return std::make_shared<ValueNode>(m_filename.data(), getLine());
  } else {
    assert(0); // Invalid type
  }
  return nullptr;
}

void ValueNode::setDomain(ASTNode domain) { m_domain = domain; }

ASTNode ValueNode::getDomain() { return m_domain; }

void ValueNode::setBoolValue(bool value) {
    assert(getNodeType() == AST::Switch);
    m_value = value;
}
