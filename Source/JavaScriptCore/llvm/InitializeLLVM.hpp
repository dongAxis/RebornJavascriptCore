//
//  InitializeLLVM.hpp
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#ifndef InitializeLLVM_hpp
#define InitializeLLVM_hpp

#include "config.h"

#include "LLVMAPI.h"
#include <string>
#include <wtf/text/CString.h>

namespace JSC {
    
    typedef void (*LoggerFunc)(const char*, ...);
    typedef JSC::LLVMAPI* (*LLVMInitializerFunction)(LoggerFunc);
    
    bool initializeLLVM();
}


#endif /* InitializeLLVM_hpp */
