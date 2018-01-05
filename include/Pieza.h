#ifndef PIEZA_H
#define PIEZA_H
//#include <cstdarg>
#include <tablero.h>
#include <global.h>
using namespace std;
using namespace sf;
struct operador;

struct Pieza{
    Sprite sprite;
    int bando;
    int id; //para hacer copias
    int sn;

    list<operador*> movs;

    Pieza(int,int,int);
    void draw(v);
};



#endif // PIEZA_H
