#include "../include/Jugador.h"
#include "../include/global.h"
#include "../include/Juego.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Estado.h"
#include "../include/Pieza.h"
#include <stdlib.h>
#include <time.h>

Humano::Humano(int bando_,tabl& tablero_)
    :Jugador(bando_,tablero_) {}

int dt=0;
int clickI=0;
bool confirm;
bool Humano::turno(){
    /*
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
    */

    if(input->click()&&input->inGameRange(_tablero.tam)){
        for(Clicker* cli:clickers){
        ///esto se pregunta 60hz
        ///Lo mejor seria hacer que se bloquee hasta recibir otro click, hacerlo bien cuando
        ///vuelva a meter solapamiento
            if(cli->update()){
                clickers.clear();
                return false; //true
            }
        }
        clickers.clear();

        cout<<"("<<input->get()<<")"<<endl;

        Holder* act=_tablero(input->get());
        if(act&&act->bando==bando){
            act->makeCli(clickers); ///armar los clickers
            for(Clicker* cli:clickers){
                cli->debug();
                cli->draw();
                cout<<"---";
            }
        }
    }
    return false;
}

Aleatorio::Aleatorio(int bando_,tabl& tablero_)
    :Jugador(bando_,tablero_){
    srand(time(NULL));
}

bool Aleatorio::turno(){
    for(int i=0; i<_tablero.tam.x; i++)
        for(int j=0; j<_tablero.tam.y; j++)
        {
            Holder* act=_tablero(v(i,j));
            if(act&&act->bando==bando)
                act->pieza->calcularMovimientos(v(i,j));
        }
    if(clickers.size()>0)
    {
        auto it=clickers.begin();
        advance(it,rand()%clickers.size());
        (*it)->accionar();
        clickers.clear();
    }
    return true;
}
