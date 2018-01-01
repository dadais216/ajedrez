#ifndef OPERADOR_H
#define OPERADOR_H

#include <global.h>

struct accion{
    vec pos;
    void accion();
};
struct condicion{
    vec pos;
    bool condicion();
};

list<accion> buffer;

struct operador{
    list<condicion> conds;
    list<accion> accs;
    bool operar(vec);
    operador* sig;
};

struct normal:public operador{};

struct desliz:public operador{
    operador* inside;
    //if inside avanzar y repetir, cuando falle pasar al siguiente
};

struct or:public operador{
    list<operador*> ops;
    //if !op ir al siguiente
};

struct multi:public operador{
    list<operador*> ops;
    list<accion> anteriores;
    //aplicar el anterior con las acciones generadas por los que den verdadero

    //algo wacky para esperar el input y seguir
};

#endif // OPERADOR_H
