#include "Clicker.h"
#include "global.h"
#include <operador.h>

Clicker::Clicker(){
    acciones.splice(acciones.begin(),buffer);
    drawClickers=true;
}

void Clicker::draw(){
    if(drawClickers){
        for(acm* a:acciones){
            if(a->tipo==colort||a->tipo==movt)
                a->func();
        }
        clickPos=pos;
        pos=org;
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



