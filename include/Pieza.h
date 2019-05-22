#ifndef PIEZA_H
#define PIEZA_H

#include "global.h"


struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias. signo indica bando. Hay dos versiones por el tema del espejado en el eje y, la otra opcion era poner ifs cada vez que se pedir la pos relativa
    int sn;

    struct base{
        operador* raiz;
        int memLocalSize;
        int size;
    };
    barray<base> movs;

    int memPiezaSize;

    bool kamikase,spawner;


    Pieza(int,int);
};

extern int memLocalSizeAct;
extern vector<Tile*> pisados;

struct Holder{
    Holder(int,Pieza*,v);
    void draw();///dibuja la pieza
    void draw(int);///dibuja la pieza en la lista de capturados

    void makeCli();
    void generar();
    void reaccionar(normalHolder*);

    Pieza* pieza;
    Tile* tile;

    barray<int> memPieza;
    barray<memTriggers> memPiezaTrigs;

    barray<movHolder*> movs;
    int id;
    bool bando;
    bool inPlay;//false cuando la pieza esta generada y capturada. Solo se usa para evitar activar triggers dinamicos a capturados
};
struct Base;
void crearMovHolder(char**,operador*,Base*);





#endif // PIEZA_H
