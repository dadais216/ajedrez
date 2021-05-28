#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT,ISOLNRM,DESOPTNRM,FAILOP};

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

//TODO si una realocacion ahora puede pasar en cualquier spawn (que por ahora es lo unico, pero puede haber mas cosas) tener punteros aca es peligroso,
//porque no se reasignan despues de cada realocacion. 
struct {
  Holder* h;
  normalHolder* nh;
  int tile;

  void(**buffer)(void);
  int* bufferPos;
  bigVector* gameState;
  bigVector* opState;
  properState* ps;//lo usa spawn nomas para acceder a las piezas y el bucket de holders
  //tengo espacio para 1 puntero mas en esta cache line
  //igual si me paso supongo que no pasa nada siempre y cuando no use todo
} actualHolder;//por ahora es global, me gustaria probar a pasarlo por parametro para ver si hay una diferencia de eficiencia. Supuestamente no pero quiero ver que pasa. Si no hay diferencia preferiría que sea no global

template<typename T>
T* gameVector(int ind){
  assert(ind>=0&&ind<actualHolder.gameState->size);
  return (T*)&actualHolder.gameState->data[ind];
}
int indGameVector(void* ptr){
  int ind=(int)((char*)ptr-actualHolder.gameState->data);
  assert(ind>=0&&ind<actualHolder.gameState->size);
  return ind;
}

template<typename T=void>
T* gameVectorInc(int ind){
  assert(ind>=0&&ind<=actualHolder.gameState->size);
  return (T*)&actualHolder.gameState->data[ind];
}
int indGameVectorInc(void* ptr){
  int ind=(int)((char*)ptr-actualHolder.gameState->data);
  assert(ind>=0&&ind<=actualHolder.gameState->size);
  return ind;
}

template<typename T> T* varrayGameElem(varray<T>* va,int ind){
  return gameVectorInc<T>(va->beg+sizeof(T)*ind);
}

#define forVGame(varray)  for(auto [indv,el]=std::tuple{0,varrayGameElem(&varray,0)};indv<elems(varray);indv++,el=varrayGameElem(&varray,indv))

template<typename T> T* opVector(int ind){
  assert(ind>=0&&ind<actualHolder.opState->size);
  return (T*)&actualHolder.opState->data[ind];
}
int indOpVector(void* ptr){
  int ind=(int)((char*)ptr-actualHolder.opState->data);
  assert(ind>=0&&ind<actualHolder.opState->size);
  return ind;
}
template<typename T=void> T* opVectorInc(int ind){
  assert(ind>=0&&ind<=actualHolder.opState->size);
  return (T*)&actualHolder.opState->data[ind];
}
template<typename T> T* varrayOpElem(varray<T>* va,int ind){
  return opVectorInc<T>(va->beg+sizeof(T)*ind);
}
#define forVOp(varray)     for(auto [indv,el]=std::tuple{0,varrayOpElem(&varray,0)};indv<elems(varray);indv++,el=varrayOpElem(&varray,indv))

v offset;


bool switchToGen;

struct nhBuffer;
struct virtualTableMov{//probar implementar lo mismo con switches a ver que pasa
  void (*generar)(int);
  void (*reaccionar)(int,int);
  void (*reaccionarVec)(int,nhBuffer*);
  void (*cargar)(movHolder*,vector<normalHolder*>*);
};

struct memLocalt{
  int size;
  int resetUntil;
};
struct Base{ ///datos compartidos de un movimiento entero
  int holder;//cte entre movimientos
  int movRoot;
  memLocalt memLocal;//estos son cte entre todos los holders por lo que podrian estar en algun lugar del lado de operador
};
//@optim? se podria hacer que esta base sea global durante la generacion para no tenerla apuntada desde cada
//movholder. el problema esta que durante la reaccion necesito recuperar esa informacion, y la unica forma de
//hacerlo es cargandolo en los triggers, lo que puede que termine haciendo todo mas lento?

struct movHolder{
  virtualTableMov* table;//TODO cambiar por un indice a un array de tablas?
  int base;
  int sig;
  int32_t bools;
};
struct normal;
struct normalHolder:public movHolder{
  int op;
  varray<int> memAct;
  //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
  //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
  v pos; //pos actual. @check por que se guarda?
};
void generarNormalH(int);
void drawNormalH(normalHolder*);
void accionarNormalH(normalHolder*);
void initNormalH(normal*,int,bigVector*);

struct desliz;
struct deslizHolder:public movHolder{
  int op;

  int beg;
  int after;//TODO sacar
  int elemSize;//TODO sacar

  int cantElems;//cantidad de iteraciones armadas
  int f;
};
void initDeslizH(desliz*,int,bigVector*);

struct exc;
struct excHolder:public movHolder{
  varray<int> movs;
  ///@optim podria probar usar barray<int> tamaños en vez de los punteros. Ocupa menos espacio y estaria del lado del operador
  ///no estoy seguro de si seria mas rapido
  int size;//no debería estar en el op? TODO sacar
  int actualBranch;
};
void initExcH(exc*,int,bigVector*);

struct isol;
struct isolHolder:public movHolder{
    int size;
    int inside;
};
void initIsolH(isol*,int,bigVector*);
void initIsolNonResetMemH(isol*,int,bigVector*);

struct desopt;
struct desoptHolder:public movHolder{
  int op;
  int dinamClusterHead;
  int movs[0];
  //en memoria los nodos estan organizados como
  //int iter;
  //blob movHolder;
  //y movimientos hermanos contiguos,
  //donde iter apunta al inicio de sus hijos en caso de que el movholder tenga valorFinal
};
void initDesoptH(desopt*,int,bigVector*);
void initDesoptNonResetMemH(desopt*,int,bigVector*);

void initFailH(bigVector*);

/*
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
*/


#endif // MOVHOLDERS_H
