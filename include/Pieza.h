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
    virtual void procesar(vector<v>&)=0; ///mirar triggers, si hay fallos llamar a recalcular
    virtual void generar()=0;///se llama solo al inicio
    movHolder* sig;
};
struct normalHolder:public movHolder{
    normalHolder(Holder*,normal*);//supongo que ni bien se crea el op le copias las accs
    operador* op;
    Holder* h;
    vector<acct*> accs;
    vector<colort*> colors;
    vector<v> triggs; //en el constructor hacer que reserven algo de memoria
    virtual void procesar(vector<v>&);
    virtual void generar();

    void accionar();///desencadena los acct, solo de normal
    void draw();///dibuja los colores, solo de normal
};

struct Holder{
    Holder(int,Pieza*,v);
    void draw();///dibuja la pieza
    void draw(int);///dibuja la pieza en la lista de capturados
    void show(list<Clicker*>& clk);///arma los clickers
    Pieza* pieza;
    v pos;
    void procesar(vector<v>&);
    void generar();
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
