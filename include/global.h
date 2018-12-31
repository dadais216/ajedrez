#ifndef GLOBAL_H
#define GLOBAL_H

#include <fstream>
#include <vector>
#include <list>
#include <forward_list>
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


extern Juego* j;
extern Input* input;
extern RenderWindow* window;
extern Manager<Texture>* imagen;

extern float escala;
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

extern list<int> tokens;
extern list<pair<drawable,v>> bufferColores;
extern list<v> limites;
extern vector<Clicker*> clickers;
extern bool bOutbounds;
extern bool cambios;
extern bool addTrigger;

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
extern getter* getterMemDebug1;
extern getter* getterMemDebug2;

extern int memGlobalSize;

void drawScreen();

extern bool clickExplicit;
extern bool switchToGen;
extern bool confirm;
extern v offset;
extern int isolCount;
extern vector<int> memGlobal;
extern vector<int> memMov;
extern int maxMemMovSize;
extern int memLocalSize;

struct AH{
    Holder* h;
    normalHolder* nh;
    Tile* tile;
    v offset;
};
extern AH actualHolder;

extern vector<normalHolder*> trigsActivados;
extern Trigger triggerInfo;
extern vector<vector<normalHolder*>> memGlobalPermaTriggers;

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT};

#endif
