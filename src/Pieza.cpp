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
        cout<<endl;
    }
}

void Pieza::draw(v vec){
    sprite.setPosition(vec.x*escala*32,vec.y*escala*32);
    window->draw(sprite);
}

list<Clicker*> clickers;

void Pieza::calcularMovimientos(v posAct){
    pos=org=posAct;
    for(operador* op:movs){
        operador* it=op;//por ahi no es necesario

        do{
            if(!it->operar()){
                buffer.clear();
                goto cont;
            }
        }while(it=it->sig);

        clickers.push_back(new Clicker(false));
        cont:
        pos=org;
    }
}





