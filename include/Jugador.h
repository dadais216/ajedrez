#ifndef JUGADOR_H
#define JUGADOR_H

struct tablero;

struct Jugador{
    Jugador(){};
    virtual bool turno(tablero&)=0;
};

struct Humano:public Jugador{
    int bando;
    Humano(int);
    virtual bool turno(tablero&);
};

#endif // JUGADOR_H
