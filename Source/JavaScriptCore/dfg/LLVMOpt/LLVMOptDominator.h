//
//  LLVMOptDominator.hpp
//  JavaScriptCore
//
//  Created by axis on 2018/2/28.
//

#ifndef LLVMOPTDOMINATOR
#define LLVMOPTDOMINATOR

#include "config.h"

#if ENABLE(DFG_JIT)

#include <wtf/CommaPrinter.h>
#include <wtf/Vector.h>
#include <wtf/HashSet.h>
#include <wtf/text/UniquedStringImpl.h>
#include <wtf/StringPrintStream.h>
#include <wtf/FilePrintStream.h>
#include <wtf/Noncopyable.h>
#include <wtf/SingleRootGraph.h>
#include <map>

#include "DFGBasicBlock.h"
//#include "DFGGraph.h"

namespace JSC {
namespace DFG {

    class Graph;
    struct BasicBlock;
    
    template<typename T>
    class MapBlock {
        
    public:
        MapBlock() { this->m_vectors.clear(); }
        
        T& operator[](BlockIndex index) {
            return this->m_vectors[index];
        }
        
        T& operator[](BasicBlock &block) {
            return this->m_vectors[block.index];
        }
        
        const T& operator[](BasicBlock &block) const {
            return this->vectors[block.index];
        }
        
        T& operator[](BasicBlock *block) {
            return this->m_vectors[block->index];
        }
        
        const T& operator[](BasicBlock *block) const {
            return this->vectors[block->index];
        }
        
    private:
        WTF::Vector<T> m_vectors;
    };
    
    class CPS {
        
    public:
        typedef Graph       G;
        typedef BasicBlock* Node;
        template<typename T> using Map = MapBlock<T>;
        
        CPS(Graph &g) : m_graph(g) {  }
        
        CPS::Node root();
        BlockIndex numBlocks();
        DFG::Node::SuccessorsIterable successors(Node node) { return node->successors(); }
        PredecessorList predecessors(Node node) { return node->predecessors; }
        CPS::Node node(unsigned int index);
        unsigned int index(Node node) { return node->index; }
        
    private:
        Graph &m_graph;
    };
    
    template<typename Node, typename V>
    struct LLVMGraphNodeWithValue {
        
        LLVMGraphNodeWithValue() = default;
        LLVMGraphNodeWithValue(Node n, V v) : node(n), value(v) {}
        
        Node     node;
        V     value;
    };
    
    template<typename Node, typename V>
    class LLVMGraphNodeWorkList {
        
    public:
        LLVMGraphNodeWorkList() {}
        
        bool isEmpty() {
            return !!this->nodes.isEmpty();
        }
        
        void reset() {
            this->nodes.clear();
            this->seens.clear();
        }
        
        void forcePush(Node node, V value) {
            auto val = LLVMGraphNodeWithValue<Node, V>(node, value);
            this->forcePush(val);
        }
        
        void forcePush(LLVMGraphNodeWithValue<Node, V> &singleNode) {
            this->nodes.append(singleNode);
        }
        
        bool push(Node node, V value) {
            auto val = LLVMGraphNodeWithValue<Node, V>(node, value);
            return this->push(val);
        }
        
        bool push(LLVMGraphNodeWithValue<Node, V> &singleNode) {
            if(seens[singleNode.node] == 0xdeadbeef) {
                return false;
            }
            
            seens[singleNode.node] = 0xdeadbeef;
            this->nodes.append(singleNode);
            return false;
        }
        
        LLVMGraphNodeWithValue<Node, V>
        pop() {
            
            if(this->isEmpty()) {
                return LLVMGraphNodeWithValue<Node, V>();
            }
            
            return this->nodes.takeLast();
        }
        
    private:
        WTF::Vector<LLVMGraphNodeWithValue<Node, V>> nodes;
        std::map<Node, unsigned> seens;
    };
    
    template<typename T>
    class LLVMOptLengauerTarjan {
        
    public:
        LLVMOptLengauerTarjan(T &g) : m_graph(g) {
            this->nodeByPrenumber.clear();
            this->worklist.reset();
            
            for(BlockIndex index = 0; index < this->m_graph.numBlocks(); index++) {
                this->m_data[this->m_graph.node(index)].label = this->m_graph.node(index);
            }
        }
        
        inline void computeDominator() {
            // 1. write dfg number for each node...
            calculateNumberBasedOnDFS();
            
            // 2. calculate the semi and idom
            calculateSemiAndImplicitlyiDom();
        }
        
        typename T::Node eval(typename T::Node node) {
            
            if(this->m_data[node].ancestor == NULL) {
                return node;
            }
            
            this->compress(node);
            return this->m_data[node].label;
        }
        
