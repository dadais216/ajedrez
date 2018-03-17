#include "operador.h"
#include "lector.h"
#include <Clicker.h>


//bool bOutbounds=false;
//bool separator;
//bool cambios;

color::color()
:cuadrado(Vector2f(32*escala,32*escala)),_color(){
    _color.r=tokens.front()-1000;tokens.pop_front();
    _color.g=tokens.front()-1000;tokens.pop_front();
    _color.b=tokens.front()-1000;tokens.pop_front();
    _color.a=40;
    tipo=colort;
    cuadrado.setFillColor(_color);
}
void color::draw(){
    window->draw(cuadrado);
}
void color::debug(){
    cout<<"color ";
}

/*
sprt::sprt(){
    int sn=tokens.front()-1000;tokens.pop_front();
    _sprt.setTexture(imagen->get("sprites.png"));
    _sprt.setTextureRect(IntRect(64+sn*64,0,32,32));
    _sprt.setScale(escala,escala);
    _sprt.setColor(Color(255,255,255,120));
    tipo=colort;
}
void sprt::func(){
    bufferColores.push_back(pair<drawable,v>(drawable(1,&_sprt),pos));
}
void sprt::debug(){
    cout<<"sprt ";
}

numShow::numShow(){
    txt.setFont(j->font);
    index=tokens.front()-1000;tokens.pop_front();
    txt.setFillColor(Color::Black);
    tipo=colort;
}
void numShow::func(){
    std::ostringstream stm;
    stm<<numeros[index];
    txt.setString(stm.str());
    bufferColores.push_back(pair<drawable,v>(drawable(2,&txt),pos));
}
void numShow::debug(){
    cout<<"numShow "<<index<<" ";
}
*/

array<int,20> numeros;
bool memcambios;

