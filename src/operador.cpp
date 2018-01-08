#include "operador.h"
#include "lector.h"
#include <Clicker.h>


bool cond;
bool outbounds;
bool separator;
list<acm*> buffer;
v pos;
v org;
Pieza* act;//para chequeos


struct color:public acm{
    sf::Color _color;
    RectangleShape cuadrado;

    color(int r,int g,int b)
    :cuadrado(Vector2f(32*escala,32*escala)),_color(-r,-g,-b,40){
        tipo=colort;
        cuadrado.setFillColor(_color);
    }
    virtual void func(){
        cuadrado.setPosition(pos.x*32*escala,pos.y*32*escala);
        window->draw(cuadrado);
    }
};

#define fabMov(NOMB,TIPO,FUNC)\
struct NOMB:public acm{\
    NOMB(){\
        tipo=TIPO;\
    };\
    virtual void func(){\
        FUNC\
    }\
}\


fabMov(mov,acct,
       (*tabl)(org.show(),nullptr);
       (*tabl)(pos.show(),act);
       org=pos;
       drawScreen();
       Sleep(60);
);
fabMov(capt,acct,
        delete (*tabl)(pos);
        (*tabl)(pos,nullptr);
);
fabMov(vacio,condt,
        cond=(*tabl)(pos)==nullptr;
);
fabMov(enemigo,condt,
        cond=(*tabl)(pos)->bando==act->bando*-1;
);
fabMov(W,movt,
       pos.y+=act->bando;
);
fabMov(S,movt,
        pos.y-=act->bando;
);
fabMov(A,movt,
        pos.x--;
);
fabMov(D,movt,
        pos.x++;
);
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tabl->tam.x&&pos.y>=0&&pos.y<tabl->tam.y){
            outbounds=true;
            cond=true;
        }else
            cond=false;
);

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
        #define caseT(TOKEN)  case lector::TOKEN: acms.push_back(new TOKEN);break
        //como la mayoria de los acm no tiene datos el new no debería alocar nada
        caseT(W);
        caseT(A);
        caseT(S);
        caseT(D);

        caseT(esp);
        caseT(vacio);
        caseT(enemigo);

        caseT(mov);
        caseT(capt);

        //movs con parametros
        case lector::color:
            acms.push_back(new color(tokens.front(),(tokens.pop_front(),tokens.front()),(tokens.pop_front(),tokens.front())));
            break;
        case lector::sep:
            separator=true;
            return;
        case lector::eol:
        case lector::lim:
            return;
        case lector::desliz:
            sig=new desliz(true);return;
        case lector::deslizCond:
            sig=new desliz(false);return;
        case lector::multi:
            sig=new multi;return;
        case lector::opt:
            sig=new opt;return;
        }
    }
}

bool normal::operar(){
    cond=true;
    for(acm* a:acms){
        if(a->tipo==condt){
            if((a->func(),cond==false))
                return false;
        }else if(a->tipo==movt)
            a->func();
    }
    for(acm* a:acms){
        if(a->tipo==acct||a->tipo==movt||a->tipo==colort)
            buffer.push_back(a);
    }
    return true;
}

void normal::debug(){
    for(acm* a:acms)
        cout<<"^"<<a->tipo<<"^";
}

desliz::desliz(bool _doClickers){
    doClickers=_doClickers;
    inside=new normal;
    sig=keepOn();
}

void desliz::debug(){
    cout<<"d ";
    inside->debug();
    cout<<"t ";
}

bool desliz::operar(){
    //mover org
    int i=0;
    v aux=pos;
    while(inside->operar()){
        cout<<"D";
        aux=pos;
        i++;
        if(doClickers){
            clickers.push_back(new Clicker(true));
        }
    }
    pos=aux;
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
        v aux=org;
        v aux2=pos;
        op->operar();
        org=aux;
        pos=aux2;
    }
}


