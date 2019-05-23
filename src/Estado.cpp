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
#include "movHolders.h"
#include "memGetters.h"

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
Selector::Selector():sel1(1),sel2(-1){
    j->change(this);
    fstream tableros;
    tableros.open("tableros.txt");
    string linea;
    int j=0;
    while(getline(tableros,linea)){
        if(!linea.empty()&&linea[0]=='"'){
            int i=1;
            for(; linea[i]!='"'; i++);
            botones.push_back(new Boton(linea.substr(1,i-1),j,32+(70*j/420)*140,40+(j*70)%420,2));
            j++;
        }
    }
    drawScreen();
}
bool debugMode;
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
            if((n=b->clicked())){
                new Proper(n-1,sel1.selected,sel2.selected);
                return;
            }
        }
        sel1.clicked();
        sel2.clicked();
    }
}
Selector::~Selector(){
    for(auto* b:botones)
        delete b;
}

lector lect;
Bucket* bucketPiezas,*bucketHolders;
Proper::Proper(int id_,int sel1,int sel2)
    :tablero(){
    id=id_;
    j->change(this);
    debugMode=false;

    int nonHuman=0;
    auto selec=[&](int sel,bool bando)->Jugador*{
        switch(sel){
        case 0:nonHuman++;return new Nadie(bando,tablero);
        case 1:return new Humano(bando,tablero);
        case 2:nonHuman++;return new Aleatorio(bando,tablero);
            //case 3: return new IA(bando,tablero);
        }
    };

    primero=selec(1,false);
    segundo=selec(1,true);

    if(nonHuman==2)
        fpsLock=0.;

    turnoBlanco.setTexture(imagen->get("tiles.png"));
    turnoNegro.setTexture(imagen->get("tiles.png"));
    turnoBlanco.setTextureRect(IntRect(0,0,32,32));
    turnoNegro.setTextureRect(IntRect(32,0,32,32));
    turnoBlanco.setScale(12,16);
    turnoNegro.setScale(12,16);
    turnoBlanco.setPosition(510,0);
    turnoNegro.setPosition(510,0);

    ///tiles de debug
    posPieza.setFillColor(sf::Color(250,240,190,150));
    posActGood.setFillColor(sf::Color(180,230,100,100));
    posActBad.setFillColor(sf::Color(240,70,40,100));
    textDebug.setFont(j->font);
    textDebug.setPosition(520,465);
    asterisco.setFont(j->font);
    asterisco.setPosition(515,450);
    asterisco.setColor(Color::Black);
    asterisco.setString("*");

    backGroundMem.setFillColor(sf::Color(240,235,200));
    backGroundMem.setOutlineColor(sf::Color(195,195,175));
    backGroundMem.setOutlineThickness(4);
    backGroundMem.setSize(Vector2f(20,40));
    backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
    backGroundMemDebug.setOutlineColor(sf::Color(195,195,175));
    backGroundMemDebug.setOutlineThickness(4);
    backGroundMemDebug.setSize(Vector2f(20,40));
    textValMem.setColor(Color::Black);
    textValMem.setFont(j->font);

    lastBucket=&bucketPiezas;new Bucket();
    lastBucket=&bucketHolders;new Bucket();

    init();
}

//turnoAct se aumenta en cada accionar, es el contador de turnos
//turno es turnoAct/2, el contador de turnos interpretando un turno como una jugada de los dos jugadores
int turno,turnoAct;
bool turno1;

