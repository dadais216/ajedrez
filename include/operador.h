#ifndef OPERADOR_H
#define OPERADOR_H

#include "global.h"

struct Pieza;
struct Holder;
struct acct;
struct condt;
struct colort;
struct Base;

struct movHolder;
struct operador{
    virtual void operar(movHolder*,Holder*)=0;
    virtual void generarMovHolder(movHolder**,Holder*,Base*)=0;
//    virtual void react(movHolder*,Holder*)=0;
    void debugMovs(){
        debug();
        if(sig) sig->debugMovs();
    }
    virtual void debug(){};
    //bool then();
    operador* sig;
    ///no es necesario tener las bases, los holders se pueden acomodar solos porque uno construye al otro
    ///sig si porque no esta especificado en el tipo
};
struct normalHolder;
struct normal:public operador
{
    normal();
    normal(bool);
    virtual void operar(movHolder*,Holder*);
    virtual void generarMovHolder(movHolder**,Holder*,Base*);
//    virtual void react(movHolder*,Holder*);
    virtual void debug();
    vector<acct*> accs;
    vector<condt*> conds;
    vector<colort*> colors;
    colort* crearColor(v);
};

//repite un operador normal hasta que falle, creando clickers en cada paso, a menos que sea deslizcond
/*
struct desliz:public operador
{
    desliz();
    virtual void debug();
    bool doDebug;
    virtual bool operar(v pos);
    virtual void generarMovHolder(movHolder*);
    bool nc,t;
    int i,backlash,ret;
    v aux;
    operador* inside;
};

struct opt:public operador
{
    opt();
    virtual bool operar(v pos);
    virtual void debug();
    virtual void generarMovHolder(movHolder*);
    bool exc,nc;
    list<operador*> ops;
};

struct joiner:public operador
{
    joiner();
    //existe con el unico fin de manejar el caso de
    // desliz < opt < ... > > , ya que desliz setea
    // el sig de opt despues de que este haya armado sus ramas
    // y puesto los sig de estas en nullptt
    virtual bool operar(v pos);
    virtual void debug();
};

struct click:public operador
{
    click(bool);
    virtual bool operar(v pos);
    virtual void debug();
};

struct contr:public operador
{
    contr();
    virtual bool operar(v pos);
    virtual void debug();
};

struct bloque:public operador
{
    bloque();
    virtual bool operar(v pos);
    virtual void debug();
    operador* inside;
};
*/
//operador* keepOn();
operador* tomar();
bool operarAislado(operador*,bool);
void crearClicker();

#endif // OPERADOR_H
