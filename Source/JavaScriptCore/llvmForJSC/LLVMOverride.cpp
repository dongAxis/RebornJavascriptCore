//
//  LLVMOverride.cpp
//  llvmForJSC
//
//  Created by axis on 2018/3/22.
//

#include "LLVMTrapCallback.h"

void (*g_llvmTrapCallback)(const char* message, ...) NO_RETURN;
