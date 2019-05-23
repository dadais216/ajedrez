#ifndef GLOBAL_H
#define GLOBAL_H

#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Manager.h"
#include <assert.h>
#include "vec.h"
#include <string>
#include <sstream>
#include <random>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <string.h>
#include <stdint.h>
#include "alloca.h"

#include "Bucket.h"


using namespace std;
using namespace sf;

typedef unsigned int uint;

struct Juego;
struct Input;
template<typename> struct Manager;
struct Jugador;
struct Boton;
struct tablero;
struct tabl; ///?
struct Clicker;
struct Trigger;
struct operador;
struct Pieza;
struct normal;
struct desliz;
struct exc;
struct isol;
struct desopt;
struct Tile;
struct Holder;
struct movHolder;
struct normalHolder;
struct Base;
struct acct;
struct condt;
struct mcondt;
struct macct;
struct colort;
struct getter;
struct getterCond;



extern Juego* j;
extern Input* input;
extern RenderWindow* window;
extern Manager<Texture>* imagen;

extern float escala;
extern float fpsLock;
extern tabl* tablptr;
extern vector<Pieza*> piezas;

#include "lector.h"
extern lector lect;

struct drawable
{
    drawable(int t,void* o)
    {
        tipo=t;
        obj=o;
    }
    int tipo;
    void* obj;
};

extern list<int> tokens;///@optim vector
extern int turno,turnoAct;
extern bool turno1;


extern bool debugMode;
extern bool drawDebugTiles;
extern bool drawMemDebug;
extern Text textDebug;
extern Text asterisco;
extern bool drawAsterisco;
extern RectangleShape* tileActDebug;
extern RectangleShape posPieza;
extern RectangleShape posActGood;
extern RectangleShape posActBad;
extern RectangleShape backGroundMem;
extern RectangleShape backGroundMemDebug;
extern Text textValMem;
extern getterCond* getterMemDebug1;
extern getterCond* getterMemDebug2;
extern v posDebugTile;


void drawScreen();


extern bool clickExplicit;
extern bool switchToGen;
extern bool confirm;
extern v offset;
extern vector<Holder*> justSpawned;
extern Tile* actualTile;
extern v actualPosColor;

extern int memGlobalSize;
extern vector<int> memGlobal;
extern vector<int> memMov;
extern int maxMemMovSize;
extern int memTileSize;
extern int memOtherSize;

extern int movSize;

extern Bucket* bucketPiezas,*bucketHolders;

struct AH{
    Holder* h;
    normalHolder* nh;
};
extern AH actualHolder;

struct memTriggers{
    vector<normalHolder*> perma,dinam;
};

extern memTriggers* trigsMaybeActivate;
extern vector<normalHolder*> trigsActivados;
extern Trigger triggerInfo;
extern vector<memTriggers> memGlobalTriggers;

struct turnTrigInfo{
    Holder* h;
    normalHolder* nh;
};
extern vector<turnTrigInfo> turnoTrigs[2];

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT};

extern const int32_t valorCadena;//la cadena de movholders es valida. Una cadena va desde la base hasta un clicker o el final
extern const int32_t valorFinal;//se llegó al final. Esto sirve para saber si seguir iterando en un desliz
extern const int32_t valor;//lo usa normal para saber si sus condiciones son verdaderas,y exc si tiene una rama valida
extern const int32_t lastNotFalse;//lo usa desliz
extern const int32_t makeClick;
extern const int32_t hasClick;
extern const int32_t doEsp;//se usa en normalh


#endif
