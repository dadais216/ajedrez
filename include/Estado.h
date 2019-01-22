#ifndef ESTADO_H
#define ESTADO_H

#include "global.h"
#include "tablero.h"
#include "Boton.h"



using namespace std;
using namespace sf;

struct Estado{
    Estado();
    virtual void update()=0;
    virtual void draw()=0;
    virtual ~Estado(){};
};

struct Arranque:public Estado{
    Arranque();
    virtual void update();
    virtual void draw();
    Sprite portada;
};

struct Selector:public Estado{
    Selector();
    vector<Boton*> botones;
    SelJugador sel1,sel2;
    virtual void update();
    virtual void draw();
    ~Selector();
};

struct Proper:public Estado{
    Proper(int,int,int);
    int id;
    void init();
    virtual void update();
    virtual void draw();
    bool inRange(v a); //uso esto?
    tabl tablero;
    int dt,clickI;
    Jugador* primero;
    Jugador* segundo;
    Sprite turnoBlanco,turnoNegro;
};

#endif // ESTADO_H
