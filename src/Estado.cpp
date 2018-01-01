#include "Estado.h"
#include <SFML/Graphics.hpp>

#include <global.h>
#include <Boton.h>
#include <tablero.h>
#include <Clicker.h>
#include <Pieza.h>
#include <lector.h>

Estado::Estado(){}

Arranque::Arranque(){
    portada.setTexture(imagen->get("portada.png"));
}
void Arranque::draw(){
    window->draw(portada);
}
void Arranque::update(){
    if(input->click()&&input->inRange())
        j->change(new Selector());
}

Selector::Selector(){
    Boton::i=0;
    Boton::tocado=false;

    fstream tableros;
    tableros.open("tableros.txt");
    string linea;
    while(getline(tableros,linea)){
        if(!linea.empty()&&linea[0]=='"'){
            int i=1;
            for(;linea[i]!='"';i++);
            botones.push_back(new Boton(linea.substr(1,i-1)));
        }
    }
}

void Selector::draw(){
    for(Boton* b:botones)
        b->draw();
}

void Selector::update(){
    if(input->click())
        for(Boton* b:botones)
            b->clicked();
    if(Boton::tocado)
        j->change(new Proper(Boton::i));
}

Proper::Proper(int id)
:tablero(){
    lector lect;
    lect.leer(id);
    lect.mostrar();

    tablero.armar(v(lect.matriz[0].size(),lect.matriz.size()));

    lect.cargarDefs();

//    for(auto i:lect.defs){
//        cout<<">>"<<i.first<<endl;
//        for(auto j:i.second){
//            cout<<j<<"-";
//        }
//        cout<<endl;
//    }

    for(int i=0;i<lect.matriz.size();i++){
        for(int j=0;j<lect.matriz[i].size();j++){
            int n=lect.matriz[i][j];
            if(n)
                tablero(v(i,j),lect.crearPieza(n));
        }
    }
    //construir piezas adicionales
}


void Proper::draw(){
    tablero.drawTiles();
    for(Clicker* cli:clickers){
        cli->draw();
    }

    return;
    tablero.drawPieces();
}

void Proper::update(){
    return;
    //turno jugador
//    if(input->click()){
//        if(input->isInRange(v(576,0),v(640,64))){
//            j->change(new Selector());
//            return;
//        }
//        if(input->inGameRange(tablero->tam)){
//            clickers.clear();
//            Pieza* act=tablero(v(input->get().x,input->get().y));
//            if(act){
//                if(act->bando==-1){
//                    for(int i=0;i<act->mov.size();i++){
//                        if(inRange(act->mov[i]+v(input->get().x,input->get().y))){
//                            clickers.push_back(Clicker(act->mov[i]+v(input->get().x,input->get().y),window));
//                        }
//                    }
//                }
//            }
//        }
//    }

    //turno makina
}

bool Proper::inRange(v a){
    return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}

//void Proper::mover(v a,v b){
//    tablero[b.x][b.y]=tablero[a.x][a.y];
//    tablero[a.x][a.y]=nullptr;
//    //se guarda el movimiento en el vector de movimientos
//}









