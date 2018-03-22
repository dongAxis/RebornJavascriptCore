//
//  LLVMHeader.h
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#ifndef LLVMHeader_h
#define LLVMHeader_h

#include <wtf/Compiler.h>

#if COMPILER(CLANG)
#pragma clang diagnostic push
#endif

#include <llvm-c/Analysis.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/Core.h>
#include <llvm-c/Disassembler.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Initialization.h>
#include <llvm-c/Linker.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/IPO.h>
#include <llvm-c/Transforms/PassManagerBuilder.h>
#include <llvm-c/Transforms/Scalar.h>

#if COMPILER(CLANG)
#pragma clang diagnostic pop
#endif

#endif /* LLVMHeader_h */
