#ifndef OPERADOR_H
#define OPERADOR_H

#include "global.h"

///el motivo de separar los movimientos normales en el operador normal y el normalholder es que toda la informacion
///normal (estructura de movimientos, posiciones relativas) se comparte entre todas las piezas del mismo tipo, como tambien
///el arbol de operadores. No tiene sentido tenerla copiada.

struct operador{
    int tipo;
    operador* sig;
    bool makeClick,hasClick;
};
struct normal:public operador{
    normal(bool);
    void debug();
    vector<acct*> accs;
    vector<condt*> conds;
    vector<colort*> colors;
    struct setupTrigInfo{
        char type; //0 global 1 pieza 2 turno
        int ind;
    };
    vector<setupTrigInfo> setUpMemTriggersPerNormalHolder;//para que se pongan triggers permanentes de memoria que apunten a cada normalholder correspondiente
    v relPos;
    bool doEsp;
};
struct desliz:public operador{
    desliz();
    operador* inside;
};
struct exc:public operador{
    exc();
    vector<operador*> ops;
};
struct isol:public operador{
    isol();
    operador* inside;
};
struct desopt:public operador{
    desopt();
    vector<operador*> ops;
};


operador* tomar();
operador* keepOn(bool*);
bool operarAislado(operador*,bool);
void crearClicker();

#endif // OPERADOR_H
