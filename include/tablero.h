#ifndef TABLERO_H
#define TABLERO_H

#include <global.h>

using namespace std;
using namespace sf;

struct Pieza;
struct tablero{
    tablero();
    armar(v);
    Sprite b,n;
    v tam;
    vector<vector<Pieza*>> matriz;
    vector<vector<bool>> tiles;
    Pieza* operator()(v);
    void operator()(v,Pieza*);
    void drawTiles();
    void drawPieces();
};


#include <Juego.h>
#endif // TABLERO_H
