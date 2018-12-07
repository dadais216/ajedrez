#ifndef MOVS_H
#define MOVS_H

#include "Pieza.h"

///no uso herencia porque no me aporta casi nada
struct acct{
    acct(v pos_,const string& n):pos(pos_){};
    virtual void func()=0;
    //func se llama para realizar la accion, pos arranca siendo relativa y se hace absoluta desde afuera
    void debug(){
        cout<<nomb<<" "<<pos<<endl;
    }
    virtual acct* clone(Holder*)=0;
    v pos;
    Holder* h;
    ///la version en normal guarda las pos relativas, las copias en los normalHolder guardan
    ///las absolutas
    const string nomb;
};
struct condt{
    condt(v pos_,const string& n):pos(pos_){};
    virtual bool check(Holder*)=0;
    void debug(){
        cout<<nomb<<" "<<pos<<endl;
    }
    v pos;
    const string nomb; //espero que no ocupe memoria, si ocupa cambiarlo por un int
};
struct colort
{
    virtual void draw()=0;
    /*los colores cambian bastante, antes func hacia que se guarden en el buffer que
    se copiaba a clicker.
    Ahora como buffereo los movimientos puedo usar esta funcion para dibujar
    en vez de hacerlo desde afuera
    */
    virtual void debug()=0;
    virtual colort* clone()=0;
    v pos;
};
struct color:public colort
{
    color(RectangleShape*,v);
    RectangleShape* rs;
    virtual void draw();
    virtual void debug();
    virtual color* clone();
};
/*
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

//esta en el header para que se reconozca en el constructor de pieza
///no sé si sigue siendo necesario, por ahi puedo mover los templates aca
struct debugInicial:public condt{
    debugInicial();
    virtual bool check(Holder*);
};
struct passCond:public condt{
    passCond(v);
    virtual bool check(Holder*);
};
struct passAcc:public acct{
    passAcc(v);
    virtual void func();
    virtual acct* clone(Holder*);
};

#endif // MOVS_H
