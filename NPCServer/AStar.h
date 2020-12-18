#pragma once

#include <list>
#include "Node.h"
#include <vector>
#include <bitset>

class AStar
{
private:
    std::vector<Node*> open_node;
    std::vector<Node*> close_node;

    bool NodeCompare(Node* n1, Node* n2);
    void InsertOpenNode(Node* node);
    Node* InsertCloseNode(Node* node);
    void SortOpenNode();
    bool FindFromOpenNode(Node* node);
    bool FindFromCloseNode(Node* node);
public:
    ~AStar()
    {
        Clear();
    }

    void Clear();
    std::list<Node*> FindPath(Node* pStart, Node* pEnd, std::bitset<8> &closets);
};

