#include "Estado.h"
#include <SFML/Graphics.hpp>

#include <global.h>
#include <Boton.h>
#include <tablero.h>
#include <Clicker.h>
#include <Pieza.h>
#include <lector.h>
#include <operador.h>
#include <Jugador.h>

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

lector lect;
Proper::Proper(int id)
:tablero(){
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
                tablero(v(j,i),lect.crearPieza(n));
        }
    }
    //construir piezas adicionales


    primero=new Humano(-1);
    segundo=new Humano(1);
    turno1=true;
}

void Proper::draw(){
    tablero.drawTiles();
    if(Clicker::drawClickers){
        for(Clicker* cli:clickers){
            cli->draw();
        }
    }
    tablero.drawPieces();
}

void Proper::update(){
    if(turno1)
        turno1=!primero->turno(tablero);
    else
        turno1=segundo->turno(tablero);
}

bool Proper::inRange(v a){
    return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}









