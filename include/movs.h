#ifndef MOVS_H
#define MOVS_H

#include "Pieza.h"

///no uso herencia porque no me aporta casi nada
struct acct
{
    virtual void func(Holder*)=0;
    //func se llama para realizar la accion, pos arranca siendo relativa y se hace absoluta desde afuera
    virtual void debug()=0;
    virtual acct* clone()=0;
    v pos;
    ///la version en normal guarda las pos relativas, las copias en los normalHolder guardan
    ///las absolutas
};
struct condt
{
    virtual bool check(Holder*,v)=0;
    virtual void debug()=0;
    v pos;
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
#endif // MOVS_H
