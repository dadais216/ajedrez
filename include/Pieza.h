#ifndef PIEZA_H
#define PIEZA_H
//#include <cstdarg>
#include <tablero.h>
#include <global.h>
using namespace std;
using namespace sf;
struct operador;

struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias
    int sn;

    list<operador*> movs;

    Pieza(int,int);

    void calcularMovimientos(v);
};

struct Holder{
    Holder(int,Pieza*);
    Pieza* pieza;
    int bando;
    bool inicial;
    Holder* next;
    void draw(v);
    void draw(int);
};



#endif // PIEZA_H
