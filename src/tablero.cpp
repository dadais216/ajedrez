#include "tablero.h"
#include <global.h>
#include <Pieza.h>

casillero::casillero(){
    pieza=nullptr;
}

tablero::tablero()
{
    b.setTexture(imagen->get("sprites.png"));
    n.setTexture(imagen->get("sprites.png"));
    b.scale(escala,escala);
    n.scale(escala,escala);
    b.setTextureRect(IntRect(0,0,32,32));
    n.setTextureRect(IntRect(32,0,32,32));
}

tablero::armar(v a){
    tam=a;
    matriz.resize(tam.x);
    for(int i=0;i<tam.x;i++){
        matriz[i].resize(tam.y);
    }
    escala=16*(1/(float)(tam.x>tam.y?tam.x:tam.y));
    for(int i=0;i<tam.x;i++){
        for(int j=0;j<tam.y;j++){
            matriz[i][j].tile=(i+j+tam.tec)%2;
        }
    }
}

Pieza* tablero::operator()(v a){
    return matriz[a.x][a.y].pieza;
}

void tablero::operator()(v a, Pieza* p){
    matriz[a.x][a.y].pieza=p;
}

void tablero::drawTiles(){
    for(int i=0;i<tam.x;i++){
        for(int j=0;j<tam.y;j++){
            if(matriz[i][j].tile){
                b.setPosition(i*escala*32,j*escala*32);
                window->draw(b);
            }else{
                n.setPosition(i*escala*32,j*escala*32);
                window->draw(n);
            }
        }
    }
}

void tablero::drawPieces(){
    for(int i=0;i<tam.x;i++){
        for(int j=0;j<tam.y;j++){
            Pieza* p;
            if(p=(*this)(v(i,j))){
                p->draw(i,j);
            }
        }
    }
}
