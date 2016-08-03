#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <limits>
#include "index.hpp"
#include "kdtree_node.hpp"

namespace kdtree
{
namespace unbuffered
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
    typedef std::shared_ptr<TreeType>         Ptr;

    static constexpr std::size_t DEFAULT_BULK_BUCKETS   = 1024;

    static_assert(std::is_default_constructible<DataType>::value,   "DataType not default constructible");
    static_assert(std::is_move_assignable<DataType>::value,         "DataType not move assignable");
    static_assert(std::is_default_constructible<IndexType>::value,  "IndexType not default constructible");
    static_assert(std::is_move_assignable<IndexType>::value,        "IndexType not move assignable");

public:
    KDTree() :
        _size(0),
        _root(nullptr),
        _bulkload_buffer(DEFAULT_BULK_BUCKETS)
    {
        _max_index.fill(std::numeric_limits<typename IndexType::value_type>::min());
        _min_index.fill(std::numeric_limits<typename IndexType::value_type>::max());
    }

    virtual ~KDTree()
    {
        clear();
    }

    /// disallow copy
    KDTree(const KDTree&) = delete;
    KDTree& operator=(const KDTree&) = delete;

    inline void clear()
    {
        if (_root)
            clear_recursive(_root);
        _size = 0;
    }

    inline void insert(IndexType index, DataType data)
    {
        if (_size == 0)
        {
            _root = new NodeType(std::move(index), std::move(data));
            _size += 1;
        }
        else
        {
            sicker_insert(_root, std::move(index), std::move(data));
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

        return sicker_find(_root, index);
    }

    template<typename F>
    inline void traverse_leafs(const F& fun)
    {
        if (_root)
            traverse_leafs_recursive(fun, _root);
    }

    template<typename F>
    inline void traverse_nodes(F&& fun)
    {
        if (_root)
            traverse_nodes_recursive(fun, _root);
    }

    inline const NodeType* get_root() const
    {
        if (_size == 0)
            return nullptr;

        return _root;
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
                node->split(new NodeType(), new NodeType(), std::move(index), std::move(data));
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

    inline void clear_recursive(NodeType* root)
    {
        if (root->left)
            clear_recursive(root->left);
        if (root->right)
            clear_recursive(root->right);

        delete root;
    }

    template<typename F>
    inline void traverse_leafs_recursive(const F& fun, NodeType* node)
    {
        if (node->is_leaf())
            fun(*node);
        else
        {
            traverse_leafs_recursive(fun, node->left);
            traverse_leafs_recursive(fun, node->right);
        }
    }

    template<typename F>
    inline void traverse_nodes_recursive(const F& fun, NodeType* node)
    {
        fun(*node);
        if (!node->is_leaf())
        {
            traverse_nodes_recursive(fun, node->left);
            traverse_nodes_recursive(fun, node->right);
        }
    }

private:
    std::size_t _size;
    NodeType*   _root;

    IndexType   _min_index;
    IndexType   _max_index;

    std::unordered_map<IndexType, DataType> _bulkload_buffer;
};

}
}
