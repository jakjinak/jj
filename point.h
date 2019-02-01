#ifndef JJ_POINT_H
#define JJ_POINT_H

namespace jj
{

/*! Represents a SIZE-dimensional point with coordinates of type T. */
template<typename T, int SIZE>
struct basic_point_t
{
    T X[SIZE]; //!< the coordinates

    /*! Ctor - defaults to origin */
    basic_point_t()
    {
        for (T& i : X)
            i = T();
    }
    /*! Ctor - for 1-dimensional points */
    template<typename T1, typename std::enable_if<!std::is_same<basic_point_t, T1>::value>::type>
    basic_point_t(T1 v1)
        : basic_point_t()
    {
        static_assert(SIZE >= 1, "Wrong size");
        X[0] = v1;
    }
    /*! Ctor - for 2-dimensional points */
    template<typename T1, typename T2>
    basic_point_t(T1 v1, T2 v2)
        : basic_point_t()
    {
        static_assert(SIZE >= 2, "Wrong size");
        X[0] = v1;
        X[1] = v2;
    }
    /*! Ctor - for 3-dimensional points */
    template<typename T1, typename T2, typename T3>
    basic_point_t(T1 v1, T2 v2, T3 v3)
        : basic_point_t()
    {
        static_assert(SIZE >= 3, "Wrong size");
        X[0] = v1;
        X[1] = v2;
        X[2] = v3;
    }
    /*! Copy ctor - copies coordinates from other instance. */
    basic_point_t(const basic_point_t& other)
    {
        operator=(other);
    }
    /*! Copies coordinates from other instance. */
    basic_point_t& operator=(const basic_point_t& other)
    {
        for (size_t i = 0; i < SIZE; ++i)
            X[i] = other.X[i];
        return *this;
    }
};

/*! A special case of basic_point_t, suitable for specifying 2-dimensional coordinates on the computer display (or window canvas). */
struct screen_point_t : public basic_point_t<int, 2>
{
    int& Row, //!< references the inner "y" (=2nd) coordinate
       & Column, //!< references the inner "x" (=1st) coordinate
       & Left, //!< references the inner "x" (=1st) coordinate
       & Top, //!< references the inner "y" (=2nd) coordinate
       & Height, //!< references the inner "y" (=2nd) coordinate
       & Width; //!< references the inner "x" (=1st) coordinate
    /*! Ctor - defaults to origin (all 0s) */
    screen_point_t() : Row(X[1]), Column(X[0]), Left(X[0]), Top(X[1]), Height(X[1]), Width(X[0]) {}
    /*! Ctor */
    screen_point_t(int column, int row) : basic_point_t(column, row), Row(X[1]), Column(X[0]), Left(X[0]), Top(X[1]), Height(X[1]), Width(X[0]) {}
    /*! Copy ctor */
    screen_point_t(const screen_point_t& other) : basic_point_t<int, 2>(static_cast<const screen_point_t&>(other)), Row(X[1]), Column(X[0]), Left(X[0]), Top(X[1]), Height(X[1]), Width(X[0]) {}
    /*! Assigment */
    screen_point_t& operator=(const screen_point_t& other) { basic_point_t<int, 2>::operator=(other); return *this; }
};

} // namespace jj

#endif // JJ_POINT_H
