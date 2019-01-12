#ifndef MOVS_H
#define MOVS_H

#include "Pieza.h"
#include "memGetters.h"
#include "movHolders.h"

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
inline bool mcmpCond(getter* a1,getter* a2){
    return *a1->val()==*a2->val();
}
inline bool msetCond(getter* a1,getter* a2){
    *a1->val()=*a2->val();
    return true;
}
inline bool msetAcc(getter* a1,getter* a2){///version para memorias globales, para activar triggers
    int* val=a1->val();
    int before=*val;
    *val=*a2->val();
    if(before!=*val)
        for(trigMemGlobal& tmg:*trigsMaybeActivate)
            if(tmg.nh->h!=actualHolder.h)
                trigsActivados.push_back(tmg.nh);
    return true;
}
inline bool msetAccTile(getter* a1,getter* a2){///version para tiles que necesitan que la pieza que puso el trigger no se haya movido a demas de que la memoria varie
    int* val=a1->val();
    int before=*val;
    *val=*a2->val();
    if(before!=*val)
        for(Tile::tileTrigInfo& tti:*reinterpret_cast<vector<Tile::tileTrigInfo>*>(trigsMaybeActivate))
            if(tti.nh->h!=actualHolder.h&&tti.step==*tti.stepCheck)
                trigsActivados.push_back(tti.nh);
    ///@optim? aca podrían removerse los invalidos. No se si vale la pena
}
inline bool maddCond(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    return true;
}
inline bool maddAcc(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    ///cargar triggers
    return true;
}
inline bool maddAccTile(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    ///cargar triggers
    return true;
}
inline bool mlessCond(getter* a1,getter* a2){
    return *a1->val()<*a2->val();
}
inline bool mmoreCond(getter* a1,getter* a2){
    return *a1->val()>*a2->val();
}


struct locala;

#endif // MOVS_H
