#ifndef KDTREE_CLUSTERING_HPP
#define KDTREE_CLUSTERING_HPP

#include <queue>
#include "kdtree.hpp"
#include "kdtree_cluster_mask.hpp"

namespace kdtree {
template<typename T, int Dim>
struct KDTreeClustering {
    /// TODO : cache leaves in map -> no tree access anymore afterwards - maybe faster

    typedef KDTree<T, Dim>                   KDTreeType;
    typedef typename KDTreeType::NodeIndex   NodeIndex;
    typedef typename KDTreeType::NodePtr     NodePtr;
    typename KDTreeType::Ptr kdtree;
    KDTreeClusterMask<Dim>   cluster_mask;
    std::vector<NodePtr>     queue;
    std::size_t              cluster_count;

    KDTreeClustering(const typename KDTreeType::Ptr &kdtree) :
        kdtree(kdtree)
    {
    }

    int getCluster(const NodeIndex &index) {
        NodePtr node;
        if(!kdtree->find(index, node))
            return -1;
        return node->cluster;
    }

    inline void cluster()
    {
        queue.reserve(kdtree->leafCount());
        kdtree->getLeaves(queue, true);
        for(NodePtr &node : queue) {
            if(node->cluster > -1)
                continue;
            node->cluster = cluster_count;
            ++cluster_count;
            clusterNode(node);
        }
    }

    inline void clusterNode(const NodePtr &node)
    {
        /// check surrounding indeces
        NodeIndex index;
        for(std::size_t i = 0 ; i < cluster_mask.rows ; ++i) {
            cluster_mask.applyToIndex(node->index, i, index);
            NodePtr neighbour;
            if(!kdtree->find(index, neighbour))
                continue;
            if(neighbour->cluster > -1)
                continue;
            neighbour->cluster = node->cluster;
            clusterNode(neighbour);
        }

    }

};
}
#endif // KDTREE_CLUSTERING_HPP
