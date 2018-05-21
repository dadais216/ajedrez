#ifndef JUGADOR_H
#define JUGADOR_H

#include "global.h"

struct tablero;

struct Jugador
{
    int bando;
    tabl& _tablero;
    Jugador(int bando_,tabl& tablero_)
        :bando(bando_),_tablero(tablero_) {}
    virtual bool turno()=0;
};

struct Nadie:public Jugador
{
    Nadie(int bando_,tabl& tablero_):Jugador(bando_,tablero_){}; //al pedo pero como uso referencias tiene que estar
    virtual bool turno()
    {
        return true;
    }
};

struct Humano:public Jugador
{
    Humano(int,tabl&);
    virtual bool turno();
    list<Clicker*> clickers;
};

struct Aleatorio:public Jugador
{
    Aleatorio(int,tabl&);
    virtual bool turno();
};

#endif // JUGADOR_H
