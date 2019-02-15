#ifndef PIEZA_H
#define PIEZA_H
//#include <cstdarg>
#include "tablero.h"
#include "global.h"
#include "operador.h"
#include "movs.h"
#include "movHolders.h"
#include "memGetters.h"


struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias. signo indica bando. Hay dos versiones por el tema del espejado en el eje y, la otra opcion era poner ifs cada vez que se pedir la pos relativa
    int sn;

    struct base{
        operador* raiz;
        int memLocalSize;
        int size;
    };
    vector<base> movs;

    int memPiezaSize;


    Pieza(int,int);
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

    Pieza* pieza;
    Tile* tile;

    vector<int> memPieza;
    vector<memTriggers> memPiezaTrigs;

    vector<movHolder*> movs;
    int id;
    int uniqueId,step;
    int bando;
    bool inPlay;//false cuando la pieza esta generada y capturada. Solo se usa para evitar activar triggers dinamicos a capturados
};
struct Base;
movHolder* crearMovHolder(Holder*,operador*,Base*);





#endif // PIEZA_H
