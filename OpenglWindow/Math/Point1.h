#ifndef POINT1F_H_INCLUDED
#define POINT1F_H_INCLUDED

#include "Point.h"

template <typename type>
class container1{
public:
    union{
        type array[1];
        struct{
            type x;
        };
    };
};

template <typename type>
class Point1 : public Point<container1<type>, 1> {
public:
    Point1( type x = 0.0f ){
        this->x = x;
    }

    Point1( const Point<container1<type>, 1>& arg ){
        this->x = arg.x;
    }

};

typedef Point1<float> Point1f;

#endif // POINT1F_H_INCLUDED
