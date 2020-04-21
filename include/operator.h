
struct parseMovData{
  bucket* b;
  parseData* pd;
  vector<int> tokens;
  int ind;
  int movSize;
  int memLocalSize;
  bool clickExplicit;
};


///el motivo de separar los movimientos normales en el operador normal y el normalholder es que toda la informacion
///normal (estructura de movimientos, posiciones relativas) se comparte entre todas las piezas del mismo tipo, como tambien
///el arbol de operadores.

struct operador{
  int tipo;
  operador* sig;
  int32_t bools;//makeClick, hasClick, doEsp en normal
};
struct normal:public operador{
  barray<void(*)(void)> accs;
  barray<bool(*)(void)> conds;
  barray<colort*> colors;
  //struct setupTrigInfo{
  //    char type; //0 global 1 pieza 2 turno
  //    int ind;
  //};
  //barray<setupTrigInfo> setUpMemTriggersPerNormalHolder;//para que se pongan triggers permanentes de memoria que apunten a cada normalholder correspondiente
  v relPos;
};
struct desliz:public operador{
  operador* inside;
  size_t insideSize;
  size_t iterSize;
};
struct exc:public operador{
  barray<operador*> ops;
  int insideSize;//tamaño de movHolders + lo que ocupe ops
};
struct isol:public operador{
  int size;
  operador* inside;
};
struct desopt:public operador{
  barray<operador*> ops;
  barray<int> movSizes;//tamaño de cada movimiento, incluyendo puntero al proximo cluster
  int desoptInsideSize;
  int clusterSize;
  int dinamClusterBaseOffset;
};


/*
Por ahora los operadores se guardan en buckets grandes porque no sé cuanto miden antes de crearlos.
Mas adelante podría probar meterlos en una memoria sola que crezca tipo vector, lo que tiene la ventaja
de que puedo usar ints en vez de punteros y no se tiene que mirar en 2 memorias si el operador esta justo
en el borde del bucket. No sé si va a ser un cambio importante, pero probar TODO
Tecnicamente podría saber cuanto mide antes de crearlos usando el parser, una vez que tengo los tokens finales
puedo sumar sizeof puntero por cada token, y sizeof movholder por cada token de movholder. Puede que sobreestime
el tamaño un poco, pero no importa. El tema es que tengo que tener los tamaños de todas las piezas, por lo que
tengo que hacer todas las tokenizaciones primero y las construcciones despues. Durante el tiempo de tokenizacion
deberia mirar los spawn para traer otras piezas. Se puede hacer, no sé si valdra la pena. Al final del dia tiene
las mismas ventajas que usar un vector, pero sin la necesidad de agrandar durante la generacion, por lo que
podría ser un poco más rapido durante el tiempo de creacion de piezas. No es muy importante, tener en cuenta que
esto solo pasa cuando se crear las piezas la primera vez.
*/

#define actionBuffer void(*)(void)
typedef  bool(*conditionBuffer)(void);

operador* parseOp(parseMovData*,bool fromNormal=false);
normal* parseNormal(parseMovData*);
desliz* parseDesliz(parseMovData*);
exc* parseExc(parseMovData*);
isol* parseIsol(parseMovData*);
desopt* parseDesopt(parseMovData*);
