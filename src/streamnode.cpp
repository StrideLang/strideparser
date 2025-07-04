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

#include "stride/parser/listnode.h"
#include "stride/parser/streamnode.h"

StreamNode::StreamNode(ASTNode left, ASTNode right, const char *filename,
                       int line)
    : AST(AST::Stream, filename, line) {
  //    assert(left); assert(right);
  assert(left->getNodeType() != AST::Stream); // This is not allowed
  addChild(left);
  addChild(right);
}

StreamNode::~StreamNode() {}

void StreamNode::setLeft(ASTNode newLeft) {
  //    ASTNode oldLeft = m_children.at(0);
  //    oldLeft->deleteChildren();
  //    oldLeft.reset();
  m_children.at(0) = newLeft;
}

void StreamNode::setRight(ASTNode newRight) {
  //    ASTNode oldRight = m_children.at(1);
  //    oldRight->deleteChildren();
  //    oldRight.reset();
  m_children.at(1) = newRight;
}

ASTNode StreamNode::deepCopy() {
  std::shared_ptr<StreamNode> newStream = std::make_shared<StreamNode>(
      m_children.at(0)->deepCopy(), m_children.at(1)->deepCopy(),
      m_filename.data(), m_line);

  //    if (this->m_CompilerProperties) {
  //        newStream->m_CompilerProperties =
  //        std::static_pointer_cast<ListNode>(this->m_CompilerProperties->deepCopy());
  //    } else {
  //        newStream->m_CompilerProperties = nullptr;
  //    }
  return newStream;
}

StreamNodeIterator::StreamNodeIterator(std::shared_ptr<StreamNode> stream) {
  mStream = stream;
}

ASTNode StreamNodeIterator::next() {
  ASTNode next;
  if (mStream) {
    next = mStream->getLeft();
    if (mStream->getRight()->getNodeType() == AST::Stream) {
      mStream = std::static_pointer_cast<StreamNode>(mStream->getRight());
    } else {
      mLastNode = mStream->getRight();
      mStream = nullptr;
    }
  } else if (mLastNode) {
    next = mLastNode;
    mLastNode = nullptr;
  }

  return next;
}
