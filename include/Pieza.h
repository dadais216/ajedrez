#ifndef PIEZA_H
#define PIEZA_H
//#include <cstdarg>
#include "tablero.h"
#include "global.h"
#include "operador.h"
#include "movs.h"
using namespace std;
using namespace sf;
struct operador;
struct normal;
struct desliz;
struct acct;
struct colort;
struct normalHolder;

struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias
    int sn;

    vector<operador*> movs;

    Pieza(int,int);
};

struct Base{ ///datos compartidos de un movimiento entero
    movHolder* beg;
    vector<int>* mem;
};
struct normalHolder;
struct movHolder{
    movHolder(Holder*,operador*,Base*);
    operador* op;
    virtual void generar()=0;
    virtual void reaccionar(normalHolder*)=0;
    virtual void cargar(vector<normalHolder*>*)=0;
    virtual void debug()=0;
    void generarSig();
    Holder* h;
    movHolder* sig;
    Base base;
    bool valido;
    bool continuar;
    bool makeClick;
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*,Base*);//supongo que ni bien se crea el op le copias las accs
    vector<acct*> accs;
    vector<colort*> colors;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    void draw();
    void accionar();///desencadena los acct, solo de normal
    v offsetAct;
};
struct deslizHolder:public movHolder{
    deslizHolder(Holder*,desliz*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    vector<movHolder*> movs;
    int f;
};
struct excHolder:public movHolder{
    excHolder(Holder*,exc*,Base*);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    vector<movHolder*> ops;
    int actualBranch;
};


struct Tile;
struct Holder{
    Holder(int,Pieza*,v);
    void draw();///dibuja la pieza
    void draw(int);///dibuja la pieza en la lista de capturados
    void makeCli();
    Pieza* pieza;
    Tile* tile;
    vector<Tile*> pisados;
    void generar();
    void reaccionar(normalHolder*);

    vector<movHolder*> movs;
    vector<int>* memMovAct;
    int id;
    int bando;
    bool inicial;
    bool outbounds;
    ///vector de limites
    ///tablero del thread
};



#endif // PIEZA_H
