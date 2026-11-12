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

#include "stride/parser/langerror.h"

#include <sstream>

using namespace strd;

LangError::LangError() {}

static std::string typeErrorText(LangError::ErrorType type,
                                 const std::vector<std::string> &errorTokens) {
  std::string errorText;
  switch (type) {
  case LangError::Syntax: {
    std::string token = (!errorTokens.empty() && !errorTokens[0].empty())
                            ? errorTokens[0]
                            : "UNKNOWN";
    if (token.size() <= 1 && token != "UNKNOWN") {
      errorText += "Syntax Error: Unexpected character '" + token + "'";
    } else {
      errorText += "Syntax Error: Unexpected token '" + token + "'";
    }
  } break;
  case LangError::UnknownType:
    errorText +=
        "Unknown Type Error. Type '" + errorTokens[0] + "' not recognized.";
    break;
  case LangError::InvalidType:
    errorText += "Invalid Type: '" + errorTokens[0] + "'";
    break;
  case LangError::InvalidDataType:
    errorText +=
        "Invalid Data Type '" + errorTokens[0] + "'. " + errorTokens[1];
    break;
  case LangError::InvalidLiteral:
    errorText += "Invalid literal '" + errorTokens[0] + "'. " + errorTokens[1];
    break;
  case LangError::InvalidIndexType:
    errorText += "Invalid index type.";
    break;
  case LangError::ArraySizeMismatch:
    errorText += "Array Size Mismatch Error";
    break;
  case LangError::ArrayIndexOutOfBounds:
    errorText += "Array Index out of Range Error";
    break;
  case LangError::InconsistentList:
    errorText += "Inconsistent List Error";
    break;
  case LangError::LossyConversion:
    errorText += "Conversion from '" + errorTokens[0] + "' to '" +
                 errorTokens[1] + "' " + errorTokens[2];
    break;
  case LangError::UnresolvedType:
    errorText += "Can't resolve the data type of '" + errorTokens[0] +
                 "'. The output type of '" + errorTokens[1] +
                 "' comes from its output port. Declare '" + errorTokens[0] +
                 "' with a type";
    break;
  case LangError::InheritedFinalType:
    errorText +=
        "Type '" + errorTokens[0] + "' is final and cannot be inherited from";
    break;
  default:
    break;
  }
  return errorText;
}

static std::string portErrorText(LangError::ErrorType type,
                                 const std::vector<std::string> &errorTokens) {
  std::string errorText;
  switch (type) {
  case LangError::InvalidPort:
    errorText += "Invalid Port '" + errorTokens[1] + "' for type '" +
                 errorTokens[0] + "'";
    break;
  case LangError::ReadOnlyPort:
    errorText += "Port '" + errorTokens[1] + "' of type '" + errorTokens[0] +
                 "' is read only. It is set by the compiler";
    break;
  case LangError::InvalidPortType:
    errorText += "Invalid port type Error. Port '" + errorTokens[1] +
                 "' in Entity '" + errorTokens[0] + "' expects '" +
                 errorTokens[3] + "'" + "' given '" + errorTokens[2] + "'";
    break;
  case LangError::PrivatePort:
    errorText += "Port '" + errorTokens[1] + "' of type '" + errorTokens[0] +
                 "' is private and cannot be accessed externally";
    break;
  case LangError::UnsizedPort:
    errorText += "'" + errorTokens[0] + "()' takes the size of its " +
                 errorTokens[1] + " from " + errorTokens[2] + " (" +
                 errorTokens[3] + ".size), so " + errorTokens[4];
    break;
  case LangError::HeaderPortsWithPortsProperty:
    errorText += errorTokens[0] + " '" + errorTokens[1] +
                 "' declares ports in its header and cannot also declare "
                 "'ports' in its body";
    break;
  case LangError::ReactionMainInput:
    errorText += errorTokens[0] + " '" + errorTokens[1] +
                 "' declares the main input port '" + errorTokens[2] +
                 "'. The input of a reaction or an action is the trigger that "
                 "runs it, which also sets the domain it runs in (a switch "
                 "converts to a trigger): pass values through property ports, "
                 "or make it a module";
    break;
  case LangError::CodeWithEntitiesOrStreams:
    errorText += errorTokens[0] + " '" + errorTokens[1] +
                 "' specifies 'code' and cannot also specify '" +
                 errorTokens[2] + "' in its body";
    break;
  case LangError::StreamMemberSizeMismatch:
    errorText += "Stream size mismatch. '" + errorTokens[1] + "' provides " +
                 errorTokens[0] + "outs , " + errorTokens[2] + " expected.";
    break;
  default:
    break;
  }
  return errorText;
}

