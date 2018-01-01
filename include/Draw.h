#ifndef DRAW_H
#define DRAW_H
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;
class Draw
{
public:
    Sprite spt;
    RenderWindow* window;
    Draw(RenderWindow*);//este constructor se llama desde el constructor de objeto
    void setText(Texture&);
    void setRect(int,int,int,int);
    void pos(int,int);
    void draw();
    virtual ~Draw();
};
class Anim:public Draw{
public:
    //no usa un vector, usa solo un sprite, va moviendo el setTextureRect nomas
    //si tiene que usar sprites de distintas plantillas uso distintos Draws
    int w,h,px,py,ipx,ipy,fpx,fpy,spx,spy;
    bool end,beg;
    Anim(RenderWindow* w,int he, int we, int ixe, int iye, int fxe, int fye,float spx,float spy);
    Anim(RenderWindow*);
    void setdata(int he, int we, int ixe, int iye, int fxe, int fye,float spx,float spy);
    //h y w dan la medida de la plantilla, en unidades de 16x16 pixeles
    //fxe esta para no forzar que todas las estructuras tengan forma cuadrada
    void sig();//mueve la animacion al siguiente cuadrante
    void ant();
    void coord(int a,int b);//pone la animacion en un cuadrante especifico
    void start(); //al cuadrante inicial
    void toend();
    bool ended();
    bool atstart();
    void updateimage();
};


#endif // DRAW_H
