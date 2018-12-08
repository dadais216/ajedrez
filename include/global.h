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
struct colort;


extern Juego* j;
extern Input* input;
extern RenderWindow* window;
extern Manager<Texture>* imagen;

extern float escala;
extern tabl* tablptr;
extern vector<Pieza*> piezas;
<<<<<<< HEAD
extern vector<int> uniqueIds;
//extern Holder* act;
//extern v pos;
//extern v org;
=======
>>>>>>> f4ac213... movimientos en templates, memoria 2

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
extern Text textDebug;
extern Text asterisco;
extern bool drawAsterisco;
extern RectangleShape* tileActDebug;
extern RectangleShape posPieza;
extern RectangleShape posActGood;
extern RectangleShape posActBad;

extern unordered_set<Base*> basesAActualizar;
extern bool memcambios;
extern array<int,20> numeros;
extern vector<Holder*> capturados;

void resetearValores();
void drawScreen();

extern bool clickExplicit;
extern bool switchToGen;
extern bool confirm;
extern v offset;
extern int isolCount;

extern vector<int> memMov;

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT};

#endif
