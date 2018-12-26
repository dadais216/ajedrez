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
Proper::Proper(int id_,int sel1,int sel2)
    :tablero(){
    id=id_;
    j->change(this);
    debugMode=true;


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


    turnoBlanco.setTexture(imagen->get("sprites.png"));
    turnoNegro.setTexture(imagen->get("sprites.png"));
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

    backGroundMemLocal.setFillColor(sf::Color(240,235,200));
    backGroundMemLocal.setOutlineColor(sf::Color(195,195,175));
    backGroundMemLocal.setOutlineThickness(4);
    backGroundMemLocal.setSize(Vector2f(20,40));
    backGroundMemLocalDebug.setFillColor(sf::Color(163,230,128,150));
    backGroundMemLocalDebug.setOutlineColor(sf::Color(195,195,175));
    backGroundMemLocalDebug.setOutlineThickness(4);
    backGroundMemLocalDebug.setSize(Vector2f(20,40));
    textValMemLocal.setColor(Color::Black);
    textValMemLocal.setFont(j->font);

    init();
}

void Proper::init(){
    clickers.clear();
    memMov.resize(0);
    maxMemMovSize=0;

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

    turno1=true;
    antTurno=false;

    for(uint i=0; i<lect.matriz.size(); i++){
        for(uint j=0; j<lect.matriz[0].size(); j++){
            Holder* act=tablero.tile(v(j,i))->holder;
            if(act) act->generar();
        }
    }
    cout<<endl;
    for(int i=0; i<tablptr->tam.y; i++){
        for(int j=0; j<tablptr->tam.x; j++){
            cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
        }
        cout<<endl;
    }
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
    if(drawDebugTiles){ ///@optim funcion aparte
        window->draw(posPieza);
        window->draw(*tileActDebug);
        if(drawAsterisco){
            window->draw(asterisco);
            drawAsterisco=false;
        }
    }
    if(drawDebugTiles||drawMemDebug){
        window->draw(textDebug);
        int memSize=triggerInfo.nh->base.movSize;
        for(int i=0;i<memSize;i++){
            backGroundMemLocal.setPosition(Vector2f(530+25*(i%4),405+45*(i/4-memSize/4)));
            window->draw(backGroundMemLocal);
        }
        if(getterMemLocalDebug1){
            getterMemLocalDebug1->drawDebugMem();
            getterMemLocalDebug2->drawDebugMem();
        }
        for(int i=0;i<memSize;i++){
            textValMemLocal.setPosition(530+25*(i%4),410+45*(i/4-memSize/4));
            textValMemLocal.setString(to_string(memMov[i]));
            window->draw(textValMemLocal);
        }
    }
}

void Proper::update(){
    if(debugMode&&window->hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
        init();//leaks obviamente
        while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
    }

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









