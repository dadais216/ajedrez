#ifndef PIEZA_H
#define PIEZA_H
//#include <cstdarg>
#include <tablero.h>
#include <global.h>
#include <operador.h>
using namespace std;
using namespace sf;
struct operador;

struct Pieza{
    Sprite spriteb,spriten;
    int id; //para hacer copias
    int sn;

    vector<operador*> movs;

    Pieza(int,int);

    void calcularMovimientos(v);
};

struct movHolder{
    operador* org;
    virtual void procesar(vector<v>&); ///mirar triggers, si hay fallos llamar a recalcular
    movHolder* sig;
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*);//supongo que ni bien se crea el op le copias las accs
    operador* op;
    Holder* h;
    vector<acct*> accs;
    vector<v> triggs; //en el constructor hacer que reserven algo de memoria
    virtual void procesar(vector<v>&);
};

struct Holder{
    Holder(int,Pieza*);
    void draw(v);
    void draw(int);
    Pieza* pieza;
    v pos;
    void procesar(vector<v>&);
    vector<movHolder*> movs;

    v ori;//se setea cuando se crea y por los movs
    int bando;
    bool inicial;
    bool outbounds;
    ///vector de limites
    ///tablero del thread
    ///Holder* next;
};



#endif // PIEZA_H
