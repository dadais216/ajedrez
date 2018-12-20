#ifndef PIEZA_H
#define PIEZA_H
//#include <cstdarg>
#include "tablero.h"
#include "global.h"
#include "operador.h"
#include "movs.h"
#include "movHolders.h"


struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias
    int sn;

    struct base{
        operador* raiz;
        int memLocalSize;
    };
    vector<base> movs;

    int memPiezaSize;


    Pieza(int,int,int);
};

extern vector<Tile*> pisados;

struct Holder{
    Holder(int,Pieza*,v);
    ~Holder();
    void draw();///dibuja la pieza
    void draw(int);///dibuja la pieza en la lista de capturados

    void makeCli();
    void generar();
    void reaccionar(normalHolder*);

    Pieza* pieza; ///@optim pasar cosas especificas que se usen
    Tile* tile;

    vector<int> memPieza;
    int movSize;

    vector<movHolder*> movs;
    int id;
    int uniqueId,step;
    int bando;
    bool outbounds;
};
struct Base;
movHolder* crearMovHolder(Holder*,operador*,Base*);





#endif // PIEZA_H
