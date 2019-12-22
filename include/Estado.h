#ifndef ESTADO_H
#define ESTADO_H

struct Proper:public Estado{
    Proper(int,int,int);
    int id;
    void init();
    virtual void update();
    virtual void draw();
    bool inRange(v a); //uso esto?
    tabl tablero;
    int dt,clickI;
    Jugador* primero;
    Jugador* segundo;
    Sprite turnoBlanco,turnoNegro;
};

//turnoAct se aumenta en cada accionar, es el contador de turnos
//turno es turnoAct/2, el contador de turnos interpretando un turno como una jugada de los dos jugadores
int turno,turnoAct;
bool turno1;


bool debugMode;

#endif // ESTADO_H
