#ifndef TABLERO_H
#define TABLERO_H

#include "global.h"

using namespace std;
using namespace sf;

struct Holder;
struct tabl
{
    tabl();
    void armar(v);
    Sprite b,n;
    v tam;
    vector<vector<Holder*>> matriz;
    vector<vector<bool>> tiles;
    Holder* operator()(v);
    void operator()(v,Holder*);
    void drawTiles();
    void drawPieces();
};


#endif // TABLERO_H
