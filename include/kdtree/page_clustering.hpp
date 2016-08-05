#pragma once

namespace kdtree {

#include "page.hpp"
#include "fill.hpp"
#include "index.hpp"
#include <assert.h>

template<typename Type, int Dimension>
class PageClustering {
public:

    typedef std::array<int, Dimension>           DataIndex;
    typedef Page<Type, Dimension>::Index         PageIndex;
    typedef detail::fill<Type, Dimension>        MaskFiller;
    typedef typename MaskFiller::Type            MaskType;
    typedef ArrayOperations<Dimension, int, int> AO;

    PageClustering(std::vector<Type*>      &_entries,
                   Page<Type*, Dim>        &_page,
                   DataIndex               &_min_index,
                   DataIndex               &_max_index) :
        cluster_count(0),
        entries(_entries),
        page(_page),
        min_index(_min_index),
        max_index(_max_index)
    {
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

    std::vector<Type*> &entries;
    Page<Type*, Dim>   &page;
    DataIndex           min_index;
    DataIndex           max_index;

    inline constexpr Type apply(const Type& base, const Type& offset)
    {
        Type result;
        for (std::size_t i = 0; i < Dimension; ++i)
            result[i] = base[i] + offset[i];
        return result;
    }

    inline void clusterEntry(Type *entry)
    {
        PageIndex page_index;
        DataIndex index;
        for(DataIndex &offset : offsets) {
            AO::add(entry->index, offset, index);

            bool out_of_bounds = false;
            for(std::size_t j = 0 ; j < Dim ; ++j) {
                out_of_bounds |= index[j] < min_index[j];
                out_of_bounds |= index[j] > max_index[j];
                page_index[j]  = index[j] - min_index[j];
            }

            if(out_of_bounds)
                continue;

            Type *neighbour = page.at(page_index);
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
