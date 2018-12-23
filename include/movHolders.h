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
    void generarSig();
    void reaccionarSig(auto nhs){
        if(sig){
            sig->reaccionar(nhs);
            if(switchToGen){
                valorCadena=hasClick||sig->valorCadena;
                valorFinal=sig->valorFinal;
            }
        }
    }
    Holder* h;
    movHolder* sig;
    Base base;
    bool valorCadena; //la cadena de movholders es valida. Una cadena va desde la base hasta un clicker o el final
    bool valorFinal;  //se llegó al final. Esto sirve para saber si seguir iterando en un desliz
    bool makeClick;
    bool hasClick;
    ///@optim meter estos bools en una variable
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*,Base*);//supongo que ni bien se crea el op le copias las accs
    vector<acct*> accs;
    vector<colort*> colors;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    void draw();
    void accionar();///desencadena los acct
    bool valor;       //las condiciones del movholder son verdaderas, las acciones guardadas validas
    v offsetAct;
    vector<int> memAct;
    //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
    //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
};
struct deslizHolder:public movHolder{
    deslizHolder(Holder*,desliz*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
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
    vector<movHolder*> ops;
    int actualBranch;
    bool valor;
};
struct isolHolder:public movHolder{
    isolHolder(Holder*,isol*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
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
    vector<node> nodes;
    vector<int> memAct;
};

//estos son todos los operadores. Püede que agregue uno más para movimientos no deterministicos


#endif // MOVHOLDERS_H
