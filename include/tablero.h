#ifndef TABLERO_H
#define TABLERO_H

#include "global.h"

struct Trigger{
    Tile* tile; //pos donde estaria la pieza que puso el trigger
    int step; //valor que se contrasta con el step de la tile. Si son el mismo la pieza que puso el trigger esta en el mismo lugar y no se movio, mh es valido
    normalHolder* nh; //movimiento a recalcular
    int condIndex;//movimiento a recalcular
};
struct Tile{

    Holder* holder;
    vector<Trigger> triggers;
    //memoria
    bool color;
    v pos;
    int step; //se actualiza por mov, capt y spawn
    void chargeTriggers();
    vector<int> memTile;
    struct tileTrigInfo{
        int step;//step de la tile de la pieza que puso el trigger
        int* stepCheck;//puntero al step de la tile de la pieza que puso el trigger
        normalHolder* nh;//para reaccionar
        int condIndex;
    };
    vector<vector<tileTrigInfo>> memTileTrigs;
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
