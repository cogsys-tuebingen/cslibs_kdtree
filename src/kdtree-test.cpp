#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>

#include "../include/kdtree/kdtree.hpp"

namespace testdata
{
struct Point
{
    double x;
    double y;
    double z;

    double weight;
};

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

using Points = std::vector<Point>;

Points getTestdata(const std::string& file)
{
    static const std::array<double, 3> OFFSETS_X{-10.0, 0.0, 10.0};
    static const std::array<double, 3> OFFSETS_Y{-10.0, 0.0, 10.0};
    static const auto VGA = 640 * 480;
    static const auto IN_DATASET = 1000;
    static const auto OFFSETS = 3 * 3;
    static const auto LOAD_PCT = 0.66;
    static const std::size_t LOAD_FACTOR = (VGA / IN_DATASET / OFFSETS) * LOAD_PCT;

    Points samples;

    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
    {
        std::vector<double> values;
        getLineContent(line, values);

        Point ref = getSample(values);

        for (int i = 0; i < LOAD_FACTOR; ++i)
            for (double off_x : OFFSETS_X)
                for (double off_y : OFFSETS_Y)
                {
                    Point cur = ref;
                    cur.x += off_x;
                    cur.y += off_y;
                    samples.emplace_back(cur);
                }
    }

    return samples;
}
}

using Point     = testdata::Point;
using Points    = testdata::Points;

// ##########################
// BEGIN KDTree structures
// ##########################
struct Index
{
    using                           Type        = std::array<int, 3>;       /// Used index type
    using                           PivotType   = double;                   /// Pivot element type, double is fine
    static constexpr std::size_t    Dimension   = 3;                        /// Dimension of index

    static inline Type create(const Point& pt)                              /// Create function for index object (not requried by api)
    {
        static double bin_sizes[3] = {0.5, 0.5, (10 * M_PI / 180.0)};
        return { static_cast<int>(std::floor(pt.x / bin_sizes[0])),
                 static_cast<int>(std::floor(pt.y / bin_sizes[1])),
                 static_cast<int>(std::floor(pt.z / bin_sizes[2])) };
    }
};

struct Data : public kdtree::KDTreeNodeClusteringSupport    /// use "kdtree::KDTreeNodeClusteringSupport" for clustering support
{
    std::vector<const Point*> samples;
    double weight;

    inline void merge(Data&& other)                         /// Called when two nodes are merged (other is discarded)
    {
        this->samples.insert(this->samples.end(),
                             other.samples.begin(),
                             other.samples.end());
        this->weight += other.weight;
    }

    static inline Data create(const Point& pt)              /// Create function for data object
    {
        Data data;
        data.samples.emplace_back(&pt);
        data.weight = pt.weight;
        return data;
    }
};

using KDTreeUnbuffered      = kdtree::unbuffered::KDTree<Index, Data>;      /// unbuffered KDTree (nodes added per new)
using ClusteringUnbuffered  = kdtree::KDTreeClustering<KDTreeUnbuffered>;
using KDTreeBuffered        = kdtree::buffered::KDTree<Index, Data>;        /// buffered KDTree (fixed capcacity, node are in in array)
using ClusteringBuffered    = kdtree::KDTreeClustering<KDTreeBuffered>;

// ##########################
// END KDTree structures
// ##########################

namespace test
{
struct Timer
{
    using clock         = std::chrono::system_clock;
    using time_point    = clock::time_point;
    using ms            = std::chrono::duration<double, std::milli>;

    std::string message;
    time_point start;
    int cluster;

    Timer(const std::string& message) :
        message(message),
        start(clock::now())
    {
    }

