#ifndef MATRIZ_H
#define MATRIZ_H
#include <array>
using namespace std;

struct c{
    int id,x,y;
    c(int,int,int);
};

struct Matriz{
    array<array<int,8>,8> self;
    Matriz();
    void armar(array<array<int,8>,8>);
};

#endif // MATRIZ_H
