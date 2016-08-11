#pragma once

#include <functional>
#include <type_traits>
#include "kdtree_node_neighbourhood.hpp"

namespace kdtree
{

struct KDTreeNodeClusteringSupport
{
    int cluster = -1;
};

template<typename TreeType>
class KDTreeClustering
{
public:
    typedef TreeType                            KDTreeType;
    typedef KDTreeClustering<TreeType>          ClusteringType;
    typedef typename KDTreeType::NodeType       NodeType;
    typedef typename KDTreeType::DataType       DataType;
    typedef typename KDTreeType::IndexTraits    IndexTraits;
    typedef typename KDTreeType::IndexType      IndexType;

    static_assert(std::is_base_of<KDTreeNodeClusteringSupport, DataType>::value,
                  "NodeType does not have KDTreeNodeClusteringSupport");

public:
    KDTreeClustering(KDTreeType& tree) :
        _tree(tree),
        _cluster_count(0),
        _neighbourhood(tree),
        _cluster_init(&KDTreeClustering::nop1),
        _cluster_extend(&KDTreeClustering::nop2)
    {
    }

    template<typename F>
    inline void set_cluster_init(const F& fun)
    {
        _cluster_init = fun;
    }

    template<typename F>
    inline void set_cluster_extend(const F& fun)
    {
        _cluster_extend = fun;
    }

    inline void cluster()
    {
        int cluster_idx = 0;

        _tree.traverse_leafs([this, &cluster_idx](NodeType& node)
        {
            if (node.data.cluster > -1)
                return;

            if (!_cluster_init(node.data))
                return;

            node.data.cluster = cluster_idx;
            ++cluster_idx;
            cluster(node);
        });

        _cluster_count = cluster_idx;
    }

    inline std::size_t cluster_count() const
    {
        return _cluster_count;
    }

private:
    inline void cluster(NodeType& node)
    {
        _neighbourhood.visit(node.index, [this, &node](NodeType& neighbour)
        {
            if (neighbour.data.cluster > -1)
                return;

            if (!_cluster_extend(node.data, neighbour.data))
                return;

            neighbour.data.cluster = node.data.cluster;
            cluster(neighbour);
        });
    }

    static inline constexpr bool nop1(const DataType&) { return true; }
    static inline constexpr bool nop2(const DataType&, const DataType&) { return true; }


private:
    KDTreeType& _tree;
    std::size_t _cluster_count;
    KDTreeIndexNeigbourhood<TreeType, IndexTraits> _neighbourhood;
    std::function<bool(const DataType&)> _cluster_init;
    std::function<bool(const DataType&, const DataType&)> _cluster_extend;
};
}
