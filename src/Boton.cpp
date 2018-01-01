#include "Boton.h"


#include <global.h>

int Boton::i;
bool Boton::tocado;
Boton::Boton(string nomb){

    text.setFont(j->font);
    text.setString(nomb);
    text.setFillColor(Color::Black);

    sprite.setTexture(imagen->get("sprites.png"));
    sprite.setTextureRect(IntRect(14*32,0,64,32));
    sprite.setScale(2,2);
    y=40+(i*70)%420;
    x=32+(70*i/420)*140;
    sprite.setPosition(x,y);
    text.setPosition(x+5,y+10);

    n=i;
    i++;
}
void Boton::clicked(){
    v ve=input->pixel();
    if(ve.x>=x&&ve.x<=x+128&&ve.y>=y&&ve.y<=y+64){
        tocado=true;
        i=n;
    }
}
void Boton::draw(){
    window->draw(sprite);
    window->draw(text);
}
