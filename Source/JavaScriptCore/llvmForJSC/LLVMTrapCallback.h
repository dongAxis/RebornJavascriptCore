//
//  LLVMTrapCallback.h
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#ifndef LLVMTrapCallback_h
#define LLVMTrapCallback_h

#include "LLVMAPI.h"

extern void (*g_llvmTrapCallback)(const char* message, ...) NO_RETURN;

namespace JSC {
    #include "LLVMHeader.h"
    #include <wtf/Compiler.h>

    extern LLVMAPI* initializeAndGetJSCLLVMAPI(void (*callback)(const char*, ...) NO_RETURN );
}

#endif /* LLVMTrapCallback_h */
