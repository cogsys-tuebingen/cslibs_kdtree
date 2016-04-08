#ifndef KDTREE_DOTTY_HPP
#define KDTREE_DOTTY_HPP

#include <deque>
#include <map>
#include <fstream>
#include <random>

namespace kdtree {
struct Color {
    double value[3];

    Color() :
        value{0.0, 0.0, 0.0}
    {
    }

    Color (double random) {
        value[0] = random;
        value[1] = 0.5;
        value[2] = 1.0;
    }

    void writeStyle(std::ofstream &out)
    {
        out << "[style=filled, fillcolor=\""
            << value[0] << " "
            << value[1] << " "
            << value[2] << "\"]";
    }
};

struct ColorMap {
    std::map<int, Color>                   palette;
    std::default_random_engine             generator;
    std::uniform_real_distribution<double> random;

    ColorMap() :
        random(0.0, 1.0)
    {
    }

    void writeStyle(const int id,
                    std::ofstream &out)
    {
        if(palette.find(id) == palette.end())
            palette.insert(std::make_pair(id, Color(random(generator))));
        palette[id].writeStyle(out);
    }

};

template<typename T, int Dim>
struct Dotty {
    typedef kdtree::KDTree<T, Dim> KDTreeType;
    ColorMap                       color_palette;

    void write(KDTreeType tree,
               std::ofstream &out)
    {
        out << "graph kdtree {" << std::endl;
        writeStyles(tree, out);
        out << std::endl;
        typename KDTreeType::NodePtr root = tree.getRoot();
        write(root, out);
        out << "}" << std::endl;
    }

    void writeStyles(KDTreeType &tree, std::ofstream &out)
    {
        std::vector<typename KDTreeType::NodePtr> nodes;
        tree.getNodes(nodes);
        for(typename KDTreeType::NodePtr &node : nodes)
        {
            writeNode(node,  out);
            if(node->cluster >= 0) {
                out << " ";
                color_palette.writeStyle(node->cluster, out);
            }
            out << std::endl;
        }
    }

    void write(typename KDTreeType::NodePtr &root,
               std::ofstream &out)
    {
        std::deque<typename KDTreeType::NodePtr> lefts;
        runLeft(root, lefts);
        for(size_t i = 0 ; i < lefts.size() ; ++i) {
            writeNode(lefts.at(i), out);
            if(i < lefts.size() - 1)
                out << "-- ";
        }
        out << std::endl;
        for(typename KDTreeType::NodePtr &left : lefts)
            runRight(left, out);
    }

    void runLeft(typename KDTreeType::NodePtr &left,
                 std::deque<typename KDTreeType::NodePtr> &lefts)
    {
        if(left) {
            lefts.push_back(left);
            runLeft(left->left, lefts);
        }
    }

    void runRight(const typename KDTreeType::NodePtr &parent,
                  std::ofstream &out)
    {
        out << "  " << std::endl;
        writeNode(parent, out);
        if(parent->right) {
            out << "-- ";
            write(parent->right, out);
        }
    }

    void writeNode(const typename KDTreeType::NodePtr &node,
                   std::ofstream &out)
    {
        out << "\"" << node.get() << " ";
        out << "[";
        for(int j = 0 ; j < Dim ; ++j) {
            out << node->index[j];
            if(j < Dim-1)
                out << " ";
        }
        out << "]\"" << " ";
    }

};

namespace buffered {
template<typename NodeType>
struct Dotty {
    typedef kdtree::buffered::KDTree<NodeType> KDTreeType;
    ColorMap color_palette;

    void write(KDTreeType &tree,
               std::ofstream &out)
    {
        out << "graph kdtree {" << std::endl;
        writeStyles(tree, out);
        out << std::endl;
        NodeType *root = tree.getRoot();
        write(root, out);
        out << "}" << std::endl;
    }

    void writeStyles(KDTreeType &tree, std::ofstream &out)
    {
        std::vector<NodeType*> nodes;
        tree.getNodes(nodes);
        for(const NodeType *node : nodes)
        {
            writeNode(node,  out);
            if(node->cluster >= 0) {
                out << " ";
                color_palette.writeStyle(node->cluster, out);
            }
            out << std::endl;
        }
    }

    void write(NodeType* root,
               std::ofstream &out)
    {
        std::deque<NodeType*> lefts;
        runLeft(root, lefts);
        for(size_t i = 0 ; i < lefts.size() ; ++i) {
            writeNode(lefts.at(i), out);
            if(i < lefts.size() - 1)
                out << "-- ";
        }
        out << std::endl;
        for(NodeType *left : lefts)
            runRight(left, out);
    }

    void runLeft(NodeType *left,
                 std::deque<NodeType*> &lefts)
    {
        if(left) {
            lefts.push_back(left);
            runLeft(left->left, lefts);
        }
    }

    void runRight(const NodeType *parent,
                  std::ofstream &out)
    {
        out << "  " << std::endl;
        writeNode(parent, out);
        if(parent->right) {
            out << "-- ";
            write(parent->right, out);
        }
    }

    void writeNode(const NodeType *node,
                   std::ofstream &out)
    {
        out << "\"" << node << " ";
        out << "[";
        for(int j = 0 ; j < NodeType::Dimension ; ++j) {
            out << node->index[j];
            if(j < NodeType::Dimension-1)
                out << " ";
        }
        out << "]\"" << " ";
    }

};
}
}
#endif // KDTREE_DOTTY_HPP
