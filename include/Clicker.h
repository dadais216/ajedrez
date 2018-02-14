#ifndef CLICKER_H
#define CLICKER_H
#include <vec.h>
#include <SFML/Graphics.hpp>

using namespace sf;

#include <global.h>

struct Clicker
{
    Clicker(bool);
    list<acm*> acciones;
    list<pair<drawable,v>> colores;
    v clickPos;
    void draw();
    bool update();
    static bool drawClickers;
    bool activo;
    void activacion(int);
    void accionar();
    int val,mod;
    Holder* pieza;
    v bOrg;
};

#endif // CLICKER_H
