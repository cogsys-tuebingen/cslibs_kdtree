#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <vector>
#include <array>
#include <limits>
#include <cmath>
#include <iostream>
#include <memory>
#include <cstring>

namespace kdtree {
template<typename T, std::size_t Dim>
class Array {
public:
    typedef Array<T, Dim>                Type;
    typedef std::array<std::size_t, Dim> Size;
    typedef std::array<std::size_t, Dim> Index;
    typedef std::array<std::size_t, Dim> Step;
    typedef std::shared_ptr<Array>       Ptr;

    Array(const Size &_size) :
        size(_size),
        data_size(1)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            data_size *= _size[i];
        }

        steps[0] = 1;
        if(Dim > 1) {
            std::size_t max_i = Dim - 1;
            for(std::size_t i = 1 ; i <= max_i ; ++i) {
                steps[i] = steps[i-1] * size[i-1];
            }
        }
        data.resize(data_size);
        data_ptr = data.data();
    }

    Array(const Array &other) = delete;
    Array & operator = (const Array &other) = delete;

    inline T & at(const Index &_index)
    {
        std::size_t pos = Array::pos(_index);
        return data_ptr[pos];
    }

    inline T & at(const Index &_index) const
    {
        std::size_t pos = Array::pos(_index);
        return data_ptr[pos];
    }

    inline void reset(const T &_v)
    {
        data.resize(data_size, _v);
        data_ptr = data.data();
    }

    inline void printInfo() const
    {
        std::cout << "[Array :] " << (data_size * sizeof(T)) / (1024.0 * 1024.0) << "MB" << std::endl;
        std::cout << "[Array :] ";
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            std::cout << " " << size[i];
        }
        std::cout << std::endl;
    }

private:
    inline std::size_t pos(const Index &_index) {
        std::size_t pos = 0;
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            std::size_t id = _index[i];
            if(id > size[i]) {
                throw  std::out_of_range("Index was out of range!");
            }
            pos += id * steps[i];
        }
        return pos;
    }

    std::vector<T>  data;
    T*              data_ptr;
    std::size_t     data_size;
    Size            size;
    Step            steps;

};




}
#endif // ARRAY_HPP
