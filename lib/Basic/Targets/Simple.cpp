//===--- Simple.cpp - Implement Simple target feature support -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements Simple TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "Simple.h"
#include "Targets.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

const char *const SimpleTargetInfo::GCCRegNames[] = {
    "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",  "r8",  "r9",  "r10",
    "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", "r20", "r21",
    "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31"
};

ArrayRef<const char *> SimpleTargetInfo::getGCCRegNames() const {
  return llvm::makeArrayRef(GCCRegNames);
}

ArrayRef<TargetInfo::GCCRegAlias> SimpleTargetInfo::getGCCRegAliases() const {
  return None;
}

bool SimpleTargetInfo::hasFeature(StringRef Feature) const {
  return llvm::StringSwitch<bool>(Feature)
      .Case("softfloat", SoftFloat)
      .Case("sparc", true)
      .Default(false);
}

SimpleTargetInfo::CPUKind SimpleTargetInfo::getCPUKind(StringRef Name) const {
  return llvm::StringSwitch<CPUKind>(Name)
      .Case("generic", CK_GENERIC)
      .Default(CK_GENERIC);
}

void SimpleTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  DefineStd(Builder, "simple", Opts);
  Builder.defineMacro("__REGISTER_PREFIX__", "");

  if (SoftFloat)
    Builder.defineMacro("SOFT_FLOAT", "1");

  switch (getCPUGeneration(CPU)) {
  case CG_V1:
    Builder.defineMacro("__simple");
    if (getTriple().getOS() != llvm::Triple::Linux)
      Builder.defineMacro("__simple__");
    break;
  }
}
