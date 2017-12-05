#ifndef JJ_GEOMETRY_H
#define JJ_GEOMETRY_H

namespace jj
{

template<typename T, int SIZE>
struct basic_point_t
{
    T X[SIZE];

    basic_point_t()
    {
        for (T& i : X)
            i = T();
    }
    template<typename T1>
    basic_point_t(T1 v1)
        : basic_point_t()
    {
        static_assert(SIZE >= 1, "Wrong size");
        X[0] = v1;
    }
    template<typename T1, typename T2>
    basic_point_t(T1 v1, T2 v2)
        : basic_point_t()
    {
        static_assert(SIZE >= 2, "Wrong size");
        X[0] = v1;
        X[1] = v2;
    }
    template<typename T1, typename T2, typename T3>
    basic_point_t(T1 v1, T2 v2, T3 v3)
        : basic_point_t()
    {
        static_assert(SIZE >= 3, "Wrong size");
        X[0] = v1;
        X[1] = v2;
        X[2] = v3;
    }
    basic_point_t& operator=(const basic_point_t& other)
    {
        for (size_t i = 0; i < SIZE; ++i)
            X[i] = other.X[i];
        return *this;
    }
};

struct screen_point_t : public basic_point_t<int, 2>
{
    int& Row,
       & Column,
       & Left,
       & Top,
       & Height,
       & Width;
    screen_point_t() : Row(X[1]), Column(X[0]), Left(X[0]), Top(X[1]), Height(X[1]), Width(X[0]) {}
    screen_point_t(int column, int row) : basic_point_t(column, row), Row(X[1]), Column(X[0]), Left(X[0]), Top(X[1]), Height(X[1]), Width(X[0]) {}
    screen_point_t& operator=(const screen_point_t& other) { basic_point_t<int, 2>::operator=(other); return *this; }
};

} // namespace jj

#endif // JJ_GEOMETRY_H
