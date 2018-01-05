#ifndef ESTADO_H
#define ESTADO_H

#include <global.h>
#include <tablero.h>

struct Boton;
struct Pieza;
struct Juego;
struct Clicker;

using namespace std;
using namespace sf;

struct Estado{
    Estado();
    virtual void update()=0;
    virtual void draw()=0;
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
    virtual void update();
    virtual void draw();
};

struct Proper:public Estado{
    Proper(int);
    virtual void update();
    virtual void draw();
    bool inRange(v a);
    tablero tablero;
    vector<Clicker*> clickers;


    void mover(v a, v b);
};

#endif // ESTADO_H
