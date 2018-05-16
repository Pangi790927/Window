#ifndef MATRIX4F_H_INCLUDED
#define MATRIX4F_H_INCLUDED

#include "Point3.h"

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <stack>
#include <iostream>
#include <string>

/*
    /// not ready - not yet implemented
    //  not ready - not yet tested but implemented
    //  ready     - function implemented and tested

*/

class Matrix4f{
public:
    float mat[4][4];

    ~Matrix4f();                           // ready
    Matrix4f();                            // not ready
    Matrix4f( const Matrix4f &mat );          // not ready
    Matrix4f( float *mat );                // not ready
    Matrix4f( float mat[4][4] );           // not ready
    Matrix4f( float a11 , float a12 , float a13 , float a14 ,
           float a21 , float a22 , float a23 , float a24 ,
           float a31 , float a32 , float a33 , float a34 ,
           float a41 , float a42 , float a43 , float a44 ); // not ready

    float *operator[]( int i );             // not ready
    Matrix4f operator * ( Matrix4f mat );         // not ready
    Matrix4f operator * ( float a );           // not ready
    Matrix4f operator / ( float a );           // not ready
    Matrix4f operator - ( Matrix4f mat );         // not ready
    Matrix4f operator + ( Matrix4f mat );         // not ready
    Point3f operator * ( Point3f point );   // not ready

    friend Matrix4f operator * ( float a , Matrix4f mat );                                        // not ready
    friend std::istream& operator >> ( std::istream& stream , std::string inputStream );    // not ready
    friend std::ostream& operator << ( std::ostream& stream , const Matrix4f &mat );           // not ready

    Matrix4f static returnRotationMatrix( float radians , float x , float y , float z );              // not ready
    Matrix4f static returnRotationMatrix( float radians , Point3f vec );                              // not ready
    Matrix4f static returnTranslationMatrix( float x , float y , float z );                           // not ready
    Matrix4f static returnTranslationMatrix( Point3f vec );                                           // not ready
    Matrix4f static returnScaleMatrix( float x , float y , float z );                                 // not ready
    Matrix4f static returnScaleMatrix( Point3f vec );                                                 // not ready
    Matrix4f static returnScaleMatrix( float scale );                                                 // not ready
    Matrix4f static returnIdentityMatrix();                                                           // not ready

    Matrix4f returnTransposedMatrix();     // not ready
    void transposeTheMatrix();          // not ready
    Matrix4f returnInverseMatrix();        /// not ready
    void inverseTheMatrix();            /// not ready

    float *returnFloatArray();          // not ready

    static std::stack <Matrix4f> MatrixStack;

    std::stack <Matrix4f> static returnIdentityStack();

    void static Rotate( float radians , float x , float y , float z );      // not ready
    void static Rotate( float radians , Point3f vec );                      // not ready
    void static Translate( float x , float y , float z );                   // not ready
    void static Translate( Point3f vec );                                   // not ready
    void static Scale( float x , float y , float z );                       // not ready
    void static Scale( Point3f vec );                                       // not ready
    void static Scale( float scale );                                       // not ready
    void static LoadIdentity();                                             // not ready

    void static PushMatrix();                                               // not ready
    void static PopMatrix();                                                // not ready
    void static EmptyStack();                                               // not ready

    void static LoadMatrix( Matrix4f mat );                                    // not ready
    Matrix4f static GetTopMatrix();                                            // not ready
};

std::stack <Matrix4f> Matrix4f::MatrixStack = Matrix4f::returnIdentityStack() ;

std::stack <Matrix4f> Matrix4f::returnIdentityStack(){
    std::stack <Matrix4f> Stack;
    Stack.push( returnIdentityMatrix() );
    return Stack;
}

Matrix4f::~Matrix4f(){}

Matrix4f::Matrix4f(){
    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            mat[i][j] = 0;
}

Matrix4f::Matrix4f( const Matrix4f& mat ){
    *this = mat ;
}

Matrix4f::Matrix4f( float mat[4][4] ){
    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            this->mat[i][j] = mat[i][j];
}

Matrix4f::Matrix4f( float* mat ){
    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            this->mat[i][j] = mat[ i * 4 + j ] ;
}

