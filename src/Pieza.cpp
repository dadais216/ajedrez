#include "Pieza.h"
#include <global.h>
#include <operador.h>

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
        o->debug();
    }
}

void Pieza::draw(v vec){
    sprite.setPosition(vec.x*escala*32,vec.y*escala*32);
    window->draw(sprite);
}





