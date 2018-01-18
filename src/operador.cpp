#include "operador.h"
#include "lector.h"
#include <Clicker.h>


bool cond;
bool bOutbounds=false;
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
void color::debug(){
    cout<<"color ";
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
    }
    virtual void debug(){
        cout<<"posRemember ";
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
        if(memcambios){
            pos.x=numeros[index];
            pos.y=numeros[jndex];
        }
    }
    virtual void debug(){
        cout<<"posRestore ";pos.show();cout<<" -> ";v(numeros[index],numeros[jndex]).show();
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
    virtual void debug(){\
        cout<<#NOMB<<" ";\
    } \
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
list<v> limites,limitesAux;
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tabl->tam.x&&pos.y>=0&&pos.y<tabl->tam.y){
            cond=true;
            for(v vec:limites){ //si realentiza mover a otra cond
                if(pos==vec){
                    cond=false;
                    break;
                }
            }
        }else{
            cond=false;
            bOutbounds=true;
        }
);
fabMov(prob,condt,
        limitesAux.emplace_back(pos);
);
fabMov(outbounds,condt,
        cond=bOutbounds;
        bOutbounds=false;
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
        caseT(outbounds);
        caseT(vacio);
        caseT(enemigo);
        caseT(prob);

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
        case lector::eol:
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
                limitesAux.clear();
                return false;
            }
        }else if(a->tipo==movt)
            a->func();
        else if(a->tipo==colort)
            a->func();
    }
    cout<<" V ";
    bufferColores.splice(bufferColores.begin(),colores);
    limites.splice(limites.begin(),limitesAux);

    cambios=true;
    for(acm* a:acms){
        if(a->tipo==acct||a->tipo==movt)
            buffer.push_back(a);
    }
    return then();
}

void normal::debug(){
    for(acm* a:acms){
        a->debug();
    }
    if(sig) sig->debug();
}

#define paramCase(PARAM) case lector::PARAM: tokens.pop_front(); PARAM=true; break

struct click;
desliz::desliz(){
    t=nc=false;
    while(true){
        switch(tokens.front()){
        paramCase(nc);
        paramCase(t);
        default: goto next;
        }
    }
    next:
    inside=tomar();

    operador* it=inside;
    while(it->sig)
        it=it->sig;
    if(!nc){
        it->sig=new click; //hago esto en vez de usar bools por casos de opt
        it=it->sig;
    }
    it->sig=this;

    i=-1;

    sig=keepOn();

}

void desliz::debug(){
    cout<<"desliz< ";
    inside->debug();
    cout<<"> ";
    if(sig) sig->debug();
}

bool desliz::operar(){
    i++;
    aux=pos;

    cout<<"DESLIZ ";
    inside->operar();

    cout<<"/DESLIZ ";
//    while(inside->operar()){
//        aux=pos;
//        i++;
//    }
    pos=aux;
    if(i||t){
        i=-1;
        return then();
    }
    i=-1;
    return false;
}

bool operarAislado(operador* op){
    v posRes=pos;
    list<acm*>::iterator bufferRes=!buffer.empty()?--buffer.end():buffer.begin();
    list<pair<RectangleShape*,v>>::iterator bColorRes=!bufferColores.empty()?--bufferColores.end():bufferColores.begin();
    Pieza* pRes=act;

    bool ret=op->operar();

    if(ret)
        crearClicker();

    pos=posRes;
    buffer.erase(++bufferRes,buffer.end());
    bufferColores.erase(++bColorRes,bufferColores.end());
    act=pRes;
    return ret;
}

bloque::bloque(){
    inside=tomar();
    sig=keepOn();
}

bool bloque::operar(){
    list<v>::iterator limitRes=!limites.empty()?--limites.end():limites.begin();
    operarAislado(inside);
    limites.erase(++limitRes,limites.end());
    return then();
}

void bloque::debug(){
    cout<<"<< ";
    inside->debug();
    cout<<">> ";
    sig->debug();
}

joiner::joiner(){sig=nullptr;};
bool joiner::operar(){return then();}
void joiner::debug(){cout<<"joiner ";}

opt::opt(){
    separator=true;
    while(separator){
        separator=false;
        ops.push_back(tomar());
    }
    sig=keepOn();

    cout<<"#######################";
    sig->debug();
    cout<<"#######################";

    if(!sig)
        sig=new joiner;
    for(operador* op:ops){
        operador* it=op;
        while(it->sig)
            it=it->sig;
        it->sig=sig;
    }
}

void opt::debug(){
    cout<<"opt <";
    for(operador* op:ops){
        op->debug();
        cout<<" | ";
    }
    cout<<"> ";
}

bool opt::operar(){
    bool ret=false;

    cout<<"OPT ";
    for(operador* op:ops){
        ret=operarAislado(op)||ret;
        cout<<"OR ";
    }
    cout<<"/OPT";
    return ret;
}

#define fabOp(NOMB,FUNC) \
NOMB::NOMB(){ \
    sig=keepOn(); \
} \
void NOMB::debug(){ \
    cout<<"NOMB "; \
} \
bool NOMB::operar(){ \
    FUNC \
    return then(); \
}

fabOp(click,
    crearClicker();
);



operador* keepOn(){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front()){
    case lector::sep:
        separator=true;
    case lector::eol:
    case lector::lim:
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
    case lector::bloque:
        return new bloque;
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

void crearClicker(){
    clickers.push_back(new Clicker(true));
}

