#ifndef MOVS_H
#define MOVS_H

#include "Pieza.h"

extern string strmov;
struct acct{
    acct(v pos_,string* n):nomb(n),pos(pos_){};
    virtual void func()=0;
    virtual acct* clone(Holder*)=0;
    string* nomb;
    v pos;
    Holder* h;
    ///la version en normal guarda las pos relativas, las copias en los normalHolder guardan
    ///las absolutas
};
struct condt{
    condt(v pos_, string* n):nomb(n),pos(pos_){};
    virtual bool check(Holder*)=0;
    string* nomb;
    v pos;
};

struct mcondt{
    mcondt(string* n):nomb(n){};
    virtual bool check()=0;
    string* nomb;
};
struct colort{
    virtual void draw()=0;
    virtual colort* clone()=0;
    v pos;
};
struct color:public colort{
    color(RectangleShape*,v);
    RectangleShape* rs;
    virtual void draw();
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

struct debugInicial:public condt{
    debugInicial();
    virtual bool check(Holder*);
};


#endif // MOVS_H
