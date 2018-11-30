#include "../include/Clicker.h"
#include "../include/global.h"
#include "../include/operador.h"
#include "../include/Pieza.h"
#include "../include/Juego.h"
#include "movHolders.h"

bool Clicker::drawClickers;

vector<Clicker*> clickers;

Clicker::Clicker(vector<normalHolder*>* normales_,Holder* h_){
    normales=*normales_;
    ///@optim estaria bueno no copiar el ultimo vector
    h=h_;

    normalHolder* lastN=normales[normales.size()-1];
    /*
    caso para manejar cosas donde se genere un clicker donde la ultima normal no tenga acciones.
    Lo dejo aparte porque no se me ocurre un caso donde sea util, y no me parece que valga la pena
    Para funcionar se deberia hacer posAct una variable de normalHolder (ahora es local)
    if(lastN->accs.empty())
        clickPos=lastN->posAct;
    else
    */
    clickPos=lastN->accs[lastN->accs.size()-1]->pos;


    //clickPos.show();
    ///solapamientos
    /*
    val=0;
    mod=1;
    int conflictos=0;
    for(Clicker* c:clickers)
        if(c->clickPos==clickPos){
            conflictos++;
            c->mod++;
        }
    if(conflictos!=0){
        val=conflictos;
        mod=conflictos+1;
    }
    */
}

void Clicker::draw(){
    for(normalHolder* n:normales){
        n->draw();
    }
    /*
    if(!activo) return;
    for(pair<drawable,v> c:colores){
        if(get<0>(c).tipo==0){
            RectangleShape* rs=(RectangleShape*)get<0>(c).obj;
            rs->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*rs);
        }else if(get<0>(c).tipo==1){
            Sprite* s=(Sprite*)get<0>(c).obj;
            s->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*s);
        }else{
            Text* t=(Text*)get<0>(c).obj;
            t->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*t);
        }
        //seguro que se puede simplificar con cast y esas boludeces

        //cout<<c->cuadrado.getPosition().x/32/escala<<" "<<c->cuadrado.getPosition().y/32/escala<<endl;
    }
    */
}

vector<Tile*> pisados;

bool Clicker::update(){
    //activo&&
    if(input->get()==clickPos){
        /*
        //esto es para confirmar el toque
        if(mod>1){
            clickers.clear();
            clickers.push_back(this);
            confirm=true;
            val=0;
            mod=1;
            return true;
        }
        */
        pisados.push_back(h->tile);
        accionar(); //por ahora solo capt agrega a pisados
        pisados.push_back(h->tile);


        for(Tile* tile:pisados){
            //cout<<"<<<"<<tile->pos<<endl;
            tile->chargeTriggers();
        }
        activateTriggers();
        pisados.clear();
        h->generar();
        ///una pieza nunca activa sus propios triggers porque al moverse los invalida
        ///necesita generar todos sus movimientos devuelta de forma explicita
        ///esto no es verdadero en movimientos que no mueven la pieza

        cout<<endl;
        for(int i=0; i<tablptr->tam.y; i++){
            for(int j=0; j<tablptr->tam.x; j++){
                cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
            }
            cout<<endl;
        }
        return true;
    }
    return false;
}

void Clicker::accionar(){
    Clicker::drawClickers=false;
    for(normalHolder* n:normales){
        n->accionar();
    }
}

/*
void Clicker::activacion(int clickI)
{
    activo=val==clickI%mod;
}

*/
void Clicker::debug(){
    for(normalHolder* n:normales)
        n->debug();
}

