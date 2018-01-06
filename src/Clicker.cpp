#include "Clicker.h"
#include "global.h"
#include <operador.h>

Clicker::Clicker(){
    acciones.splice(acciones.begin(),buffer);
}

void Clicker::draw(){
    for(acm* a:acciones){
        if(a->tipo==colort||a->tipo==movt)
            a->func();
    }
    pos=org;
}



