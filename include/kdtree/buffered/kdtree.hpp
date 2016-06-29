#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include "kdtree_node.hpp"

namespace std
{
    template<std::size_t N>
    struct hash<std::array<int, N>>
    {
        typedef std::array<int, N> argument_type;
        typedef std::size_t result_type;
        static constexpr auto BITS = sizeof(result_type) * 8;
        static constexpr auto SHIFT = BITS / N;

        inline result_type operator()(argument_type const& s) const
        {
            result_type h = std::abs(s[0]);
            for (std::size_t i = 1; i < N; ++i)
                h ^= std::abs(s[i]) << SHIFT;
            return h;
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

    static constexpr std::size_t DEFAULT_CAPACITY       = 320 * 240;
    static constexpr std::size_t DEFAULT_BULK_BUCKEST   = 1024;
    static constexpr std::size_t GROW_SLOW_THRESHOLD    = DEFAULT_CAPACITY * 8;
    static constexpr double      GROW_FAST_RATE         = 2;
    static constexpr double      GROW_SLOW_RATE         = 1.25;

    static_assert(std::is_default_constructible<DataType>::value,   "DataType not default constructible");
    static_assert(std::is_move_assignable<DataType>::value,         "DataType not move assignable");
    static_assert(std::is_default_constructible<IndexType>::value,  "IndexType not default constructible");
    static_assert(std::is_move_assignable<IndexType>::value,        "IndexType not move assignable");

public:
    KDTree(std::size_t capacity = DEFAULT_CAPACITY) :
        _capacity(std::max<std::size_t>(1, capacity)),
        _size(0),
        _nodes(new NodeType[_capacity]),
        _bulkload_buffer(DEFAULT_BULK_BUCKEST)
    {
        for (std::size_t i = 0; i < _capacity; ++i)
            _nodes[i] = NodeType();
    }

    /// disallow copy
    KDTree(const KDTree&) = delete;
    KDTree& operator=(const KDTree&) = delete;

    inline void clear()
    {
        for (std::size_t i = 0; i < _size; ++i)
            _nodes[i].clear();
        _size = 0;
    }

    inline void insert(IndexType index, DataType data)
    {
//        std::cout << "Size: " << _size << " Capacity: " << _capacity << std::endl;
        if (_size == 0)
        {
            _nodes[0].index = std::move(index);
            _nodes[0].data = std::move(data);

            _size += 1;
        }
        else
        {
            // at maximum we insert two nodes
            if (_size + 2 >= _capacity)
                grow();

            sicker_insert(&(_nodes[0]), std::move(index), std::move(data));
        }
    }

    inline void insert_bulk(IndexType index, DataType data)
    {
        auto find = _bulkload_buffer.find(index);
        if (find != _bulkload_buffer.end())
            find->second.merge(std::move(data));
        else
            _bulkload_buffer.emplace(std::move(index), std::move(data));
    }

    inline void load_bulk()
    {
        for (std::pair<IndexType, DataType>&& pair : _bulkload_buffer)
            insert(std::move(pair.first), std::move(pair.second));

        _bulkload_buffer.clear();
    }

    inline void clear_bulk()
    {
        _bulkload_buffer.clear();
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
//                if (_size + 2 >= _capacity)
//                    throw std::runtime_error("Should not happen...");

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

    inline void grow()
    {
        throw std::length_error("Capacity to small, resize not yet implemented");
//        std::size_t new_size;
//        if (_capacity < GROW_SLOW_THRESHOLD)
//            new_size = _capacity * GROW_FAST_RATE;
//        else
//            new_size = _capacity * GROW_SLOW_RATE;

//        std::cout << "Grow: " << new_size << " from " << _capacity << " " << _size << std::endl;

//        std::unique_ptr<NodeType[]> new_nodes(new NodeType[new_size]);
//        for (std::size_t i = 0; i < _capacity; ++i)
//            new_nodes[i] = std::move(_nodes[i]);

//        for (std::size_t i = _capacity; i < new_size; ++i)
//            new_nodes[i] = NodeType();

//        _nodes = std::move(new_nodes);

//        _capacity = new_size;
    }


private:
    std::size_t _capacity;
    std::size_t _size;
    std::unique_ptr<NodeType[]> _nodes;
    std::unordered_map<IndexType, DataType> _bulkload_buffer;
};

}
}
