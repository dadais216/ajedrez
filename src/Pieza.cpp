#include "../include/Pieza.h"
#include "../include/global.h"
#include "../include/operador.h"
#include "../include/Clicker.h"

#define BASE nullptr
#define NOLIM (movHolder*)1
vector<Pieza*> piezas;

Pieza::Pieza(int _id,int _sn){
    id=_id;
    sn=_sn;
    spriteb.setTexture(imagen->get("sprites.png"));
    spriteb.setTextureRect(IntRect(64+sn*64,0,32,32));
    spriteb.setScale(escala,escala);
    spriten.setTexture(imagen->get("sprites.png"));
    spriten.setTextureRect(IntRect(64+sn*64+32,0,32,32));
    spriten.setScale(escala,escala);

    while(!tokens.empty()){
//        for(int tok:tokens){
//            cout<<tok<<"-";
//        }
//        cout<<endl;


        operador* op=tomar();
        if(debugMode){
            normal* n=new normal(false);
            n->conds.push_back(new debugInicial(v(0,0)));
            n->sig=op;
            movs.push_back(n);
        }else
            movs.push_back(op);

    }
    //for(operador* o:movs){
    //    o->debugMovs();

    //    cout<<endl;
    //}
    piezas.push_back(this);
}
movHolder* crearMovHolder(Holder* h,operador* op,Base* base){
    movHolder* m;
    switch(op->tipo){
    case NORMAL:
        m=new normalHolder(h,static_cast<normal*>(op),base);
    break;case DESLIZ:
        m=new deslizHolder(h,static_cast<desliz*>(op),base);
    break;
    }
    if(op->sig)
        m->sig=crearMovHolder(h,op->sig,base);
    else
        m->sig=nullptr;
    return m;
}
/*
Base::Base(Holder* h,operador* op){
    mov=crearMovHolder(h,op,this);
    lim=NOLIM;///asi no hay que hacer nada en caso de desliz por ej, que no llama a esta base nunca
}
*/
Holder::Holder(int _bando,Pieza* p,v pos_){
    bando=_bando;
    inicial=true;
    pieza=p;
    tile=tablptr->tile(pos_);
    movs.reserve(sizeof(movHolder*)*pieza->movs.size());
    for(operador* op:pieza->movs){
        Base* base=new Base;
        base->beg=nullptr;
        movs.push_back(crearMovHolder(this,op,base));
        delete base;
    }
}
Holder::~Holder(){
    uniqueIds.erase(find(uniqueIds.begin(),uniqueIds.end(),uniqueId));
}
void Holder::draw()
{
    //todo el sprite debería actualizarse cada vez que se mueve en lugar de cada vez que se dibuja, pero bueno
    if(bando==1)
    {
        pieza->spriten.setPosition(tile->pos.x*escala*32,tile->pos.y*escala*32);
        window->draw(pieza->spriten);
    }
    else
    {
        pieza->spriteb.setPosition(tile->pos.x*escala*32,tile->pos.y*escala*32);
        window->draw(pieza->spriteb);
    }
}
void Holder::draw(int n)  //pos en capturados
{
    Sprite* sp;
    if(bando==1)
        sp=&pieza->spriten;
    else
        sp=&pieza->spriteb;
    sp->setScale(1,1);
    sp->setPosition(515+(16*n)%112,20+(n/7)*10);
    window->draw(*sp);
    sp->setScale(escala,escala);
}
void Holder::makeCli(){
    ///aca habria una funcion polimorfica que toma normales y le mete su lista de normales
    for(movHolder* b:movs){
        if(!b->continuar) continue;
        vector<normalHolder*>* normales=new vector<normalHolder*>;
        b->cargar(normales);
        clickers.push_back(new Clicker(normales,this));//va a haber un flag que desactive este para desliz
    }
    Clicker::drawClickers=true;
}

