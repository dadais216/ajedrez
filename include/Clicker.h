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
    v clickPos;
    void draw();
    bool update();
};

#endif // CLICKER_H
