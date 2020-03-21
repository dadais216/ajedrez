#ifndef OPERADOR_H
#define OPERADOR_H

///el motivo de separar los movimientos normales en el operador normal y el normalholder es que toda la informacion
///normal (estructura de movimientos, posiciones relativas) se comparte entre todas las piezas del mismo tipo, como tambien
///el arbol de operadores. No tiene sentido tenerla copiada.

struct operador{
  int tipo;
  operador* sig;
  int32_t bools;//makeClick, hasClick, doEsp en normal
};
struct normal:public operador{
  ///@optim esto podria estar adentro del bucket, antes de la informacion especifica. Lector puede pasar
  ///la longitud de cada uno de estos vectores.
  barray<void(*)(void)> accs;
  barray<bool(*)(void)> conds;
  barray<void(*)(void)> colors;
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


operador* tomar();
operador* keepOn(int32_t*);
void crearClicker();


bool separator;
bool clickExplicit;///cuando se usa click explicitamente no se pone un click implicitamente
///al final del movimiento si este termina en una no normal
///@detail una condicion mejor sería no poner click implicito si el ultimo operador no normal contiene algun click explicito


#endif // OPERADOR_H
