#include "../include/Pieza.h"
#include "../include/global.h"
#include "../include/operador.h"
#include "../include/Clicker.h"

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

        clickExplicit=false;
        operador* op=tomar();
        if(debugMode){
            normal* n=new normal(false);
            n->conds.push_back(new debugInicial(v(0,0)));
            n->accs.push_back(new pass(v(-9000,-9000))); ///placeholder para evitar crashes en casos raros donde queda un clicker solo con esta normal (antes de un desliz que no genera nada). Le pongo un numero alto para que el clicker no aparezca en el tablero
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
        m=new normalHolder(h,static_cast<normal*>(op),base);break;
    case DESLIZ:
        m=new deslizHolder(h,static_cast<desliz*>(op),base);break;
    case EXC:
        m=new excHolder(h,static_cast<exc*>(op),base);
    }
    ///m->makeClick=op->makeClick; @??? este codigo dejaba m->makeClick sin setear y no entendi por que. Lo movi a los constructores y anda
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
    }
    Clicker::drawClickers=true;
}

void Holder::generar(){
    for(movHolder* m:movs){
        offset=tile->pos;
        m->generar();
    }
}
movHolder::movHolder(Holder* h_,operador* org,Base* base_){
    if(!base_->beg)
        base_->beg=this;
    base=*base_;
    h=h_;
    op=org;
    makeClick=op->makeClick;
}
void movHolder::generarSig(){
    if(sig){
        sig->generar();
        if(!makeClick&&!sig->continuar)
            continuar=false;
    }
}
normalHolder::normalHolder(Holder* h_,normal* org,Base* base_)
:movHolder(h_,org,base_){
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
    offsetAct=offset;///se setea el offset con el que arrancó la normal para tenerlo cuando se recalcula. Cuando se recalcula se setea devuelta al pedo, pero bueno. No justifica hacer una funcion aparte para el recalculo
    v posAct;
    ///habria que distinguir a los cond que no son posicionales, creo que son los de memoria nomas
    for(condt* c:n->conds){
        posAct=c->pos+offset;
        ///no definitivo. lo de addTrigger esta para evitar que esp tire triggers, no sé si esp es algo final o se va
        ///a sacar. Podría volver a ponerse la idea de que todos los conds tiren triggers, depende como implemente memoria
        addTrigger=false;
        cout<<c->nomb<<posAct;
        if(!c->check(h,posAct)){
            continuar=valido=false;

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

    continuar=valido=true;
    generarSig();
}
void normalHolder::reaccionar(normalHolder* nh){
    if(nh==this){
        offset=nh->offsetAct;
        switchToGen=true;
        generar();
    }else if(valido&&sig){
        sig->reaccionar(nh);
        continuar=sig->continuar;
    }
}
void normalHolder::accionar(){
    for(acct* ac:accs)
        ac->func(h);
}
void normalHolder::cargar(vector<normalHolder*>* norms){
    if(!continuar) return;
    norms->push_back(this);
    //cout<<"#"<<makeClick<<"  ";
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
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
deslizHolder::deslizHolder(Holder* h_,desliz* org,Base* base_)
:movHolder(h_,org,base_){
    movs.reserve(10*sizeof(movHolder));///@todo @optim temporal, eventualmente voy a usar buckets
    movs.push_back(crearMovHolder(h,org->inside,&base));
    valido=true;///desliz es siempre valido
}
void deslizHolder::generar(){
    continuar=true;
    for(int i=0;;){
        movHolder* act=movs[i];
        act->generar();
        if(!act->continuar){
            f=i;
            break;
        }
        i++;
        if(movs.size()==i)
            movs.push_back(crearMovHolder(h,static_cast<desliz*>(op)->inside,&base));
    }
    generarSig();
}
bool switchToGen;
void deslizHolder::reaccionar(normalHolder* nh){
    for(int i=0;i<=f;i++){
        movHolder* act=movs[i];
        act->reaccionar(nh);
        if(switchToGen){
            if(act->continuar){
                i++;
                if(movs.size()==i)
                    movs.push_back(crearMovHolder(h,static_cast<desliz*>(op)->inside,&base));
                for(;;){
                    act=movs[i];
                    act->generar();
                    if(!act->continuar){
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
            else f=i;

            return;
        }
    }
    if(sig)
        sig->reaccionar(nh);
}
void deslizHolder::cargar(vector<normalHolder*>* norms){
    if(!continuar) return;
    for(int i=0;i<f;i++)
        movs[i]->cargar(norms);
    if(makeClick&&!norms->empty()) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
        clickers.push_back(new Clicker(norms,h));
    cout<<endl;
    if(sig)
        sig->cargar(norms);
}
void deslizHolder::debug(){
    for(movHolder* m:movs)
        if(m->valido)
            m->debug();
}
excHolder::excHolder(Holder* h_,exc* org,Base* base_)
:movHolder(h_,org,base_){
    ops.reserve(10*sizeof(movHolder));///@todo @optim temporal, eventualmente voy a usar buckets

    for(operador* opos:org->ops)
        ops.push_back(crearMovHolder(h,opos,&base));
}
void excHolder::generar(){
    for(movHolder* mh:ops){
        mh->generar();
        if(mh->continuar){
            continuar=valido=true;
            actualBranch=mh; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que continuar==true
            generarSig();
            return;
        }
    }
    continuar=valido=false;
    actualBranch=nullptr; ///@optim creo que en ningun caso se pregunta por esto, no vale la pena nulificarlo
}
void excHolder::reaccionar(normalHolder* nh){
    //actualBranch es valido porque se esta respondiendo a un trigger puesto por el
    actualBranch->reaccionar(nh);
    if(switchToGen){
        if(!actualBranch->continuar){
            ///si el ab al recalcularse se invalida generar todo devuelta, saltandolo
            for(movHolder* mh:ops){
                if(mh!=actualBranch){
                    mh->generar();
                    if(mh->continuar){
                        continuar=valido=true;
                        generarSig();
                        return;
                    }
                }
            }
            continuar=valido=false;
        }
    }else if(valido&&sig)
        sig->reaccionar(nh);
}
void excHolder::cargar(vector<normalHolder*>* norms){
    if(!continuar) return;
    actualBranch->cargar(norms);
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
void excHolder::debug(){

}
