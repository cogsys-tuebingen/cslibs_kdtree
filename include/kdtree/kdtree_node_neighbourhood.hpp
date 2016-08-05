#pragma once

#include <cstdint>
#include <array>
#include "fill.hpp"

namespace kdtree
{
template<typename Tree, typename ITraits>
class KDTreeIndexNeigbourhood
{
public:
    typedef typename ITraits::Type              Type;
    static constexpr std::size_t Dimension = ITraits::Dimension;
    typedef detail::fill<Type, Dimension>       MaskFiller;
    typedef typename MaskFiller::Type           MaskType;

public:
    KDTreeIndexNeigbourhood(Tree& tree):
        _tree(tree),
        offsets()
    {
        MaskFiller::assign(offsets);
    }

    template<typename F>
    void visit(const Type& reference, F&& fun)
    {
        for (const Type& offset : offsets)
        {
            Type index = apply(reference, offset);
            auto node = _tree.find(index);
            if (node)
                fun(*node);
        }
    }

private:
    inline constexpr Type apply(const Type& base, const Type& offset)
    {
        Type result;
        for (std::size_t i = 0; i < Dimension; ++i)
            result[i] = base[i] + offset[i];
        return result;
    }

private:
    Tree& _tree;
    MaskType offsets;
};

}
