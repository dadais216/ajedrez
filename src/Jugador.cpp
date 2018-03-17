#include "Jugador.h"
#include "global.h"
#include "Juego.h"
#include "Clicker.h"
#include "tablero.h"
#include "Estado.h"
#include "Pieza.h"
#include <stdlib.h>
#include <time.h>

Humano::Humano(int bando_,tablero& tablero_)
:Jugador(bando_,tablero_){}

int dt=0;
int clickI=0;
bool confirm;
bool Humano::turno(){
    dt++;//se podría mover adentro del if?
    if(!clickers.empty()){
        if(dt>20){
            dt=0;
            clickI++;
            for(Clicker* cli:clickers)
                cli->activacion(clickI);
        }
        drawScreen();
        confirm=false;
        if(input->click()){
            bool any=false;
            for(Clicker* cli:clickers)
                if(cli->update()){
                    any=true;
                    break;
                }
            if(confirm){
                drawScreen();
                return false;
            }
            clickers.clear();
            if(!any)
                drawScreen();
            return any;
        }
    }
    if(input->click()){
        if(input->inGameRange(_tablero.tam)){
            act=_tablero(input->get().show());
            if(act&&act->bando==bando)
                act->pieza->calcularMovimientos(input->get());
        }
    }
    return false;
}

Aleatorio::Aleatorio(int bando_,tablero& tablero_)
:Jugador(bando_,tablero_){
    srand(time(NULL));
}

bool Aleatorio::turno(){
    for(int i=0;i<_tablero.tam.x;i++)
        for(int j=0;j<_tablero.tam.y;j++){
            act=_tablero(v(i,j));
            if(act&&act->bando==bando)
                act->pieza->calcularMovimientos(v(i,j));
        }
    if(clickers.size()>0){
        auto it=clickers.begin();
        advance(it,rand()%clickers.size());
        (*it)->accionar();
        clickers.clear();
    }
    return true;
}
