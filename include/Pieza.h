#ifndef PIEZA_H
#define PIEZA_H
#include <SFML/Graphics.hpp>
#include <Manager.h>
#include <vector>
#include <cstdarg>
#include <vec.h>
#include <tablero.h>
using namespace std;
using namespace sf;
struct Juego;

struct Pieza{
    RenderWindow* window;
    Sprite sprite;
    tablero* t;
    int bando;

    Pieza();
    void draw(int,int);
};



#endif // PIEZA_H
