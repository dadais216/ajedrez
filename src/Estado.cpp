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

Selector::Selector()
:sel1(1),sel2(-1){
    fstream tableros;
    tableros.open("tableros.txt");
    string linea;
    int j=0;
    while(getline(tableros,linea)){
        if(!linea.empty()&&linea[0]=='"'){
            int i=1;
            for(;linea[i]!='"';i++);
            botones.push_back(new Boton(linea.substr(1,i-1),j,32+(70*j/420)*140,40+(j*70)%420,2));
            j++;
        }
    }


}
void Selector::draw(){
    for(Boton* b:botones)
        b->draw();
    sel1.draw();
    sel2.draw();
}
void Selector::update(){
    if(input->click()){
        for(Boton* b:botones){
            int n;
            if(n=b->clicked()){
                j->change(new Proper(n-1,sel1.selected,sel2.selected));
                return;
            }
        }
        sel1.clicked();
        sel2.clicked();
    }
}

lector lect;
Proper::Proper(int id,int sel1,int sel2)
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

    auto selec=[&](int sel,int bando)->Jugador*{
        switch(sel){
        case 0: return new Nadie(bando,tablero);
        case 1: return new Humano(bando,tablero);
        case 2: return new Aleatorio(bando,tablero);
        //case 3: return new IA(bando,tablero);
        }
    };

    primero=selec(sel1,-1);
    segundo=selec(sel2,1);
    turno1=true;
    antTurno=false;

    turnoBlanco.setTexture(imagen->get("sprites.png"));
    turnoNegro.setTexture(imagen->get("sprites.png"));
    turnoBlanco.setTextureRect(IntRect(0,0,32,32));
    turnoNegro.setTextureRect(IntRect(32,0,32,32));
    turnoBlanco.setScale(12,16);
    turnoNegro.setScale(12,16);
    turnoBlanco.setPosition(510,0);
    turnoNegro.setPosition(510,0);

}

void Proper::draw(){
    tablero.drawTiles();
    if(Clicker::drawClickers)
        for(Clicker* cli:clickers)
            cli->draw();
    tablero.drawPieces();
    if(turno1)
        window->draw(turnoBlanco);
    else
        window->draw(turnoNegro);
}

void Proper::update(){
    if(antTurno!=turno1){
        drawScreen();
        antTurno=turno1;
    }
    if(turno1)
        turno1=!primero->turno();
    else
        turno1=segundo->turno();
}

bool Proper::inRange(v a){
    return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}









