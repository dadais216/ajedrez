#ifndef TABLERO_H
#define TABLERO_H

#include "global.h"

struct Trigger{
    Tile* tile; //pos donde estaria la pieza que puso el trigger
    normalHolder* nh; //puntero al movimiento a recalcular
    int step; //valor que se contrasta con el step de la tile. Si son el mismo la pieza que puso el trigger esta en el mismo lugar y no se movio, mh es valido
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
        normalHolder* nh;//para reaccionar
        getterCond* gc;//para identificar triggers y no poner de mas
        int step;//step de la tile de la pieza que puso el trigger
        int* stepCheck;//puntero al step de la tile de la pieza que puso el trigger
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
