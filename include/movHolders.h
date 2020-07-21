#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT,FAILOP};

//las cadenas terminan en un clicker o cuando no hay nada despues

//valor indica si el movHolder es valido. Se usa para saber si seguir buscando en una cadena o cortar. Solo lo usan normal y exc
//valorCadena si la cadena es valida, puede darse que un eslabon lo sea pero el proximo no e invalida la cadena, no se tiene que cargar
//valorFinal si las cadenas contenidas en un operador son validas hasta el final, lo que solo es importante en operadores con varias cadenas

//valorCadena no es absolutamente necesario en la carga pero la hace mas rapida. Tambien se usa en exc y desliz para diferenciar una rama completa y una a medias

const int32_t valorCadena=1;
const int32_t valorFinal=1<<1;
const int32_t valor=1<<2;
const int32_t lastNotFalse=1<<2;//lo usa desliz
const int32_t makeClick=1<<3;
const int32_t hasClick=1<<4;
const int32_t doEsp=1<<5;//se usa en normalh

struct {
  Holder* h;
  normalHolder* nh;
  board* brd;
  Tile* tile;

  void(**buffer)(void);
  int* bufferPos;
  properState* ps;//lo usa spawn nomas para acceder a las piezas y el bucket de holders
  //tengo espacio para 1 puntero mas en esta cache line
  //igual si me paso supongo que no pasa nada siempre y cuando no use todo
} actualHolder;//por ahora es global, me gustaria probar a pasarlo por parametro para ver si hay una diferencia de eficiencia. Supuestamente no pero quiero ver que pasa. Si no hay diferencia preferiría que sea no global


v offset;


bool switchToGen;

struct virtualTableMov{//probar implementar lo mismo con switches a ver que pasa
  void (*generar)(movHolder*);
  void (*reaccionar)(movHolder*,normalHolder*);
  void (*reaccionarVec)(movHolder*,vector<normalHolder*>*);
  void (*cargar)(movHolder*,vector<normalHolder*>*);
};

struct memLocalt{
  int size;
  int resetUntil;
};
struct Base{ ///datos compartidos de un movimiento entero
  Holder* h;
  movHolder* root;
  memLocalt memLocal;//estos son cte entre todos los holders por lo que podrian estar en algun lugar del lado de operador
};
//@optim? se podria hacer que esta base sea global durante la generacion para no tenerla apuntada desde cada
//movholder. el problema esta que durante la reaccion necesito recuperar esa informacion, y la unica forma de
//hacerlo es cargandolo en los triggers, lo que puede que termine haciendo todo mas lento?
//@optim lo que se podria hacer para solucionar esto es guardar en operador el offset de la base, y usar eso
//en lugar de informacion en trigger para recuperar el estado en reaccion (osea, cambiaria un puntero en cada
//movHolder por un offset en normal y el uso de una global)

struct movHolder{
  virtualTableMov* table;
  Base* base; //ver de pasarselo al trigger como un offset para no tenerlo en cada movHolder, ver arriba
    movHolder* sig;
    int32_t bools;
};
struct normal;
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

struct desliz;
struct deslizHolder:public movHolder{
    desliz* op;
    barrayE movs;
    int cantElems;//cantidad de iteraciones armadas
    int f;
};
void initDeslizH(desliz*,Base*,char**);

struct exc;
struct excHolder:public movHolder{
    barray<movHolder*> ops;
    ///@optim podria probar usar barray<int> tamaños en vez de los punteros. Ocupa menos espacio.
    ///no estoy seguro de si seria mas rapido
  int size;//no debería estar en el op?
  int actualBranch;
};
void initExcH(exc*,Base*,char**);

struct isol;
struct isolHolder:public movHolder{
    int size;
    movHolder* inside;
};
void initIsolH(isol*,Base*,char**);

struct desopt;
struct desoptHolder:public movHolder{
  desopt* op;
  struct node{
    node* iter;
    //movimiento
  };
  char* dinamClusterHead;
  node movs[0];
};
void initDesoptH(desopt*,Base*,char**);

void initFailH(char**);

//comparten la tabla y base con los movHolders
struct spawnerGen{
  virtualTableMov* table;
  Base* base;
  Holder* h;
  bool kamikaseNext;
};
void initSpawner(spawnerGen*,Holder*,Base*,bool);
struct kamikaseCntrl{
  virtualTableMov* table;
  Holder* h;
};
void initKamikase(kamikaseCntrl*,Holder*);
void kamikaseCheckAlive(movHolder*);



#endif // MOVHOLDERS_H
