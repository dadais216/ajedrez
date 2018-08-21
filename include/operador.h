#ifndef OPERADOR_H
#define OPERADOR_H

#include "global.h"

struct Pieza;
struct Holder;
struct acct;
struct condt;
struct colort;
struct Base;

///el motivo de separar los movimientos normales en el operador normal y el normalholder es que toda la informacion
///normal (estructura de movimientos, posiciones relativas) se comparte entre todas las piezas del mismo tipo, como tambien
///el arbol de operadores. No tiene sentido tenerla copiada.

struct movHolder;
struct operador{
    int tipo;
    bool contGenCl;//@optim puede obviarse preguntando directamente por los tipos, los unicos true son desliz y paso
    operador* sig;
};

struct normalHolder;
struct normal:public operador{
    normal(bool);
    void operar(normalHolder*,Holder*);
    void debug();
    vector<acct*> accs;
    vector<condt*> conds;
    vector<colort*> colors;
    colort* crearColor(v);
};
///mover cosas de operar a generar, mantener normal como la base de datos de pos relativas y eso.
///Supongo que la eficiencia es exactamente igual y hace el codigo menos bizarro
struct desliz:public operador{
    desliz();
    operador* inside;
};


operador* tomar();
operador* keepOn();
bool operarAislado(operador*,bool);
void crearClicker();

#endif // OPERADOR_H
