#ifndef OPERADOR_H
#define OPERADOR_H

#include <global.h>
#include <Pieza.h>

enum t{condt,acct,movt};
struct acm{
    t tipo;
    void(*func)(void);
};

struct operador{
    virtual bool operar()=0;
    virtual void debug(){};
    operador* sig;
};

struct normal:public operador{
    normal();
    virtual void debug();
    virtual bool operar();
    list<acm*> acms;
};

struct desliz:public operador{
    desliz();
    virtual bool operar();
    operador* inside;
};

struct opt:public operador{
    opt();
    virtual bool operar();
    list<operador*> ops;
};

struct multi:public operador{
    multi();
    virtual bool operar();
    list<operador*> ops;
    //aplicar el anterior con las acciones generadas por los que den verdadero

    //algo wacky para esperar el input y seguir
};

#endif // OPERADOR_H
