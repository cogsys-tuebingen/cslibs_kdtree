#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <assert.h>
#include <queue>
#include "kdtree_node.hpp"

namespace kdtree {
template<typename T, int Dim>
class KDTree {
public:
    typedef std::shared_ptr<KDTree<T,Dim>>      Ptr;
    typedef typename KDTreeNode<T,Dim>::Ptr     NodePtr;
    typedef typename KDTreeNode<T,Dim>::Index   NodeIndex;

    KDTree() :
        root(nullptr),
        leaves(0),
        nodes(0)
    {
    }

    inline void clear()
    {
        leaves = 0;
        nodes = 0;
        root.reset();
    }

    inline std::size_t leafCount()
    {
        return leaves;
    }

    inline std::size_t nodeCount()
    {
        return nodes;
    }

    inline NodePtr getRoot()
    {
        return root;
    }

    inline bool find(const NodeIndex &index,
                     NodePtr &node)
    {
        node.reset();
        if(root == nullptr)
            return false;
        findNodeRecursive(index, root, node);
        return node != nullptr;
    }


    inline void insertNode(const KDTreeNode<T, Dim> &node)
    {
        NodePtr ptr(new KDTreeNode<T, Dim>(node));
        insertNode(ptr);
    }

    inline void insertNode(const NodePtr &node)
    {
        /// no root of tree yet
        if(root == nullptr) {
            root = node;
            leaves = 1;
            nodes = 1;
        } else {
            insertNodeRecursive(node, root);
        }
    }

    inline void getLeaves(std::vector<NodePtr> &leaves,
                          const bool reset_cluster = false)
    {
        if(root)
            getLeavesRecursive(root, reset_cluster, leaves);
    }

    inline void getNodes(std::vector<NodePtr> &nodes)
    {
        if(root)
            getNodesRecursive(root, nodes);
    }

private:
    NodePtr     root;
    std::size_t leaves;
    std::size_t nodes;

    inline void insertNodeRecursive(const NodePtr &node,
                                    const NodePtr &current)
    {
        /// check wether new and old have the same index
        if(current->isLeaf()) {
            if(current->equals(node)) {
                /// node already exists with given index
                current->overwrite(node);
            } else {
                /// we need to split;
                current->split(node);
                ++leaves;
                nodes+=2;
            }
        } else {
//            assert(current->left != nullptr);
//            assert(current->right != nullptr);

            if(node->index[current->pivot_dim] < current->pivot_value) {
                insertNodeRecursive(node, current->left);
            } else {
                insertNodeRecursive(node, current->right);
            }
        }
    }

    inline void findNodeRecursive(const NodeIndex &index,
                                  const NodePtr   &current,
                                  NodePtr &node)
    {
        if(current == nullptr)
            return;
        if(current->isLeaf()) {
            if(current->hasIndex(index)) {
                node = current;
            }
        } else {
            if(index[current->pivot_dim] < current->pivot_value) {
                findNodeRecursive(index, current->left, node);
            } else {
                findNodeRecursive(index, current->right, node);
            }
        }
    }

    inline void getLeavesRecursive(const NodePtr &node,
                                   const bool reset_cluster,
                                   std::vector<NodePtr> &leaves)
    {
        if(node->isLeaf()) {
            if(reset_cluster)
                node->cluster = -1;
            leaves.emplace_back(node);
        } else {
            getLeavesRecursive(node->left, reset_cluster, leaves);
            getLeavesRecursive(node->right,reset_cluster, leaves);
        }

    }

    inline void getNodesRecursive(const NodePtr &node,
                                  std::vector<NodePtr> &nodes)
    {
        nodes.push_back(node);
        if(node->isLeaf()) {
            getNodesRecursive(node->right, nodes);
            getNodesRecursive(node->left, nodes);
        }
    }
};
}
#endif // KDTREE_HPP
