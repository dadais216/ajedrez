#include "Pieza.h"
#include <global.h>
#include <operador.h>
#include <Clicker.h>

list<Pieza*> piezas;

Pieza::Pieza(int _id,int _sn){
    id=_id;
    sn=_sn;
    spriteb.setTexture(imagen->get("sprites.png"));
    spriteb.setTextureRect(IntRect(64+sn*64,0,32,32));
    spriteb.setScale(escala,escala);
    spriten.setTexture(imagen->get("sprites.png"));
    spriten.setTextureRect(IntRect(64+sn*64+32,0,32,32));
    spriten.setScale(escala,escala);

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
    piezas.push_back(this);
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

Holder::Holder(int _bando,Pieza* p){
    bando=_bando;
    inicial=true;
    pieza=p;
}

void Holder::draw(v vec){
    if(bando==1){
        pieza->spriten.setPosition(vec.x*escala*32,vec.y*escala*32);
        window->draw(pieza->spriten);
    }else{
        pieza->spriteb.setPosition(vec.x*escala*32,vec.y*escala*32);
        window->draw(pieza->spriteb);
    }
}


