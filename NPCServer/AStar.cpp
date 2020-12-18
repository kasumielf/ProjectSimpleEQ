#pragma once

#include <bitset>
#include "AStar.h"

bool AStar::NodeCompare(Node * n1, Node * n2)
{
    if (n1->dist < n2->dist) return true;
    if (n1->dist > n2->dist) return false;
    if (n1->depth <= n2->depth) return true;
    return false;
}

void AStar::InsertOpenNode(Node * node)
{
    for (int i = 0; i < open_node.size(); ++i)
    {
        if (open_node[i]->IsSame(node))
        {
            InsertCloseNode(open_node[i]);
            open_node[i] = node;
            return;
        }
    }

    open_node.push_back(node);
}

Node * AStar::InsertCloseNode(Node * node)
{
    close_node.push_back(node);
    return node;
}

void AStar::SortOpenNode()
{
    if (open_node.size() < 2) return;

    Node* pNode;

    bool cont = true;

    while (cont)
    {
        cont = false;

        for (int i = 0; i < open_node.size() - 1; ++i)
        {
            if (!NodeCompare(open_node[i], open_node[i + 1]))
            {
                pNode = open_node[i];
                open_node[i] = open_node[i + 1];
                open_node[i + 1] = pNode;
                cont = true;
            }
        }
    }
}

bool AStar::FindFromOpenNode(Node * node)
{
    int len = open_node.size();

    for (int i = 0; i < len; ++i)
    {
        if (open_node[i]->IsSame(node))
            return true;
    }
    return false;
}

bool AStar::FindFromCloseNode(Node * node)
{
    int len = close_node.size();

    for (int i = -0; i < len; ++i)
    {
        if (close_node[i]->IsSame(node))
            return true;
    }

    return false;
}

void AStar::Clear()
{
    for (auto node : open_node)
    {
        delete
            node;
        node = nullptr;
    }
    open_node.clear();

    for (auto node : close_node)
    {
        delete node;
        node = nullptr;
    }
    close_node.clear();
}

std::list<Node*> AStar::FindPath(Node * pStart, Node * pEnd, std::bitset<8> &closets)
{
    std::list<Node*> result;

    Clear();

    Node* pNode = pStart->Clone();

    open_node.push_back(pNode);

    int nDepth = 0;
    pNode->depth = nDepth;

    std::vector<Node*> vecchilds;

    while (true)
    {
        if (open_node.size() == 0)
            break;

        pNode = open_node[0];

        if (pEnd->IsSame(pNode))
        {
            while (pNode != nullptr)
            {
                if (pNode == nullptr)
                    break;

                result.push_front(pNode);
                pNode = pNode->parent;

            }

            return result;
        }
        open_node.erase(open_node.begin());

        pNode = InsertCloseNode(pNode);

        nDepth++;

        vecchilds.clear();


        for (int i = 0; i<8; i++)
        {
            if (closets[i] == false)
            {
                int x = pNode->x;
                int y = pNode->y;

                switch (i)
                {
                case 0: y--; break;
                case 1: x++; y--; break;
                case 2: x++; break;
                case 3: x++; y++; break;
                case 4: y++; break;
                case 5: x--; y++; break;
                case 6: x--; break;
                case 7: x--; y--; break;
                }
                Node* n = new Node(x, y);
                vecchilds.push_back(n);
            }
        }

        for (int i = 0; i < vecchilds.size(); ++i)
        {
            if (FindFromCloseNode(vecchilds[i]))
                continue;

            vecchilds[i]->CalcDist(pEnd, nDepth);
            vecchilds[i]->SetParent(pNode);
            InsertOpenNode(vecchilds[i]);
        }

        SortOpenNode();
    }

    Clear();

    return result;
}
