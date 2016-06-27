#pragma once

#include <type_traits>
#include "kdtree_node_neighbourhood.hpp"

namespace kdtree
{
namespace buffered
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
    typedef typename KDTreeType::IndexTraits    IndexTraits;
    typedef typename KDTreeType::IndexType      IndexType;

    static_assert(std::is_base_of<KDTreeNodeClusteringSupport, typename NodeType::DataType>::value,
                  "NodeType does not have KDTreeNodeClusteringSupport");

public:
    KDTreeClustering(KDTreeType& tree) :
        _tree(tree),
        _cluster_count(0),
        _neighbourhood(tree)
    {
    }

    inline void cluster()
    {
        int cluster_idx = 0;

        _tree.traverse_leafs([this, &cluster_idx](NodeType& node)
        {
            if (node.data.cluster > -1)
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

            neighbour.data.cluster = node.data.cluster;
            cluster(neighbour);
        });
    }


private:
    KDTreeType& _tree;
    std::size_t _cluster_count;
    KDTreeIndexNeigbourhood<TreeType, IndexTraits> _neighbourhood;
};
}
}
