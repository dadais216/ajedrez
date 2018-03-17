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
    Holder* next;
    void draw(v);
    void draw(int);
    Pieza* pieza;
    int bando;
    bool inicial;
    v ori;//se setea cuando se crea y por los movs
    v pos;
    ///estructuras de mov
    ///tablero del thread
};



#endif // PIEZA_H
