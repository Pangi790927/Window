#ifndef POINT3F_H_INCLUDED
#define POINT3F_H_INCLUDED

#include "Point.h"
#include "Point2.h"
#include "Point1.h"

template <typename type>
class container3{
public:
    union{
        type array[3];
        struct{
            type x;
            type y;
            type z;
        };
    };
};

template <typename type>
class Point3 : public Point <container3<type>, 3> {
public:
    Point3( type x = 0.0f, type y = 0.0f, type z = 0.0f ){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point3( const Point<container3<type>, 3>& arg ){
        this->x = arg.x;
        this->y = arg.y;
        this->z = arg.z;
    }

    Point3( const Point<container2<type>, 2>& arg ){
        this->x = arg.x;
        this->y = arg.y;
        this->z = 0;
    }

    Point3( const Point<container1<type>, 1>& arg ){
        this->x = arg.x;
        this->y = 0;
        this->z = 0;
    }

    static Point3 cross( const Point3& arg1, const Point3& arg2 ){
        return Point3( arg1.y * arg2.z - arg1.z * arg2.y,
                       arg1.z * arg2.x - arg1.x * arg2.z,
                       arg1.x * arg2.y - arg1.y * arg2.x );
    }

    Point3 cross( const Point3& arg ){
        return cross( *this, arg );
    }
};

typedef Point3<float> Point3f;
typedef Point3<int> Point3i;
typedef Point3f point3f;
typedef point3f vec3f ;

#endif // POINT3F_H_INCLUDED
