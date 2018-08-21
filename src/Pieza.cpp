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
            n->conds.push_back(new debugInicial(v(5,5)));
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
movHolder* crearMovHolder(Holder* h,operador* op,Base* b){
    movHolder* m;
    switch(op->tipo){
    case NORMAL:
        m=new normalHolder(h,static_cast<normal*>(op));
    break;case DESLIZ:
        m=new deslizHolder(h,static_cast<desliz*>(op));
    break;
    }
    m->base=b;
    if(op->sig)
        m->sig=crearMovHolder(h,op->sig,b);
    else
        m->sig=nullptr;
}
mvBase::mvBase(Holder* h,operador* op){
    mov=crearMovHolder(h,op,this);
}
Holder::Holder(int _bando,Pieza* p,v pos_){
    bando=_bando;
    inicial=true;
    pieza=p;
    tile=tablptr->tile(pos_);
    movs.reserve(sizeof(mvBase)*pieza->movs.size());
    for(operador* op:pieza->movs){
        movs.emplace_back(this,op);
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
    for(mvBase b:movs){
        if(b.lim==BASE) continue;
        vector<normalHolder*>* normales=new vector<normalHolder*>;
        b.mov->cargar(normales);
        clickers.push_back(new Clicker(normales,this));//va a haber un flag que desactive este para desliz
        //por ahi es mejor un metodo no polimorfico a todo esto
    }
    Clicker::drawClickers=true;
}

unordered_set<Base*> basesAActualizar;
void Holder::generar(){
    for(mvBase b:movs){
        b.mov->generar();
        for(Base* ba:basesAActualizar)
            ba->reaccionar();
            ///este reaccionar puede agregar cosas al vector. Creo que no rompe nada
    }
}



void mvBase::reaccionar(){
    ///otros reaccionar actualizarian su base
    //lim==BASE es el equivalente a invalido
    lim=BASE;
    movHolder* i=mov;
    do
        if(i->valido){
            if(i->op->contGenCl)
                lim=i;
        }else{
            return;
        }
    while(i=i->sig);
    lim=NOLIM;
}

normalHolder::normalHolder(Holder* h_,normal* org){
    h=h_;
    op=org;
    valido=true;
    accs.reserve(org->accs.size()*sizeof(acct*));
    ///no es la mejor forma pero bueno
    for(acct* a:org->accs)
        accs.push_back(a->clone());
    colors.reserve(org->colors.size()*sizeof(colort*));
    for(colort* c:org->colors)
        colors.push_back(c->clone());
}
void normalHolder::generar(){
    cout<<"GENERANDO"<<endl;
    static_cast<normal*>(op)->operar(this,h);
    //se hace esto en vez de tener un op para cada holdertype para que estos se puedan recorrer y acceder a la info de op
}
void normalHolder::accionar(){
    for(acct* ac:accs)
        ac->func(h);
}
void normalHolder::cargar(vector<normalHolder*>* norms){
    norms->push_back(this);
    if(sig)
        sig->cargar(norms);
    //otros holders preguntarian si base->lim==this. Como normal nunca tiene potencial de contener clickers no hace nada
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
deslizHolder::deslizHolder(Holder* h_,desliz* org){
    h=h_;
    op=org;
    movs.reserve(10*sizeof(movHolder));//temporal, eventualmente voy a usar buckets
    movs[0]=crearMovHolder(h,static_cast<desliz*>(op)->inside,this);
    valido=true;///desliz es siempre verdadero
}
void deslizHolder::generar(){
    ///recalcular desde el principio
    ignoreRecalc=true;
}
void deslizHolder::cargar(vector<normalHolder*>* norms){

}
void deslizHolder::reaccionar(){
    if(ignoreRecalc){
        ignoreRecalc=false;
        return;
    }
    for(movHolder* mh:movs){//@optim no es necesario preguntar por el final del vector en cada interacion
        if(mh->valido&&mh!=lim)///pueden darse dos casos: que alguno en medio de la cadena se haya hecho falso, o que el ultimo actual, que siempre es falso, se haga verdadero
            continue;
        ///recalcular desde este
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

