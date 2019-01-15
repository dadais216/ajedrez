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
void Humano::turno(){
    /*
    dt++;//se podr�a mover adentro del if?
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
    while(true){
        sleep(milliseconds(20));
        input->check();
        if(debugMode&&window->hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
            static_cast<Proper*>(j->actual)->init();///@leaks
            while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
            throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
        }
        if(input->click()&&input->inGameRange(_tablero.tam)){
            for(Clicker* cli:clickers){
            ///@todo @optim esto se pregunta 60hz
            ///Lo mejor seria hacer que se bloquee hasta recibir otro click, hacerlo bien cuando
            ///vuelva a meter solapamiento
                if(cli->update()){ //accionar

                    clickers.clear();
                    turno1=!turno1;
                    drawScreen();
                    return;
                }
            }
            if(!clickers.empty()){
                clickers.clear();
                drawScreen();
            }
            cout<<"("<<input->get()<<")"<<endl;


            act=_tablero.tile(input->get())->holder;
            if(act&&act->bando==bando){
                act->makeCli();
                drawScreen();
            }
        }
    }
}

Aleatorio::Aleatorio(int bando_,tabl& tablero_)
:Jugador(bando_,tablero_){
    srand(time(NULL));
}

void Aleatorio::turno(){
    for(int i=0; i<_tablero.tam.x; i++)
        for(int j=0; j<_tablero.tam.y; j++){
            Holder* act=_tablero.tile(v(i,j))->holder;
            if(act&&act->bando==bando)
                ;//act->pieza->calcularMovimientos(v(i,j));
        }
    if(clickers.size()>0)
    {
        auto it=clickers.begin();
        advance(it,rand()%clickers.size());
        (*it)->accionar();
        clickers.clear();
    }
}
