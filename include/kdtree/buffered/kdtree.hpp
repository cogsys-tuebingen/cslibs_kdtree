#pragma once

#include <vector>
#include <stdexcept>
#include "kdtree_node.hpp"

namespace kdtree
{
namespace buffered
{

template<typename ITraits, typename DType>
class KDTree
{
public:
    typedef ITraits                           IndexTraits;
    typedef typename ITraits::Type            IndexType;
    typedef DType                             DataType;
    typedef KDTree<IndexTraits, DataType>     TreeType;
    typedef KDTreeNode<IndexTraits, DataType> NodeType;

    static constexpr std::size_t DEFAULT_CAPACITY = 1024;

public:
    KDTree(std::size_t capacity = DEFAULT_CAPACITY) :
        _capacity(capacity),
        _size(0),
        _nodes(capacity)
    {
    }

    // disallow copy
    KDTree(const KDTree&) = delete;
    KDTree& operator=(const KDTree&) = delete;

    void clear()
    {
        _size = 0;
        /// todo: clear vs reset each node state?
        _nodes.clear();
    }

    inline void insert(IndexType index, DataType data)
    {
        if (_size == 0)
        {
            _nodes[0].index = std::move(index);
            _nodes[0].data = std::move(data);

            _size += 1;
        }
        else
            sicker_insert(&(_nodes[0]), std::move(index), std::move(data));
    }

    inline NodeType* find(const IndexType& index)
    {
        if (_size == 0)
            return nullptr;

        return sicker_find(&(_nodes[0]), index);
    }

    template<typename F>
    inline void traverse_leafs(F&& fun)
    {
        for (std::size_t i = 0; i < _size; ++i)
        {
            NodeType& node = _nodes[i];
            if (!node.is_leaf())
                continue;

            fun(node);
        }
    }

private:
    inline void sicker_insert(NodeType* node, IndexType&& index, DataType&& data)
    {
        if (node->is_leaf())
        {
            if (node->equals(index))
                node->merge(std::move(data));
            else
            {
                if (_size + 2 >= _capacity)
                    throw std::length_error("Capacity to small, resize not yet implemented");

                node->split(&(_nodes[_size + 0]), &(_nodes[_size + 1]), std::move(index), std::move(data));
                _size += 2;
            }
        }
        else
        {
            if (node->check_split(index))
                sicker_insert(node->left, std::move(index), std::move(data));
            else
                sicker_insert(node->right, std::move(index), std::move(data));
        }
    }

    inline NodeType* sicker_find(NodeType* node, const IndexType& index)
    {
        if (node == nullptr)
            return nullptr;

        if (node->is_leaf())
        {
            if (node->equals(index))
                return node;
            else
                return nullptr;
        }
        else
        {
            if (node->check_split(index))
                return sicker_find(node->left, index);
            else
                return sicker_find(node->right, index);
        }
    }


private:
    std::size_t _capacity;
    std::size_t _size;
    std::vector<NodeType> _nodes;
};

}
}
