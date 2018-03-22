//
//  initializeLLVMPOSIX.hpp
//  JavaScriptCore
//
//  Created by axis on 2018/3/22.
//

#ifndef initializeLLVMPOSIX_hpp
#define initializeLLVMPOSIX_hpp

#include "config.h"
#include "InitializeLLVM.hpp"

namespace JSC {

    LLVMInitializerFunction getLLVMInitializerFunctionPOSIX(const char* library_path);

}
#endif /* initializeLLVMPOSIX_hpp */
