#ifndef CLICKER_H
#define CLICKER_H
#include "vec.h"
#include <SFML/Graphics.hpp>

using namespace sf;

#include "global.h"

struct Clicker{
    Clicker(vector<normalHolder*>*,Holder*);
    vector<normalHolder*> normales;
    v clickPos;
    void draw();
    static bool drawClickers;
    bool update();
    //bool activo;
    //void activacion(int);
    //int val,mod;
    void accionar();
    void debug();
    Holder* h;
};

#endif // CLICKER_H
