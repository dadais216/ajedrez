#ifndef OPERADOR_H
#define OPERADOR_H

#include <global.h>

struct Pieza;
struct Holder;

///no uso herencia porque no me aporta casi nada
struct acct{
    virtual void func(Holder*)=0;
    //func se llama para realizar la accion, pos arranca siendo relativa y se hace absoluta desde afuera
    virtual void debug()=0;
    virtual acct* clone();
    v pos;
};
struct condt{
    virtual bool check(Holder*,v)=0;
    virtual void debug()=0;
    v pos;
};
struct colort{
    virtual void draw()=0;
    /*los colores cambian bastante, antes func hacia que se guarden en el buffer que
    se copiaba a clicker.
    Ahora como buffereo los movimientos puedo usar esta funcion para dibujar
    en vez de hacerlo desde afuera
    */
    virtual void debug()=0;
    v pos;
};
/*
struct color:public acm{
    sf::Color _color;
    RectangleShape cuadrado;

    color();
    virtual void func();
    virtual void debug();
};
struct sprt:public acm{
    sprt();
    Sprite _sprt;
    virtual void func();
    virtual void debug();
};
struct numShow:public acm{
    numShow();
    Text txt;
    int index;
    virtual void func();
    virtual void debug();
};
*/

struct movHolder;
struct operador{
    virtual bool operar(movHolder*,Holder*)=0;
    virtual void debug(){};
    virtual void generarMovHolder(movHolder*)=0;
    bool then();
    operador* sig;
};
struct normalHolder;
struct normal:public operador{
    normal();
    virtual void debug();
    virtual bool operar(movHolder*,Holder*);
    virtual void generarMovHolder(movHolder*);
    vector<acct*> accs;
    vector<condt*> conds;
    vector<colort*> colors;
};

//repite un operador normal hasta que falle, creando clickers en cada paso, a menos que sea deslizcond
struct desliz:public operador{
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

struct opt:public operador{
    opt();
    virtual bool operar(v pos);
    virtual void debug();
    virtual void generarMovHolder(movHolder*);
    bool exc,nc;
    list<operador*> ops;
};

struct joiner:public operador{
    joiner();
    //existe con el unico fin de manejar el caso de
    // desliz < opt < ... > > , ya que desliz setea
    // el sig de opt despues de que este haya armado sus ramas
    // y puesto los sig de estas en nullptt
    virtual bool operar(v pos);
    virtual void debug();
};

struct click:public operador{
    click(bool);
    virtual bool operar(v pos);
    virtual void debug();
};

struct contr:public operador{
    contr();
    virtual bool operar(v pos);
    virtual void debug();
};

struct bloque:public operador{
    bloque();
    virtual bool operar(v pos);
    virtual void debug();
    operador* inside;
};

operador* keepOn();
operador* tomar();
bool operarAislado(operador*,bool);
void crearClicker();

#endif // OPERADOR_H
