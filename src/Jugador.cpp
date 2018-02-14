#include "Jugador.h"
#include "global.h"
#include "Juego.h"
#include "Clicker.h"
#include "tablero.h"
#include "Estado.h"
#include "Pieza.h"

Humano::Humano(int bando_,tablero& tablero_)
:bando(bando_),_tablero(tablero_){}

int dt=0;
int clickI=0;
bool confirm;
bool Humano::turno(){
    dt++;
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
