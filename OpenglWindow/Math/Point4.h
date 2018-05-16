#ifndef POINT4F_H_INCLUDED
#define POINT4F_H_INCLUDED

#include "Point.h"
#include "Point4.h"

#include "Point.h"
#include "Point3.h"

template <typename type>
class container4{
public:
    union{
        type array[4];
        struct{
            type x;
            type y;
            type z;
            type w;
        };
        struct{
            type r;
            type g;
            type b;
            type a;
        };
    };
};

template <typename type>
class Point4 : public Point <container4<type>, 3> {
public:
    Point4( type x = 0.0f, type y = 0.0f, type z = 0.0f, type w = 0.0f ){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Point4( const Point<container4<type>, 4>& arg ){
        this->x = arg.x;
        this->y = arg.y;
        this->z = arg.z;
        this->w = arg.w;
    }

    Point4( const Point<container3<type>, 3>& arg ){
        this->x = arg.x;
        this->y = arg.y;
        this->z = arg.z;
        this->w = 0;
    }

    Point4( const Point<container2<type>, 2>& arg ){
        this->x = arg.x;
        this->y = arg.y;
        this->z = 0;
        this->w = 0;
    }

    Point4( const Point<container1<type>, 1>& arg ){
        this->x = arg.x;
        this->y = 0;
        this->z = 0;
        this->w = 0;
    }
};

typedef Point4<float> Point4f;
typedef Point4<float> Color4f;

#endif // POINT4F_H_INCLUDED
