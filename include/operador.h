#ifndef OPERADOR_H
#define OPERADOR_H

#include <global.h>

struct accion{
    void operator()(vec);
};
struct condicion{
    bool operator()(vec);
};

list<accion> buffer;

vec* or; //lo carga la pieza

bool separator;

struct operador{
    list<condicion> conds;
    list<accion> accs;
    bool operar(vec)=0;
    operador* sig;
};

struct normal:public operador{};

struct desliz:public operador{
    operador* inside;
};

struct or:public operador{
    list<operador*> ops;
};

struct multi:public operador{
    list<operador*> ops;
    list<accion> anteriores;
    //aplicar el anterior con las acciones generadas por los que den verdadero

    //algo wacky para esperar el input y seguir
};

#endif // OPERADOR_H
