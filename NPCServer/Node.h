#pragma once

struct Node
{
    int x;
    int y;
    int dist;
    int depth;
    Node* parent;

    Node() { parent = nullptr; }
    Node(int sx, int sy, int dx, int dy, int dep)
    {
        x = sx;
        y = sy;

        int deltx = dx - sx;
        int delty = dy - sy;

        dist = (deltx * deltx) + (delty * delty);
        depth = dep;
        parent = nullptr;
    }

    Node(int sx, int sy)
    {
        x = sx;
        y = sy;
        parent = nullptr;
    }

    void CalcDist(Node* node, int _depth)
    {
        int deltx = node->x - x;
        int delty = node->y - y;

        dist = (deltx * deltx) + (delty * delty);
        depth = _depth;
    }

    void SetParent(Node* n) { parent = n; }
    Node* GetParent() const { return parent; }

    Node* Clone()
    {
        Node* new_node = new Node();

        new_node->x = x;
        new_node->y = y;
        new_node->dist = dist;
        new_node->depth = depth;
        new_node->parent = parent;

        return new_node;
    }

    void Copy(Node& node)
    {
        node.x = x;
        node.y = y;
        node.dist = dist;
        node.depth = depth;
        node.parent = parent;
    }

    bool IsSame(Node* node)
    {
        if (node->x != x || node->y != y) return false;

        return true;
    }
};
