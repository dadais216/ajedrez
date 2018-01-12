
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
#include <Manager.h>
#include <assert.h>
#include <vec.h>
#include <string>
#include <windows.h>

using namespace std;
using namespace sf;

struct Juego;
struct Input;
template<typename T>
struct Manager;
struct Pieza;
struct tablero;
struct Clicker;

extern Juego* j;
extern Input* input;
extern RenderWindow* window;
extern Manager<Texture>* imagen;

extern float escala;
extern tablero* tabl;
extern Pieza* act;
extern v pos;
extern v org;

struct acm;
struct color;
extern list<int> tokens;
extern list<acm*> buffer;
extern list<pair<RectangleShape*,v>> bufferColores;
extern list<Clicker*> clickers;
extern bool cambios;

extern bool memcambios;
extern array<int,20> numeros;

extern bool drawJustSquares;

void resetearValores();
void drawScreen();

#endif
