#include "operador.h"
#include "lector.h"


bool cond;
bool outbounds;
bool separator;
list<acm*> buffer;
v* org; //de la pieza, lo usa y actualiza mov. Su contenido se pasa al primer mov tambien
v pos;
Pieza* act;//para chequeos

//el tablero va a tener que ser global
void movf(){
    *org=pos;
}
void captf(){
    //mover pieza en pos a capturados
}
void vaciof(){
    cond=true;
}
void enemigof(){
    cond=true;
}
void Wf(){
    pos.y+=act->bando;
}
void Sf(){
    pos.y-=act->bando;
}
void Af(){
    pos.x--;
}
void Df(){
    pos.x++;
}
void espf(){
    //controlar que la pos este dentro del tablero
    outbounds=true;
    cond=false;
}

acm mov={acct,movf};
acm capt={acct,captf};
acm vacio={condt,vaciof};
acm enemigo={condt,enemigof};
acm W={movt,Wf};
acm S={movt,Sf};
acm A={movt,Af};
acm D={movt,Df};
acm esp={condt,espf};

operador* keepOn(){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front()){
    case lector::eol:
    case lector::sep:
    case lector::lim:
        return nullptr;
    }
    return new normal;
}

normal::normal(){
    sig=nullptr;
    while(true){
        if(tokens.empty()) return;
        int tok=tokens.front();
        tokens.pop_front();
        switch(tok){
        #define caseT(TOKEN)  case lector::TOKEN: acms.push_back(&TOKEN);break;
        caseT(W);
        caseT(A);
        caseT(S);
        caseT(D);

        caseT(esp);
        caseT(vacio);
        caseT(enemigo);

        caseT(mov);
        caseT(capt);

        case lector::sep:
            separator=true;
            return;
        case lector::eol:
        case lector::lim:
            return;
        case lector::desliz:
            sig=new desliz;break;
        case lector::multi:
            sig=new multi;break;
        case lector::opt:
            sig=new opt;break;
        }
    }
}

bool normal::operar(){
    v aux=pos;
    for(acm* a:acms){
        if(a->tipo==condt){
            a->func();
            if(cond==false)
                return false;
        }else if(a->tipo==movt)
            a->func();
    }
    pos=aux;
    for(acm* a:acms){
        if(a->tipo==acct||a->tipo==movt)
            buffer.push_back(a);
    }
    return true;
}

void normal::debug(){
    for(acm* a:acms)
        cout<<"|"<<a->tipo<<"|";
    cout<<endl;
}

desliz::desliz(){
    //tomar movs
    inside=new normal;
    sig=keepOn();
}

bool desliz::operar(){
    //mover org
    int i=-1;
    while(inside->operar()){
        i++;
        //crear cliker con buffer parcial
    }
    return i;
}

opt::opt(){
    separator=true;
    while(separator){
        separator=false;
        v aux=pos;
        ops.push_back(new normal);
        pos=aux;
    }
    sig=keepOn();
}

bool opt::operar(){
    for(auto op:ops)
        if(op->operar())
            return true;
    return false;
}

multi::multi(){
    separator=false;
    while(!separator){
        v aux=pos;
        ops.push_back(new normal);
        pos=aux;
    }
    sig=keepOn();
}

bool multi::operar(){
    for(auto op:ops){
        v aux=*org;
        v aux2=pos;
        op->operar();
        *org=aux;
        pos=aux2;
    }
}


