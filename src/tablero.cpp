#include "../include/tablero.h"
#include "../include/global.h"
#include "../include/Pieza.h"

float escala;
tabl* tablptr;

tabl::tabl(){
    tablptr=this;
}

void tabl::armar(v a){
    tam=a;
    matriz.resize(tam.x);
    for(int i=0; i<tam.x; i++)
        matriz[i].resize(tam.y);
    escala=16*(1/(float)(tam.x>tam.y?tam.x:tam.y));
    for(int i=0; i<tam.x; i++)
        for(int j=0; j<tam.y; j++){
            Tile* tile=new Tile;
            tile->color=(i+j)%2;
            tile->pos=v(i,j);
            tile->step=0;
            matriz[i][j]=tile;
        }

    b.setTexture(imagen->get("sprites.png"));
    n.setTexture(imagen->get("sprites.png"));
    b.setScale(escala,escala);
    n.setScale(escala,escala);
    b.setTextureRect(IntRect(0,0,32,32));
    n.setTextureRect(IntRect(32,0,32,32));
}

Tile* tabl::tile(v pos){
    return matriz[pos.x][pos.y];
}

void tabl::drawTiles(){
    for(int i=0; i<tam.x; i++)
        for(int j=0; j<tam.y; j++)
            if(matriz[i][j]->color){
                b.setPosition(i*escala*32,j*escala*32);
                window->draw(b);
            }else{
                n.setPosition(i*escala*32,j*escala*32);
                window->draw(n);
            }
}

void tabl::drawPieces(){
    for(int i=0; i<tam.x; i++)
        for(int j=0; j<tam.y; j++){
            Holder* p;
            if(p=matriz[i][j]->holder)
                p->draw();
        }
}
void Tile::activateTriggers(){
    unordered_set<normalHolder*> nhs; //para llamar a todos los mh una vez, despues de procesar pisados y limpiar
    for(Trigger trig:triggers)
        if(trig.step==trig.tile->step)//la pieza que puso el trigger no se movio desde que lo puso
            nhs.insert(trig.nh);
    triggers.clear();
    for(normalHolder* nh:nhs){
        cout<<"TRIGGERED ";
        nh->base.beg->reaccionar(nh);
    }
}
