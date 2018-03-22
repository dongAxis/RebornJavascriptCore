//
//  initializeLLVMPOSIX.cpp
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#include "config.h"

extern "C" {
    #include <dlfcn.h>
}
#include <wtf/DataLog.h>

#include "initializeLLVMPOSIX.hpp"

namespace JSC {
    
    LLVMInitializerFunction getLLVMInitializerFunctionPOSIX(const char* library_path) {
        
        int dl_flags = RTLD_NOW;
        
        void *library = dlopen(library_path, dl_flags);
        if(!library) {
            dataLog("Failed to load LLVM library at ", library_path, ":", dlerror(), "\n");
            return nullptr;
        }
        
        const char *symbolName = "initializeAndGetJSCLLVMAPI";
        LLVMInitializerFunction initializer = bitwise_cast<LLVMInitializerFunction>
                                                                    (dlsym(library, symbolName));
        if(!initializer) {
            dataLog("failed to get the symbol ", symbolName, " from ", library_path, ":", dlerror());
            return nullptr;
        }
        
        return initializer;
    }
    
}
