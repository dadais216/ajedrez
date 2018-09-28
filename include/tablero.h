#ifndef TABLERO_H
#define TABLERO_H

#include "global.h"

using namespace std;
using namespace sf;

struct Holder;
struct Tile;
struct normalHolder;
struct Trigger{
    Tile* tile; //pos donde estaria la pieza que puso el trigger
    normalHolder* nh; //puntero al movimiento a recalcular
    int step; //valor que se contrasta con el step de la tile. Si son el mismo la pieza que puso el trigger esta en el mismo lugar y no se movio, mh es valido
};
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