static std::string symbolErrorText(LangError::ErrorType type,
                                   const std::vector<std::string> &errorTokens) {
  std::string errorText;
  switch (type) {
  case LangError::DuplicateSymbol:
    errorText += "Duplicate Symbol Error. '" + errorTokens[0] +
                 "' redefined in " + errorTokens[1] + ":" + errorTokens[2] + "";
    break;
  case LangError::ShadowedSymbol:
    errorText += "Shadowed Symbol Error. '" + errorTokens[0] + "' in '" +
                 errorTokens[1] + "' hides the declaration in " +
                 errorTokens[2] + ":" + errorTokens[3];
    break;
  case LangError::UndeclaredSymbol:
    errorText += "Undeclared Symbol '" + errorTokens[0] + "'";
    break;
  case LangError::SystemError:
    errorText += "Undeclared Symbol '" + errorTokens[0] + "'";
    break;
  case LangError::UnknownPlatform:
    errorText += "Unknown Platform '" + errorTokens[0] + "'";
    break;
  case LangError::ConstantWrittenTwice:
    errorText += "Constant '" + errorTokens[0] +
                 "' is written once and is already written by " +
                 errorTokens[1];
    break;
  case LangError::IgnoredRateInOneShotDomain:
    errorText += "Rate on '" + errorTokens[0] +
                 "' has no effect in one-shot domain '" + errorTokens[1] + "'";
    break;
  case LangError::UnresolvedRate:
    errorText += "Rate Unresolved for '" + errorTokens[0] + "'";
    break;
  case LangError::ConstraintFail:
    errorText += errorTokens[3] + " [Failed Constraint in Module '" +
                 errorTokens[0] + "' in " + errorTokens[1] + ":" +
                 errorTokens[2] + "]";
    break;
  case LangError::UndeclaredFeature:
    errorText += "Undeclared feature symbol '" + errorTokens[0] + "'";
    break;
  case LangError::InvalidFeatureVariant:
    errorText += "'" + errorTokens[1] +
                 "' is not a valid variant of feature '" + errorTokens[0] + "'";
    break;
  case LangError::NonExhaustiveProperty:
    errorText += "Non-exhaustive conditional property value for '" +
                 errorTokens[0] + "'";
    break;
  case LangError::AmbiguousProperty:
    errorText += "Ambiguous conditional property value for '" + errorTokens[0] +
                 "': multiple conditions matched";
    break;
  case LangError::InvalidCrossDomainAccess:
    errorText += "Signal '" + errorTokens[0] + "' in domain '" +
                 errorTokens[1] + "' cannot be accessed from domain '" +
                 errorTokens[2] +
                 "': domains do not share a common parent domain or connection";
    break;
  case LangError::None:
    errorText += "Unknown error";
    break;
  default:
    break;
  }
  return errorText;
}

std::string LangError::getErrorText() const {
  std::string errorText = "In file " + filename + ":\n    ";
  std::stringstream lineNumString;
  lineNumString << lineNumber;
  if (columnNumber > 0) {
    lineNumString << ":" << columnNumber;
  }
  errorText += "Line " + lineNumString.str() + " : ";
  if (severity == Warning) {
    errorText += "Warning: ";
  }
  errorText += typeErrorText(type, errorTokens);
  errorText += portErrorText(type, errorTokens);
  errorText += symbolErrorText(type, errorTokens);
  return errorText;
}
