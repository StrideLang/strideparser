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

#ifndef PACKAGENODE_H
#define PACKAGENODE_H

#include <string>
#include <vector>

#include "ast.h"

namespace strd {
class PackageNode : public AST {
public:
  PackageNode(std::string name, ASTNode scope, const char *filename, int line);
  PackageNode(std::string name, const char *filename, int line);

  std::string packageName() const;

  const std::string &fullPackageName() const;
  const std::vector<std::string> &packageSegments() const;

  virtual void resolveScope(ASTNode scope) override;
  virtual ASTNode deepCopy() override;

  std::string toText(int indentOffset = 0, int indentSize = 2,
                     bool newLine = true) const override;

private:
  std::string m_packageName;
  mutable std::string m_cachedFullName;
  mutable std::vector<std::string> m_cachedSegments;
};
} // namespace strd

#endif // PACKAGENODE_H
