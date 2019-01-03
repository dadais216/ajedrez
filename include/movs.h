#ifndef MOVS_H
#define MOVS_H

#include "Pieza.h"

//antes separaba acct en posicionales y de memoria, pero como estoy usando polimorfismo para los dos da lo mismo
//supongo que abstraer las cosas en comun en pos y mem reduce un poco el nivel de instrucciones, pero dentro de todo
//es lo mismo. Lo que mas ocupa es el clone de pos y ese no se puede abstraer porque el new necesita el tipo propio para
//que anden las funciones virtuales. Y puede que agregar otro nivel de herencia haga todo un poco mas lento
struct acct{
    acct(string* n):nomb(n){};
    virtual void func()=0;
    string* nomb;
};

struct condt{
    condt(string* n):nomb(n){};
    virtual bool check()=0;
    string* nomb;
};
struct colort{
    virtual void draw()=0;
    v pos;
};
struct color:public colort{
    color(RectangleShape*,v);
    RectangleShape* rs;
    virtual void draw();
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
struct getter{ //getters en acciones
    getter(){}
    virtual int* val()=0;
};
struct getterCond:public getter{ //getters locales
    getterCond(){}
    virtual int* val()=0;
    virtual void drawDebugMem()=0;
};
struct getterCondTrig:public getterCond{ //getters condiciones globales
    getterCondTrig(){}
    virtual int* val()=0;
    virtual void drawDebugMem()=0;
    int before;
    virtual bool change()=0;
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
template<bool(*chck)(getter*,getter*),string* n> struct macc:public acct{
    getter* i1;
    getter* i2;
    macc(getter* i1_,getter* i2_):acct(n),i1(i1_),i2(i2_){}
    virtual void func(){
        chck(i1,i2);
    }
};
inline bool mcmp(getter* a1,getter* a2){
    return *a1->val()==*a2->val();
}
inline bool mset(getter* a1,getter* a2){
    *a1->val()=*a2->val();
    return true;
}
inline bool madd(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    return true;
}
inline bool mless(getter* a1,getter* a2){
    return *a1->val()<*a2->val();
}


struct locala;

#endif // MOVS_H
