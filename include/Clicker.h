#ifndef CLICKER_H
#define CLICKER_H
#include <vec.h>
#include <SFML/Graphics.hpp>

using namespace sf;

#include <global.h>

struct Clicker
{
    Clicker(v ve);
    RectangleShape rectangle;
    v ve;
    bool tocado;
    void draw();
};

#endif // CLICKER_H
