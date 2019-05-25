#include "../include/tablero.h"
#include "../include/global.h"
#include "../include/Pieza.h"
#include "movHolders.h"

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

    b.setTexture(imagen->get("tiles.png"));
    n.setTexture(imagen->get("tiles.png"));
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

vector<normalHolder*> trigsActivados; //para llamar a todos los mh una vez, despues de procesar pisados y limpiar
void Tile::chargeTriggers(){
    for(Trigger trig:triggers)
        if(trig.step==trig.tile->step)//la pieza que puso el trigger no se movio desde que lo puso
            trigsActivados.push_back(trig.nh);
    triggers.clear();
}
void activateTriggers(){
    //los triggers duplicados (por dos condiciones poniendo dos triggers a un mismo normalHolder, o por
    //dos lecturas a una memoria dinamica en distintos turnos) no son un problema, no causan calculos extra.
    //Van a mandarse juntos en la misma activacion, el primero va a causar la generacion y el segundo va a quedar colgado
    if(trigsActivados.size()==0) return;
    if(trigsActivados.size()==1){
        switchToGen=false;
        Base* base=trigsActivados[0]->base;
        actualHolder.h=base->h;
        base->beg->reaccionar(trigsActivados[0]);
    }
    else{
        ///@optim supongo que volcarlo a una matriz es mas rapido que ordenarlo y trocearlo
        sort(trigsActivados.begin(),trigsActivados.end(),[](normalHolder* a,normalHolder* b)->bool
             {return a->base->beg<b->base->beg;});
        int i=0;
        while(i<trigsActivados.size()){
            switchToGen=false;
            movHolder* base=trigsActivados[i]->base->beg;
            int j=i+1;
            while(j<trigsActivados.size()&&trigsActivados[j]->base->beg==base)
                j++;
            actualHolder.h=base->base->h;
            if(j==i+1)
                base->reaccionar(trigsActivados[i]);
            else{
                vector<normalHolder*> nhs(&trigsActivados[i],&trigsActivados[j]);
                //estaria bueno no hacer una copia, no es muy importante igual
                base->reaccionar(&nhs);
            }
            i=j;
        }
    }
    trigsActivados.clear();
}

/*
MULTIPLES TRIGGERS
cuando una pieza se mueve hacia una torre le activa dos triggers, primero el mas lejano por salir de ese espacio,
y despues el mas cercano por llegar a ese espacio.
Si se reacciona cada trigger individualmente en el orden que llegan primero se va a recalcular el del espacio vacio,
que va a recalcular todo el recorrido despues de la pieza. Y esto es al pedo, porque el segundo trigger va a invalidar
todo.
Por ahora este problema se soluciona procesando los triggers en orden inverso, asi primero invalida y el segundo no se
procesa. Pero esto es una heuristica, no va a servir para todos los casos.


Un problema distinto pasa cuando se activan dos triggers en una pieza con un desliz exc, estando los dos triggers en ramas
distintas.
desliz exc tiene el mismo problema que desliz, pero a demas tiene un otro especifico
Tenemos un desliz exc P , S end end
la rama actual generada es SP...
Una pieza activa el trigger que invalida P1, y tambien el trigger de P2.
Si se recalcula primero P2 se va a armar todo un recorrido que despues se va a invalidar por el segundo
trigger, que hace S1->P1 y recalcula todo el recorrido.

Si se recalcula en el otro orden pasa algo mas raro. Se hace S1->P1, se arma todo el recorrido. Cuando se hace la segunda
corrida buscando P2, que es de un trigger que pertenencia al recorrido que fue reemplazado, no deberia pasar nada. Pero
como se busca por punteros, el nuevo P2 tiene la misma posicion en memoria que P2 viejo, y se va a encontrar y se va a
recalcular al pedo.

Las dos formas de recorrer hacen recalculos innecesarios. El segundo caso se puede solucionar preguntando por mas cosas
a demas de el puntero, solo en exc.

Pero esta pensando que estos dos casos surgen del mismo problema, tener mas de un trigger en un mismo movimiento, y recalcular
uno por uno.
Una solucion sería recalcular preguntando por todos los triggers al mismo tiempo. Cuando se encuentre alguno se pasa a
generar, y el resto se ignora. Si hay partes del movimiento que no se regeneran y podrian tener triggers se revisan. Triggers
en zonas que fueron regeneradas no se activan dos veces, triggers que hayan quedado sueltos se ignoran. Creo que no romperia
nada.

En casos de multiples triggers seria mas rapido porque solo se recorre una vez, y no tiene errores de regenerar al pedo.
En casos de un solo trigger sería mas lento porque agrega manejo de vectores cuando no se necesita. Pero este caso podria
separarse del otro con un if y listo.

Lo unico malo de agregar esto es que hace al codigo mas bizarro, porque agregaria una version vector de reaccionar. Pero lo vale
*/
