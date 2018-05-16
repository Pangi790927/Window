#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

#include <cmath>
#include <iostream>
#include <type_traits>

using namespace std;

// container must have an member type array[N]
template<class container, int N>
class Point : public container{
public:
    Point(){
        for(int i = 0; i < N; i++)
            container::array[i] = 0;
    }

    Point operator+ ( const Point& arg ){
        Point result = Point();

        for( int i = 0; i < N; i++ )
            result.array[i] = container::array[i] + arg.array[i];

        return result;
    }

    Point operator- ( const Point& arg ){
        Point result = Point();

        for( int i = 0; i < N; i++ )
            result.array[i] = container::array[i] - arg.array[i];

        return result;
    }

    Point operator+= ( const Point& arg ){
        for( int i = 0; i < N; i++ )
            container::array[i] = container::array[i] + arg.array[i];

        return (*this);
    }

    Point operator-= ( const Point& arg ){
        for( int i = 0; i < N; i++ )
            container::array[i] = container::array[i] - arg.array[i];

        return (*this);
    }


    template <typename type>
    Point operator* ( type arg ){
        Point result = Point();

        for( int i = 0; i < N; i++ )
            result.array[i] = container::array[i] * arg;

        return result;
    }

    template <typename type>
    friend Point operator*( type val, const Point& vec ){
        Point result = Point();

        for( int i = 0; i < N; i++ )
            result.array[i] = val * vec.array[i];

        return result;
    }

    template <typename type>
    Point operator/ ( type arg ){
        Point result = Point();

        for( int i = 0; i < N; i++ )
            result.array[i] = container::array[i] / arg;

        return result;
    }

    template <typename type>
    Point operator*= ( type arg ){
        for( int i = 0; i < N; i++ )
            container::array[i] = container::array[i] * arg;

        return (*this);
    }

    template <typename type>
    Point operator/= ( type arg ){
        for( int i = 0; i < N; i++ )
            container::array[i] = container::array[i] / arg;

        return (*this);
    }


    typedef typename remove_reference<decltype(container::array[0])>::type ellementType;

    static ellementType dot( const Point& arg1, const Point& arg2 ){
        ellementType result = 0;

        for( int i = 0; i < N; i++ )
            result += arg1.array[i] * arg2.array[i];

        return result;
    }

     ellementType dot( const Point& arg ){
        ellementType result = 0;

        for( int i = 0; i < N; i++ )
            result += arg.array[i] * container::array[i];

        return result;
    }

    ellementType abs(){
        ellementType resultSquared = 0;

        for( int i = 0; i < N; i++ )
            resultSquared += container::array[i] * container::array[i];

        return sqrt( resultSquared );
    }

    static ellementType abs( const Point& arg ){
        ellementType resultSquared = 0;

        for( int i = 0; i < N; i++ )
            resultSquared += arg.array[i] * arg.array[i];

        return sqrt( resultSquared );
    }

    ellementType absSquared(){
        ellementType resultSquared = 0;

        for( int i = 0; i < N; i++ )
            resultSquared += container::array[i] * container::array[i];

        return resultSquared;
    }

    static ellementType absSquared( const Point& arg ){
        ellementType resultSquared = 0;

        for( int i = 0; i < N; i++ )
            resultSquared += arg.array[i] * arg.array[i];

        return resultSquared;
    }

    Point normalize(){
        return (*this) = ( (*this) / (*this).abs() );
    }

    static Point normalize( Point& arg ){
        return arg.normalize();
    }


    friend ostream& operator<< ( ostream& stream, const Point& arg ){
        for( int i = 0; i < N; i++ )
            stream << arg.array[i] << ' ';

        return stream;
    }

    friend istream& operator>> ( istream& stream, Point& arg ){
        for( int i = 0; i < N; i++ )
            stream >> arg.array[i];

        return stream;
    }
};

//
//    template <typename type>
//    class containerX{
//    public:
//        union{
//            type array[X];
//            struct{
//                type a1;
//                type a2;
//                ...
//                type aX;
//            };
//        };
//    };
//
//    template <typename type>
//    class PointX : public Point<containerX<type>, X>{
//    public:
//        PointX( type a1, type a2, ..., type aX ){
//            this->a1 = a1;
//            ...
//            this->aX = aX;
//        }
//    };
//
//    typedef PointXT PointX<T>;

#endif // POINT_H_INCLUDED
