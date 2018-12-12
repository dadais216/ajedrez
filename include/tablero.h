#ifndef TABLERO_H
#define TABLERO_H

#include "global.h"


struct Tile{ //sería mejor usar vectores amigos supongo
    Holder* holder;
    vector<Trigger> triggers;
    //memoria
    bool color;
    v pos;
    int step; //se actualiza por mov, capt y spawn
    void chargeTriggers();
};
void activateTriggers();

struct tabl{
    tabl();
    void armar(v);
    Sprite b,n;
    v tam;
    vector<vector<Tile*>> matriz;
    Tile* tile(v);
    void drawTiles();
    void drawPieces();
};


#endif // TABLERO_H
