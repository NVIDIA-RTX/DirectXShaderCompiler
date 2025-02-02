///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilCoopVector.cpp                                                        //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "dxc/DXIL/DxilCoopVector.h"
#include "dxc/DXIL/DxilInstructions.h"
#include "dxc/DXIL/DxilModule.h"
#include "dxc/DXIL/DxilOperations.h"
#include "dxc/DXIL/DxilShaderModel.h"
#include "dxc/DXIL/DxilUtil.h"
#include "dxc/Support/Global.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace hlsl {

DxilCoopVectorProperties
coopVector_helper::LoadInfoFromConstant(llvm::Constant *C) {
  DXASSERT(!isa<ConstantAggregateZero>(C),
           "otherwise, DxilCoopVectorProperties has invalid value");
  const ConstantStruct *CS = cast<ConstantStruct>(C);
  DXASSERT(CS->getType()->getNumElements() == 2,
           "otherwise, struct is not expected layout");
  DxilCoopVectorProperties info;
  info.compType = (DXIL::ComponentType)cast<ConstantInt>(CS->getOperand(0))
                      ->getLimitedValue();
  info.length =
      (uint32_t)cast<ConstantInt>(CS->getOperand(1))->getLimitedValue();
  return info;
}

Constant *
coopVector_helper::GetInfoConstantFromCoopVectorPtr(llvm::Value *coopVecPtr) {
  DXASSERT_NOMSG(isa<AllocaInst>(coopVecPtr));
  for (auto *U : coopVecPtr->users()) {
    Instruction *I = cast<Instruction>(U);
    DxilInst_CoopVector_Annotate annotate(I);
    if (annotate) {
      DXASSERT_NOMSG(isa<Constant>(annotate.get_coopvectorprops()));
      return cast<Constant>(annotate.get_coopvectorprops());
    }
  }
  return nullptr;
}

DxilCoopVectorProperties
coopVector_helper::GetInfoFromCoopVectorPtr(llvm::Value *coopVecPtr) {
  Constant *infoC =
      coopVector_helper::GetInfoConstantFromCoopVectorPtr(coopVecPtr);
  DXASSERT(infoC, "otherwise, no CoopVectorAnnotate call found for ptr");
  return coopVector_helper::LoadInfoFromConstant(infoC);
}

llvm::Constant *
coopVector_helper::GetAsConstant(const DxilCoopVectorProperties &info,
                                 llvm::StructType *infoTy) {
  LLVMContext &Ctx = infoTy->getContext();
  IntegerType *i8Ty = IntegerType::get(Ctx, 8);
  IntegerType *i32Ty = IntegerType::get(Ctx, 32);
  return ConstantStruct::get(cast<StructType>(infoTy),
                             {ConstantInt::get(i8Ty, (unsigned)info.compType),
                              ConstantInt::get(i32Ty, (unsigned)info.length)});
}

} // namespace hlsl
