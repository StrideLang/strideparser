/*
    Stride is licensed under the terms of the 3-clause BSD license.

    Copyright (C) 2026. Andres Cabrera.
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

    Authors: Andres Cabrera
*/

#include <cassert>

#include "stride/parser/packagenode.h"
#include "stride/parser/listnode.h"
#include "stride/parser/scopenode.h"

using namespace strd;

PackageNode::PackageNode(std::string name, ASTNode scope, const char *filename,
                         int line)
    : AST(AST::Package, filename, line) {
  m_packageName = name;
  if (scope) {
    for (unsigned int i = 0; i < scope->getChildren().size(); i++) {
      assert(scope->getChildren().at(i)->getNodeType() == AST::Scope);
      m_scope.push_back(
          (static_cast<ScopeNode *>(scope->getChildren().at(i).get()))
              ->getName());
    }
  }
}

PackageNode::PackageNode(std::string name, const char *filename, int line)
    : AST(AST::Package, filename, line) {
  m_packageName = name;
}

std::string PackageNode::packageName() const { return m_packageName; }

const std::string &PackageNode::fullPackageName() const {
  if (m_cachedFullName.empty()) {
    size_t totalLen = m_packageName.size();
    for (const auto &seg : m_scope) {
      totalLen += seg.size() + 2; // for "::"
    }
    m_cachedFullName.reserve(totalLen);
    for (const auto &seg : m_scope) {
      m_cachedFullName.append(seg);
      m_cachedFullName.append("::");
    }
    m_cachedFullName.append(m_packageName);
  }
  return m_cachedFullName;
}

const std::vector<std::string> &PackageNode::packageSegments() const {
  if (m_cachedSegments.empty()) {
    m_cachedSegments.reserve(m_scope.size() + 1);
    m_cachedSegments.insert(m_cachedSegments.end(), m_scope.begin(),
                            m_scope.end());
    m_cachedSegments.push_back(m_packageName);
  }
  return m_cachedSegments;
}

void PackageNode::resolveScope(ASTNode scope) {
  m_cachedFullName.clear();
  m_cachedSegments.clear();
  if (scope) {
    for (unsigned int i = 0; i < scope->getChildren().size(); i++) {
      assert(scope->getChildren().at(i)->getNodeType() == AST::Scope);
      m_scope.push_back(
          (static_cast<ScopeNode *>(scope->getChildren().at(i).get()))
              ->getName());
    }
  }
}

ASTNode PackageNode::deepCopy() {
  auto newPackageNode = std::make_shared<PackageNode>(
      m_packageName, m_filename.data(), getLine());
  for (unsigned int i = 0; i < this->getScopeLevels(); i++) {
    newPackageNode->addScope(this->getScopeAt(i));
  }
  if (this->m_CompilerProperties) {
    newPackageNode->m_CompilerProperties = std::static_pointer_cast<ListNode>(
        this->m_CompilerProperties->deepCopy());
  } else {
    newPackageNode->m_CompilerProperties = nullptr;
  }
  return newPackageNode;
}

std::string PackageNode::toText(int indentOffset, int indentSize,
                                bool newLine) const {
  std::string text;
  text += "package ";
  for (const auto &seg : m_scope) {
    text += seg + "::";
  }
  text += m_packageName + ";";
  if (newLine) {
    text += "\n";
  }
  return text;
}
