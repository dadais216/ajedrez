#include "operador.h"
#include "lector.h"


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
    :cuadrado(Vector2f(32*escala,32*escala)),_color(-r,-g,-b){
        tipo=colort;
        cuadrado.setFillColor(_color);
        cuadrado.setPosition(pos.x*32*escala,pos.y*32*escala);
    }
    virtual void func(){
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
       //(*tabl)(org,nullptr);
       //(*tabl)(pos,act);
       //org=pos;
);
fabMov(capt,acct,

);
fabMov(vacio,condt,
        cond=true;
);
fabMov(enemigo,condt,
        cond=true;
);
fabMov(W,movt,
       pos.y+=act->bando;
       cout<<"W";
);
fabMov(S,movt,
        pos.y-=act->bando;
        cout<<"S";
);
fabMov(A,movt,
        pos.x--;
);
fabMov(D,movt,
        pos.x++;
);
fabMov(esp,condt,
        outbounds=true;
        cond=true;
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
            sig=new desliz;break;
        case lector::multi:
            sig=new multi;break;
        case lector::opt:
            sig=new opt;break;
        }
    }
}

bool normal::operar(){
    v org=pos;
    cond=true;
    for(acm* a:acms){
        if(a->tipo==condt){
            if((a->func(),cond==false))
                return false;
        }else if(a->tipo==movt)
            a->func();
    }
    cout<<"AAAAAAAAA";
    pos=org;
    for(acm* a:acms){
        if(a->tipo==acct||a->tipo==movt||a->tipo==colort)
            buffer.push_back(a);
    }
    return true;
}

void normal::debug(){
    for(acm* a:acms)
        cout<<"^"<<a->tipo<<"^";
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
        v aux=org;
        v aux2=pos;
        op->operar();
        org=aux;
        pos=aux2;
    }
}