        void compress(typename T::Node initializeBlock) {
            
            typename T::Node ancestor = this->m_data[initializeBlock].ancestor;
            if(!this->m_data[ancestor].ancestor) {
                return ;
            }
            
            WTF::Vector<typename T::Node, 16> stack;
            for(typename T::Node block = initializeBlock;
                block;
                block = this->m_data[block].ancestor) {
                stack.append(block);
            }
            
            for(uint64_t index = stack.size()-2; index--; ) {
                typename T::Node block = stack[index];
                typename T::Node& labelOfBlock = this->m_data[block].label;
                typename T::Node& ancestorOfBlock = this->m_data[block].ancestor;
                typename T::Node labelOfAncestorOfBlock = this->m_data[ancestorOfBlock].label;
                
                if(this->m_data[labelOfAncestorOfBlock].semiNumber < this->m_data[ancestorOfBlock].semiNumber)
                    labelOfBlock = labelOfAncestorOfBlock;
                
                ancestorOfBlock = this->m_data[ancestorOfBlock].ancestor;;
            }
        }
        
        // for G(node, edge, r), we can add an edge for node w and node v
        void link(typename T::Node from, typename T::Node to) {
            this->m_data[to].ancestor = from;
        }
        
        void calculateSemiAndImplicitlyiDom() {
            for(unsigned int index = this->nodeByPrenumber.size()-1; index > 2; index--) {
                
                typename T::Node vertex = this->nodeByPrenumber[index];
                
                // [step2]: try to calculate the value of semi-dominator for each node
                // the following comments are from papers:
                // compute the semidominators of all vertices by applying Theorem 4. Carry out the computation
                // veriex by vertex in decreasing order by number
                // **** Theorem 4 ****
                // for any vertex w != r,
                // sdom(w) = min({v | (v, w) E E, and v < w} and {sdom(u) | u > w and there is an edge(v, w) such that u->v})
                for(uint64_t j = 0; j < vertex->predecessors.size(); j++) {
                    typename T::Node w = vertex->predecessors[j];
                    typename T::Node tmpNode = this->eval(w);
                    if(this->m_data[tmpNode].semiNumber < this->m_data[vertex].semiNumber) {
                        this->m_data[vertex].semiNumber = this->m_data[tmpNode].semiNumber;
                    }
                }
                
                unsigned int semi = this->m_data[vertex].semiNumber;
                this->m_data[this->nodeByPrenumber[semi]].bucket.append(vertex);            // for vertext's semi dominator is "{this->nodeByPrenumber[semi]}"
                this->link(this->m_data[vertex].parent, vertex);
                
                // step3: try to get the implicitility idom for each node
                // implicitly define the immediate dominator of each vertex by applying corollary
                
            }
        }
        
        void calculateNumberBasedOnDFS() {
            
//            // 0. declare the global counter..
//            uint64_t counter = 0;
            
            // 1. push root node into worklist
            this->worklist.reset();
            this->worklist.push(this->m_graph.root(), 0);
//            this->nodeByPrenumber.append(this->m_graph.root());
            
            while(!this->worklist.isEmpty()) {
                
                // 2. pop a node and
                LLVMGraphNodeWithValue<typename T::Node, uint64_t> item = this->worklist.pop();
                typename T::Node node = item.node;
                uint64_t successorIndex = item.value;
                
                if(!successorIndex) {
                    this->m_data[node].semiNumber = this->nodeByPrenumber.size();
                    this->nodeByPrenumber.append(node);
                }
                
                // 3. push successtor
                if(successorIndex < this->m_graph.successors(node).size()) {
                    uint64_t nextSuccessorIndex = successorIndex + 1;
                    
                    // add node to stack again for revisiting the rest nodes
                    if(nextSuccessorIndex < this->m_graph.successors(node).size()) {
                        this->worklist.forcePush(node, nextSuccessorIndex);
                    }
                    
                    // add new block to the list
                    typename T::Node successBlock = this->m_graph.successors(node)[successorIndex];
                    if(this->worklist.push(successBlock, 0)) {
                        m_data[successBlock].parent = node;
                    }
                }
            }
        }
        
        struct BlockData {
            
            BlockData()
                : parent(nullptr)
                , preNumber(0)
                , semiNumber(0)
                , ancestor(nullptr)
                , label(nullptr)
                , dom(nullptr) {
                    this->bucket.clear();
            }
            
            typename T::Node  parent;
            unsigned preNumber;
            unsigned semiNumber;
            typename T::Node  ancestor;
            typename T::Node  label;
            WTF::Vector<typename T::Node> bucket;
            typename T::Node dom;
        };
        
    private:
        T &m_graph;
        WTF::Vector<typename T::Node>  nodeByPrenumber;
        typename T::template Map<BlockData> m_data;
        LLVMGraphNodeWorkList<typename T::Node, uint64_t> worklist;
    };
    
    class LLVMOptDominator {
    public:
        LLVMOptDominator(Graph &g) : m_graph(g) {
            
            CPS cps(this->m_graph);
            
            LLVMOptLengauerTarjan<CPS> lengauerTarjan(cps);
            lengauerTarjan.computeDominator();
            
        }
    private:
        Graph &m_graph;
    };
    
}
}

#endif /* LLVMOptDominator_hpp */
#endif

