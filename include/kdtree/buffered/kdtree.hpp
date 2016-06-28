#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "kdtree_node.hpp"

namespace std
{
    template<>
    struct hash<std::array<int, 3>>
    {
        typedef std::array<int, 3> argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const
        {
            result_type const h1 ( std::abs(s[0]) );
            result_type const h2 ( std::abs(s[1]) );
            result_type const h3 ( std::abs(s[2]) );
            return h1 ^ (h2 << 10) ^ (h3 << 20);
        }
    };
}

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

    static_assert(std::is_default_constructible<DataType>::value,   "DataType not default constructible");
    static_assert(std::is_move_assignable<DataType>::value,         "DataType not move assignable");
    static_assert(std::is_default_constructible<IndexType>::value,  "IndexType not default constructible");
    static_assert(std::is_move_assignable<IndexType>::value,        "IndexType not move assignable");

public:
    KDTree(std::size_t capacity = DEFAULT_CAPACITY) :
        _capacity(capacity),
        _size(0),
        _nodes(capacity),
        _node_lookup(DEFAULT_CAPACITY)
    {
    }

    /// disallow copy
    KDTree(const KDTree&) = delete;
    KDTree& operator=(const KDTree&) = delete;

    inline void clear()
    {
        /// todo: clear vs reset each node state?
        _nodes.clear();
        for (std::size_t i = 0; i < _size; ++i)
            _nodes[i].clear();
        _node_lookup.clear();
        _size = 0;
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
        {
//            auto find = _node_lookup.find(index);
//            if (find != _node_lookup.end())
//                find->second->merge(std::move(data));
//            else
                sicker_insert(&(_nodes[0]), std::move(index), std::move(data));
        }
    }

    std::unordered_map<IndexType, DataType> aggregate;

    inline void prepare(IndexType index, DataType data)
    {
        auto find = aggregate.find(index);
        if (find != aggregate.end())
            find->second.merge(std::move(data));
        else
            aggregate.emplace(std::move(index), std::move(data));
    }

    inline void load()
    {
        for (std::pair<IndexType, DataType>&& pair : aggregate)
            insert(std::move(pair.first), std::move(pair.second));
        aggregate.clear();
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
//                _node_lookup[_nodes[_size + 0].index] = &(_nodes[_size + 0]);
//                _node_lookup[_nodes[_size + 1].index] = &(_nodes[_size + 1]);
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
    std::unordered_map<IndexType, NodeType*> _node_lookup;
};

}
}
