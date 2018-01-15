//===--- Simple.cpp - Tools Implementations ---------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Simple.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Option/ArgList.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang;
using namespace llvm::opt;

simple::FloatABI simple::getSimpleFloatABI(const Driver &D,
                                        const ArgList &Args) {
  simple::FloatABI ABI = simple::FloatABI::Invalid;
  if (Arg *A = Args.getLastArg(clang::driver::options::OPT_msoft_float,
                               options::OPT_mhard_float,
                               options::OPT_mfloat_abi_EQ)) {
    if (A->getOption().matches(clang::driver::options::OPT_msoft_float))
      ABI = simple::FloatABI::Soft;
    else if (A->getOption().matches(options::OPT_mhard_float))
      ABI = simple::FloatABI::Hard;
    else {
      ABI = llvm::StringSwitch<simple::FloatABI>(A->getValue())
                .Case("soft", simple::FloatABI::Soft)
                .Case("hard", simple::FloatABI::Hard)
                .Default(simple::FloatABI::Invalid);
      if (ABI == simple::FloatABI::Invalid &&
          !StringRef(A->getValue()).empty()) {
        D.Diag(clang::diag::err_drv_invalid_mfloat_abi) << A->getAsString(Args);
        ABI = simple::FloatABI::Hard;
      }
    }
  }

  // If unspecified, choose the default based on the platform.
  // Only the hard-float ABI on Simple is standardized, and it is the
  // default. GCC also supports a nonstandard soft-float ABI mode, also
  // implemented in LLVM. However as this is not standard we set the default
  // to be hard-float.
  if (ABI == simple::FloatABI::Invalid) {
    ABI = simple::FloatABI::Hard;
  }

  return ABI;
}

void simple::getSimpleTargetFeatures(const Driver &D, const ArgList &Args,
                                   std::vector<StringRef> &Features) {
  simple::FloatABI FloatABI = simple::getSimpleFloatABI(D, Args);
  if (FloatABI == simple::FloatABI::Soft)
    Features.push_back("+soft-float");
}
