//
//  LLVMAPI.h
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#ifndef LLVMAPI_h
#define LLVMAPI_h

#include "config.h"

#include "LLVMAPIFunctions.h"
#include "LLVMHeader.h"

namespace JSC {
    struct LLVMAPI {
#define LLVM_API_FUNCTION_DECLARATION(returnType, name, signature)      \
        returnType (*name) signature;
        
        FOR_EACH_LLVM_API_FUNCTION(LLVM_API_FUNCTION_DECLARATION)
#undef LLVM_API_FUNCTION_DECLARATION
    };
    
    extern LLVMAPI* llvm;
}

#endif /* LLVMAPI_h */