    ~Timer()
    {
        auto delta = clock::now() - start;
        std::cout << message << ": " << cluster << " cluster in " << std::chrono::duration_cast<ms>(delta).count() << "ms" << std::endl;
    }
};

struct Benchmark
{
    template<std::size_t Itr, typename Call>
    static void timing(const std::string& prefix, Call&& callable)
    {
        auto start = Timer::clock::now();

        for (std::size_t i = 0; i < Itr; ++i)
            callable();

        auto end = Timer::clock::now();
        auto delta = end - start;
        auto in_ms = std::chrono::duration_cast<Timer::ms>(delta).count();

        std::cout << prefix << ": Total: " << in_ms << "ms Avg: " << in_ms / Itr << std::endl;
    }
};

inline std::size_t reserve(double factor, std::size_t size)
{
    return factor * size;
}

int unbuffered_clustering_bulk(const Points& samples)
{
    KDTreeUnbuffered tree;

    for (const Point& sample : samples)
        tree.insert_bulk(Index::create(sample), Data::create(sample));
    tree.load_bulk();

    ClusteringUnbuffered clustering(tree);
    clustering.cluster();

    return clustering.cluster_count();
}

int unbuffered_clustering(const Points& samples)
{
    KDTreeUnbuffered tree;

    for (const Point& sample : samples)
        tree.insert(Index::create(sample), Data::create(sample));

    ClusteringUnbuffered clustering(tree);
    clustering.cluster();

    return clustering.cluster_count();
}

int buffered_clustering_bulk(const Points& samples, double factor)
{
    KDTreeBuffered tree(reserve(factor, samples.size()));

    for (const Point& sample : samples)
        tree.insert_bulk(Index::create(sample), Data::create(sample));
    tree.load_bulk();

    ClusteringBuffered clustering(tree);
    clustering.cluster();

    return clustering.cluster_count();
}

int buffered_clustering(const Points& samples, double factor)
{
    KDTreeBuffered tree(reserve(factor, samples.size()));

    for (const Point& sample : samples)
        tree.insert(Index::create(sample), Data::create(sample));

    ClusteringBuffered clustering(tree);
    clustering.cluster();

    return clustering.cluster_count();
}

/// example use case for reuse and bulk loading
template<typename Tree>
void reuse_clustering_bulk(const Points& samples, Tree& tree)
{
    tree.clear();

    for (const Point& sample : samples)
        tree.insert_bulk(Index::create(sample), Data::create(sample));
    tree.load_bulk();

    kdtree::KDTreeClustering<Tree> clustering(tree);
    clustering.cluster();
}

/// example use case for reuse
template<typename Tree>
void reuse_clustering(const Points& samples, Tree& tree)
{
    tree.clear();

    for (const Point& sample : samples)
        tree.insert(Index::create(sample), Data::create(sample));

    kdtree::KDTreeClustering<Tree> clustering(tree);
    clustering.cluster();
}

}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <sample_data>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    Points points = testdata::getTestdata(path);
    std::cout << "Testdata : " << std::endl
              << "\tFile   : " << path << std::endl
              << "\tSamples: " << points.size() << std::endl
              << std::endl;

    std::cout << "Validate: " << std::endl
              << "\tExpected: 9 clusters" << std::endl
              << "Runs: " << std::endl;
    {
        auto timer  = test::Timer("\tUnbuffered Clustering       ");
        timer.cluster =  test::unbuffered_clustering(points);
    }
    {
        auto timer  = test::Timer("\tUnbuffered Clustering (bulk)");
        timer.cluster = test::unbuffered_clustering_bulk(points);
    }
    {
        auto timer  = test::Timer("\tBuffered Clustering         ");
        timer.cluster =  test::buffered_clustering(points, 2);
    }
    {
        auto timer  = test::Timer("\tBuffered Clustering (bulk)  ");
        timer.cluster = test::buffered_clustering_bulk(points, 2);
    }

    std::cout << std::endl
              << "Timings: " << std::endl
              << "\tExpected: bulk faster than non-bulk" << std::endl
              << "\tExpected: buffered faster than un-buffered" << std::endl
              << "Runs: " << std::endl;
    {
        test::Benchmark::timing<500>("\tUnbuffered       ", std::bind(&test::unbuffered_clustering, points));
        test::Benchmark::timing<500>("\tUnbuffered (bulk)", std::bind(&test::unbuffered_clustering_bulk, points));
        test::Benchmark::timing<500>("\tBuffered         ", std::bind(&test::buffered_clustering, points, 0.2));
        test::Benchmark::timing<500>("\tBuffered   (bulk)", std::bind(&test::buffered_clustering_bulk, points, 0.2));
    }
    {
        KDTreeUnbuffered unbuffered;
        KDTreeBuffered buffered(test::reserve(0.2, points.size()));
        test::Benchmark::timing<500>("\tUnbuffered        (reuse): ", std::bind(&test::reuse_clustering<KDTreeUnbuffered>, std::cref(points), std::ref(unbuffered)));
        test::Benchmark::timing<500>("\tUnbuffered (bulk) (reuse): ", std::bind(&test::reuse_clustering_bulk<KDTreeUnbuffered>, points, std::ref(unbuffered)));
        test::Benchmark::timing<500>("\tBuffered          (reuse): ", std::bind(&test::reuse_clustering<KDTreeBuffered>, points, std::ref(buffered)));
        test::Benchmark::timing<500>("\tBuffered   (bulk) (reuse): ", std::bind(&test::reuse_clustering_bulk<KDTreeBuffered>, points, std::ref(buffered)));
    }

    return 0;
}

