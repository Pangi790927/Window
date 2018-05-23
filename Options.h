#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include <map>

using namespace std;

class Options{
public:
    map <string, int> options;

    Options(){
        options.insert( pair < string , bool >( "vSync", true ) );
        options.insert( pair < string , bool >( "Perspective", false ) );
        options.insert( pair < string , bool >( "Closed", false ) );
    }

    void InsertOption( string name, int val = 0 ){
        options.insert( pair < string , bool >( name, val ) );
    }

    int& operator [] ( string name ){
        return options[name];
    }
};

#endif // OPTIONS_H_INCLUDED
