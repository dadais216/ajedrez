#include "operador.h"
#include "lector.h"
#include <Clicker.h>


bool cond;
bool outbounds;
bool separator;
bool cambios;

struct color;
list<pair<RectangleShape*,v>> colores;
list<pair<RectangleShape*,v>> bufferColores;
list<acm*> buffer;
v pos;
v org;
Pieza* act;//para chequeos

#define M(algo) (cout<<"|"<<algo<<"|",algo)

color::color()
:cuadrado(Vector2f(32*escala,32*escala)),_color(){
    _color.r=-tokens.front();tokens.pop_front();
    _color.g=-tokens.front();tokens.pop_front();
    _color.b=-tokens.front();tokens.pop_front();
    _color.a=40;
    tipo=colort;
    cuadrado.setFillColor(_color);
}
void color::func(){
    colores.push_back(pair<RectangleShape*,v>(&cuadrado,pos));
}

array<int,20> numeros;
bool memcambios;

struct posRemember:public acm{
    int index,jndex;
    posRemember(){
        tipo=movt;
        if(!tokens.empty()&&tokens.front()<0){
            index=-tokens.front(); tokens.pop_front();
            jndex=-tokens.front(); tokens.pop_front();
        }else{
            index=0;
            jndex=1;
        }
    }
    virtual void func(){
        memcambios=true;
        numeros[index]=pos.x;
        numeros[jndex]=pos.y;
        cout<<"remember ";
    }
};

struct posRestore:public acm{
    int index,jndex;
    posRestore(){
        tipo=movt;
        if(!tokens.empty()&&tokens.front()<0){
            index=-tokens.front(); tokens.pop_front();
            jndex=-tokens.front(); tokens.pop_front();
        }else
            index=0;
            jndex=1;
    }
    virtual void func(){
        cout<<"restore ";pos.show();cout<<" -> ";v(numeros[index],numeros[jndex]).show();
        if(memcambios){
            pos.x=numeros[index];
            pos.y=numeros[jndex];
        }
    }
};

#define fabMov(NOMB,TIPO,FUNC)\
struct NOMB:public acm{\
    NOMB(){\
        tipo=TIPO;\
    };\
    virtual void func(){\
        FUNC\
        cout<<#NOMB<<" ";\
    }\
}\


fabMov(mov,acct,
       (*tabl)(org.show(),nullptr);
       (*tabl)(pos.show(),act);
       org=pos;
);
fabMov(capt,acct,
        delete (*tabl)(pos);//en realidad mover a capturados
        (*tabl)(pos,nullptr);
);
fabMov(pausa,acct,
        drawScreen();
        Sleep(60);
);

fabMov(vacio,condt,
        cond=(*tabl)(pos)==nullptr;
);
fabMov(enemigo,condt,
        cond=(*tabl)(pos)->bando==act->bando*-1;
);
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tabl->tam.x&&pos.y>=0&&pos.y<tabl->tam.y){
            outbounds=true;
            cond=true;
        }else
            cond=false;
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

normal::normal(){
    sig=nullptr;
    while(true){
        if(tokens.empty()) return;
        int tok=tokens.front();tokens.pop_front();
        cout<<"TOK:"<<tok<<endl;
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
        caseT(pausa);
        caseT(posRemember);
        caseT(posRestore);

        //movs con parametros
        case lector::color:
            acms.push_back(new color());
            break;
        case lector::sep:
            separator=true;
            return;
        case lector::eol:
            return;
        case lector::lim:
            return;
        default:
            tokens.push_front(tok);
            sig=tomar();
            return;
        }
    }
}

bool normal::operar(){
    cond=true;
    for(acm* a:acms){
        if(a->tipo==condt){
            if((a->func(),cond==false)){
                cout<<" F ";
                colores.clear();
                return false;
            }
        }else if(a->tipo==movt)
            a->func();
        else if(a->tipo==colort)
            a->func();
    }
    cout<<" V ";
    bufferColores.splice(bufferColores.begin(),colores);

    cambios=true;
    for(acm* a:acms){
        if(a->tipo==acct||a->tipo==movt)
            buffer.push_back(a);
    }
    return then();
}

void normal::debug(){
    for(acm* a:acms)
        cout<<"^"<<a->tipo<<"^";
    if(sig) sig->debug();
}

desliz::desliz(){
    inside=tomar();
    sig=keepOn();
}

void desliz::debug(){
    cout<<"d ";
    inside->debug();
    cout<<"t ";
    if(sig) sig->debug();
}

bool desliz::operar(){
    //mover org
    int i=0;
    v aux=pos;
    while(inside->operar()){
        cout<<"D";
        aux=pos;
        i++;
    }
    pos=aux;
    if(i)
        return then();
    return false;
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

void opt::debug(){

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


#define fabOp(NOMB,FUNC) \
NOMB::NOMB(){ \
    sig=keepOn(); \
} \
void NOMB::debug(){ \
    cout<<" NOMB "; \
} \
bool NOMB::operar(){ \
    FUNC \
    return then(); \
}

fabOp(click,
    clickers.push_back(new Clicker(true));
);



operador* keepOn(){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front()){
    case lector::eol:
    case lector::sep:
    case lector::lim:
        tokens.pop_front();
        return nullptr;
    }
    return tomar();
}

operador* tomar(){
    if(tokens.empty()) return nullptr;
    int tok=tokens.front();tokens.pop_front();
    cout<<"TOK T "<<tok<<endl;
    switch(tok){
    case lector::desliz:
        return new desliz();
    case lector::opt:
        return new opt;
    case lector::click:
        return new click;
    default:
        tokens.push_front(tok);
        return new normal;
    }
}

bool operador::then(){
    if(!sig)
        return true;
    return sig->operar();
}

