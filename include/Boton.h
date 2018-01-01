#ifndef BOTON_H
#define BOTON_H
#include <array>
#include <iostream>
#include <SFML/Graphics.hpp>


using namespace sf;
using namespace std;
struct Boton{
    Text text;
    Sprite sprite;
    static int i;
    static bool tocado;
    int n;
    int x,y;
    Boton(string nombre);
    void clicked();
    void draw();
};

#endif // BOTON_H
