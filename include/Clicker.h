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
    list<pair<RectangleShape*,v>> colores;
    v clickPos;
    void draw();
    bool update();
    static bool drawClickers;
};

#endif // CLICKER_H
