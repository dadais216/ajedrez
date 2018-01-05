
#ifndef GLOBAL_H
#define GLOBAL_H

#include <fstream>
#include <vector>
#include <list>
#include <forward_list>
#include <map>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Manager.h>
#include <assert.h>
#include <vec.h>
#include <string>

using namespace std;
using namespace sf;

struct Juego;
struct Input;
template<typename T>
struct Manager;


extern Juego* j;
extern Input* input;
extern RenderWindow* window;
extern Manager<Texture>* imagen;

extern float escala;

struct acm;
extern list<int> tokens;
extern list<acm*> buffer;


#endif
