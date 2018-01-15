//===--- Simple.h - Simple-specific Tool Helpers --------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SIMPLE_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SIMPLE_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace simple {

enum class FloatABI {
  Invalid,
  Soft,
  Hard,
};

FloatABI getSimpleFloatABI(const Driver &D, const llvm::opt::ArgList &Args);

void getSimpleTargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
                             std::vector<llvm::StringRef> &Features);

} // end namespace simple
} // end namespace target
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_SIMPLE_H
