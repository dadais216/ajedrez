





#define debugMode 1

#if debugMode
#define debug(...) __VA_ARGS__
#else
#define debug(...)
#endif
//decidi tener una version debug y una version no debug, esto me permite hacer mas cosas. Antes para no afectar la velocidad necesitaba meter las cosas debug de forma opcional en lugares donde no molestacen, especificamente en listas de polimorfismo de movs y movholders, lo que me limitaba lo que podia hacer y era incomodo.
//la cagada de tener una version normal y una version debug es que en este proyecto quiero que el usuario tenga acceso a las cosas debug, por lo que tendria que tener 2 exes del juego. Por lo menos por esta version. Cuando haga la version compilada, como se compila en el momento, puedo tener la opcion de compilar en distintas formas y listo.
//Otra opcion es dejar la version compilada como la version rapida sin nada, y dejar la interpretada con todas las cosas debug


#define noSizeAttribute(name,type)\
type* name(){\
  return (type*)((char*)this+sizeof(this));\
}
//hay un atributo en el ultimo c++ que hace esto tambien, es un poco mejor
//porque no es un metodo, no hay que poner ()


#define fromCast(new,old,type) type new=(type) old;


template<typename T>
struct deferObj{
  T lambda;
  deferObj(T l){lambda=l}
  ~deferObj(){lambda()}
};
#define defer(op) deferObj([&](){op});




#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <iostream>
#include <assert.h>
#include <string>
#include <sstream>
#include <random>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <string.h>
#include <stdint.h>
#include "alloca.h"
#include <cstring>
#include <stdlib.h>
#include <time.h>
#include <ctime>

using namespace std;
#include <SFML/Graphics.hpp>
using namespace sf;

#include "Manager.h"

#include "vec.h"

#include "Bucket.h"
#include "bucket.cpp"

#include "Input.h"
#include "Input.cpp"


#include "main.cpp"

#include "arranque.cpp"
#include "selector.cpp"

#include "tablero.h"
#include "memGetters.h"
#include "movs.h"
#include "operador.h"
#include "Pieza.h"
#include "movHolders.h"
#include "Jugador.h"
#include "Estado.h"

#include "tablero.cpp"

#include "Clicker.h"
#include "Clicker.cpp"

#include "lector.h"
#include "lector.cpp"

#include "Jugador.cpp"
#include "Pieza.cpp"


#include "movHolders.cpp"
#include "memGetters.cpp"
#include "movs.cpp"
#include "memMov.cpp"
#include "operador.cpp"


#include "Estado.cpp"

//en el momento me parecio buena idea delegar toda la construccion a este archivo,
//se podría haber dejado los headers en cada cpp, como es todo una unica unidad de compilacion
//no sería detectablemente mas lento y no cambiaria la memoria, pero como ahora las cosas se 
//compilan asi es redundate y los includes van a ir quedando desactualizados
