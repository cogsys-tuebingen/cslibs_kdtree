#ifndef INDEX_HPP
#define INDEX_HPP

#include <array>

namespace kdtree {

template <std::size_t Dim, typename S, typename D>
struct ArrayOperations {
    typedef S                  src_type;
    typedef D                  dst_type;
    typedef std::array<D, Dim> dst_array_type;
    typedef std::array<S, Dim> src_array_type;

    static inline dst_array_type copy(const src_array_type &_src)
    {
        dst_array_type dst;
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            dst[i] = (D) _src[i];
        }
        return dst;
    }

    static inline void copy(const src_array_type &_src,
                            dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = (dst_type) _src[i];
        }
    }

    static inline dst_array_type add(const src_array_type &_a,
                                     const src_array_type &_b)
    {
        dst_array_type dst;
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            dst[i] = (dst_type)(_a[i] + _b[i]);
        }
    }

    static inline void add(dst_array_type &_dst,
                           const src_array_type &_src)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] += (dst_type)(_src[i]);
        }
    }

    static inline void add(const src_array_type &_a,
                           const src_array_type &_b,
                            dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = (dst_type)(_a[i] + _b[i]);
        }
    }

    static inline dst_array_type sub(const src_array_type &_a,
                                     const src_array_type &_b)
    {
        dst_array_type dst;
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            dst[i] = (dst_type)(_a[i] - _b[i]);
        }
    }

    static inline void sub(dst_array_type &_dst,
                           const src_array_type &_src)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] -= (dst_type)(_src[i]);
        }
    }

    static inline void sub(const src_array_type &_a,
                           const src_array_type &_b,
                            dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = (dst_type)(_a[i] - _b[i]);
        }
    }

    static inline dst_array_type cwise_min(const src_array_type &_a,
                                           const src_array_type &_b)
    {
        dst_array_type dst;
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            dst[i] = std::min(_a[i], _b[i]);
        }
        return dst;
    }

    static inline void cwise_min(const src_array_type &_src,
                                 dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = std::min(_src[i], _dst[i]);
        }
    }

    static inline dst_array_type cwise_min(const src_array_type &_a,
                                           const src_array_type &_b,
                                           dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = std::min(_a[i], _b[i]);
        }
    }

    static inline dst_array_type cwise_max(const src_array_type &_a,
                                           const src_array_type &_b)
    {
        dst_array_type dst;
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            dst[i] = std::max(_a[i], _b[i]);
        }
        return dst;
    }

    static inline void cwise_max(const src_array_type &_src,
                                 dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = std::max(_src[i], _dst[i]);
        }
    }

    static inline dst_array_type cwise_max(const src_array_type &_a,
                                           const src_array_type &_b,
                                           dst_array_type &_dst)
    {
        for(std::size_t i = 0 ; i < Dim ; ++i) {
            _dst[i] = std::max(_a[i], _b[i]);
        }
    }
};
}
#endif // INDEX_HPP
