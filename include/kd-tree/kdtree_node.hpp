#ifndef KDTREE_NODE_HPP
#define KDTREE_NODE_HPP

#include <array>
#include <memory>

namespace kdtree {
template<typename T, int Dim>
struct KDTreeNode {
    typedef std::array<T, Dim>              Index;
    typedef KDTreeNode<T, Dim>              KDTreeNodeType;
    typedef std::shared_ptr<KDTreeNodeType> Ptr;

    Index           index;
    Ptr             left;
    Ptr             right;
    KDTreeNodeType *parent;
    std::size_t depth;
    int    pivot_dim;
    double pivot_value;
    int    cluster;

    KDTreeNode() :
        left(nullptr),
        right(nullptr),
        parent(nullptr),
        depth(0),
        pivot_dim(-1),
        pivot_value(0.0),
        cluster(-1)
    {
    }

    KDTreeNode(const KDTreeNodeType &other) :
        index(other.index),
        parent(other.parent),
        depth(other.depth),
        pivot_dim(other.pivot_dim),
        cluster(other.cluster)
    {
    }

    virtual inline Ptr clone() const
    {
        Ptr node(new KDTreeNodeType(*this));
        node->left = left;
        node->right = right;
        return node;
    }

    virtual inline Ptr copy() const
    {
        return Ptr(new KDTreeNodeType(*this));
    }

    inline bool isLeaf() const
    {
        return left == nullptr && right == nullptr;
    }

    inline bool equals(const KDTreeNodeType &other) const
    {
        for(int i = 0 ; i < Dim ; ++i) {
            if(other.index[i] != index[i])
                return false;
        }
        return true;
    }

    inline bool hasIndex(const Index &other_index) const
    {
        for(int i = 0 ; i < Dim ; ++i) {
            if(other_index[i] != index[i])
                return false;
        }
        return true;
    }

    inline bool equals(const Ptr &other) const
    {
        for(int i = 0; i < Dim ; ++i) {
            if(other->index[i] != index[i])
                return false;
        }
        return true;
    }

    inline virtual void overwrite(const Ptr &other)
    {
    }

    inline virtual void split(const Ptr &other) {
        T max_split = 0;
        pivot_dim = -1;
        for(int i = 0 ; i < Dim; ++i) {
            T split = abs(index[i] - other->index[i]);
            if(split > max_split) {
                max_split = split;
                pivot_dim = i;
            }
        }
        pivot_value = (index[pivot_dim] + other->index[pivot_dim]) / 2.0;
        if(index[pivot_dim] < pivot_value) {
            left  = copy();
            ++(left->depth);
            right = other;
            ++(right->depth);
            left->parent = this;
            right->parent = this;
        } else {
            left  = other;
            ++(left->depth);
            right = copy();
            ++(right->depth);
            left->parent = this;
            right->parent = this;
        }
    }
};
}
#endif // KDTREE_NODE_HPP
