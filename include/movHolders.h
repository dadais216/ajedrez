#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

#include "global.h"

struct Base{ ///datos compartidos de un movimiento entero
    Holder* h;
    movHolder* beg;
    int memLocalSize;//este es cte entre todos los holders por lo que podria estar en algun lugar del lado de operador
};
//@optim? se podria hacer que esta base sea global durante la generacion para no tenerla apuntada desde cada
//movholder. el problema esta que durante la reaccion necesito recuperar esa informacion, y la unica forma de
//hacerlo es cargandolo en los triggers, lo que puede que termine haciendo todo mas lento?
//@optim lo que se podria hacer para solucionar esto es guardar en operador el offset de la base, y usar eso
//en lugar de informacion en trigger para recuperar el estado en reaccion (osea, cambiaria un puntero en cada
//movHolder por un offset en normal y el uso de una global)


struct movHolder{
    movHolder(operador*,Base*);
    virtual void generar()=0;
    virtual void reaccionar(normalHolder*)=0;
    virtual void reaccionar(vector<normalHolder*>)=0;
    virtual void cargar(vector<normalHolder*>*)=0;
    void generarSig();
    void reaccionarSig(auto nhs){
        if(sig){
            sig->reaccionar(nhs);
            if(switchToGen){
                if(bools&hasClick||sig->bools&valorCadena)
                    bools|=valorCadena;
                else
                    bools&=~valorCadena;
                bools|=sig->bools&valorFinal;
            }
        }
    }
    Base* base;
    movHolder* sig;
    int32_t bools;
};
struct normalHolder:public movHolder{
    normalHolder(normal*,Base*,char**);//supongo que ni bien se crea el op le copias las accs
    normal* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    void draw();
    void accionar();///desencadena los acct
    barray<int> memAct;
    //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
    //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
    v offsetAct;
    v relPos; //pos actual = relPös + offset. Todas las acciones y condiciones la comparten
};
struct deslizHolder:public movHolder{
    deslizHolder(desliz*,Base*,char**);
    desliz* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    barray<void> movs;
    int cantElems;//cantidad de iteraciones armadas
    void maybeAddIteration(int);
    int f;
};
struct excHolder:public movHolder{
    excHolder(exc*,Base*,char**);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    barray<movHolder*> ops;
    ///@optim podria probar usar barray<int> tamaños en vez de los punteros. Ocupa menos espacio.
    ///no estoy seguro de si seria mas rapido
    int actualBranch;
};
struct isolHolder:public movHolder{
    isolHolder(isol*,Base*,char**);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    movHolder* inside;
};

struct desoptHolder:public movHolder{
    desoptHolder(desopt*,Base*,char**);
    desopt* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    struct node{
        node* iter;
        //movimiento
    };
    char* dinamClusterHead;
    void generarNodo(node*);
    void construirYGenerarNodo();
    void cargarNodos(node*,vector<normalHolder*>*);


    char* movs(){
        return (char*)this+sizeof(desoptHolder);
    }
};

#endif // MOVHOLDERS_H