Matrix4f::Matrix4f( float a11 , float a12 , float a13 , float a14 ,
              float a21 , float a22 , float a23 , float a24 ,
              float a31 , float a32 , float a33 , float a34 ,
              float a41 , float a42 , float a43 , float a44 )
{
    mat[0][0] = a11;
    mat[0][1] = a12;
    mat[0][2] = a13;
    mat[0][3] = a14;

    mat[1][0] = a21;
    mat[1][1] = a22;
    mat[1][2] = a23;
    mat[1][3] = a24;

    mat[2][0] = a31;
    mat[2][1] = a32;
    mat[2][2] = a33;
    mat[2][3] = a34;

    mat[3][0] = a41;
    mat[3][1] = a42;
    mat[3][2] = a43;
    mat[3][3] = a44;
}

float* Matrix4f::operator[]( int i ){
    return mat[i];
}

Matrix4f Matrix4f::operator*( Matrix4f mat ){
    Matrix4f aux;

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            for( int k = 0 ; k < 4 ; k++ )
                aux[i][j] += this->mat[i][k] * mat[k][j];

    return aux;
}

Matrix4f Matrix4f::operator*( float a ){
    Matrix4f aux;

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            aux[i][j] = this->mat[i][j] * a;

    return aux;
}

Matrix4f Matrix4f::operator/( float a ){
    Matrix4f aux;

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            aux[i][j] = this->mat[i][j] / a;

    return aux;
}

Matrix4f Matrix4f::operator-( Matrix4f mat ){
    Matrix4f aux;

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            aux[i][j] = this->mat[i][j] - mat[i][j];

    return aux;
}

Matrix4f Matrix4f::operator+( Matrix4f mat ){
    Matrix4f aux;

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            aux[i][j] = this->mat[i][j] + mat[i][j];

    return aux;
}

Point3f Matrix4f::operator * ( Point3f point ){
    Point3f aux = Point3f(0, 0, 0);

    aux.x = mat[0][0] * point.x + mat[0][1] * point.y + mat[0][2] * point.z + mat[0][3] * 1 ;
    aux.y = mat[1][0] * point.x + mat[1][1] * point.y + mat[1][2] * point.z + mat[1][3] * 1 ;
    aux.z = mat[2][0] * point.x + mat[2][1] * point.y + mat[2][2] * point.z + mat[2][3] * 1 ;

    return aux;
}

Matrix4f operator*( float a , Matrix4f mat ){
    return mat * a;
}

std::istream& operator >> ( std::istream& stream , Matrix4f &mat ){
    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            stream >> mat[i][j];

    return stream;
}

std::ostream& operator << ( std::ostream& stream , const Matrix4f &mat ){
    for( int i = 0 ; i < 4 ; i++ , stream << '\n' )
        for( int j = 0 ; j < 4 ; j++ )
            stream << mat.mat[i][j] << ' ' ;

    return stream;
}

Matrix4f Matrix4f::returnRotationMatrix( float radians , float x , float y , float z ){

    float cos_radians = cos( radians );
    float sin_radians = sin( radians );

    Matrix4f MatrixRotate ( x * x * ( 1.0f - cos_radians ) + cos_radians
                       , x * y * ( 1.0f - cos_radians ) + z * sin_radians
                       , x * z * ( 1.0f - cos_radians ) - y * sin_radians
                       , 0.0f
                       , x * y * ( 1.0f - cos_radians ) - z * sin_radians
                       , y * y * ( 1.0f - cos_radians ) + cos_radians
                       , y * z * ( 1.0f - cos_radians ) + x * sin_radians
                       , 0.0f
                       , x * z * ( 1.0f - cos_radians ) + y * sin_radians
                       , y * z * ( 1.0f - cos_radians ) - x * sin_radians
                       , z * z * ( 1.0f - cos_radians ) + cos_radians
                       , 0.0f
                       , 0.0f
                       , 0.0f
                       , 0.0f
                       , 1.0f
                       );

    return MatrixRotate;
}

Matrix4f Matrix4f::returnRotationMatrix( float radians , Point3f point ){
    return returnRotationMatrix( radians , point.x , point.y , point.z );
}

