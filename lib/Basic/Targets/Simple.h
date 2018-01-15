//===--- Simple.h - Declare Simple target feature support -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Simple TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_SIMPLE_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_SIMPLE_H
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {
class LLVM_LIBRARY_VISIBILITY SimpleTargetInfo : public TargetInfo {
  static const TargetInfo::GCCRegAlias GCCRegAliases[];
  static const char *const GCCRegNames[];
  bool SoftFloat;

public:
  SimpleTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
      : TargetInfo(Triple), SoftFloat(false) {
    // resetDataLayout("E-m:e-p:32:32-i64:64-f128:64-n32-S64");  // big-endian
    resetDataLayout("e-m:e-p:32:32-i64:64-f128:64-n32-S64");
    // NetBSD / OpenBSD use long (same as llvm default); everyone else uses int.
    switch (getTriple().getOS()) {
    default:
      SizeType = UnsignedInt;
      IntPtrType = SignedInt;
      PtrDiffType = SignedInt;
      break;
    case llvm::Triple::NetBSD:
    case llvm::Triple::OpenBSD:
      SizeType = UnsignedLong;
      IntPtrType = SignedLong;
      PtrDiffType = SignedLong;
      break;
    }
    // Up to 32 bits are lock-free atomic, but we're willing to do atomic ops
    // on up to 64 bits.
    MaxAtomicPromoteWidth = 64;
    MaxAtomicInlineWidth = 32;
  }

  int getEHDataRegisterNumber(unsigned RegNo) const override {
    if (RegNo == 0)
      return 24;
    if (RegNo == 1)
      return 25;
    return -1;
  }

  bool handleTargetFeatures(std::vector<std::string> &Features,
                            DiagnosticsEngine &Diags) override {
    // Check if software floating point is enabled
    auto Feature = std::find(Features.begin(), Features.end(), "+soft-float");
    if (Feature != Features.end()) {
      SoftFloat = true;
    }
    return true;
  }
  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  bool hasFeature(StringRef Feature) const override;

  bool hasSjLjLowering() const override { return true; }

  ArrayRef<Builtin::Info> getTargetBuiltins() const override {
    // FIXME: Implement!
    return None;
  }
  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }
  ArrayRef<const char *> getGCCRegNames() const override;
  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override;
  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &info) const override {
    // FIXME: Implement!
    switch (*Name) {
    case 'I': // Signed 13-bit constant
    case 'J': // Zero
    case 'K': // 32-bit constant with the low 12 bits clear
    case 'L': // A constant in the range supported by movcc (11-bit signed imm)
    case 'M': // A constant in the range supported by movrcc (19-bit signed imm)
    case 'N': // Same as 'K' but zext (required for SIMode)
    case 'O': // The constant 4096
      return true;

    case 'f':
    case 'e':
      info.setAllowsRegister();
      return true;
    }
    return false;
  }
  const char *getClobbers() const override {
    // FIXME: Implement!
    return "";
  }

  // No Sparc V7 for now, the backend doesn't support it anyway.
  enum CPUKind {
    CK_GENERIC,
  } CPU = CK_GENERIC;

  enum CPUGeneration {
    CG_V1,
  };

  CPUGeneration getCPUGeneration(CPUKind Kind) const {
    switch (Kind) {
    case CK_GENERIC:
      return CG_V1;
    }
    llvm_unreachable("Unexpected CPU kind");
  }

  CPUKind getCPUKind(StringRef Name) const;

  bool isValidCPUName(StringRef Name) const override {
    return getCPUKind(Name) != CK_GENERIC;
  }

  bool setCPU(const std::string &Name) override {
    CPU = getCPUKind(Name);
    return CPU != CK_GENERIC;
  }
};
} // namespace targets
} // namespace clang
#endif // LLVM_CLANG_LIB_BASIC_TARGETS_SIMPLE_H
