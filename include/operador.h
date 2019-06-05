#ifndef OPERADOR_H
#define OPERADOR_H

#include "global.h"

///el motivo de separar los movimientos normales en el operador normal y el normalholder es que toda la informacion
///normal (estructura de movimientos, posiciones relativas) se comparte entre todas las piezas del mismo tipo, como tambien
///el arbol de operadores. No tiene sentido tenerla copiada.

struct operador{
    int tipo;
    operador* sig;
    int32_t bools;//makeClick, hasClick, doEsp en normal
};
struct normal:public operador{
    normal(bool);
    ///@optim esto podria estar adentro del bucket, antes de la informacion especifica. Lector puede pasar
    ///la longitud de cada uno de estos vectores.
    barray<acct*> accs;
    barray<condt*> conds;
    barray<colort*> colors;
    struct setupTrigInfo{
        char type; //0 global 1 pieza 2 turno
        int ind;
    };
    barray<setupTrigInfo> setUpMemTriggersPerNormalHolder;//para que se pongan triggers permanentes de memoria que apunten a cada normalholder correspondiente
    v relPos;
};
struct desliz:public operador{
    desliz();
    operador* inside;
    size_t insideSize;
    size_t iterSize;
};
struct exc:public operador{
    exc();
    barray<operador*> ops;
    int insideSize;//tamaño de movHolders + lo que ocupe ops
};
struct isol:public operador{
    isol();
    int size;
    operador* inside;
};
struct desopt:public operador{
    desopt();
    barray<operador*> ops;
    barray<int> movSizes;//tamaño de cada movimiento, incluyendo puntero al proximo cluster
    int desoptInsideSize;
    int clusterSize;
    int dinamClusterBaseOffset;
};


operador* tomar();
operador* keepOn(int32_t*);
void crearClicker();

#endif // OPERADOR_H
