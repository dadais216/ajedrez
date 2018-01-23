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
//        for(int tok:tokens){
//            cout<<tok<<"-";
//        }
//        cout<<endl;
        movs.push_back(tomar());
    }
    for(operador* o:movs){
        o->debug();
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
        if(op->operar()&&cambios){
            clickers.push_back(new Clicker(false));
        }else{
            buffer.clear();
            bufferColores.clear();
        }
        cout<<endl<<"FIN DE MOVIMIENTO"<<endl;
        resetearValores();
    }
}

void resetearValores(){
    if(memcambios){ //no es necesario
        numeros.fill(0);
        memcambios=false;
    }
    pos=org;
    limites.clear();
}



