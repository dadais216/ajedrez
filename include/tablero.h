#ifndef TABLERO_H
#define TABLERO_H
#include <vector>
#include <vec.h>
#include <SFML/Graphics.hpp>
#include <Manager.h>

using namespace std;
using namespace sf;
struct Pieza;
struct casillero{
    Pieza* pieza;
    bool tile;
    casillero();
};
struct tablero{
    tablero();
    armar(v);
    Sprite b,n;
    v tam;
    float escala;
    vector<vector<casillero>> matriz;
    Pieza* operator()(v);
    void operator()(v,Pieza*);
    void drawTiles();
    void drawPieces();
};


#include <Juego.h>
#endif // TABLERO_H
