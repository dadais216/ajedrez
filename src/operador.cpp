#include "operador.h"
#include "lector.h"
#include <Clicker.h>
#include <tablero.h>
#include <Pieza.h>



color::color(RectangleShape* rs_,v pos_){
    rs=rs_;
    pos=pos_;
}
void color::draw(){
    window->draw(*rs);///no se si aca se hace una copia, haciendo todo el tema del vector de colores al pedo
}
void color::debug(){
    cout<<"color ";
}
color* color::clone(){
    return new color(rs,pos);
}
list<RectangleShape*> colores;
colort* normal::crearColor(v pos){
    ///se crea una instancia del sprite y cada colort la guarda en un puntero, se diferencia por tipo
    ///en un parametro de esta funcion, por ahora solo manejo colores

    int r=tokens.front()-1000;tokens.pop_front();
    int g=tokens.front()-1000;tokens.pop_front();
    int b=tokens.front()-1000;tokens.pop_front();



    for(RectangleShape* c:colores){
        if(c->getFillColor().r==r&&c->getFillColor().g==g&&c->getFillColor().b==b){
            return new color(c,pos);
        }
    }
    RectangleShape* rs=new RectangleShape(Vector2f(32*escala,32*escala));
    rs->setFillColor(sf::Color(r,g,b,40));
    colores.push_back(rs);
    return new color(rs,pos);
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
/*
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
*/
/*
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

*/
/*
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
*/

///parece que hay un pos global en algun lado
//usando herencia podría evitar tener 800 constructores iguales, pero de todas formas va a haber 800 clone
//y con esto puedo hacer lo de debug
#define Func(TIPO,FUNC) Func##TIPO(FUNC)
#define Funcacct(FUNC)\
    virtual void func(Holder* h){\
        FUNC\
    }
#define Funccondt(FUNC)\
    virtual bool check(Holder* h,v pos){\
        FUNC\
    }
#define Clone(TIPO,NOMB) Clone##TIPO(NOMB)
#define Cloneacct(NOMB) \
    virtual NOMB* clone(){\
        return new NOMB(pos);\
    }
#define Clonecondt(NOMB)
#define fabMov(NOMB,TIPO,FUNC)\
struct NOMB:public TIPO{\
    v pos;\
    NOMB(v pos_){\
        pos=pos_;\
        pos.show();\
    };\
    Func(TIPO,FUNC)\
    virtual void debug(){\
        cout<<#NOMB<<" "<<&pos;pos.show();\
    } \
    Clone(TIPO,NOMB)\
}\


fabMov(mov,acct,
       (*tabl)(h->ori.show(),nullptr);
       (*tabl)(h->pos.show(),h);
       h->ori=h->pos;
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
        return (*tabl)(pos)==nullptr;
);
fabMov(pieza,condt,
       return (*tabl)(pos);
);
fabMov(enemigo,condt,
        if((*tabl)(pos))
            return (*tabl)(pos)->bando==h->bando*-1;
        return false;
);
///list<v> limites;
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tabl->tam.x&&pos.y>=0&&pos.y<tabl->tam.y){
            h->outbounds=false;
            /*
            for(v vec:limites){ //si realentiza mover a otra cond
                if(pos==vec){
                    return false;
                }
            }
            */
            ///mirar el tema de los limites, ahora tienen que ser locales
            ///probablemente esten en holder y listo
            return true;
        }else{
            h->outbounds=true;
            return false;
        }
);
/*
fabMov(prob,condt,
        limites.emplace_back(pos);
);
*/
fabMov(outbounds,condt,
        return h->outbounds;
);
fabMov(inicial,condt,
        return h->inicial;
);
/*
fabMov(ori,movt,
        pos=org;
);
*/
bool separator;
///mirar este tema, por ahi se puede hacer una variable de normal y listo
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

//        cond(posRemember);
//        cond(numSet);
//        cond(numAdd);
//        cond(numSeti);
//        cond(numAddi);
//        cond(numCmp);
//        cond(numDst);
//        cond(numCmpi);
//        cond(numDsti);
//        cond(numLess);
//        cond(numLessi);

        cond(esp);
        cond(outbounds);
        cond(vacio);
        cond(pieza);
        cond(enemigo);
//        cond(prob);
        cond(inicial);

        #define acc(TOKEN) caseT(accs,TOKEN)

        acc(mov);
        acc(capt);
        acc(pausa);
//        acc(spwn);
        acc(del);

        case lector::color:
        colors.push_back(crearColor(pos));cout<<"color\n";break;
 //       colorr(sprt);
 //       colorr(numShow);

        #undef acc(TOKEN)
        #undef cond(TOKEN)
        #undef colorr(TOKEN)
        #undef caseT(TIPO,TOKEN)

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

void normal::generarMovHolder(movHolder*& mh,Holder* h){
    mh=new normalHolder(h,this);//podría pasarle el vector de accs si es lo unico que necesita
    if(sig)
        sig->generarMovHolder(mh->sig,h);
    else
        mh->sig=nullptr;
}

bool normal::operar(movHolder* mh,Holder* h){
//    list<acm*>::iterator bufferRes=!buffer.empty()?--buffer.end():buffer.begin();
//    list<pair<drawable,v>>::iterator bColorRes=!bufferColores.empty()?--bufferColores.end():bufferColores.begin();
//    list<v>::iterator limitRes=!limites.empty()?--limites.end():limites.begin();
//

    ///se determino que se necesita calcular este movimiento, se lo llama desde holder pasandose a si mismo como
    ///parametro, un elemento de holder indica el espacio de memoria del movimiento actual

    normalHolder* nh=static_cast<normalHolder*>(mh);

    nh->triggs.clear(); //no libera memoria
    ///habria que distinguir a los cond que no son posicionales, creo que son los de memoria nomas
    for(condt* c:conds){
        v posAct=c->pos+h->pos;
        nh->triggs.push_back(posAct);

        cout<<&c->pos;
        c->pos.show();
        c->debug();

        if(!c->check(h,posAct)){
            //h->valido=false;
            return false;
        }
    }
    //h->valido=true;
    //accs en holder ya esta generado

    for(int i=0;i<accs.size();i++){
        nh->accs[i]->pos.show();cout<<" == ";
        nh->accs[i]->pos=accs[i]->pos+h->pos.show();
    }
        //solo se actualiza la pos porque la accion (y sus parametros si tiene) no varian
    for(int i=0;i<colors.size();i++)
        nh->colors[i]->pos=colors[i]->pos+h->pos;
    return true;
}

void normal::debug(){
    /*
    for(acm* a:acms){
        a->debug();
    }
    if(sig) sig->debug();
    */
}

/*

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
*/

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
//    caseTomar(desliz);
//    caseTomar(opt);
//    caseTomar(bloque);
//    caseTomar(click);
//    caseTomar(contr);
    default:
        tokens.push_front(tok);
        return new normal;
    }
}

bool operador::then(){
    if(!sig)
        return true;
    return false;
    ///no se si la seguidilla se va a hacer desde aca, no creo porque no tengo acceso a el movHolder, aunque podría conseguirlo
    //return sig->operar();
}

void crearClicker(){
//    if(cambios) new Clicker(true);
//    cambios=false;
}