void Proper::init(){
    clickers.clear();
    memMov.clear();
    maxMemMovSize=0;
    memGlobal.clear();
    memGlobalTriggers.clear();
    memGlobalSize=0;
    memTileSize=0;
    turnoTrigs[0].clear();
    turnoTrigs[1].clear();
    turnoAct=2;
    turno=1;

    if(lect.archPiezas.is_open())
        lect.archPiezas.close();
    lect.archPiezas.open("piezas.txt");

    lect.generarIdsTablero(id);

    tablero.armar(v(lect.matriz[0].size(),lect.matriz.size()));

    lect.cargarDefs();

    //esto esta aca porque escala se setea en armar
    posPieza.setSize(Vector2f(32*escala,32*escala));
    posActGood.setSize(Vector2f(32*escala,32*escala));
    posActBad.setSize(Vector2f(32*escala,32*escala));



    piezas.clear();
    cout<<"-----"<<endl;
    for(uint i=0; i<lect.matriz.size(); i++){
        for(uint j=0; j<lect.matriz[0].size(); j++){
            int n=lect.matriz[i][j];
            v pos(j,i);
            //cout<<pos<<"  "<<tablero.tam<<endl;
            if(n)
                tablero.tile(pos)->holder=lect.crearPieza(n,pos);
            else
                tablero.tile(pos)->holder=nullptr;
        }
    }

    memset(memGlobal.data(),0,memGlobalSize*sizeof(int));

    for(uint i=0; i<lect.matriz.size(); i++){
        for(uint j=0; j<lect.matriz[0].size(); j++){
            Holder* hAct=tablero.tile(v(j,i))->holder;
            if(hAct){
                turno1=hAct->bando;///@check
                hAct->generar();
            }
        }
    }
    cout<<endl;
    for(int i=0; i<tablptr->tam.y; i++){
        for(int j=0; j<tablptr->tam.x; j++){
            cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
        }
        cout<<endl;
    }
    turno1=true;
    drawScreen();
}

void Proper::draw(){
    tablero.drawTiles();
    if(Clicker::drawClickers)
        for(Clicker& cli:clickers)
            cli.draw();
    tablero.drawPieces();
    if(turno1)
        window->draw(turnoBlanco);
    else
        window->draw(turnoNegro);

    textValMem.setPosition(570,10);
    textValMem.setString(to_string(turno));
    window->draw(textValMem);

    if(drawDebugTiles){ ///@optim funcion aparte
        window->draw(*tileActDebug);
        if(drawAsterisco){
            window->draw(asterisco);
            drawAsterisco=false;
        }
    }
    if(drawDebugTiles||drawMemDebug){
        window->draw(posPieza);
        window->draw(textDebug);
        int memSize=actualHolder.nh->base->memLocalSize;
        for(int i=0;i<memSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),405+45*(i/4-memSize/4)));
            window->draw(backGroundMem);
        }
        for(int i=0;i<memGlobalSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),305+45*(i/4-memGlobalSize/4)));
            window->draw(backGroundMem);
        }
        int memPiezaSize=actualHolder.h->memPieza.count();
        for(int i=0;i<memPiezaSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),205+45*(i/4-memPiezaSize/4)));
            window->draw(backGroundMem);
        }
        for(int i=0;i<memTileSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),105+45*(i/4-memTileSize/4)));
            window->draw(backGroundMem);
        }
        if(getterMemDebug1){
            getterMemDebug1->drawDebugMem();
            getterMemDebug2->drawDebugMem();
        }
        for(int i=0;i<memSize;i++){
            textValMem.setPosition(530+25*(i%4),410+45*(i/4-memSize/4));
            textValMem.setString(to_string(memMov[i]));
            window->draw(textValMem);
        }
        for(int i=0;i<memGlobalSize;i++){
            textValMem.setPosition(530+25*(i%4),310+45*(i/4-memGlobalSize/4));
            textValMem.setString(to_string(memGlobal[i]));
            window->draw(textValMem);
        }
        for(int i=0;i<memPiezaSize;i++){
            textValMem.setPosition(530+25*(i%4),210+45*(i/4-memPiezaSize/4));
            textValMem.setString(to_string(*actualHolder.h->memPieza[i]));
            window->draw(textValMem);
        }
        for(int i=0;i<memTileSize;i++){
            textValMem.setPosition(530+25*(i%4),110+45*(i/4-memTileSize/4));
            textValMem.setString(to_string(tablptr->tile(posDebugTile)->memTile[i]));
            window->draw(textValMem);
        }
        if(memOtherSize){
            for(int i=0;i<memOtherSize;i++){
                textValMem.setPosition(530+25*(i%4),160+45*(i/4-memOtherSize/4));
                textValMem.setString(to_string(*tablptr->tile(posDebugTile)->holder->memPieza[i]));
                window->draw(textValMem);
            }
            memOtherSize=0;
        }
    }
}

void Proper::update(){
    try{
        primero->turno();
        segundo->turno();
    }catch(...){}
}

bool Proper::inRange(v a)
{
    return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}