void Holder::generar(){
    for(movHolder* m:movs){
        offset=tile->pos;
        m->generar();
    }
}
/*
bool ended;
void Base::reaccionar(normalHolder* nh){
    ended=false;
    //lim==BASE es el equivalente a invalido
    lim=BASE;
    movHolder* i=mov;
    do{
        if(i==nh){
            nh->generar();
            while(i->sig&&i->valido&&!i->sig->valido){
                i=i->sig;
                i->generar();
            }
            return;
        }else{
            i->reaccionar(nh);///@optim en caso de normal se llama a una funcion vacia. Por ahi es mas rapido ignorar normales con un bool, no sé
            if(ended)
                return;
        }
        if(i->valido){
            if(i->op->contGenCl)
                lim=i;
        }else{
            return;
        }
    }while(i=i->sig);
    lim=NOLIM;
}
void Base::generar(){
    movHolder* i=mov;
    lim=BASE;
    do{
        i->generar();
        if(i->valido){
            if(i->op->contGenCl)
                lim=i;
        }else
            return;
    }while(i=i->sig);
    lim=NOLIM;
}
*/
normalHolder::normalHolder(Holder* h_,normal* org,Base* base_){
    if(!base_->beg)
        base_->beg=this;
    base=*base_;
    h=h_;
    op=org;
    //valido=true;
    accs.reserve(org->accs.size()*sizeof(acct*));
    ///no es la mejor forma pero bueno
    for(acct* a:org->accs)
        accs.push_back(a->clone());
    colors.reserve(org->colors.size()*sizeof(colort*));
    for(colort* c:org->colors)
        colors.push_back(c->clone());
}
v offset;
void normalHolder::generar(){
    cout<<"GENERANDO"<<endl;
    normal* n=static_cast<normal*>(op);
    v posAct;
    ///habria que distinguir a los cond que no son posicionales, creo que son los de memoria nomas
    for(condt* c:n->conds){
        posAct=c->pos+offset;
        ///no definitivo. lo de addTrigger esta para evitar que esp tire triggers, no sé si esp es algo final o se va
        ///a sacar. Podría volver a ponerse la idea de que todos los conds tiren triggers, depende como implemente memoria
        addTrigger=false;
        if(!c->check(h,posAct)){
            valido=false;
            continuar=false;

            if(h->outbounds) return;
            if(addTrigger) tablptr->tile(posAct)->triggers.push_back({h->tile,this,h->tile->step});
            return;
        }
        if(addTrigger) tablptr->tile(posAct)->triggers.push_back({h->tile,this,h->tile->step});
    }
    //accs en holder ya esta generado
    //solo se actualiza la pos porque la accion (y sus parametros si tiene) no varian
    for(int i=0; i<accs.size(); i++){
        //cout<<n->accs[i]->pos<<" + "<<h->tile->pos<<" = ";
        accs[i]->pos=n->accs[i]->pos+offset;///podria mandar el tile en vez de la pos, pero como no todas las acciones lo usan mientras menos procesado se haga antes mejor
        //cout<<accs[i]->pos<<endl;
    }
    for(int i=0; i<colors.size(); i++)
        colors[i]->pos=n->colors[i]->pos+offset;

    offset=posAct;

    valido=true;
    continuar=true;
    if(sig){
        sig->generar();
        if(!sig->continuar)
            continuar=false;
    }
}
void normalHolder::reaccionar(normalHolder* nh){
    if(nh==this)
        generar();
    else if(sig){
        sig->reaccionar(nh);
        continuar=valido&&sig->continuar;
    }
}
void normalHolder::accionar(){
    for(acct* ac:accs)
        ac->func(h);
}
void normalHolder::cargar(vector<normalHolder*>* norms){
    norms->push_back(this);
    if(sig)
        sig->cargar(norms);
}

void normalHolder::draw(){
    for(colort* c:colors)
        c->draw();
}
void normalHolder::debug(){
    cout<<"normalHolder:\n";
    //cout<<"base: "<<base<<endl;
    if(!valido){
        cout<<"invalido\n";
        return;
    }
    cout<<"accs:\n";
    for(acct* a:accs)
        a->debug();
    cout<<"colors:\n";
    for(colort* c:colors)
        c->debug();
    if(sig){
        cout<<endl;
        sig->debug();
    }
}
deslizHolder::deslizHolder(Holder* h_,desliz* org,Base* base_){
    if(!base_->beg)
        base_->beg=this;
    base=*base_;
    h=h_;
    op=org;
    movs.reserve(10*sizeof(movHolder));///@todo @optim temporal, eventualmente voy a usar buckets
    movs.push_back(crearMovHolder(h,static_cast<desliz*>(op)->inside,&base));
    valido=true;///desliz es siempre valido
    continuar=true;
}
void deslizHolder::generar(){
    for(int i=0;;){
        movHolder* act=movs[i];
        act->generar();
        if(!act->valido){
            f=i;
            break;
        }
        i++;
        if(movs.size()==i)
            movs.push_back(crearMovHolder(h,static_cast<desliz*>(op)->inside,&base));
    }
    if(sig)
        sig->generar();
}
void deslizHolder::reaccionar(normalHolder* nh){
    /*
    for(int i=0;i<=f;i++)
        if(movs[i]==nh){
            ended=true;
            nh->generar();
            if(nh->valido){
                for(int j=i;;){
                    movHolder* act=movs[j];
                    act->generar();
                    if(!act->valido){
                        f=j;
                        break;
                    }
                    j++;
                    if(movs.size()==j)
                        movs.push_back(crearMovHolder(h,static_cast<desliz*>(op)->inside,&deslizBase));
                }
            }else
                f=i;
            if(sig){///acomodar cadena despues del if, si hay
                movHolder* j=sig;
                do{
                    j->generar();
                    if(j->valido){
                        if(j->op->contGenCl)
                            base->lim=j;
                        }else
                            return;
                }while(j=j->sig);
                base->lim=NOLIM;
            }

        }else{
            movs[i]->reaccionar();
            if(!movs[i]->valido)
                return;
        }
        */
}
void deslizHolder::cargar(vector<normalHolder*>* norms){
    for(int i=0;i<f;i++){
        movs[i]->cargar(norms);
    }
}
void deslizHolder::debug(){
    for(movHolder* m:movs)
        if(m->valido)
            m->debug();
}
void deslizHolder::draw(){
    for(movHolder* m:movs)
        if(m->valido)
            m->draw();
}
