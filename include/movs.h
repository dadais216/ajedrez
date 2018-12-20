#ifndef MOVS_H
#define MOVS_H

#include "Pieza.h"

///podrían ser dos ramas de herencia separadas sino, creo que da lo mismo
struct macct{
    macct(string* n):nomb(n){};
    virtual void func()=0;
    virtual acct* clone(Holder*)=0;
    string* nomb;
};
struct acct:public macct{
    acct(string* n,v pos_):macct(n),pos(pos_){};
    virtual void func()=0;
    virtual acct* clone(Holder*)=0;
    v pos;
    Holder* h;
    ///la version en normal guarda las pos relativas, las copias en los normalHolder guardan
    ///las absolutas
};
struct condt{
    condt(string* n):nomb(n){};
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
    virtual bool check();
};
///si tarda mucho en compilar hacerlo por polimorfismo, total esta no va a ser la version definitiva
template<typename ma1,typename ma2,bool(*chck)(ma1,ma2),string* n> struct mcond:public condt{
    ma1 i1;
    ma2 i2;
    mcond(ma1 i1_,ma2 i2_):condt(n),i1(i1_),i2(i2_){}
    virtual bool check(){
        return chck(i1,i2);
    }
};
template<typename ma1,typename ma2,bool(*chck)(ma1,ma2),string* n> struct macc:public macct{
    ma1 i1;
    ma2 i2;
    macc(ma1 i1_,ma2 i2_):macct(n),i1(i1_),i2(i2_){}
    virtual bool check(){
        return chck(i1,i2);
    }
};
template<typename ma1,typename ma2> bool mcmp(ma1 a1,ma2 a2){
    cout<<*a1.val()<<" == "<<*a2.val()<<endl;
    return *a1.val()==*a2.val();
}
template<typename ma1,typename ma2> bool mset(ma1 a1,ma2 a2){
    cout<<*a1.val()<<" <- "<<*a2.val()<<endl;
    *a1.val()=*a2.val();
    return true;
}
struct locala;

#endif // MOVS_H
