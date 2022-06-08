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

#include "blocknode.h"
#include "scopenode.h"
#include "listnode.h"

using namespace std;

BlockNode::BlockNode(string name, const char *filename, int line, vector<string> scope) :
    AST(AST::Block, filename, line, scope)
{
    m_name = name;

    m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

BlockNode::BlockNode(string name, ASTNode scope, const char *filename, int line) :
    AST(AST::Block, filename, line)
{
    m_name = name;
    resolveScope(scope);

    m_CompilerProperties = make_shared<ListNode>(__FILE__, __LINE__);
}

BlockNode::~BlockNode()
{

}

void BlockNode::resolveScope(ASTNode scope)
{
    if (scope) {
        for (unsigned int i = 0; i < scope->getChildren().size(); i++) {
            assert(scope->getChildren().at(i)->getNodeType() == AST::Scope);
            m_scope.push_back((static_cast<ScopeNode *>(scope->getChildren().at(i).get()))->getName());
        }
    }
}

ASTNode BlockNode::deepCopy()
{
    std::shared_ptr<BlockNode> newNode = std::make_shared<BlockNode>(m_name, m_filename.data(), m_line, m_scope);
//    if (this->m_CompilerProperties) {
//        newNode->m_CompilerProperties = std::static_pointer_cast<ListNode>(this->m_CompilerProperties->deepCopy());
//    } else {
//        newNode->m_CompilerProperties = nullptr;
//    }
    return newNode;
}
