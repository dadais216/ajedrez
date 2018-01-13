#include "Clicker.h"
#include "global.h"
#include <operador.h>

bool Clicker::drawClickers;

Clicker::Clicker(bool copiarBuffer){
    if(copiarBuffer){
        acciones.assign(buffer.begin(),buffer.end());
        colores.assign(bufferColores.begin(),bufferColores.end());
        cambios=false;
    }else{
        acciones.splice(acciones.begin(),buffer);
        colores.splice(colores.begin(),bufferColores);
    }

    drawClickers=true; //cuando me ponga a  limpiar memoria tendría que borrar el buffer en lugar de
    clickPos=pos;
    cout<<"y: "<<acciones.size()<<" "<<colores.size()<<endl;
}

void Clicker::draw(){
    for(pair<RectangleShape*,v> c:colores){
        get<0>(c)->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
        window->draw(*get<0>(c));
        //cout<<c->cuadrado.getPosition().x/32/escala<<" "<<c->cuadrado.getPosition().y/32/escala<<endl;
    }
}

bool Clicker::update(){
    if(input->get()==clickPos){
        drawClickers=false;
        for(acm* a:acciones){
            a->func();
        }
        return true;
    }
    return false;
}



