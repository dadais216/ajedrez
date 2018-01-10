#include "Clicker.h"
#include "global.h"
#include <operador.h>

bool drawClickers;

Clicker::Clicker(bool copiarBuffer){
    if(copiarBuffer){
        acciones.assign(buffer.begin(),buffer.end());
        cambios=false;
    }
    else
        acciones.splice(acciones.begin(),buffer);
    drawClickers=true;
    cout<<"y: "<<acciones.size()<<endl;
}

void Clicker::draw(){
    if(drawClickers){
        clickPos=pos;
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



