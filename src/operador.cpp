#include "../include/operador.h"
#include "../include/lector.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Pieza.h"
#include "movs.cpp"


bool separator;
///mirar este tema, por ahi se puede hacer una variable de normal y listo
normal::normal()
{
    v pos(0,0);
    sig=nullptr;
    while(true)
    {
        if(tokens.empty()) return;
        int tok=tokens.front();
        tokens.pop_front();
        switch(tok)
        {
        case lector::W:
            pos.y--; //el espejado se va a tener que hacer cuando se construyan las absolutas
            break;
        case lector::S:
            pos.y++;
            break;
        case lector::D:
            pos.x++;
            break;
        case lector::A:
            pos.x--;
            break;
            //cout<<#TOKEN<<endl;
#define caseT(TIPO,TOKEN)  case lector::TOKEN: TIPO.push_back(new TOKEN(pos));break
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

            cond(vacio);
            cond(esp);
            cond(outbounds);
            cond(pieza);
            cond(enemigo);
//        cond(prob);
            cond(inicial);

#define acc(TOKEN) caseT(accs,TOKEN)

            acc(mov);
            acc(capt);
            acc(pausa);
//        acc(spwn);

        case lector::color:
            colors.push_back(crearColor(pos));
            break;
//       colorr(sprt);
//       colorr(numShow);

#undef acc //(TOKEN)
#undef cond //(TOKEN)
#undef colorr //(TOKEN)
#undef caseT //(TIPO,TOKEN)

        case lector::sep:
            //cout<<"sep"<<endl;
            separator=true;
            return;
        case lector::eol:
            //cout<<"eol"<<endl;
            return;
        case lector::lim:
            //cout<<"lim"<<endl;
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

void normal::operar(movHolder* mh,Holder* h){
//    list<acm*>::iterator bufferRes=!buffer.empty()?--buffer.end():buffer.begin();
//    list<pair<drawable,v>>::iterator bColorRes=!bufferColores.empty()?--bufferColores.end():bufferColores.begin();
//    list<v>::iterator limitRes=!limites.empty()?--limites.end():limites.begin();
//

    ///se determino que se necesita calcular este movimiento, se lo llama desde un holder pasandose a si mismo

    normalHolder* nh=static_cast<normalHolder*>(mh);

    ///habria que distinguir a los cond que no son posicionales, creo que son los de memoria nomas
    for(condt* c:conds){
        v posAct=c->pos+h->tile->pos;

        //todo podría hacerse que cuando el mov es falso se tenga en cuenta solo
        //el trigger final, el que hizo falso al mov. Creo que funcionaria para todos
        //los casos. Habria que meter el concepto de normalholders verdaderas y falsas, no se si lo valga

        ///no definitivo. lo de addTrigger esta para evitar que esp tire triggers, no sé si esp es algo final o se va
        ///a sacar. Podría volver a ponerse la idea de que todos los conds tiren triggers, depende como implemente memoria
        addTrigger=false;
        if(!c->check(h,posAct)){
            nh->valido=false;
            if(h->outbounds) return;
            if(addTrigger) tablptr->tile(posAct)->triggers.push_back({h->tile,mh,h->tile->step});
            return;
        }
        if(addTrigger) tablptr->tile(posAct)->triggers.push_back({h->tile,mh,h->tile->step});
    }
    nh->valido=true;
    //accs en holder ya esta generado
    //solo se actualiza la pos porque la accion (y sus parametros si tiene) no varian
    for(int i=0; i<accs.size(); i++)
        nh->accs[i]->pos=accs[i]->pos+h->tile->pos;///podria mandar el tile en vez de la pos, pero como no todas las acciones lo usan mientras menos procesado se haga antes mejor
    for(int i=0; i<colors.size(); i++)
        nh->colors[i]->pos=colors[i]->pos+h->tile->pos;
}

void normal::debug(){
    cout<<"normal: conds:\n";
    for(condt* c:conds)
        c->debug();
    cout<<"accs:\n";
    for(acct* a:accs)
        a->debug();
    cout<<"colors:\n";
    for(colort* c:colors)
        c->debug();
    cout<<endl;
    if(sig) sig->debug();
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

operador* keepOn()
{
    if(tokens.empty())
        return nullptr;
    switch(tokens.front())
    {
    case lector::sep:
        separator=true;
    case lector::eol:
    case lector::lim:
        tokens.pop_front(); //por ahi rompe todo
        return nullptr;
    }
    return tomar();
}

operador* tomar()
{
    if(tokens.empty()) return nullptr;
    int tok=tokens.front();
    tokens.pop_front();
#define caseTomar(TOKEN) case lector::TOKEN: cout<<#TOKEN<<endl;return new TOKEN
    switch(tok)
    {
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

bool operador::then()
{
    if(!sig)
        return true;
    return false;
    ///no se si la seguidilla se va a hacer desde aca, no creo porque no tengo acceso a el movHolder, aunque podría conseguirlo
    //return sig->operar();
}

void crearClicker()
{
//    if(cambios) new Clicker(true);
//    cambios=false;
}

