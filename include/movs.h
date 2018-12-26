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
struct getter{
    getter(){}
    virtual int* val()=0;
    virtual void drawDebugMem()=0;
};
//antes habia una version por combinacion de getters para ahorrarme hacer los getters polimorficos, pero era mucho bloat
//total no es la version final, no creo que la velocidad lo valga
//mas que nada lo malo de los templates es que en el constructor de normal habia que discernir cada combinacion posible
template<bool(*chck)(getter*,getter*),string* n> struct mcond:public condt{
    getter* i1;
    getter* i2;
    mcond(getter* i1_,getter* i2_):condt(n),i1(i1_),i2(i2_){}
    virtual bool check(){
        return chck(i1,i2);
    }
};
/*
template<bool(*chck)(ma1,ma2),string* n> struct macc:public macct{
    getter* i1;
    getter* i2;
    macc(getter* i1_,getter* i2_):macct(n),i1(i1_),i2(i2_){}
    virtual bool check(){
        return chck(i1,i2);
    }
};
*/
inline bool mcmp(getter* a1,getter* a2){
    cout<<*a1->val()<<" == "<<*a2->val()<<endl;
    return *a1->val()==*a2->val();
}
inline bool mset(getter* a1,getter* a2){
    cout<<*a1->val()<<" <- "<<*a2->val()<<endl;
    *a1->val()=*a2->val();
    return true;
}
inline bool madd(getter* a1,getter* a2){
    cout<<*a1->val()<<" += "<<*a2->val()<<endl;
    *a1->val()+=*a2->val();
    return true;
}
inline bool mless(getter* a1,getter* a2){
    cout<<*a1->val()<<" < "<<*a2->val()<<endl;
    return *a1->val()<*a2->val();
}


struct locala;

#endif // MOVS_H
