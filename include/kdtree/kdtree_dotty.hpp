#pragma once

#include <deque>
#include <map>
#include <cmath>
#include <fstream>
#include <random>

#include "kdtree.hpp"

namespace kdtree
{
namespace visualize
{

struct Color
{
    double value[3];

    Color() :
        value{0.0, 0.0, 0.0}
    {
    }

    Color (double r)
    {
        value[0] = r;
        value[1] = 0.5;
        value[2] = 1.0;
    }

    Color (double r,
           double g,
           double b)
    {
        value[0] = r;
        value[1] = g;
        value[2] = b;
    }

    void writeStyle(std::ofstream& out)
    {
        out << "[style=filled, fillcolor=\""
            << value[0] << " "
            << value[1] << " "
            << value[2] << "\"]";
    }
};

template<bool full_random = false>
struct ColorMap
{
    std::map<int, Color>                   palette;
    std::default_random_engine             generator;
    std::uniform_real_distribution<double> random;

    ColorMap() :
        random(0.0, 1.0)
    {
    }

    void writeStyle(const int id,
                    std::ofstream& out)
    {
        if (palette.find(id) == palette.end())
        {
            if (full_random)
                palette.insert(std::make_pair(id, Color(random(generator),
                                                        random(generator),
                                                        random(generator))));
            else
                palette.insert(std::make_pair(id, Color(random(generator))));
        }

        palette[id].writeStyle(out);
    }

    void getColor(const int id,
                  Color& color)
    {
        if (palette.find(id) == palette.end())
        {
            if (full_random)
                palette.insert(std::make_pair(id, Color(random(generator),
                                                        random(generator) ,
                                                        random(generator))));
            else
                palette.insert(std::make_pair(id, Color(random(generator))));
        }

        color = palette[id];
    }
};

template<typename Tree>
struct Dotty
{
    typedef Tree KDTree;
    typedef typename KDTree::DataType DataType;
    typedef typename KDTree::NodeType NodeType;
    static constexpr auto Dim = NodeType::IndexDimension;

    ColorMap<>   color_palette;

    static_assert(std::is_base_of<KDTreeNodeClusteringSupport, DataType>::value,
                  "NodeType does not have KDTreeNodeClusteringSupport");

    void write(KDTree& tree,
               std::ofstream& out)
    {
        out << "graph kdtree {" << std::endl;
        writeStyles(tree, out);
        out << std::endl;
        auto root = tree.get_root();
        write(root, out);
        out << "}" << std::endl;
    }

    void writeStyles(KDTree& tree, std::ofstream& out)
    {
        auto visitor = [&](NodeType& node)
        {
            writeNode(&node,  out);
            if (node.data.cluster >= 0)
            {
                out << " ";
                color_palette.writeStyle(node.data.cluster, out);
            }
            out << std::endl;
        };
        tree.traverse_nodes(visitor);
    }

    void write(const NodeType* root,
               std::ofstream& out)
    {
        std::deque<const NodeType*> lefts;
        runLeft(root, lefts);
        for(size_t i = 0 ; i < lefts.size() ; ++i) {
            writeNode(lefts.at(i), out);
            if(i < lefts.size() - 1)
                out << "-- ";
        }
        out << std::endl;
        for(const NodeType* left : lefts)
            runRight(left, out);
    }

    void runLeft(const NodeType* left,
                 std::deque<const NodeType*>& lefts)
    {
        if( left)
        {
            lefts.push_back(left);
            runLeft(left->left, lefts);
        }
    }

    void runRight(const NodeType* parent,
                  std::ofstream& out)
    {
        out << "  " << std::endl;
        writeNode(parent, out);
        if (parent->right)
        {
            out << "-- ";
            write(parent->right, out);
        }
    }

    void writeNode(const NodeType* node,
                   std::ofstream& out)
    {
        out << "\"" << node << " ";
        out << "[";
        for (int j = 0 ; j < Dim ; ++j)
        {
            out << node->index[j];
            if (j < Dim - 1)
                out << " ";
        }
        out << "]\"" << " ";
    }

};

template<typename Tree>
void visualize_dotty(Tree& tree, const std::string& filename)
{
    Dotty<Tree> vis;
    std::ofstream out(filename);
    vis.write(tree, out);
    out.flush();
}

}
}
