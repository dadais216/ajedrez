





#define debugMode 1

//decidi tener una version debug y una version no debug, esto me permite hacer mas cosas. Antes para no afectar la velocidad necesitaba meter las cosas debug de forma opcional en lugares donde no molestacen, especificamente en listas de polimorfismo de movs y movholders, lo que me limitaba lo que podia hacer y era incomodo.
//la cagada de tener una version normal y una version debug es que en este proyecto quiero que el usuario tenga acceso a las cosas debug, por lo que tendria que tener 2 exes del juego. Por lo menos por esta version. Cuando haga la version compilada, como se compila en el momento, puedo tener la opcion de compilar en distintas formas y listo.
//Otra opcion es dejar la version compilada como la version rapida sin nada, y dejar la interpretada con todas las cosas debug

/*
  #if debugMode
  #define debug(...) __VA_ARGS__
  #else
  #define debug(...)
  #endif
  antes tenia esto sin darme cuenta de que no andaría porque se come las comas xd
*/
/*
void debugFunc(auto lambda){
#if debugMode
  lambda();
#endif
}
esto es una idea tambien, pero la notacion de lambda es media fea
*/

#define noSizeAttribute(name,type)\
type* name(){\
  return (type*)((char*)this+sizeof(this));\
}
//hay un atributo en el ultimo c++ que hace esto tambien, es un poco mejor
//porque no es un metodo, no hay que poner ()


#define fromCast(new,old,type) type new=(type) old;

//TODO guardar una lambda, asi puedo hacer cualquier cosa
/*template<typename T,void(*func)(T)>
struct deferObj{
  T obj;
  deferObj(T obj_){obj=obj_;}
  ~deferObj(){func(obj);}
};
#define defer(func,obj) deferObj<decltype(obj),func>(obj);*/

void free(char* c){
  delete c;
}
template<typename T>
struct deferObj{
  T obj;
  deferObj(T obj_){obj=obj_;}
  ~deferObj(){free(obj);}
};
#define defer(obj) deferObj<decltype(obj)>  defer(obj);//Medio choto, solo borra cosas. Cuando tenga internet voy a mirar lo de la lambda
#define defer2(obj) deferObj<decltype(obj)> defer2(obj);
#define defer3(obj) deferObj<decltype(obj)> defer3(obj);

#include <utility>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

template<typename... Ts>
void fail(char const* err,Ts... args){
  printf(err,args...);//me parece un poco mas lindo que va_list
  printf("\n\n");
  exit(0);
}

template<typename... Ts>
void failIf(bool cond,char const* err,Ts... args){
  if(cond){
    fail(err,args...);
  }
}

template<typename... Ts>
void assertf(bool cond,char const* err,Ts... args){
#if debugMode
  if(!cond){
    printf(err,args...);
    assert(cond);
  }
#endif
}

#define defaultAssert break; default: assert(false&&"bad switching\n");

//creo que es mas inteligente cargar todo el archivo y listo
//al principio quería manejarme con funciones de archivo porque tenia la idea de
//que iban a ser mas rapidas, pero supongo que cargar todo el archivo de una y
//hacer el procesado yo va a ser mejor, y mas comodo
char* loadFile(char const* fileName){
  FILE* file=fopen(fileName,"r");
  failIf(!file,"%s bad file",fileName);
  fseek(file,0,2);
  int size=ftell(file);
  char* content=new char[size+1];
  rewind(file);
  int read=fread(content,1,size,file);
  failIf(read!=size,"bad read of file %s",fileName);
  //TODO mirar ferror y limpiarlo
  fclose(file);
  content[size]=0;
  return content;
}




#include <list>
#include <map>
#include <iostream>
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
#include <time.h>
#include <ctime>

#include <SFML/Graphics.hpp>
using namespace sf;

struct normalHolder;
struct Holder;
struct movHolder;
struct Tile;
struct board;
struct properState;
struct Clicker;

typedef intptr_t intptr;


#include "Manager.h" //TODO mirar

#include "vec.h"

#include "Bucket.h"


#include "movHolders.h"
#include "Pieza.h"
#include "movs.h"
#include "memGetters.h"
#include "lector.h"
#include "operator.h"

#include "Input.h"
#include "proper.h"
#include "selector.h"


#include "main.cpp"
#include "Input.cpp"
#include "arranque.cpp"
#include "selector.cpp"

#include "tablero.h"
#include "operador.h"

#include "movs.cpp"
#include "tablero.cpp"

#include "Clicker.h"
#include "Clicker.cpp"

#include "lector.cpp"
#include "operator.cpp"

#include "Pieza.cpp"


#include "movHolders.cpp"
#include "memGetters.cpp"
#include "memMov.cpp"
#include "operador.cpp"


#include "proper.cpp"

//en el momento me parecio buena idea delegar toda la construccion a este archivo,
//se podría haber dejado los headers en cada cpp, como es todo una unica unidad de compilacion
//no sería detectablemente mas lento y no cambiaria la memoria, pero como ahora las cosas se 
//compilan asi es redundate y los includes van a ir quedando desactualizados
