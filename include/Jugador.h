#ifndef JUGADOR_H
#define JUGADOR_H

struct tablero;

struct Jugador{
    int bando;
    tablero& _tablero;
    Jugador(int bando_,tablero& tablero_)
    :bando(bando_),_tablero(tablero_){}
    virtual bool turno()=0;
};

struct Nadie:public Jugador{
    Nadie(int bando_,tablero& tablero_):Jugador(bando_,tablero_){};//al pedo pero como uso referencias tiene que estar
    virtual bool turno(){return true;}
};

struct Humano:public Jugador{
    Humano(int,tablero&);
    virtual bool turno();
};

struct Aleatorio:public Jugador{
    Aleatorio(int,tablero&);
    virtual bool turno();
};

#endif // JUGADOR_H
