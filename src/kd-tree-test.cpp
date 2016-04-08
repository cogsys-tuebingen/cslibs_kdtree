#include <kdtree/kdtree_cluster_mask.hpp>
#include <kdtree/kdtree_clustering.hpp>
#include <kdtree/kdtree.hpp>
#include <kdtree/buffered_kdtree.hpp>
#include <kdtree/buffered_kdtree_clustering.hpp>
#include <kdtree/kdtree_dotty.hpp>
#include <ctime>
#include <vector>

struct Point {
    double x;
    double y;
    double z;

    double weight;
};

struct PFKDTreeNode : public kdtree::KDTreeNode<int, 3>
{
    std::vector<Point*> samples;

    PFKDTreeNode() :
        KDTreeNode()
    {
    }

    PFKDTreeNode(const PFKDTreeNode &other) :
        KDTreeNode(other),
        samples(other.samples)
    {
    }

    inline Ptr clone() const
    {
        PFKDTreeNode *node = new PFKDTreeNode(*this);
        node->left = left;
        node->right = right;
        return Ptr(node);
    }

    inline Ptr copy() const
    {
        PFKDTreeNode *node = new PFKDTreeNode(*this);
        return Ptr(node);
    }

    inline void overwrite(const Ptr &other) override
    {
        PFKDTreeNode *other_ptr = (PFKDTreeNode*) other.get();
        samples.insert(samples.end(), other_ptr->samples.begin(), other_ptr->samples.end());
    }

    inline void split(const Ptr &other) override
    {
        KDTreeNodeType::split(other);
        samples.clear();
    }

    inline double weight() const
    {
        double weight = 0.0;
        for(Point * sample : samples)
            weight += sample->weight;
        return weight;
    }
};

struct PFKDTreeIndex {
    static PFKDTreeNode::Ptr get(Point *sample)
    {
        double size[3] = {0.5, 0.5, (10 * M_PI / 180.0)};
        PFKDTreeNode *node = new PFKDTreeNode;
        node->samples.emplace_back(sample);
        node->index[0] = floor(sample->x / size[0]);
        node->index[1] = floor(sample->y / size[1]);
        node->index[2] = floor(sample->z / size[2]);
        return PFKDTreeNode::Ptr(node);
    }
};


void test_cluster_mask() {
    kdtree::KDTreeClusterMask<3> cluster_mask;
    for(std::size_t i = 0 ; i < cluster_mask.rows ; ++i) {
        for(std::size_t j = 0 ; j < cluster_mask.cols ; ++j) {
            std::cout << cluster_mask[i * cluster_mask.cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

void test_kdtree_simple()
{
    kdtree::KDTree<int, 3>::Ptr tree(new kdtree::KDTree<int, 3>);
    std::vector<Point> samples;
    Point sample;
    sample.x = -1.0 ; sample.y = -1.0; sample.z = M_PI;
    sample.weight = 0.5;
    samples.push_back(sample);
    sample.x = -0.9 ; sample.y = -0.9; sample.z = M_PI;
    sample.weight = 0.25;
    samples.push_back(sample);
    sample.x = -0.8 ; sample.y = -0.8; sample.z = M_PI;
    sample.weight = 0.125;
    samples.push_back(sample);
    sample.x = +0.5 ; sample.y = +0.5; sample.z = M_PI;
    sample.weight = 0.5;
    samples.push_back(sample);
    sample.x = +0.5 ; sample.y = +0.5; sample.z = M_PI;
    sample.weight = 0.5;
    samples.push_back(sample);
    sample.x = +0.5 ; sample.y =  0.25; sample.z = M_PI;
    sample.weight = 0.25;
    samples.push_back(sample);
    sample.x = +0.5 ; sample.y = -1.0; sample.z = M_PI;
    sample.weight = 0.125;
    samples.push_back(sample);
    for(std::size_t i = 0 ; i < samples.size() ; ++i) {
        PFKDTreeNode::Ptr node = PFKDTreeIndex::get(&samples[i]);
        tree->insertNode(node);
    }
    std::cout << "leaves " << tree->leafCount() << std::endl;
    std::vector<PFKDTreeNode::Ptr> leaves;
    tree->getLeaves(leaves);
    std::cout << "leaves fetched " << leaves.size() << std::endl;
    std::cout << "nodes " << tree->nodeCount() << std::endl;
    std::array<int, 3> index_to_be_found     = {-2, -2, 18};
    std::array<int, 3> index_not_to_be_found = {-3, -2, 18};

    PFKDTreeNode::Ptr node;
    if(tree->find(index_to_be_found, node)) {
        assert(node != nullptr);
        std::cout << "found - that's true" << std::endl;
        std::cout << node << std::endl;
        std::cout << node->index[0] << " " << node->index[1] << " " << node->index[2] << std::endl;
        std::cout << ((PFKDTreeNode *) node.get())->weight() << std::endl;
    }
    if(!tree->find(index_not_to_be_found, node)) {
        std::cout << "not found - that's true" << std::endl;
    }



    kdtree::KDTreeClustering<int, 3> clustering(tree);
    clustering.cluster();
    std::cout << clustering.cluster_count << std::endl;

    kdtree::Dotty<int, 3> dotty;
    std::ofstream out("/tmp/simple.dot");
    dotty.write(*tree, out);
    out.close();

}

int main(int argc, char *argv[])
{
    test_cluster_mask();
    test_kdtree_simple();
    return 0;
}
