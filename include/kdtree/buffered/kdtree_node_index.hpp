#pragma once

#include <cstdint>
#include <array>

namespace kdtree
{
namespace buffered
{

template<typename DimType, std::size_t Dim, typename GType, typename PType = double>
class KDTreeNodeIndex
{
public:
    typedef DimType              DimensionType;
    static constexpr std::size_t Dimension = Dim;
    typedef PType                PivotType;
    typedef std::array<DimensionType, Dimension> ValueType;
    typedef KDTreeNodeIndex<DimensionType, Dimension, GType, PType> IndexType;
    typedef GType                GeneratorType;

    KDTreeNodeIndex() :
        value(),
        pivot_value(0),
        pivot_index(0)
    {
    }

    inline bool check_split(const IndexType& other) const
    {
        return other.value[pivot_index] < pivot_value;
    }

    inline bool create_split(const IndexType& other)
    {
        DimensionType max_delta = 0;
        for (std::size_t i = 0; i < Dimension; ++i)
        {
            auto delta = std::abs(value[i] - other.value[i]);
            if (delta > max_delta)
            {
                max_delta = delta;
                pivot_index = i;
            }
        }

        pivot_value = (value[pivot_index] + other.value[pivot_index]) / PivotType(2);
    }

    inline bool operator==(const IndexType& other) const
    {
        return value == other.value;
    }

private:
    std::array<DimensionType, Dimension> value;

    PivotType pivot_value;
    std::size_t   pivot_index;
};

}
}
