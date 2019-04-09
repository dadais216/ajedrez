#ifndef CLICKER_H
#define CLICKER_H
#include "vec.h"
#include <SFML/Graphics.hpp>

using namespace sf;

#include "global.h"

struct Clicker{
    Clicker(vector<normalHolder*>*,Holder*);
    vector<normalHolder*> normales;
    v clickPos;//a aleatorio y ia no les interesa esto, deberia estar en otra cosa junto con las cosas graficas
    void draw();
    static bool drawClickers;
    void update();
    //bool activo;
    //void activacion(int);
    //int val,mod;
    void accionar();
    Holder* h;
};

extern vector<Clicker> clickers;

#endif // CLICKER_H
