#ifndef PAGE_HPP
#define PAGE_HPP

#include <vector>
#include <array>
#include <limits>
#include <cmath>
#include <iostream>
#include <memory>

namespace kdtree {

template<std::size_t Depth, typename T>
class Page {
public:
    typedef typename Page<Depth, T>      Type;
    typedef std::array<std::size_t, Dim> Size;
    typedef std::array<std::size_t, Dim> Index;
    typedef std::shared_ptr<Page>        Ptr;

    struct Table {
        typedef std::shared_ptr<Table> Ptr;
    };

    struct MTable : public Table {
        std::vector<Table::Ptr> entries;
    };

    struct LTable : public Table {
        T value;
    };

    Page(const Size &_size) :
        size(_size)
    {
    }

    /// overwrite at function an do auto allocation



private:




};



}

#endif // PAGE_HPP
