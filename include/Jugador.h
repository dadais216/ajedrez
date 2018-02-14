#ifndef JUGADOR_H
#define JUGADOR_H

struct tablero;

struct Jugador{
    Jugador(){};
    virtual bool turno()=0;
};

struct Nadie:public Jugador{
    virtual bool turno(){return true;}
};

struct Humano:public Jugador{
    int bando;
    tablero& _tablero;
    Humano(int,tablero&);
    virtual bool turno();
};

#endif // JUGADOR_H
