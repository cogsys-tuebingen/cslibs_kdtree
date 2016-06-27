#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <array>
#include <iterator>

#include "../include/kdtree/buffered/kdtree.hpp"
#include "../include/kdtree/buffered/kdtree_clustering.hpp"

namespace helper
{
struct Point {
    double x;
    double y;
    double z;

    double weight;
};

static constexpr double INDEX_DIM[3] = {0.5, 0.5, (10 * M_PI / 180.0)};

inline Point getSample(const std::vector<double> &values)
{
    Point s;
    s.x = values[0];
    s.y = values[1];
    s.z = values[2];
    s.weight = values[3];
    return s;
}

template<typename T>
void getLineContent(const std::string &s,
                    std::vector<T> &values)
{
    std::stringstream ss(s);
    T value;
    while(ss >> value) {
        values.push_back(value);
    }
}

std::vector<Point> load_samples(const std::string& filename)
{
    std::vector<Point> samples;

    std::ifstream            in(filename);
    std::string              line;
    while(std::getline(in, line)) {
        std::vector<double> values;
        getLineContent(line, values);
        Point sample_orig = getSample(values);
        Point sample_trans = sample_orig;
        sample_trans.x += 10.0;
        sample_trans.y += 10.0;

        samples.emplace_back(sample_orig);
        samples.emplace_back(sample_trans);

    }

    std::cout << "Loaded Samples" << std::endl
              << "\tCount: " << samples.size() << std::endl
              << "\tFile : " << filename << std::endl;

    return samples;
}

}

namespace new_version
{
struct Index
{
    typedef std::array<int, 3> Type;
    typedef double PivotType;
    static constexpr std::size_t Dimension = 3;

    static constexpr Type create(const helper::Point& pt)
    {
        return {(int)std::floor(pt.x / helper::INDEX_DIM[0]),
                 (int)std::floor(pt.y / helper::INDEX_DIM[1]),
                 (int)std::floor(pt.z / helper::INDEX_DIM[2])};
    }
};

struct Data : public kdtree::buffered::KDTreeNodeClusteringSupport
{
    std::vector<const helper::Point*> samples;
    double weight;

    Data() = default;

    Data(const helper::Point& pt)
    {
        samples.emplace_back(&pt);
        weight += pt.weight;
    }

    inline void merge(Data&& other)
    {
        samples.insert(samples.end(),
                       other.samples.begin(),
                       other.samples.end());
        weight += other.weight;
    }
};

void test(const std::vector<helper::Point>& samples)
{
    using clock = std::chrono::steady_clock;
    using ms = std::chrono::duration<double, std::milli>;

    clock::duration total;
    for (int i = 0; i < 1000; ++i)
    {

        auto start = clock::now();

        kdtree::buffered::KDTree<Index, Data> tree(2 * samples.size() + 1);

        for (const helper::Point& pt : samples)
            tree.insert(Index::create(pt), {pt});

        kdtree::buffered::KDTreeClustering<decltype(tree)> clustering(tree);
        clustering.cluster();

        auto end = clock::now();
        total += end - start;
    }
    std::cout << "Time    : " << std::chrono::duration_cast<ms>(total).count() << "ms" << std::endl;
    //std::cout << "Clusters: " << clustering.cluster_count() << std::endl;
}

}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <sample_data>" << std::endl;
        return 1;
    }

    auto samples = helper::load_samples(argv[1]);

    new_version::test(samples);


    return 0;
}

