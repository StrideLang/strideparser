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

#include <vector>

#include "stride/parser/listnode.h"
#include "stride/parser/platformnode.h"

using namespace strd;

SystemNode::SystemNode(std::string platformName, int majorVersion,
                       int minorVersion, const char *filename, int line,
                       std::vector<std::string> hwPlatform)
    : AST(AST::Platform, filename, line) {
  m_systemName = platformName;
  m_majorVersion = majorVersion;
  m_minorVersion = minorVersion;
  m_targetPlatforms = hwPlatform;
}

SystemNode::~SystemNode() {}

int SystemNode::majorVersion() const { return m_majorVersion; }

int SystemNode::minorVersion() const { return m_minorVersion; }

ASTNode SystemNode::deepCopy() {
  auto newNode = std::make_shared<SystemNode>(m_systemName, m_majorVersion,
                                              m_minorVersion, m_filename.data(),
                                              m_line, m_targetPlatforms);
  for (auto &child : getChildren()) {
    newNode->addChild(child->deepCopy());
  }
  if (this->m_CompilerProperties) {
    newNode->m_CompilerProperties = std::static_pointer_cast<ListNode>(
        this->m_CompilerProperties->deepCopy());
  } else {
    newNode->m_CompilerProperties = nullptr;
  }
  return newNode;
}

// std::vector<std::string> SystemNode::hwPlatforms() const {
//   return m_targetPlatforms;
// }

// void SystemNode::setHwPlatforms(const std::vector<std::string> &hwPlatform) {
//   m_targetPlatforms = hwPlatform;
// }

std::string SystemNode::platformName() const { return m_systemName; }

void SystemNode::setPlatformName(const std::string &platformName) {
  m_systemName = platformName;
}
