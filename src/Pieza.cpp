#include "Pieza.h"
#include <global.h>
#include <operador.h>
#include <Clicker.h>

Pieza::Pieza(int _id,int _bando,int _sn){
    id=_id;
    sn=_sn;
    bando=_bando;
    sprite.setTexture(imagen->get("sprites.png"));
    sprite.setTextureRect(IntRect(64+sn*64+(16+16*bando),0,32,32));
    sprite.setScale(escala,escala);



    while(!tokens.empty()){
        movs.push_back(new normal);
    }
    for(operador* o:movs){
        do{
            o->debug();
        }while(o=o->sig);
    }
}

void Pieza::draw(v vec){
    sprite.setPosition(vec.x*escala*32,vec.y*escala*32);
    window->draw(sprite);
}

list<Clicker*> clickers;

void Pieza::calcularMovimientos(v posAct){
    org=posAct;
    for(operador* op:movs){
        pos=org;
        operador* it=op;//por ahi no es necesario

        while(it->operar()&&(it=it->sig));

        clickers.push_back(new Clicker);
    }
}





