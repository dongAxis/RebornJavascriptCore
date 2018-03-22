//
//  LLVMOptDominator.cpp
//  JavaScriptCore
//
//  Created by axis on 2018/2/28.
//

#include "LLVMOptDominator.h"
#include "DFGGraph.h"

namespace JSC {
namespace DFG {
    
    CPS::Node  CPS::node(unsigned int index) { return (m_graph.block(index)); }
    CPS::Node  CPS::root() { return m_graph.block(0); }
    BlockIndex CPS::numBlocks() { return this->m_graph.numBlocks(); }
}}
