//
//  DFGGraphWriter.hpp
//  JavaScriptCore
//
//  Created by axis on 2018/2/26.
//

#ifndef DFGGraphWriter_hpp
#define DFGGraphWriter_hpp

#include "config.h"

#if ENABLE(DFG_JIT)

#include <iostream>
#include <limits>
#include <string>
#include <wtf/text/UniquedStringImpl.h>
#include <wtf/StringPrintStream.h>
#include <wtf/FilePrintStream.h>

extern "C" {
    #include <fcntl.h>
}

enum GraphType {
    DOT,
    NEATO,
    FDP,
    SFDP,
    TWOPI,
    CIRCO
};

std::string getGraphTypeString(GraphType type) ;

namespace sys {
namespace path {
    
    enum FileType {
        SYSPATH_DIR,
        SYSPATH_REGULAR_FILE
    };
    
    std::string systemTempDirectory() ;
    bool checkPath(std::string path, FileType type);
    
}};

namespace JSC { namespace DFG {
    
class Graph;
struct BasicBlock;
struct Node;
    
class GraphWriter {
public:
    GraphWriter(std::string fileAbsPath) {
        outFileStream = WTF::FilePrintStream::open(fileAbsPath.c_str(), "w");
    }
    
    ~GraphWriter() {
    }
    
    void writeGraphHeader(std::string graphName);
    void writeGraphBody(Graph &m_graph);
    void writeGraphTail();
    void writeGraph(Graph &m_graph, GraphType type, std::string graphName="");
    
    inline std::string getPointerNumber(void *p) {
        char buf[32];
        bzero(buf, 32*sizeof(char));
        snprintf(buf, sizeof(buf), "Node%p", p);
    
        return std::string(buf);
    }
    
    void writeGraphNode(Graph &m_graph, BasicBlock &block);
    void writeGraphEdge(BasicBlock &block);
    
private:
    WTF::StringPrintStream              outStrStream;
    std::unique_ptr<FilePrintStream>    outFileStream;
};


/// Graph G : this is the graph of prepare optimized graph
/// WTF::CString fileName : file name of graph, but dose not contain the absolute path
/// GraphType type : type of graph
void viewCFGBaseOnGraph(Graph &m_graph,
                        std::string fileName,
                        GraphType type = DOT);
    
}}

#endif
#endif /* DFGGraphWriter_hpp */
