#pragma once

#include <vector>
#include <array>
#include <limits>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>

namespace kdtree {
template<typename T, std::size_t Depth>
class Page {
public:
    static_assert(Depth > 1, "Depth > 1 required!");

    typedef Page<T,Depth>                  Type;
    typedef std::array<std::size_t, Depth> Size;
    typedef std::array<std::size_t, Depth> Index;
    typedef std::shared_ptr<Page>          Ptr;

    /// size :  size[i] = max_index[i] - min_index[i] + 1;

    //// ------------------------- internal helper classes for allocation ------------------------------ ////
    template<std::size_t Stage, typename V>
    struct Table {
        typedef std::shared_ptr<Table<Stage,V>> Ptr;
        typedef Table<Stage+1, V>               NextStage;

        Table(const Size &_size) :
            size(_size),
            data(size[Stage]),
            data_ptr(data.data())
        {
        }

        inline V & at(const Index &_index)
        {
            if(_index[Stage] >= size[Stage])
                throw std::out_of_range("Index is out of range at entry '" + str(Stage) + "'");

            typename Table<Stage+1,V>::Ptr &t = data_ptr[_index[Stage]];
            if(!t) {
                t.reset(new NextStage(size));
            }
            return t->at(_index);
        }

        inline void printInfo() const
        {
            std::size_t s  = getSize();
            std::size_t bs = getByteSize();
            std::cout << "[Page :] '" << s << "' elements" << std::endl;
            std::cout << "[Page :] '" << bs / (1024.0 * 1024.0) << "' MB" << std::endl;

        }

        inline std::size_t getSize() const
        {
            std::size_t s = size[Stage];
            for(typename NextStage::Ptr entry : data) {
                if(entry)
                    s += entry->getSize();
            }
            return s;
        }

        inline std::size_t getByteSize() const
        {
            std::size_t s = sizeof(typename NextStage::Ptr) * size[Stage];
            for(typename NextStage::Ptr entry : data) {
                if(entry)
                    s += entry->getByteSize();
            }
            return s;
        }

        const Size                           size;
        std::vector<typename NextStage::Ptr> data;
        typename NextStage::Ptr             *data_ptr;

    };

    template<typename V>
    struct Table<Depth-1, V> {
        typedef std::shared_ptr<Table<Depth-1,V>> Ptr;

        Table(const Size _size) :
            Stage(Depth-1),
            size(_size[Stage]),
            data(size),
            data_ptr(data.data())
        {
        }

        Table(const Table &other) = delete;
        Table & operator = (const Table &other) = delete;

        inline V & at(const Index &_index)
        {
            if(_index[Stage] >= size)
                throw std::out_of_range("Index is out of range at entry '" + str(Stage) + "'");

            return data_ptr[_index[Stage]];
        }

        inline std::size_t getSize() const
        {
            return size;
        }

        inline std::size_t getByteSize() const
        {
            return size * sizeof(V);
        }

        const std::size_t Stage;
        const std::size_t size;
        std::vector<V>    data;
        V                *data_ptr;
    };


    //// ------------------------- paging ------------------------------ ////
    Page(const Size &_size) :
        table(_size)
    {
    }

    Page(const Page &other) = delete;
    Page & operator = (const Page &other) = delete;

    inline T & at(const Index &_index)
    {
        return table.at(_index);
    }

    inline void printInfo() const
    {
        table.printInfo();
    }

private:
    Table<0,T> table;

    template<typename S>
    inline std::string str(const S &value)
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

};
}
