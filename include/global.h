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

using namespace std;
using namespace sf;

typedef unsigned int uint;

struct Juego;
struct Input;
template<typename T>
struct Manager;
struct Holder;
struct tabl;
struct Clicker;
struct Pieza;

extern Juego* j;
extern Input* input;
extern RenderWindow* window;
extern Manager<Texture>* imagen;

extern float escala;
extern tabl* tablptr;
extern vector<Pieza*> piezas;
extern vector<int> uniqueIds;
//extern Holder* act;
//extern v pos;
//extern v org;

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

extern bool memcambios;
extern array<int,20> numeros;
extern vector<Holder*> capturados;

void resetearValores();
void drawScreen();

extern bool confirm;

#endif
