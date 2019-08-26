









#define noSizeAttribute(name,type)\
type* name(){\
  return (type*)((char*)this+sizeof(this));\
}
//hay un atributo en el ultimo c++ que hace esto tambien, es un poco mejor
//porque no es un metodo, no hay que poner ()










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
#include "Bucket.cpp"

#include "Input.h"
#include "Input.cpp"

#include "Juego.h"

#include "Boton.h"
#include "Boton.cpp"

#include "tablero.h"
#include "memGetters.h"
#include "movs.h"
#include "operador.h"
#include "Pieza.h"
#include "movHolders.h"
#include "Jugador.h"
#include "Estado.h"

#include "tablero.cpp"
#include "Juego.cpp"
#include "main.cpp"

#include "Clicker.h"
#include "Clicker.cpp"

#include "lector.h"
#include "lector.cpp"

#include "Jugador.cpp"
#include "Pieza.cpp"


#include "movHolders.cpp"
#include "memGetters.cpp"
#include "movs.cpp"
#include "operador.cpp"


#include "Estado.cpp"

//en el momento me parecio buena idea delegar toda la construccion a este archivo,
//se podría haber dejado los headers en cada cpp, como es todo una unica unidad de compilacion
//no sería detectablemente mas lento y no cambiaria la memoria, pero como ahora las cosas se 
//compilan asi es redundate y los includes van a ir quedando desactualizados
