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

#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

#include "langerror.h"

namespace strd {
class AST;
class ListNode;
class PropertyNode;

// A comment in the source ('#' to the end of the line, without the '#').
struct SourceComment {
  int line{-1};
  int column{-1}; // Column of the '#'
  std::string text;
};

using ASTNode = std::shared_ptr<AST>;

using ScopeStack = std::vector<std::pair<ASTNode, std::vector<ASTNode>>>;

class AST {
public:
  AST();

  typedef enum {
    None,
    Platform,
    Array,
    Declaration,
    ArrayDeclaration,
    Stream,
    Property,
    Range,
    List,
    Import,
    Package,
    For,
    Scope,
    MemberAccess,
    GeneratorRange,

    // Built-in types (leaf nodes)
    Int = 0x80,
    Real = 0x81,
    String = 0x82,
    Switch = 0x83,

    Entity = 0x20,
    Expression = 0x21,
    Function = 0x22,
    Keyword = 0x23,

    // Invalid
    Invalid
  } Token;

  AST(Token token, const char *filename, int line = -1, int column = -1,
      std::vector<std::string> scope = std::vector<std::string>());
  AST(Token token, const char *filename, int line,
      std::vector<std::string> scope);
  virtual ~AST();

  // parsing
  static ASTNode parseFile(const char *fileName,
                           const char *sourceFilename = nullptr);
  static std::vector<LangError> getParseErrors();
  // Comments in the last parsed file, in source order. They are not attached
  // to nodes; tools that edit source use them with node source ranges.
  static std::vector<SourceComment> getParseComments();

  //
  Token getNodeType() const { return m_token; }
  bool isNil() { return m_token == AST::None; }

  virtual void addChild(ASTNode t);
  const std::vector<ASTNode> &getChildren() const { return m_children; }
  virtual void setChildren(const std::vector<ASTNode> &newChildren);

  virtual ASTNode deepCopy();

  std::string getFilename() const;
  void setFilename(const std::string &filename);
  std::string getRelativeFilename() const;
  void setRelativeFilename(const std::string &relFilename);
  std::string getAbsoluteFilename() const;
  void setAbsoluteFilename(const std::string &absFilename);
  void setFilenames(const std::string &relFilename, const std::string &absFilename);

  int getLine() const { return m_line; }
  void setLine(int line) { m_line = line; }
  int getColumn() const { return m_column; }
  void setColumn(int column) { m_column = column; }
  // Last line and column of the node's source text (inclusive), or -1 when
  // unknown. Set for declarations, properties and streams.
  int getEndLine() const { return m_endLine; }
  int getEndColumn() const { return m_endColumn; }
  void setEnd(int line, int column) {
    m_endLine = line;
    m_endColumn = column;
  }

  void copyLocationFrom(const AST &other);
  void copyLocationTo(ASTNode other) const;
  void copyLocationTo(LangError &error) const;

  void addScope(std::string newScope);
  void setRootScope(std::string scopeName);
  size_t getScopeLevels();
  std::string getScopeAt(unsigned int scopeLevel);

  std::vector<std::string> getNamespaceList() const;
  void setNamespaceList(std::vector<std::string> list);

  void setCompilerProperty(std::string propertyName, ASTNode value);
  ASTNode getCompilerProperty(std::string propertyName) const;
  void removeCompilerProperty(std::string propertyName);
  void appendToPropertyValue(std::string propertyName, ASTNode value);
  std::shared_ptr<ListNode> getCompilerPropertiesList() const {
    return m_CompilerProperties;
  }
  void setCompilerPropertiesList(std::shared_ptr<ListNode> props) {
    m_CompilerProperties = std::move(props);
  }

  // Code processing
  virtual std::string toText(int indentOffset = 0, int indentSize = 2,
                             bool newLine = true) const;

  static std::string toText(ASTNode node, int indentOffset = 0,
                            int indentSize = 2, bool newLine = true);

protected:
  virtual void resolveScope(ASTNode scope);

  Token m_token;                   // From which token did we create node?
  std::vector<ASTNode> m_children; // normalized list of children
  std::string m_filename;          // file where the node was generated
  std::string m_relativeFilename;  // relative path from project root / include path
  std::string m_absoluteFilename;  // canonical absolute path
  int m_line;
  int m_column{-1};
  int m_endLine{-1};
  int m_endColumn{-1};
  std::vector<std::string> m_scope;
  std::shared_ptr<ListNode> m_CompilerProperties;
};
} // namespace strd

#endif // AST_H
