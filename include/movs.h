#ifndef MOVS_H
#define MOVS_H

//TODO cambiar y medir. Por ahi hacerlo en pasos, primero reemplazar rs por uno global,
//despues cambiar polimorfismo por puntero de funcion (teoricamente deberia cambiar un poco, ver que pasa)
//antes para colores tenía un coso polimorfico, para los rectangulos
//tambien guardaba un puntero a un RectangleShape con los colores seteados
//por ahora voy a mantener los 3 valores de rgb directamente, y setear un rectangleShape global.
//es mas simple y me parece que va a ser mas rapido que buscar en un indice, setear los colores debería
//ser gratis. Tambien uso menos memoria, paso de usar 2 punteros a 3 chars.
//cuando agregue los otros 2 graficos, podría implementarlos como una union sobre el mismo tipo
//r==255 sería el discriminador (poniendo en 254 si lo mete el usuario),
//y g sería el indice de la pieza o el numero a imprimir
//struct colort{
//  char r,g,b;
//}


struct colort{
  virtual void draw(void)=0;
};
struct color:public colort{
  void init(RectangleShape* rs_){
    rs=rs_;//init en vez de constructor para poder usarlo en uno de mis vectores
  }
  RectangleShape* rs;
  virtual void draw();
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


void* getNextInBuffer();

void mov();
void capt();
void spwn();
void pausa();
void msetG();
void msetT();
void msetGi();
void msetTi();
void msetP();

bool vacio();
bool enemigo();
bool aliado();
bool self();
bool pass();
bool piece();
bool msetC();
bool maddC();
bool mcmp();
bool mdist();
bool mmore();
bool mless();
bool mgoto();

colort* crearColor(int,int,int);

#endif // MOVS_H
