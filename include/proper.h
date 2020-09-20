
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

struct properState{
  int player1,player2;
  Sprite turnoBlanco,turnoNegro;
  bucket pieceOps;
  vector<Piece*> pieces;
  vector<Clicker> clickers;
  vector<int> memMov;//se podría mover a gameState, consiguendo su max TODO
  bucket   gameState;//por ahora
  //char* board; //tablero+memorias globales
  //char* holderSpace; //vector que voy a implementar a mano, porque necesito correr codigo para determinar si crecer o no, y cuanto

  int boardId;
  parseData pd;

  int hsSize;

  int turno;
};


/*
la idea que tenia al principio es tener todo en memoria contigua, porque hay mas chances de estar en cache y copiar la memoria es rapido y simple, lo que va a ser importante cuando genere turnos futuros.
Los triggers no se pueden poner ahi porque crecen de forma aleatoria, asi que los meti en un vector aparte. Dentro de todo no debería ser mucho problema,es otro acceso nomas.
Creo que voy a hacer lo mismo con los holders y su estructura de movimientos, porque puede crecer aleatoriamente con spawns. Igual me gustaria medir para ver si hay una diferencia relevante antes.
Mantener la memoria contigua y usar buckets para holders que aparezcan despues es la otra opcion, pero no me cierra porque cuando deje de usar punteros y pase a relativos estos van a tener que tener 2 partes, un indicador de bucket y la posicion en ese bucket. Ahi voy a meter branching y va a ser medio feo. Igual me gustaría probarlo y medirlo.
Otro tema es que tamaño van a tener los buckets de overflow. Podría estimarse con algo como la suma de los tamaños de las piezas spawneables o algo, por ahora
tienen el mismo tamaño que el bucket inicial lo que es bastante arbitrario

Al final igual creo que me quedo con el vector. Estaría bueno ver que piezas son las de spawn y crear espacios para ellas preventivamente, para evitar la copia en caso de que se necesite una que no este.
El mecanismo de reutilizar espacio de piezas capturadas por piezas del mismo tipo ya esta.
Se podría agregar un mecanismo para manejar el espacio de piezas capturadas que no pueden volver a aparecer, que sea ocupado por piezas nuevas que tengan menos tamaño, o comprimir el vector.

en cada turno futuro se copia el vector. Si durante el procesado del vector hay un spawn y no hay espacio se copia de vuelta. 

El tablero, la memoria global y tile quedaría en un malloc aparte
*/

void properDraw(char*);
template<bool =false> void properInit(char*,int,int,int);
template<bool =false> void properGameInit(properState*);
void properUpdate(char*);


void randomTurnTestPlayer(bool,properState*);

#endif // ESTADO_H
