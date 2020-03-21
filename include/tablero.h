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
//@todo stepCheck no es necesario, tengo la tile cuando hago charge. Por ahora lo dejo para comparar velocidades


const int triggersPerBox=3;//3=1 cache suponiendo que no haya padding xd
struct triggerBox{
  Trigger triggers[triggersPerBox];
  int next;
};

struct triggerSpace{
  int firstFree:
  int lastFree;
  int size;
  triggerBox* mem;
};

struct Tile{
  Holder* holder;
  int triggersUsed;
  int firstTriggerBox;
  v pos;//tecnicamente es calculable, que sé yo
  int step; //se actualiza por mov, capt y spawn
};

//hice las cuentas y se podría meter 1 trigger en Tile para aprovechar la cache line. No sé si vale la pena porque es un trigger solo, pero bueno

struct memData{
  int val;
  int triggersUsed;
  int firstTriggerBox;
};

/*
no estoy seguro de si tener un triggerBox prealocado para cada tile, que se mantiene, o no.
Si se tiene hace el codigo más simple, no necesito tener el if de si el tile tiene una caja inicial en push, ni necesito
guardar el indice inicial (se calcula a traves de pos). Esto es especialmente util con las memorias, porque no necesito un
vector de ints que indique si cierto indice de cierta tile tiene una caja inicial, y cual es su posicion. (aunque si necesitaria un vector
de usados y cual es el valor de memoria)
El argumento en contra es que se esta agregando memoria para cosas que pueden usarse o no, lo que para tableros grandes podría
ser malo. Tambien que se va a tener la memoria de cada tile mezclada suponiendo que tenga mas de un trigger, lo que hace accesos
dispersos y va a necesitar mas copias cuando haga paralelismo.

Por ahora voy a ir por la opcion de alocar solo lo necesario, pero la otra opcion podría ser mejor no sé.
Para probar la alternativa debería:
-Sacar el int que direcciona a la primer caja de trigger posicional, y el vector de ints que direcciona a los primeros triggers de memoria
-Sacar el if de push
-en un init alocar las cajas de cada tile, que se van a acceder por pos.x+pos.y*width en posicionales y id+pos.x*slots+pos.y*width*slots
Algo interesante que se podría intentar es hacer que este primer triggerBox este incluido en el Tile, porque siempre que se accede a tile
se va acceder a este

Otra cuestion es si hacer que se comparta o no el mismo triggerSpace para todo
 */

void activateTriggers();

struct board{
  Sprite b,n;
  v dims;
  triggerSpace ts;
  memData* memGlobals;//alocado despues de tiles
  memData* memTiles;
  Tile tiles[0];
};

//@optim cuando tenga forma de medir velocidad probar meter triggers fijos devuelta.
//actualmente los accesos a memoria global directos se hacen poniendo un trigger cada turno, y borrando en charge.
//se podría ahorrar procesado haciendo que los triggers solo se pongan en la creacion de la pieza, y se borren cuando la capturen
//estos triggers fijos serían apuntados en una version de memData que tenga otro triggersUsed, usando el mismo sistema que los demas triggers.
//Estos triggers solo se accederían desde setG, y no usarian en step y charge no los limpiaria.
//lo complicado sería borrarlos despues de una captura, supongo que habra un flag que indique si una pieza tiene triggers de este tipo, y si
//los tiene se va a tener que buscar en una tabla cuales son, buscarlos y borrarlos. Puede que baste con reutilizar step como un id del holder,
//y si el holder tiene el flag recorrer los memData de las memorias a las que accede (informacion que esta en Pieza para la construccion) y borrar los que
//tengan el mismo id






#endif // TABLERO_H
