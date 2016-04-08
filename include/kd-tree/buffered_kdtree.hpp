#ifndef BUFFERED_KDTREE_HPP
#define BUFFERED_KDTREE_HPP

#include <assert.h>
#include <queue>
#include <stdio.h>
#include <cstring>
#include "buffered_kdtree_node.hpp"

namespace kdtree {
namespace buffered {
/// size should be 2n+1 where n is the sample size
template<typename NodeType>
class KDTree {
public:
    typedef std::shared_ptr<KDTree<NodeType>> Ptr;
    typedef typename NodeType::Index   NodeIndex;

    KDTree(const std::size_t buffer_size) :
        buffer_(new NodeType[buffer_size]),
        buffer_size_(buffer_size),
        leaves_(0),
        nodes_(0)
    {
    }

    virtual ~KDTree()
    {
        delete[] buffer_;
    }

    inline void clear()
    {
        leaves_ = 0;
        nodes_ = 0;
        std::memset(buffer_, 0, sizeof(NodeType) * buffer_size_);
    }

    inline std::size_t leafCount()
    {
        return leaves_;
    }

    inline std::size_t nodeCount()
    {
        return nodes_;
    }

    inline NodeType* getRoot()
    {
        return buffer_;
    }

    inline NodeType* find(const NodeIndex &index)
    {
        if(nodes_ == 0)
            return nullptr;

       return findNodeRecursive(index, buffer_);

    }

    inline void insertNode(const NodeType &node)
    {
        /// no root of tree yet
        if(nodes_ == 0) {
            (*buffer_) = NodeType();
            (*buffer_) = node;
            leaves_ = 1;
            nodes_ = 1;
        } else {
            insertNodeRecursive(node, buffer_);
        }
    }

    inline void getLeaves(std::vector<NodeType*> &leaves,
                          const bool reset_cluster = false)
    {
        if(nodes_ > 0)
            getLeavesRecursive(buffer_, reset_cluster, leaves);
    }

    inline void getNodes(NodeType** nodes)
    {
        NodeType *ptr = buffer_;
        for(std::size_t i = 0 ; i < nodes_ ; ++i, ++ptr)
            nodes[i] = ptr;
    }

    inline void getNodes(std::vector<NodeType*> nodes)
    {
        NodeType *ptr = buffer_;
        for(std::size_t i = 0 ; i < nodes_ ; ++i, ++ptr)
            nodes.emplace_back(ptr);
    }

private:
    NodeType             *buffer_;
    std::size_t           buffer_size_;
    std::size_t leaves_;
    std::size_t nodes_;

    inline void insertNodeRecursive(const NodeType &node,
                                    NodeType *current)
    {
        /// check wether new and old have the same index
        if(current->isLeaf()) {
            if(current->equals(node)) {
                /// node already exists with given index
                current->wrapped.overwrite(node.wrapped);
            } else {
                if(nodes_ + 2 <= buffer_size_) {
                    splitNode(node, current, buffer_ + nodes_, buffer_ + nodes_ + 1);
                    ++leaves_;
                    nodes_+=2;
                }
            }
        } else {
            assert(current->left != nullptr);
            assert(current->right != nullptr);

            if(node.index[current->pivot_dim] < current->pivot_value) {
                insertNodeRecursive(node, (NodeType*) current->left);
            } else {
                insertNodeRecursive(node, (NodeType*) current->right);
            }
        }
    }

    inline NodeType* findNodeRecursive(const NodeIndex &index,
                                       NodeType *current)
    {
        if(current == nullptr)
            return nullptr;
        if(current->isLeaf()) {
            if(current->hasIndex(index)) {
                return current;
            }
        } else {
            if(index[current->pivot_dim] < current->pivot_value) {
                return findNodeRecursive(index, current->left);
            } else {
                return findNodeRecursive(index, current->right);
            }
        }
        return nullptr;
    }

    inline void getLeavesRecursive(NodeType *node,
                                   const bool reset_cluster,
                                   std::vector<NodeType*> &leaves)
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

    inline void getNodesRecursive(const NodeType *node,
                                  std::vector<const NodeType*> &nodes)
    {
        nodes.push_back(node);
        if(node->right)
            getNodesRecursive(node->right, nodes);
        if(node->left)
            getNodesRecursive(node->left, nodes);
    }

    inline void splitNode(const NodeType &other,
                          NodeType *current,
                          NodeType *left,
                          NodeType *right)
    {
        NodeType &node = *current;
        new (left) NodeType;
        new (right) NodeType;

        typename NodeType::Type max_split = 0;
        node.pivot_dim = -1;
        for(int i = 0 ; i < NodeType::Dimension; ++i) {
            typename NodeType::Type split = abs(node.index[i] - other.index[i]);
            if(split > max_split) {
                max_split = split;
                node.pivot_dim = i;
            }
        }
        node.pivot_value = (node.index[node.pivot_dim] + other.index[node.pivot_dim]) / 2.0;
        if(node.index[node.pivot_dim] < node.pivot_value) {
            (*left) = node;
            node.left = left;
            ++(node.left->depth);
            (*right) = other;
            node.right = right;
            ++(node.right->depth);
            node.left->parent  = current;
            node.right->parent = current;
        } else {
            (*left) = other;
            node.left = left;
            ++(node.left->depth);
            (*right) = node;
            node.right = right;
            ++(node.right->depth);
            node.left->parent  = current;
            node.right->parent = current;
        }
    }

};
}
}
#endif // BUFFERED_KDTREE_HPP
