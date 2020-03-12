#ifndef TABLERO_H
#define TABLERO_H

struct Holder;
struct tile;
struct normalHolder;
//si usara step en pieza en vez de tiles podría no pasar stepCheck
struct Trigger{
  normalHolder* nh; //puntero al movimiento a recalcular
  int* stepCheck; //step de tile donde estaría la pieza que puso el trigger
  int step; //valor. Si son el mismo que el check la pieza que puso el trigger esta en el mismo lugar y no se movio, nh es valido
};

struct triggerBox{
  Trigger triggers[6];//6 porque hace medir 2 caches y es un numero razonable de triggers
  triggerBox* next;
};

struct Tile{
  Holder* holder;
  int triggersUsed;
  triggerBox* triggers;
  v pos;//tecnicamente es calculable, que sé yo
  int step; //se actualiza por mov, capt y spawn
};

//hice las cuentas y se podría meter 1 trigger en Tile para aprovechar la cache line. No sé si vale la pena porque es un trigger solo, pero bueno


//la idea es tener la matriz de tiles, cada tile con un primer conjunto de triggers y el resto se tira a una estructura de buckets
//la memoria tile se mantiene aparte, son una matriz 3d que se accede con x,y,ind

//creo que los triggers van a estar junto a las memorias, porque como en los triggers comunes, cuando se accede a una memoria se van a
//escribir o leer los triggers.



void activateTriggers();

struct board{
  Sprite b,n;
  v dims;
  Tile tiles[0];
};

#endif // TABLERO_H
