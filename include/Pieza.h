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

struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias
    int sn;

    vector<operador*> movs;

    Pieza(int,int);
};

///TODO como ahora se que es cada cosa en cada contexto no tiene sentido usar funciones virtuales
struct Base{
    virtual void reaccionar()=0;
    movHolder* lim;
};
struct normalHolder;
struct movHolder:public Base{
    operador* op;
    virtual void generar()=0;
    virtual void cargar(vector<normalHolder*>*)=0;
    virtual void debug()=0;
    virtual void draw()=0;
    virtual void reaccionar()=0;
    Holder* h;
    movHolder* sig;
    Base* base;
    bool valido;
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*);//supongo que ni bien se crea el op le copias las accs
    vector<acct*> accs;
    vector<colort*> colors;
    virtual void generar();
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    virtual void draw();
    virtual void reaccionar(){};
    void accionar();///desencadena los acct, solo de normal
};
struct deslizHolder:public movHolder{
    deslizHolder(Holder*,desliz*);
    vector<movHolder*> movs;
    virtual void generar();
    virtual void cargar(vector<normalHolder*>*);
    virtual void debug();
    virtual void draw();
    virtual void reaccionar();
    bool ignoreRecalc;
};

struct mvBase:public Base{
    mvBase(Holder*,operador*);
    movHolder* mov;
    vector<int> mem;
    //lim reemplaza a validez en las bases, porque una base puede estar en distintos grados de validez si contiene
    //operadores que generen clickers dentro suyo
    //por ej desliz normal1 end normal2 genera clickers aunque normal2 sea falso
    virtual void reaccionar();
};

struct Tile;
struct Holder{
    Holder(int,Pieza*,v);
    ~Holder();
    void draw();///dibuja la pieza
    void draw(int);///dibuja la pieza en la lista de capturados
    void makeCli();
    Pieza* pieza;
    Tile* tile;
    vector<Tile*> pisados;
    void generar();

    vector<mvBase> movs;
    vector<int>* memMovAct;
    int id;
    int uniqueId,step;
    int bando;
    bool inicial;
    bool outbounds;
    ///vector de limites
    ///tablero del thread
};



#endif // PIEZA_H
