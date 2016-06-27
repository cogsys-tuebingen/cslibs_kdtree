#pragma once

#include <cstdint>

namespace kdtree
{
namespace buffered
{

template<typename ITraits, typename DType>
class KDTreeNode
{
public:
    typedef ITraits                         IndexTraits;
    typedef typename IndexTraits::Type      IndexType;
    typedef typename IndexTraits::PivotType   IndexPivotType;
    static constexpr std::size_t IndexDimension = IndexTraits::Dimension;
    typedef DType                           DataType;
    typedef KDTreeNode<ITraits, DType>        NodeType;

    KDTreeNode() :
        left(nullptr),
        right(nullptr),
        index(),
        data()
    {
    }

    KDTreeNode(const KDTreeNode&) = delete;
    KDTreeNode& operator=(const KDTreeNode&) = delete;

    inline constexpr bool is_leaf() const
    {
        return left == nullptr && right == nullptr;
    }

    inline constexpr bool equals(const IndexType& index) const
    {
        return this->index == index;
    }

    inline constexpr bool check_split(const IndexType& index) const
    {
        return index[pivot_index] < pivot_value;
    }

    inline void merge(DataType&& data)
    {
        this->data.merge(std::move(data));
    }

public: /// todo: make private
    inline void split(NodeType* left, NodeType* right, IndexType&& index, DataType&& data)
    {
        {
            IndexPivotType max_delta = 0;
            for (std::size_t i = 0; i < IndexDimension; ++i)
            {
                auto delta = std::abs(this->index[i] - index[i]);
                if (delta > max_delta)
                {
                    max_delta = delta;
                    pivot_index = i;
                }
            }

            pivot_value = (this->index[pivot_index] + index[pivot_index]) / IndexPivotType(2.0);
        }

        if (check_split(this->index))
        {
            std::swap(left->data, this->data);
            left->index = this->index;

            right->index = std::move(index);
            right->data = std::move(data);
        }
        else
        {
            std::swap(right->data, this->data);
            right->index = this->index;

            left->index = std::move(index);
            left->data = std::move(data);
        }

        this->left = left;
        this->right = right;
    }


public: /// todo: make private
    NodeType* left;
    NodeType* right;

    IndexType index;
    IndexPivotType pivot_value;
    std::size_t pivot_index;

    DataType data;
};

}
}
