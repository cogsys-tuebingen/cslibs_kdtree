#pragma once

#include "array.hpp"
#include "fill.hpp"
#include "index.hpp"
#include <assert.h>

namespace kdtree {
template<typename Type, int Dimension>
class ArrayClustering {
public:
    typedef std::array<int, Dimension>                   DataIndex;
    typedef typename Array<Type, Dimension>::Index       ArrayIndex;
    typedef detail::fill<int, Dimension>                 MaskFiller;
    typedef typename MaskFiller::Type                    MaskType;
    typedef ArrayOperations<Dimension, int, int>         AO;
    typedef ArrayOperations<Dimension, int, std::size_t> AOA;

    ArrayClustering(std::vector<Type*>     &_entries,
                   Array<Type*, Dimension> &_array,
                   DataIndex               &_min_index,
                   DataIndex               &_max_index) :
        cluster_count(0),
        entries(_entries),
        array(_array),
        min_index(_min_index),
        max_index(_max_index)
    {
        MaskFiller::assign(offsets);
    }

    inline void cluster()
    {
        for(Type *entry : entries)
        {
            if(entry->cluster > -1)
                continue;
            entry->cluster = cluster_count;
            ++cluster_count;
            clusterEntry(entry);
        }
    }

private:
    MaskType offsets;
    int      cluster_count;

    std::vector<Type*>       &entries;
    Array<Type*, Dimension>  &array;
    DataIndex                 min_index;
    DataIndex                 max_index;

    inline constexpr Type apply(const Type& base, const Type& offset)
    {
        Type result;
        for (std::size_t i = 0; i < Dimension; ++i)
            result[i] = base[i] + offset[i];
        return result;
    }

    inline void clusterEntry(Type *entry)
    {
        ArrayIndex page_index;
        DataIndex index;
        for(DataIndex &offset : offsets) {
            AO::add(entry->index, offset, index);

            bool out_of_bounds = false;
            for(std::size_t j = 0 ; j < Dimension ; ++j) {
                out_of_bounds |= index[j] < min_index[j];
                out_of_bounds |= index[j] > max_index[j];
                page_index[j]  = index[j] - min_index[j];
            }

            if(out_of_bounds)
                continue;

            Type *neighbour = array.at(page_index);
            if(!neighbour)
                continue;
            if(neighbour->cluster > -1)
                continue;
            assert(neighbour->cluster == -1);
            neighbour->cluster = entry->cluster;
            clusterEntry(neighbour);
        }
    }
};
}
