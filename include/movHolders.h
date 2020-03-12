#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT};

const int32_t valorCadena=1;//la cadena de movholders es valida. Una cadena va desde la base hasta un clicker o el final
const int32_t valorFinal=1<<1;//se llegó al final. Esto sirve para saber si seguir iterando en un desliz
const int32_t valor=1<<2;//lo usa normal para saber si sus condiciones son verdaderas,y exc si tiene una rama valida
const int32_t lastNotFalse=1<<2;//lo usa desliz
const int32_t makeClick=1<<3;
const int32_t hasClick=1<<4;
const int32_t doEsp=1<<5;//se usa en normalh

struct normalHolder;
struct {
  Holder* h;
  normalHolder* nh;

  void(**buffer)(void);
  int* bufferPos;
  //no hay un costo en agregar boludeces siempre que no me pase de la cache line
} actualHolder;//por ahora es global, me gustaria probar a pasarlo por parametro para ver si hay una diferencia de eficiencia. Supuestamente no pero quiero ver que pasa. Si no hay diferencia preferiría que sea no global


v offset;
Tile* actualTile;


bool switchToGen;

struct virtualTableMov{//probar implementar lo mismo con switches a ver que pasa
  void (*generar)(movHolder*);
  void (*reaccionar)(movHolder*,normalHolder*);
  void (*reaccionarVec)(movHolder*,vector<normalHolder*>*);
  void (*cargar)(movHolder*,vector<normalHolder*>*);
};


struct Base{ ///datos compartidos de un movimiento entero
    Holder* h;
    movHolder* beg;
    int memLocalSize;//este es cte entre todos los holders por lo que podria estar en algun lugar del lado de operador
};
//@optim? se podria hacer que esta base sea global durante la generacion para no tenerla apuntada desde cada
//movholder. el problema esta que durante la reaccion necesito recuperar esa informacion, y la unica forma de
//hacerlo es cargandolo en los triggers, lo que puede que termine haciendo todo mas lento?
//@optim lo que se podria hacer para solucionar esto es guardar en operador el offset de la base, y usar eso
//en lugar de informacion en trigger para recuperar el estado en reaccion (osea, cambiaria un puntero en cada
//movHolder por un offset en normal y el uso de una global)
inline v getActualPos(v relPos,v offset){
    bool negate=actualHolder.h->bando;
    relPos.y=(relPos.y^-negate)+negate;
    return relPos+offset;
}
inline v getOffset(v relPos,v pos){
    bool negate=actualHolder.h->bando;
    relPos.y=(relPos.y^-negate)+negate;
    return pos-relPos;
}

struct movHolder{
  virtualTableMov* table;
  Base* base; //ver de pasarselo al trigger como un offset para no tenerlo en cada movHolder, ver arriba
    movHolder* sig;
    int32_t bools;
};

struct normalHolder:public movHolder{
    normal* op;
    barray<int> memAct;
    //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
    //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
    v relPos; //pos actual = relPös + offset. Todas las acciones y condiciones la comparten
    v pos; //pos actual. @check por que se guarda?
};
void drawNormalH(normalHolder*);
void accionarNormalH(normalHolder*);
void initNormalH(normal*,Base*,char**);

struct deslizHolder:public movHolder{
    desliz* op;
    barrayE movs;
    int cantElems;//cantidad de iteraciones armadas
    int f;
};
void initDeslizH(desliz*,Base*,char**);

struct excHolder:public movHolder{
    barray<movHolder*> ops;
    ///@optim podria probar usar barray<int> tamaños en vez de los punteros. Ocupa menos espacio.
    ///no estoy seguro de si seria mas rapido
  int size;//no debería estar en el op?
    int actualBranch;
};
void initExcH(exc*,Base*,char**);

struct isolHolder:public movHolder{
    int size;
    movHolder* inside;
};
void initIsolH(isol*,Base*,char**);

struct desoptHolder:public movHolder{
    desopt* op;
    struct node{
        node* iter;
        //movimiento
    };
    char* dinamClusterHead;
};
void initDesoptH(desopt*,Base*,char**);

//comparten esto con movHolders
struct spawnerGen{
  virtualTableMov* table;
  Base* base;
};
void initSpawner(spawnerGen*,Base*);
struct kamikaseCntrl{
  virtualTableMov* table;
  Base* base;
};
void initKamikase(kamikaseCntrl*,Base*);

#endif // MOVHOLDERS_H
