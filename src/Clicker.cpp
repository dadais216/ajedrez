#include "Clicker.h"
#include "global.h"
#include <operador.h>
#include <Pieza.h>

bool Clicker::drawClickers;

Clicker::Clicker(vector<normalHolder*>* normales_){
    ///con la estructura de movimiento hecha se arman listas de normales y se pasan a clickers
    normales=normales_;

    normalHolder* lastN=normales->operator[](normales->size()-1);
    clickPos=lastN->accs[lastN->accs.size()-1]->pos;

    cout<<"############";
    for(normalHolder* n:*normales){
        for(acct* a:n->accs)
            a->pos.show();
    }

    cout<<"!!";
    clickPos.show();
    cout<<"#########################";
    Sleep(4000);

    //clickPos??
    ///solapamientos
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
    clickers.push_back(this);
}

void Clicker::draw(){
    for(normalHolder* n:*normales){
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

bool Clicker::update(){
    if(activo&&input->get()==clickPos){
        /*
        //esto era para confirmar el toque
        if(mod>1){
            clickers.clear();
            clickers.push_back(this);
            confirm=true;
            val=0;
            mod=1;
            return true;
        }
        */
        //drawClickers=false;
        accionar();
        return true;
    }
    return false;
}

void Clicker::accionar(){
    for(normalHolder* n:*normales){
        n->accionar();
    }
}

void Clicker::activacion(int clickI){
    activo=val==clickI%mod;
}



