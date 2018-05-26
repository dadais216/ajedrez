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
        movs.push_back(tomar());
    }
    for(operador* o:movs){
        o->debug();
        cout<<endl;
    }
    piezas.push_back(this);
}

void Pieza::calcularMovimientos(v posAct)
{
    /*
    for(operador* op:movs){
        if(op->operar(posAct)&&cambios){
            new Clicker(false);
        }else{
            buffer.clear();
            bufferColores.clear();
        }
        cout<<endl<<"FIN DE MOVIMIENTO"<<endl;
        resetearValores();
    }
    */
}

void resetearValores()
{
    /*
    if(memcambios){ //no es necesario
        numeros.fill(0);
        memcambios=false;
    }
    bOutbounds=false;
    pos=org;
    limites.clear();
    */
}

int idCount=0;
vector<int> uniqueIds;
Holder::Holder(int _bando,Pieza* p,v pos_){
    bando=_bando;
    inicial=true;
    pieza=p;
    tile=tablptr->tile(pos_);
    uniqueId=idCount++;
    uniqueIds.push_back(uniqueId);
    step=0;
    for(operador* op:pieza->movs){
        movHolder* mh;
        op->generarMovHolder(mh,this);
        movs.push_back(mh);
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
    for(movHolder* mh:movs){
        if(!mh->valido) continue;
        vector<normalHolder*>* normales=new vector<normalHolder*>;
        normales->push_back(static_cast<normalHolder*>(mh));
        clickers.push_back(new Clicker(normales,this));
    }
    Clicker::drawClickers=true;
}


void Holder::generar(){
    for(movHolder* mh:movs)
        mh->generar();
}

normalHolder::normalHolder(Holder* h_,normal* org){
    h=h_;
    op=org;
    accs.reserve(org->accs.size()*sizeof(acct*));
    for(acct* a:org->accs)
        accs.push_back(a->clone());
    colors.reserve(org->colors.size()*sizeof(colort*));
    for(colort* c:org->colors)
        colors.push_back(c->clone());
}
void normalHolder::generar(){
    cout<<"GENERANDO"<<endl;
    op->operar(this,h);
}
void normalHolder::accionar(){
    for(acct* ac:accs)
        ac->func(h);
}
void normalHolder::draw(){
    for(colort* c:colors)
        c->draw();
}
void normalHolder::debug(){
    cout<<"normalHolder:\n";
    cout<<"accs:\n";
    for(acct* a:accs)
        a->debug();
    cout<<"colors:\n";
    for(colort* c:colors)
        c->debug();
    //if(sig) sig->debug();
}


