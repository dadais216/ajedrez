#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

#include "global.h"

struct Base{ ///datos compartidos de un movimiento entero
    movHolder* beg;
    int memLocalSize;
};

struct movHolder{
    movHolder(Holder*,operador*,Base*);
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
    Base base;///@optim pointer
    bool valorCadena; //la cadena de movholders es valida. Una cadena va desde la base hasta un clicker o el final
    bool valorFinal;  //se llegó al final. Esto sirve para saber si seguir iterando en un desliz
    bool makeClick;
    bool hasClick;
    ///@optim meter estos bools en una variable
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*,Base*,char**);//supongo que ni bien se crea el op le copias las accs
    normal* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    void draw();
    void accionar();///desencadena los acct
    bool valor;       //las condiciones del movholder son verdaderas, las acciones guardadas validas
    barray<int> memAct;
    //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
    //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
    bool doEsp;//ṕarece que vale la pena tener copias. En especial de este porque lo escribo
    v offsetAct;
    v relPos; //pos actual = relPös + offset. Todas las acciones y condiciones la comparten
};
struct deslizHolder:public movHolder{
    deslizHolder(Holder*,desliz*,Base*,char**);
    desliz* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    barray<void> movs;
    int cantElems;//cantidad de iteraciones armadas
    void maybeAddIteration(int);
    int f;
    bool lastNotFalse;
};
struct excHolder:public movHolder{
    excHolder(Holder*,exc*,Base*,char**);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    barray<movHolder*> ops;
    ///@optim podria probar usar barray<int> tamaños en vez de los punteros. Ocupa menos espacio.
    ///no estoy seguro de si seria mas rapido
    int actualBranch;
    bool valor;
};
struct isolHolder:public movHolder{
    isolHolder(Holder*,isol*,Base*,char**);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>);
    virtual void cargar(vector<normalHolder*>*);
    movHolder* inside;
};

struct desoptHolder:public movHolder{
    desoptHolder(Holder*,desopt*,Base*,char**);
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
