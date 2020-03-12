#ifndef ESTADO_H
#define ESTADO_H

/*
struct Proper{
    int id;
    tabl tablero;
    int dt,clickI;
    Jugador* primero;
    Jugador* segundo;
    Sprite turnoBlanco,turnoNegro;
};
*/

//turnoAct se aumenta en cada accionar, es el contador de turnos
//turno es turnoAct/2, el contador de turnos interpretando un turno como una jugada de los dos jugadores
int turno,turnoAct;
bool turno1;

#endif // ESTADO_H
