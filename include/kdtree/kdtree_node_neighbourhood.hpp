#pragma once

#include <cstdint>
#include <array>

namespace kdtree
{
namespace detail
{
    namespace math
    {
        template<std::size_t n>
        struct pow3 {
            static const std::size_t value = 3 * pow3<n - 1>::value;
        };
        template<>
        struct pow3<0> {
            static const std::size_t value = 1;
        };
    }

    template<std::size_t row_count, std::size_t row, std::size_t col_count, std::size_t col>
    struct fill_row
    {
        static constexpr std::size_t div = math::pow3<col - 1>::value;

        template<typename T>
        static inline constexpr void assign(T& value)
        {
            using value_type = typename std::decay<decltype(value[row][col_count - col])>::type;

            value[row][col_count - col] = static_cast<value_type>((row / div) % 3 - 1);
            fill_row<row_count, row + 1, col_count, col>::assign(value);
        }
    };

    template<std::size_t row_count, std::size_t col_count, std::size_t col>
    struct fill_row<row_count, row_count, col_count, col>
    {
        template<typename T>
        static inline constexpr void assign(T&)
        {}
    };


    template<std::size_t row_count, std::size_t col_count, std::size_t col>
    struct fill_col
    {
        template<typename T>
        static inline constexpr void assign(T& value)
        {
            fill_row<row_count, 0, col_count, col>::assign(value);

            fill_col<row_count, col_count, col - 1>::assign(value);
        }
    };

    template<std::size_t row_count, std::size_t col_count>
    struct fill_col<row_count, col_count, 0>
    {
        template<typename T>
        static inline constexpr void assign(T&)
        {}
    };


    template<typename T, std::size_t Dim>
    struct fill
    {
        static constexpr auto rows = math::pow3<Dim>::value;
        static constexpr auto cols = Dim;
        typedef std::array<T, rows> Type;

        static inline constexpr void assign(Type& value)
        {
            fill_col<rows, cols, cols>::assign(value);
        }
    };
}

template<typename Tree, typename ITraits>
class KDTreeIndexNeigbourhood
{
public:
    typedef typename ITraits::Type              Type;
    static constexpr std::size_t Dimension = ITraits::Dimension;
    typedef detail::fill<Type, Dimension>       MaskFiller;
    typedef typename MaskFiller::Type           MaskType;


public:
    KDTreeIndexNeigbourhood(Tree& tree):
        _tree(tree),
        offsets()
    {
        MaskFiller::assign(offsets);
    }

    template<typename F>
    void visit(const Type& reference, F&& fun)
    {
        for (const Type& offset : offsets)
        {
            Type index = apply(reference, offset);
            auto node = _tree.find(index);
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
    Tree& _tree;
    MaskType offsets;
};

}
