#ifndef MOVS_H
#define MOVS_H

//antes separaba acct en posicionales y de memoria, pero como estoy usando polimorfismo para los dos da lo mismo
struct acct{
  void (*func)(void);
};

struct condt{
  bool (*check)(void);//esto queda asi para mantener una coherencia con la version debug
  debug(
        char name[16];
        )
};
struct colort{
  void (*draw)(void);//probar con un switch despues
  v pos;
};
struct color:public colort{
    RectangleShape* rs;
};
/*
struct drawable
{
    drawable(int t,void* o)
    {
        tipo=t;
        obj=o;
    }
    int tipo;
    void* obj;
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

struct locala;

vector<Holder*> justSpawned;

v actualPosColor;

#endif // MOVS_H
