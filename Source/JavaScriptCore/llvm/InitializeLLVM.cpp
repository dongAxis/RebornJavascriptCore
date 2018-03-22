//
//  InitializeLLVM.cpp
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#include "config.h"

#include "LLVMAPI.h"
#include <pthread.h>
#include <wtf/DataLog.h>

#include "InitializeLLVM.hpp"
#include "initializeLLVMPOSIX.hpp"


namespace JSC {
    
    static pthread_once_t initializeLLVMOnceKey = PTHREAD_ONCE_INIT;
    
    LLVMInitializerFunction getLLVMInitializeFunction() {
#warning "need to replace it with other path"
        return getLLVMInitializerFunctionPOSIX("/Users/axis/Library/Developer/Xcode/DerivedData/WebKit-bbvoodzfgqhvgsfluxynqdiyzlcn/Build/Products/Debug/libllvmForJSC.dylib");
    }
    
    void _WTFLogAlwaysAndCrash(const char* format, ...) {
        
    }
    
    static void initializeLLVMImpl() {
        
        LLVMInitializerFunction initializer = getLLVMInitializeFunction();
        if(!initializer)    return ;
        
        llvm = initializer(WTFLogAlwaysAndCrash);
        if(!llvm) {
            dataLog("llvm initialize failed...\n");
        }
        
    }
    
    bool initializeLLVM() {
        pthread_once(&initializeLLVMOnceKey, initializeLLVMImpl);
    }
}
