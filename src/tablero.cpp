#include "../include/tablero.h"
#include "../include/global.h"
#include "../include/Pieza.h"

float escala;
tabl* tablptr;

tabl::tabl()
{
    tablptr=this;
}

void tabl::armar(v a)
{
    tam=a;
    matriz.resize(tam.x);
    tiles.resize(tam.x);
    for(int i=0; i<tam.x; i++)
    {
        matriz[i].resize(tam.y);
        tiles[i].resize(tam.y);
    }
    escala=16*(1/(float)(tam.x>tam.y?tam.x:tam.y));
    cout<<escala<<endl;
    for(int i=0; i<tam.x; i++)
    {
        for(int j=0; j<tam.y; j++)
        {
            tiles[i][j]=(i+j)%2;
        }
    }

    b.setTexture(imagen->get("sprites.png"));
    n.setTexture(imagen->get("sprites.png"));
    b.scale(escala,escala);
    n.scale(escala,escala);
    b.setTextureRect(IntRect(0,0,32,32));
    n.setTextureRect(IntRect(32,0,32,32));
}

Holder* tabl::operator()(v a)
{
    return matriz[a.x][a.y];
}

void tabl::operator()(v a, Holder* p)
{
    matriz[a.x][a.y]=p;
}

void tabl::drawTiles()
{
    for(int i=0; i<tam.x; i++)
    {
        for(int j=0; j<tam.y; j++)
        {
            if(tiles[i][j])
            {
                b.setPosition(i*escala*32,j*escala*32);
                window->draw(b);
            }
            else
            {
                n.setPosition(i*escala*32,j*escala*32);
                window->draw(n);
            }
        }
    }
}

void tabl::drawPieces()
{
    for(int i=0; i<tam.x; i++)
    {
        for(int j=0; j<tam.y; j++)
        {
            Holder* p;
            if(p=matriz[i][j])
            {
                p->draw();
            }
        }
    }
    for(int i=0; i<capturados.size(); i++)
        capturados[i]->draw(i);
}
