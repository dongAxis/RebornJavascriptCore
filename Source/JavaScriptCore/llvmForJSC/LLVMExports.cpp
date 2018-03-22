//
//  llvmForJSC.m
//  llvmForJSC
//
//  Created by axis on 2018/3/22.
//

#include "LLVMHeader.h"
#include "LLVMTrapCallback.h"

#include <wtf/Compiler.h>
#include <wtf/FastMalloc.h>

extern "C" {
#include <stdlib.h>
}

#if COMPILER(CLANG)
#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

#include <llvm/Support/CommandLine.h>

#if COMPILER(CLANG)
#pragma clang diagnostic pop
#endif

static void llvmCrash(const char *reason) NO_RETURN ;

template <typename... Args>
void initCommandLine(Args... args) {
    const char* theArgs[] = { args... };
    llvm::cl::ParseCommandLineOptions(sizeof(theArgs)/sizeof(char*), theArgs);
}

static void llvmCrash(const char *reason) {
    g_llvmTrapCallback("LLVM fatal error: %s", reason);
}

extern "C" JSC::LLVMAPI* initializeAndGetJSCLLVMAPI(
           void (*callback)(const char*, ...) NO_RETURN )  {
    
    // 1. register global error handling
    g_llvmTrapCallback = callback;
    
    // 2. register global error callback for LLVM itself
    LLVMInstallFatalErrorHandler(llvmCrash);
    
    // 3. init MC jit engine
    LLVMLinkInMCJIT();
    
    // 4. register os information for MC jit engine.
    LLVMInitializeX86TargetInfo();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmPrinter();
    LLVMInitializeX86Disassembler();
    
    // 5. use specific command line to notify the MC engine
    initCommandLine("llvmForJSC.dylib", "-enable-patchpoint-liveness=true");
    
    // 6. initialize LLVM api for jsc wrapper
    JSC::LLVMAPI *result = (JSC::LLVMAPI*)malloc(sizeof(JSC::LLVMAPI));  //bitwise_cast<JSC::LLVMAPI*>(fastMalloc(sizeof(JSC::LLVMAPI)));
    memset(result, 0x00, sizeof(JSC::LLVMAPI));
    
#define LLVM_API_FUNCTION_ASSIGNMENT(returnType, name, signature)       \
    result->name = LLVM##name;
    FOR_EACH_LLVM_API_FUNCTION(LLVM_API_FUNCTION_ASSIGNMENT)
#undef LLVM_API_FUNCTION_ASSIGNMENT
    
    return result;
}

