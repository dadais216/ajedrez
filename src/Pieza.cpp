#include "Pieza.h"
#include <global.h>

Pieza::Pieza(){
    sprite.setTexture(imagen->get("sprites.png"));
//    sprite.setTextureRect(IntRect(64+p.sn*64+(16+16*bando),0,32,32));
    sprite.setScale(2,2);
}

void Pieza::draw(int x,int y){
    sprite.setPosition(x*64,y*64);
    window->draw(sprite);
}





