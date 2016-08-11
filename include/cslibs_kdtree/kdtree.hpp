#pragma once

#include "kdtree_buffered.hpp"
#include "kdtree_unbuffered.hpp"
#include "kdtree_node.hpp"
#include "kdtree_clustering.hpp"

namespace kdtree
{
template<typename ITraits, typename DType>
using KDTree = unbuffered::KDTree<ITraits, DType>;
}
