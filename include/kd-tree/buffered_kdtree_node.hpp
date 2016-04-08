#ifndef BUFFERED_KDTREE_NODE_HPP
#define BUFFERED_KDTREE_NODE_HPP

#include <array>
#include <memory>

namespace kdtree {
namespace buffered {
template<typename T, std::size_t Dim, typename Wrapped>
struct KDTreeNode {
    typedef std::array<T, Dim>           Index;
    typedef KDTreeNode<T, Dim, Wrapped>  KDTreeNodeType;
    typedef T Type;
    const static int Dimension = Dim;

    Index           index;
    KDTreeNodeType *left;
    KDTreeNodeType *right;
    KDTreeNodeType *parent;
    Wrapped         wrapped;
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
        wrapped(other.wrapped),
        depth(other.depth),
        pivot_dim(other.pivot_dim),
        cluster(other.cluster)
    {
    }


    KDTreeNode(KDTreeNodeType &&other) noexcept :
        index(std::move(other.index)),
        parent(std::move(other.parent)),
        wrapped(std::move(other.wrapped)),
        depth(std::move(other.depth)),
        pivot_dim(std::move(other.pivot_dim)),
        cluster(std::move(other.cluster))
    {
    }

    virtual ~KDTreeNode() noexcept
    {
    }

    KDTreeNodeType & operator = (const KDTreeNodeType &other)
    {
        index = other.index;
        parent = other.parent;
        wrapped = other.wrapped;
        depth = other.depth;
        pivot_dim = other.pivot_dim;
        cluster = other.cluster;
        left = nullptr;
        right = nullptr;
        return *this;
    }

    KDTreeNodeType & operator = (KDTreeNodeType &&other) noexcept
    {
        index = std::move(other.index);
        parent = std::move(other.parent);
        wrapped = std::move(other.wrapped);
        depth = std::move(other.depth);
        pivot_dim = std::move(other.pivot_dim);
        cluster = std::move(other.cluster);
        return *this;
    }

    inline bool isLeaf() const
    {
        return left == nullptr && right == nullptr;
    }

    inline bool equals(const KDTreeNodeType &other) const
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            if(other.index[i] != index[i])
                return false;
        }
        return true;
    }

    inline bool hasIndex(const Index &other_index) const
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            if(other_index[i] != index[i])
                return false;
        }
        return true;
    }

    inline bool equals(const KDTreeNodeType *other) const
    {
        for(std::size_t i = 0; i < Dim ; ++i) {
            if(other->index[i] != index[i])
                return false;
        }
        return true;
    }

    inline void overwrite(const KDTreeNodeType *other)
    {
        wrapped.overwrite(other->wrapped);
    }

    inline void clone() const
    {
        KDTreeNodeType node(*this);
        node.left = left;
        node.right = right;
        return node;
    }
};
}
}
#endif // BUFFERED_KDTREE_NODE_HPP
