#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

#include "global.h"

struct Base{ ///datos compartidos de un movimiento entero
    movHolder* beg;
    int movSize;
};

struct movHolder{
    movHolder(Holder*,operador*,Base*);
    operador* op;
    virtual void generar()=0;
    virtual void reaccionar(normalHolder*)=0;
    virtual void reaccionar(vector<normalHolder*>)=0;
    virtual void cargar(vector<normalHolder*>*)=0;
    virtual void debug()=0;
    void generarSig();
    Holder* h;
    movHolder* sig;
    Base base;
    bool valido;
    bool continuar;
    bool allTheWay;///@optim meter estos bools en una variable?
    bool makeClick;
    bool hasClick;
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*,Base*);//supongo que ni bien se crea el op le copias las accs
    vector<acct*> accs;
    vector<colort*> colors;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    void draw();
    void accionar();///desencadena los acct
    v offsetAct;
    vector<int> memAct;
    //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
    //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
};
struct deslizHolder:public movHolder{
    deslizHolder(Holder*,desliz*,Base*);
    void generarSig();
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    vector<movHolder*> movs;
    int f;
    bool lastNotFalse;
};
struct excHolder:public movHolder{
    excHolder(Holder*,exc*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    vector<movHolder*> ops;
    int actualBranch;
};
struct isolHolder:public movHolder{
    isolHolder(Holder*,isol*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    movHolder* inside;
    int selfCount;
    v tempPos;
};
struct node{
    node(movHolder*);
    movHolder* mh;
    vector<node*> nodes;
};
struct desoptHolder:public movHolder{
    desoptHolder(Holder*,desopt*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    vector<node> nodes;
    vector<int> memAct;
};

//estos son todos los operadores. Püede que agregue uno más para movimientos no deterministicos


#endif // MOVHOLDERS_H
