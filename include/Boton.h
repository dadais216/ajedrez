#ifndef BOTON_H
#define BOTON_H

struct Boton{
    Text text;
    Sprite sprite;
    int n,x,y,escala;
    Boton(string,int,int,int,int);
    int clicked();
    void draw();
};

struct SelJugador{
    int bando;
    list<Boton> botones;
    RectangleShape cuadrado;
    SelJugador(int);
    int selected;
    void clicked();
    void draw();
};

#endif // BOTON_H
