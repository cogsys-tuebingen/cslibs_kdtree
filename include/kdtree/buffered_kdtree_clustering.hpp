#ifndef BUFFERED_KDTREE_CLUSTERING_HPP
#define BUFFERED_KDTREE_CLUSTERING_HPP

#include <queue>
#include "buffered_kdtree.hpp"
#include "kdtree_cluster_mask.hpp"

namespace kdtree {
namespace buffered {
template<typename NodeType>
struct KDTreeClustering {
    /// TODO : cache leaves in map -> no tree access anymore afterwards - maybe faster

    typedef KDTree<NodeType>                 KDTreeType;
    typedef typename KDTreeType::NodeIndex   NodeIndex;
    typename KDTreeType::Ptr                 kdtree;
    KDTreeClusterMask<NodeType::Dimension>   cluster_mask;
    std::vector<NodeType*>                   queue;
    NodeType**                               queue_ptr_;
    std::size_t                              queue_size;
    std::size_t                              queue_pos;
    std::size_t                              cluster_count;


    KDTreeClustering(const typename KDTreeType::Ptr &kdtree) :
        kdtree(kdtree),
        queue(kdtree->nodeCount()),
        queue_ptr_(queue.data()),
        queue_size(kdtree->nodeCount()),
        queue_pos(0),
        cluster_count(0)
    {
    }

    KDTreeClustering(const KDTreeClustering &other) :
        kdtree(other.kdtree),
        queue(other.queue),
        queue_ptr_(queue.data()),
        queue_size(other.queue.size()),
        queue_pos(other.queue_pos),
        cluster_count(other.cluster_count)
    {
    }

    KDTreeClustering<NodeType> & operator = (const KDTreeClustering<NodeType> &other)
    {
        kdtree = other.kdtree;
        queue = other.queue;
        queue_ptr_ = queue.data();
        queue_size = other.queue_size;
        queue_pos  = other.queue_pos;
        cluster_count = other.cluster_count;

        return *this;
    }

    virtual ~KDTreeClustering()
    {
    }

    inline int getCluster(const NodeIndex &index)
    {
        NodeType *node;
        if(!kdtree->find(index, node))
            return -1;
        return node->cluster;
    }

    inline void cluster()
    {

        kdtree->getNodes(queue);
        for(std::size_t i = 0 ; i < queue_size ; ++i) {
            NodeType *node = queue_ptr_[i];
            if(node->isLeaf()) {
                if(node->cluster > -1)
                    continue;
                node->cluster = cluster_count;
                ++cluster_count;
                clusterNode(node);
            }
        }
    }

    inline void clusterNode(NodeType *node)
    {
        /// check surrounding indeces
        NodeIndex index;
        NodeType *neighbour;
        std::size_t rows = cluster_mask.rows;
        for(std::size_t i = 0 ; i < rows; ++i) {
            cluster_mask.applyToIndex(node->index, i, index);
            neighbour = kdtree->find(index);
            if(!neighbour)
                continue;
            if(neighbour->cluster > -1)
                continue;
            neighbour->cluster = node->cluster;
            clusterNode(neighbour);
        }

    }

};
}
}



#endif // BUFFERED_KDTREE_CLUSTERING_HPP
