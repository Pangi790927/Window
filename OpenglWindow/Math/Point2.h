#ifndef POINT2F_H_INCLUDED
#define POINT2F_H_INCLUDED

#include "Point.h"

template <typename type>
class container2{
public:
    union{
        type array[2];
        struct{
            type x;
            type y;
        };
    };
};

template <typename type>
class Point2 : public Point <container2<type>, 2> {
public:
    Point2( type x = 0.0f, type y = 0.0f){
        this->x = x;
        this->y = y;
    }

    Point2( const Point<container2<type>, 2>& arg ){
        this->x = arg.x;
        this->y = arg.y;
    }

    Point2( const Point<container2<type>, 1>& arg ){
        this->x = arg.x;
        this->y = 0;
    }

    float cross( const Point<container2<type>, 2>& arg ){
        return this->x * arg.y - this->y * arg.x;
    }
};

typedef Point2<float> Point2f;

#endif // POINT2F_H_INCLUDED
