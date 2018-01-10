#ifndef OPERADOR_H
#define OPERADOR_H

#include <global.h>
#include <Pieza.h>

enum t{condt,acct,movt,colort};
struct acm{
    t tipo;
    virtual void func()=0;
};

struct operador{
    virtual bool operar()=0;
    virtual void debug(){};
    bool then();
    operador* sig;
};

struct normal:public operador{
    normal();
    virtual void debug();
    virtual bool operar();
    list<acm*> acms;
};

//repite un operador normal hasta que falle, creando clickers en cada paso, a menos que sea deslizcond
struct desliz:public operador{
    desliz();
    virtual void debug();
    virtual bool operar();
    operador* inside;
};

struct opt:public operador{
    opt();
    virtual bool operar();
    virtual void debug();
    list<operador*> ops;
};

struct click:public operador{
    click();
    virtual bool operar();
    virtual void debug();
};

struct multi:public operador{
    multi();
    virtual bool operar();
    list<operador*> ops;
    //aplicar el anterior con las acciones generadas por los que den verdadero

    //algo wacky para esperar el input y seguir
};

operador* keepOn();
operador* tomar();

#endif // OPERADOR_H
