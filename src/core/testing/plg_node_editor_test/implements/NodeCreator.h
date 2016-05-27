
#ifndef __NODE_CREATOR_H__
#define __NODE_CREATOR_H__

#include "plugins/plg_node_editor/interfaces/i_node.hpp"
#include <string>

class NodeCreator
{
public:
    NodeCreator(const std::string &nodeClass)
        : m_nodeClass(nodeClass)
    {}
    ~NodeCreator(){}

    INode* createNode()
    {
        switch (m_nodeClass)
        {
        }
    }

private:
    const std::string& m_nodeClass;
};

#endif //__NODE_CREATOR_H__