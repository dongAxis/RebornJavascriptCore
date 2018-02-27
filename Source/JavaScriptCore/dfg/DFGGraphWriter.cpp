//
//  DFGGraphWriter.cpp
//  JavaScriptCore
//
//  Created by axis on 2018/2/26.
//

extern "C" {
    #include <fcntl.h>
}
#include "DFGGraphWriter.h"
#include "DFGCommon.h"
#include "DFGGraph.h"
#include "DFGBasicBlockInlines.h"

namespace sys {
namespace path {
    
    std::string systemTempDirectory() {
        const char *enviromentvariables[] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};
        
        for(const char *env : enviromentvariables) {
            if(const char *dir = std::getenv(env)) {
                return dir;
            }
        }
        
        ASSERT("can not find the temp directory");
        return "";
    }
    
//    bool fileIsExits(std::string path) {
//        int ok = 0;
//        if((ok = access(path.c_str(), F_OK)) == 0) {
//            return true;
//        }
//        return false;
//    }
    
    bool checkPath(std::string path __unused, FileType type) {
        
        switch (type) {
            case SYSPATH_DIR:
                break;
                
            case SYSPATH_REGULAR_FILE:
//                return fileIsExits(path);
                
            default:
                break;
        }
        
        return false;
    }
    
}}

std::string getGraphTypeString(GraphType type) {
    
    switch (type) {
        case DOT:
            return std::string("dot");
            
        case NEATO:
            return std::string("neato");
            
        case FDP:
            return std::string("fdp");
            
        case SFDP:
            return std::string("sfdp");
            
        case TWOPI:
            return std::string("twopi");
            
        case CIRCO:
            return std::string("circo");
            
        default:
            return std::string("nonetype");
    }
    
}

namespace JSC { namespace DFG {
    
    void GraphWriter::writeGraphHeader(std::string graphName __unused) {
        outFileStream->print("digraph \"", graphName.c_str() , "\" {");
    }
    
    void GraphWriter::writeGraphBody(Graph &m_graph __unused) {
        
        for(BlockIndex i = 0; i < m_graph.numBlocks(); i++) {
            BasicBlock *block = m_graph.block(i);
            if(block == NULL)
                continue;

            // 1. write node
            this->writeGraphNode(m_graph, *block);
            
            // 2. write edge
            this->writeGraphEdge(*block);
        }
    }
    
    
    void GraphWriter::writeGraphTail() {
        outFileStream->print(" } ");
    }
    void GraphWriter::writeGraph(Graph &m_graph __unused, GraphType type __unused, std::string graphName) {
        // 1. write header
        this->writeGraphHeader(graphName);
        
        // 2. write body
        this->writeGraphBody(m_graph);
        
        // 3. write tail
        this->writeGraphTail();
    }

    void GraphWriter::writeGraphNode(Graph &m_graph, BasicBlock &block) {
        
        // 1. declare of node
        outFileStream->print("\t", this->getPointerNumber(&block).c_str(), " [shape=record, width=60");
        
        // 2. now we try to declare the label
        outFileStream->print("label=\"{");
        
        // 3. dump the ins of the basic block
        {
            outStrStream.reset();
            
            for(size_t j=0; j<block.size(); j++) {
                m_graph.dump(this->outStrStream, "", block.at(j));
                this->outStrStream.print("\\r");
            }
            
            WTF::String cstr = this->outStrStream.toString();
            cstr = cstr.replace("\n", "\\l");
            cstr = cstr.replace("\r", "");
            
            WTF::String newStr("");
            
            for(unsigned int i=0; i < cstr.length(); i++) {
                
                if(cstr[i] == '-') {
                    newStr.append("\\");
                }
                else if(cstr[i] == '>') {
                    newStr.append("\\");
                }
                else if(!((cstr[i] >= 'a' && cstr[i] <= 'z') ||
                        (cstr[i] >= 'A' && cstr[i] <= 'Z') ||
                        (cstr[i] >= '0' && cstr[i] <= '9'))) {
                    if(cstr[i] != '\\') {
                        newStr.append("\\");
                    }
                }
                else {
                }
            
                newStr.append(cstr[i]);
            }
            
            this->outFileStream->print(newStr);
        }
        
        // 4. tail part of node
        this->outFileStream->print("}\"];\n");
    }
    
    void GraphWriter::writeGraphEdge(BasicBlock &block) {
        
        std::string srcNodeName = this->getPointerNumber(&block);
        
        for(unsigned int i = 0;
            i < block.numSuccessors(); i++) {
            BasicBlock *succ = block.successor(i);
            
            // write edge right away
            outFileStream->print(srcNodeName.c_str(), "->", this->getPointerNumber(succ).c_str(), ";\n");
        }
        
    }
    
    void viewCFGBaseOnGraph(Graph &m_graph,
                            std::string fileName,
                            GraphType type) {
        // 1. check the length of file name .
        //    for now, we only allow 140 bytes as maxinum
        std::string fileShortName = std::string(fileName.data());
        fileShortName = fileShortName.substr(0, std::min<std::size_t>(fileShortName.size(), 140));
        
        // 2. get the tmp os path:
        std::string absPath = sys::path::systemTempDirectory();
        
        // 3. construct the abs path
        absPath = absPath + "/" + fileShortName;
        
        // 4. open temp file for reading
        //    int fd __unused = open(absPath.c_str(), O_RDWR | O_CREAT);
        
        // 5. write graph
        GraphWriter graphWriter(absPath);
        graphWriter.writeGraph(m_graph, type/*, m_graph.getCodeBlockInnerName().data()*/);
    }
    
} }
