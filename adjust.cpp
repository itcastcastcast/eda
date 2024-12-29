#include "common.h"
#include "newcommon.h"
using namespace std;
void adjust()
{
    for (auto it = BUF2.begin(); it != BUF2.end(); )
    {
        Node* node = (Node*)*it;
        if (node->children.empty())
        {
            it = BUF2.erase(it); // 从 BUF2 中移除节点
            deleteNodeRecursively(node);
        }
        else
        {
            ++it;
        }
    }
}

void deleteNodeRecursively(Node* node)
{
    Node* parent = node->parent;
    // 从父节点的 children 中移除当前节点
    if (parent != nullptr)
    {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());
    }
    // 删除当前节点
    delete node;

    // 如果父节点的 children 也为空，递归删除父节点
    if (parent != nullptr && parent->children.empty())
    {
        deleteNodeRecursively(parent);
    }
}
