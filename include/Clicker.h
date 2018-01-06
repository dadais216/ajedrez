#ifndef CLICKER_H
#define CLICKER_H
#include <vec.h>
#include <SFML/Graphics.hpp>

using namespace sf;

#include <global.h>

struct Clicker
{
    Clicker();
    list<acm*> acciones;
    bool tocado;
    void draw();
    void activar();
};

#endif // CLICKER_H