struct posRemember:public acm{
    int index,jndex;
    posRemember(){
        tipo=movt;
        if(!tokens.empty()&&tokens.front()>=1000){
            index=tokens.front()-1000; tokens.pop_front();
            jndex=tokens.front()-1000; tokens.pop_front();
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
        if(!tokens.empty()&&tokens.front()>=1000){
            index=tokens.front()-1000; tokens.pop_front();
            jndex=tokens.front()-1000; tokens.pop_front();
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

#define numFab(NOMB,TIPO,FUNC) \
struct NOMB:public acm{ \
    int val,index; \
    NOMB(){ \
        tipo=TIPO; \
        index=tokens.front()-1000; tokens.pop_front();  \
        val=tokens.front()-1000; tokens.pop_front(); \
    } \
    virtual void func(){ \
        FUNC \
    } \
    virtual void debug(){ \
        cout<<#NOMB<<" "<<index<<" "<<val<<" "; \
    } \
}

numFab(numSet,movt,numeros[index]=val;);
numFab(numAdd,movt,numeros[index]+=val;);
numFab(numCmp,condt,cond=numeros[index]==val;);
numFab(numDst,condt,cond=numeros[index]!=val;);
numFab(numLess,condt,cond=numeros[index]<val;);

numFab(numSeti,movt,numeros[index]=numeros[val];);
numFab(numAddi,movt,numeros[index]+=numeros[val];);
numFab(numCmpi,condt,cond=numeros[index]==numeros[val];);
numFab(numDsti,condt,cond=numeros[index]!=numeros[val];);
numFab(numLessi,condt,cond=numeros[index]<numeros[val];);


struct spwn:public acm{
    int n;
    spwn(){
        tipo=acct;
        n=tokens.front()-1000; tokens.pop_front();
        //cuando anden los negativos se pueden invocar píezas del bando opuesto
    }
    virtual void func(){
        (*tabl)(pos.show(),lect.crearPieza(n*act->bando));
        cout<<"spwn "<<n<<" ";
    }
    virtual void debug(){
        cout<<"spwn "<<n<<" ";
    }
};

//usando herencia podría evitar tener 800 constructores iguales, pero trae sus cosillas eso
#define fabMov(NOMB,TIPO,FUNC)\
struct NOMB:public TIPO{\
    NOMB(pos_){\
        pos=pos_;\
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
fabMov(pausa,acct,
        drawScreen();
        Sleep(60);
);
vector<Holder*> capturados;
fabMov(capt,acct,
        capturados.push_back((*tabl)(pos));
        (*tabl)(pos,nullptr);
);
fabMov(del,acct,
        delete (*tabl)(pos);
        (*tabl)(pos,nullptr);
);


fabMov(vacio,condt,
        cond=(*tabl)(pos)==nullptr;
);
fabMov(pieza,condt,
       cond=(*tabl)(pos);
);
fabMov(enemigo,condt,
        if((*tabl)(pos))
            cond=(*tabl)(pos)->bando==act->bando*-1;
        else
            cond=false;
);
list<v> limites;
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tabl->tam.x&&pos.y>=0&&pos.y<tabl->tam.y){
            bOutbounds=false;
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
        limites.emplace_back(pos);
);
fabMov(outbounds,condt,
        cond=bOutbounds;
);
fabMov(inicial,condt,
        cond=act->inicial;
);
);
fabMov(ori,movt,
        pos=org;
);

normal::normal(){
    v pos(0,0);
    sig=nullptr;
    while(true){
        if(tokens.empty()) return;
        int tok=tokens.front();tokens.pop_front();
        switch(tok){
        case lector::W:
            pos.y++; //el espejado se va a tener que hacer cuando se construyan las absolutas
        break;case lector::S:
            pos.y--;
        break;case lector::D:
            pos.x++;
        break;case lector::A:
            pos.x--;
        break;
        #define caseT(TIPO,TOKEN)  case lector::TOKEN: cout<<#TOKEN<<endl;TIPO.push_back(new TOKEN(pos));break
        #define cond(TOKEN) caseT(conds,TOKEN)

        cond(posRemember);
        cond(numSet);
        cond(numAdd);
        cond(numSeti);
        cond(numAddi);
        cond(numCmp);
        cond(numDst);
        cond(numCmpi);
        cond(numDsti);
        cond(numLess);
        cond(numLessi);

        cond(esp);
        cond(outbounds);
        cond(vacio);
        cond(pieza);
        cond(enemigo);
        cond(prob);
        cond(inicial);

        #define acc(TOKEN) caseT(accs,TOKEN)

        acc(mov);
        acc(capt);
        acc(pausa);
        acc(spwn);
        acc(del);


        caseT(color);
        caseT(sprt);
        caseT(numShow);

        #undef acc
        #undef cond
        #undef caseT

        case lector::sep:
            cout<<"sep"<<endl;
            separator=true;
            return;
        case lector::eol:
            cout<<"eol"<<endl;
            return;
        case lector::lim:
            cout<<"lim"<<endl;
            return;
        default:
            tokens.push_front(tok);
            sig=tomar();
            return;
        }
    }
}

bool normal::operar(Holder* h){
//    list<acm*>::iterator bufferRes=!buffer.empty()?--buffer.end():buffer.begin();
//    list<pair<drawable,v>>::iterator bColorRes=!bufferColores.empty()?--bufferColores.end():bufferColores.begin();
//    list<v>::iterator limitRes=!limites.empty()?--limites.end():limites.begin();
//

    ///calcular el movimiento





//    cond=true;
//    for(acm* a:acms){
//        if(a->tipo==condt){
//            if((a->func(),cond==false)){
//                cout<<" F ";
//                buffer.erase(++bufferRes,buffer.end());
//                bufferColores.erase(++bColorRes,bufferColores.end());
//                limites.erase(++limitRes,limites.end());
//                return false;
//            }
//        }else{
//            if(a->tipo==movt||a->tipo==colort)
//                a->func();
//            if(a->tipo==movt||a->tipo==acct)
//                buffer.push_back(a);
//        }
//    }
//    cout<<" V ";
//    cambios=true;
//    if(then())
//        return true;
//    //esto esta para manejar el caso de desliz normal opt, si realentiza sacarlo y prohibir ese caso
//    buffer.erase(++bufferRes,buffer.end());
//    bufferColores.erase(++bColorRes,bufferColores.end());
//    //no se limpian limites de normales terminados, aun cuando su seguida de falso.
//    //si algun movimiento raro lo necesita meter un booleano
//    return false;
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
    doDebug=true;
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
        it->sig=new click(false); //hago esto en vez de usar bools por casos de opt
        it=it->sig;
    }
    it->sig=this;

    i=-1;

    sig=keepOn();
}

void desliz::debug(){
    if(doDebug){
        doDebug=false;
        cout<<"desliz< ";
        inside->debug();
        cout<<"> ";
        if(sig) sig->debug();
    }
}

bool desliz::operar(v pos){
    i++;
    aux=pos;
    backlash=true;

    cout<<"DESLIZ ";
    inside->operar();

    cout<<"/DESLIZ ";
//    while(inside->operar()){
//        aux=pos;
//        i++;
//    }
    if(backlash){
        backlash=false;
        pos=aux;
        if(i||t){
            i=-1;
            ret=then();
            return ret;
        }
        i=-1;
        ret=false;
        return false;
    }
    return ret;
}

bool operarAislado(operador* op,bool nc=false){
    //debería guardar org tambien?
    v posRes=pos;
    list<acm*>::iterator bufferRes=!buffer.empty()?--buffer.end():buffer.begin();
    list<pair<drawable,v>>::iterator bColorRes=!bufferColores.empty()?--bufferColores.end():bufferColores.begin();
    Holder* pRes=act;

    bool ret=op->operar();

    if(ret&&!nc)
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

bool bloque::operar(v pos){
    list<v>::iterator limitRes=!limites.empty()?--limites.end():limites.begin();
    cout<<"BLOQUE ";
    operarAislado(inside);
    cout<<"\BLOQUE ";
    limites.erase(++limitRes,limites.end());
    return then();
}

void bloque::debug(){
    cout<<"<< ";
    inside->debug();
    cout<<">> ";
    if(sig) sig->debug();
}

joiner::joiner(){sig=nullptr;};
bool joiner::operar(v pos){return then();}
void joiner::debug(){cout<<"joiner ";}
opt::opt(){
    nc=exc=false;
    while(true){
        switch(tokens.front()){
        paramCase(exc);
        paramCase(nc);
        default: goto next;
        }
    }
    next:

    separator=true;
    while(separator){
        separator=false;
        ops.push_back(tomar());
    }
    sig=keepOn();


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

bool opt::operar(v pos){
    bool ret=false;

    cout<<"OPT ";
    for(operador* op:ops){
        ret=operarAislado(op,nc)||ret;
        if(exc&&ret)
            return true;
        cout<<"OR ";
    }
    cout<<"/OPT";
    return ret;
}

//#define fabOp(NOMB,FUNC) \
//NOMB::NOMB(){ \
//    sig=keepOn(); \
//} \
//void NOMB::debug(){ \
//    cout<<"NOMB "; \
//} \
//bool NOMB::operar(){ \
//    FUNC \
//    return then(); \
//}

click::click(bool keep=true){
    if(keep)
        sig=keepOn();
    else
        sig=nullptr;
}
void click::debug(){
    cout<<"click";
}
bool click::operar(v pos){
    crearClicker();
    return then();
}


struct contr_acc:public acm{
    contr_acc(){
        control_func();
    }
    virtual void func(){
        control_func();
    }
    virtual void debug(){
        cout<<"control ";
    }
    void control_func(){
        org=pos;
        act=(*tabl)(pos);
    }
};
struct contr_clean:public acm{
    v posRes;Holder* pRes;
    contr_clean(v _pos,Holder* p){
        posRes=_pos;pRes=p;
        control_func();
    }
    virtual void func(){
        control_func();
    }
    virtual void debug(){
        cout<<"control ";
    }
    void control_func(){
        org=posRes;
        act=pRes;
    }
};
contr::contr(){
    sig=keepOn();
}
void contr::debug(){
    cout<<"control ";
    sig->debug();
}
bool contr::operar(v pos){
    v orgRes=org;
    Holder* piezaRes=act;


    buffer.push_back(new contr_acc());

    bool ret=then();

    buffer.push_back(new contr_clean(orgRes,piezaRes));

    return ret;
}


operador* keepOn(){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front()){
    case lector::sep:
        separator=true;
    case lector::eol:
    case lector::lim:
        tokens.pop_front(); //por ahi rompe todo
        return nullptr;
    }
    return tomar();
}

operador* tomar(){
    if(tokens.empty()) return nullptr;
    int tok=tokens.front();tokens.pop_front();
    #define caseTomar(TOKEN) case lector::TOKEN: cout<<#TOKEN<<endl;return new TOKEN
    switch(tok){
    caseTomar(desliz);
    caseTomar(opt);
    caseTomar(bloque);
    caseTomar(click);
    caseTomar(contr);
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
    if(cambios) new Clicker(true);
    cambios=false;
}

