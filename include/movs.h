#ifndef MOVS_H
#define MOVS_H

struct colort{
  void (*draw)(void);//probar con un switch despues
  v pos;
};
struct color:public colort{
  color(RectangleShape*);
    RectangleShape* rs;
  void draw();
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


//antes habia una version por combinacion de getters para ahorrarme hacer los getters polimorficos, pero era mucho bloat
//total no es la version final, no creo que la velocidad lo valga
//mas que nada lo malo de los templates es que en el constructor de normal habia que discernir cada combinacion posible
/*template<bool(*chck)(getter*,getter*),string* n> struct mcond{
    getter* i1;
    getter* i2;
    mcond(getter* i1_,getter* i2_):i1(i1_),i2(i2_){}
    virtual bool check(){
        return chck(i1,i2);
    }
};
template<bool(*chck)(getter*,getter*),string* n> struct macc{
    getter* i1;
    getter* i2;
    macc(getter* i1_,getter* i2_):i1(i1_),i2(i2_){}
    virtual void func(){
        chck(i1,i2);
    }
};viendolo ahora podría haber abstraido el void y bool en un typename para hacerlo mas criptico*/

struct Holder;
vector<Holder*> justSpawned;

v actualPosColor;

void* getNextInBuffer();

void mov();
void capt();
void spwn();
void pausa();
void msetG();
void msetT();
void msetGi();
void msetTi();

bool vacio();
bool enemigo();
bool aliado();
bool pass();
bool piece();

colort* crearColor(int,int,int);

#endif // MOVS_H
