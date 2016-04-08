#ifndef CLUSTER_MASK_HPP
#define CLUSTER_MASK_HPP

#include <vector>
#include <array>
#include <iostream>
#include <assert.h>

namespace kdtree {
template<int Iteration>
struct pow3 {
    static const std::size_t value = 3 * pow3<Iteration - 1>::value;
};
template<>
struct pow3<0> {
    static const std::size_t value = 1;
};

template<int Iteration, typename KDTreeClusterMask>
struct fill {
    static constexpr void assign(KDTreeClusterMask &mask)
    {
        const std::size_t off = mask.cols - Iteration;
        const std::size_t div = kdtree::pow3<Iteration - 1>::value;
        for(std::size_t i = 0 ; i < mask.rows; ++i) {
            mask[i * mask.cols + off] = ((i / div) % 3 - 1);
        }
        fill<Iteration - 1, KDTreeClusterMask>::assign(mask);
    }

};

template<typename KDTreeClusterMask>
struct fill<0, KDTreeClusterMask>
{
    static constexpr void assign(KDTreeClusterMask &mask)
    {
        return;
    }
};

template<int Dim>
struct KDTreeClusterMask {
    static const std::size_t rows = kdtree::pow3<Dim>::value;
    static const std::size_t cols = Dim;
    std::array<int, rows * cols> mask;
    std::size_t                  pos[rows];

    inline int & operator [] (const std::size_t i)
    {
        return mask[i];
    }

    inline const int & operator [] (const std::size_t i) const
    {
        return mask[i];
    }

    inline void applyToIndex(const std::array<int, Dim> &index,
                             const std::size_t           row,
                             std::array<int, Dim>       &neighbour)
    {
        int *mask_ptr = &mask[pos[row]];
        for(int i = 0 ; i < Dim ; ++i, ++mask_ptr)
            neighbour[i] = index[i] + *mask_ptr;
    }

    KDTreeClusterMask()
    {
        /// fill the mask
        fill<Dim, KDTreeClusterMask<Dim>>::assign(*this);
        for(std::size_t i = 0 ; i < rows ; ++i) {
            pos[i] = i * cols;
        }
    }
};
}
#endif // CLUSTER_MASK_HPP
