#pragma once

#include <type_traits>
#include <array>

namespace kdtree
{
namespace buffered
{

struct KDTreeNodeClusteringSupport
{
    int cluster = -1;
};

namespace math
{
template<int Iteration>
struct pow3 {
    static const std::size_t value = 3 * pow3<Iteration - 1>::value;
};
template<>
struct pow3<0> {
    static const std::size_t value = 1;
};
}

template<typename ITraits>
class KDTreeIndexNeigbourGenerator
{
public:
    typedef typename ITraits::Type Type;
    static constexpr std::size_t Dimension = ITraits::Dimension;

    typedef std::array<Type, math::pow3<Dimension>::value> NeighbourList;

public:
    KDTreeIndexNeigbourGenerator(const Type& reference):
        _reference(reference)
    {
        int idx = 0;
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
                for (int k = -1; k <= 1; ++k, ++idx)
                    offsets[idx] = {i, j, k};
    }

    template<typename T, typename F>
    void visit(T& tree, F&& fun)
    {
        for (const Type& offset : offsets)
        {
            Type index = apply(_reference, offset);
            auto node = tree.find(index);
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
    Type _reference;
    NeighbourList offsets;
};

template<typename TreeType>
class KDTreeClustering
{
public:
    typedef TreeType KDTreeType;
    typedef KDTreeClustering<TreeType> ClusteringType;
    typedef typename KDTreeType::NodeType NodeType;
    typedef typename KDTreeType::IndexTraits IndexTraits;
    typedef typename KDTreeType::IndexType IndexType;

    static_assert(std::is_base_of<KDTreeNodeClusteringSupport,
                  typename NodeType::DataType>::value, "NodeType does not have KDTreeNodeClusteringSupport");

public:
    KDTreeClustering(KDTreeType& tree) :
        _tree(tree),
        _cluster_count(0)
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
        KDTreeIndexNeigbourGenerator<IndexTraits> neighbours(node.index);
        neighbours.visit(_tree, [this,&node](NodeType& neighbour)
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
};
}
}
