#ifndef JUGADOR_H
#define JUGADOR_H

#include "global.h"


struct Jugador{
    bool bando;
    tabl& _tablero;
    Holder* act;
    Jugador(bool bando_,tabl& tablero_)
        :bando(bando_),_tablero(tablero_) {}
    virtual void turno()=0;
};

struct Nadie:public Jugador{
    Nadie(bool bando_,tabl& tablero_):Jugador(bando_,tablero_){}; //al pedo pero como uso referencias tiene que estar
    virtual void turno();
};

struct Humano:public Jugador{
    Humano(bool,tabl&);
    virtual void turno();
};

struct Aleatorio:public Jugador{
    Aleatorio(bool,tabl&);
    virtual void turno();
};

#endif // JUGADOR_H