Matrix4f Matrix4f::returnTranslationMatrix( float x , float y , float z ){
    Matrix4f MatrixTranslate ( 1.0f , 0.0f , 0.0f , x ,
                            0.0f , 1.0f , 0.0f , y ,
                            0.0f , 0.0f , 1.0f , z ,
                            0.0f , 0.0f , 0.0f , 1.0f );

    return MatrixTranslate;
}

Matrix4f Matrix4f::returnTranslationMatrix( Point3f point ){
    return returnTranslationMatrix( point.x , point.y , point.z );
}

Matrix4f Matrix4f::returnScaleMatrix( float x , float y , float z ){
    Matrix4f MatrixScale ( x    , 0.0f , 0.0f , 0.0f ,
                        0.0f , y    , 0.0f , 0.0f ,
                        0.0f , 0.0f , z    , 0.0f ,
                        0.0f , 0.0f , 0.0f , 1.0f );

    return MatrixScale;
}

Matrix4f Matrix4f::returnScaleMatrix( Point3f point ){
    return returnScaleMatrix( point.x , point.y , point.z );
}

Matrix4f Matrix4f::returnScaleMatrix( float scale ){
    return returnScaleMatrix( scale , scale , scale );
}

Matrix4f Matrix4f::returnIdentityMatrix(){
    Matrix4f MatrixScale ( 1.0f , 0.0f , 0.0f , 0.0f ,
                        0.0f , 1.0f , 0.0f , 0.0f ,
                        0.0f , 0.0f , 1.0f , 0.0f ,
                        0.0f , 0.0f , 0.0f , 1.0f );

    return MatrixScale;
}

float* Matrix4f::returnFloatArray(){
    float *array = new float[16];

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            array[ i * 4 + j ] = mat[i][j];

    return array;
}

Matrix4f Matrix4f::returnTransposedMatrix(){
    Matrix4f aux;

    for( int i = 0 ; i < 4 ; i++ )
        for( int j = 0 ; j < 4 ; j++ )
            aux[i][j] = mat[j][i];

    return aux;
}

void Matrix4f::transposeTheMatrix(){
    for( int i = 0 ; i < 4 ; i++ )
        for( int j = i ; j < 4 ; j++ )
            std::swap( mat[i][j] , mat[j][i] );
}

void Matrix4f::Rotate( float radians , float x , float y , float z ){
    Matrix4f NewTop = returnRotationMatrix( radians , x , y , z ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::Rotate( float radians , Point3f vec ){
    Matrix4f NewTop = returnRotationMatrix( radians , vec ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );}

void Matrix4f::Translate( float x , float y , float z ){
    Matrix4f NewTop = returnTranslationMatrix( x , y , z ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::Translate( Point3f vec ){
    Matrix4f NewTop = returnTranslationMatrix( vec ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::Scale( float x , float y , float z ){
    Matrix4f NewTop = returnScaleMatrix( x , y , z ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::Scale( Point3f vec ){
    Matrix4f NewTop = returnScaleMatrix( vec ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::Scale( float scale ){
    Matrix4f NewTop = returnScaleMatrix( scale ) * MatrixStack.top();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::LoadIdentity(){
    Matrix4f NewTop = returnIdentityMatrix();
    MatrixStack.pop();
    MatrixStack.push( NewTop );
}

void Matrix4f::PushMatrix(){
    MatrixStack.push( GetTopMatrix() );
}

void Matrix4f::PopMatrix(){
    MatrixStack.pop();
}

void Matrix4f::EmptyStack(){
    while( !MatrixStack.empty() ){
        MatrixStack.pop();
    }
    LoadIdentity();
}

void Matrix4f::LoadMatrix( Matrix4f mat ){
    Matrix4f NewTop(mat);
    MatrixStack.pop();
    MatrixStack.push(mat);
}

Matrix4f Matrix4f::GetTopMatrix(){
    return MatrixStack.top();
}

Matrix4f returnInverseMatrix(){
    Matrix4f invMatrix;



    return invMatrix;
}

#endif // MATRIX_H_INCLUDED


#endif // MATRIX4F_H_INCLUDED
