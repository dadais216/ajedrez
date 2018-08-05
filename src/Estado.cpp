#include "../include/Estado.h"
#include <SFML/Graphics.hpp>

#include "../include/Boton.h"
#include "../include/tablero.h"
#include "../include/Clicker.h"
#include "../include/Pieza.h"
#include "../include/lector.h"
#include "../include/operador.h"
#include "../include/Jugador.h"
#include "../include/Input.h"
#include "../include/Juego.h"

Estado::Estado() {}

Arranque::Arranque(){
    portada.setTexture(imagen->get("portada.png"));
}
void Arranque::draw(){
    window->draw(portada);
}
void Arranque::update(){
    if(input->click()&&input->inRange())
        new Selector();
}

Selector::Selector()
    :sel1(1),sel2(-1){
    j->change(this);
    fstream tableros;
    tableros.open("tableros.txt");
    string linea;
    int j=0;
    while(getline(tableros,linea))
    {
        if(!linea.empty()&&linea[0]=='"')
        {
            int i=1;
            for(; linea[i]!='"'; i++);
            botones.push_back(new Boton(linea.substr(1,i-1),j,32+(70*j/420)*140,40+(j*70)%420,2));
            j++;
        }
    }
    drawScreen();
}
bool debugMode;
void Selector::draw()
{
    for(Boton* b:botones)
        b->draw();
    sel1.draw();
    sel2.draw();
    debugMode=false;
}
void Selector::update()
{
    if(input->click())
    {
        for(Boton* b:botones)
        {
            int n;
            if((n=b->clicked()))
            {
                new Proper(n-1,sel1.selected,sel2.selected);
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
    j->change(this);
    debugMode=true;

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
    cout<<"-----"<<endl;
    for(uint i=0; i<lect.matriz.size(); i++){
        for(uint j=0; j<lect.matriz[0].size(); j++){
            int n=lect.matriz[i][j];
            v pos(j,i);
            cout<<pos<<"  "<<tablero.tam<<endl;
            if(n)
                tablero.tile(pos)->holder=lect.crearPieza(n,pos);
            else
                tablero.tile(pos)->holder=nullptr;
        }
    }
    //construir piezas adicionales

    auto selec=[&](int sel,int bando)->Jugador*
    {
        switch(sel)
        {
        case 0:
            return new Nadie(bando,tablero);
        case 1:
            return new Humano(bando,tablero);
        case 2:
            return new Aleatorio(bando,tablero);
            //case 3: return new IA(bando,tablero);
        }
        return nullptr;
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

    ///tiles de debug
    posPieza.setSize(Vector2f(32*escala,32*escala));
    posActGood.setSize(Vector2f(32*escala,32*escala));
    posActBad.setSize(Vector2f(32*escala,32*escala));
    posPieza.setFillColor(sf::Color(250,240,190,150));
    posActGood.setFillColor(sf::Color(180,230,100));
    posActBad.setFillColor(sf::Color(240,70,40,100));

    /*
    esto esta con la intencion de separar la rama de debug de la otra
    para que el modo debug no afecte la velocidad cuando se corre normalmente.
    Por ahora separo las ramas adentro de generar porque es mucho quilombo sino,
    pero eventualmente podría separarlas bien. A menos que no se necesite, ver que pasa
    #define generarInicial(debug) \
    for(uint i=0; i<lect.matriz.size(); i++){\
        for(uint j=0; j<lect.matriz[0].size(); j++){\
            Holder* act=tablero.tile(v(j,i))->holder;\
            if(act) act->generar##debug##();\
        }\
    }
    if(debugMode)
        generarInicial(Debug)
    else
        generarInicial()
    #undef generarInicial
    */
    for(uint i=0; i<lect.matriz.size(); i++){
        for(uint j=0; j<lect.matriz[0].size(); j++){
            Holder* act=tablero.tile(v(j,i))->holder;
            if(act) act->generar();
        }
    }


    /*
    for(int i=0; i<tablptr->tam.y; i++){
        for(int j=0; j<tablptr->tam.x; j++){
            cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
        }
        cout<<endl;
    }
    */
    drawScreen();
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
    if(drawDebugTiles){
        window->draw(posPieza);
        window->draw(*tileActDebug);
    }
}

void Proper::update()
{
    if(antTurno!=turno1)
    {
        //drawScreen();
        antTurno=turno1;
    }
    if(turno1)
        turno1=!primero->turno();
    else
        turno1=segundo->turno();
}

bool Proper::inRange(v a)
{
    return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}









