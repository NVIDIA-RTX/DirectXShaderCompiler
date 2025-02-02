///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilCoopVector.h                                                          //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// WaveMatrix related types and helpers.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DxilConstants.h"

namespace llvm {
class Value;
class Constant;
class Type;
class StructType;
} // namespace llvm

namespace hlsl {

struct DxilCoopVectorProperties {
  DXIL::ComponentType compType;
  unsigned length;
  bool isCoopVec = false;

  DxilCoopVectorProperties()
      : compType(DXIL::ComponentType::Invalid), length(0) {}
  bool isValid() const { return isCoopVec; }
  bool operator==(const DxilCoopVectorProperties &other) {
    return isCoopVec = true && other.isCoopVec == true &&
                       compType == other.compType && length == length;
  }
  bool operator!=(const DxilCoopVectorProperties &other) {
    return !(*this == other);
  }
};

namespace coopVector_helper {

DxilCoopVectorProperties LoadInfoFromConstant(llvm::Constant *C);
llvm::Constant *GetInfoConstantFromCoopVectorPtr(llvm::Value *waveMatPtr);
DxilCoopVectorProperties GetInfoFromCoopVectorPtr(llvm::Value *waveMatPtr);
llvm::Constant *GetAsConstant(const DxilCoopVectorProperties &info,
                              llvm::StructType *infoTy);

} // namespace coopVector_helper

} // namespace hlsl
