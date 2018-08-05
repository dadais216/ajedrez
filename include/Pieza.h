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
struct acct;
struct colort;

struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias
    int sn;

    vector<operador*> movs;

    Pieza(int,int);
};

struct movHolder{
    operador* org;
    virtual void generar()=0;
    virtual void debug(){};
    movHolder* sig;
    bool valido; //por ahora solo de normal
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*);//supongo que ni bien se crea el op le copias las accs
    operador* op;
    Holder* h;
    vector<acct*> accs;
    vector<colort*> colors;
    virtual void generar();
    virtual void debug();

    void accionar();///desencadena los acct, solo de normal
    void draw();///dibuja los colores, solo de normal
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
    vector<movHolder*> movs;
    int id;
    int uniqueId,step;
    int bando;
    bool inicial;
    bool outbounds;
    ///vector de limites
    ///tablero del thread
    ///Holder* next;
};



#endif // PIEZA_H
