#include "../include/Pieza.h"
#include "../include/global.h"
#include "../include/operador.h"
#include "../include/Clicker.h"

list<Pieza*> piezas;

Pieza::Pieza(int _id,int _sn)
{
    id=_id;
    sn=_sn;
    spriteb.setTexture(imagen->get("sprites.png"));
    spriteb.setTextureRect(IntRect(64+sn*64,0,32,32));
    spriteb.setScale(escala,escala);
    spriten.setTexture(imagen->get("sprites.png"));
    spriten.setTextureRect(IntRect(64+sn*64+32,0,32,32));
    spriten.setScale(escala,escala);

    while(!tokens.empty())
    {
//        for(int tok:tokens){
//            cout<<tok<<"-";
//        }
//        cout<<endl;
        movs.push_back(tomar());
    }
    for(operador* o:movs)
    {
        o->debug();
        cout<<endl;
    }
    piezas.push_back(this);
}

list<Clicker*> clickers;

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

Holder::Holder(int _bando,Pieza* p,v pos_)
{
    bando=_bando;
    inicial=true;
    pieza=p;
    pos=pos_;

    for(operador* op:pieza->movs)
    {
        movHolder* mh;
        op->generarMovHolder(mh,this);
        movs.push_back(mh);
    }
}
void Holder::draw()
{
    ///el sprite debería actualizarse cada vez que se mueve en lugar de cada vez que se dibuja, pero bueno
    if(bando==1)
    {
        pieza->spriten.setPosition(pos.x*escala*32,pos.y*escala*32);
        window->draw(pieza->spriten);
    }
    else
    {
        pieza->spriteb.setPosition(pos.x*escala*32,pos.y*escala*32);
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
void Holder::show(list<Clicker*>& clk){
    ///aca habria una funcion polimorfica que toma normales y le mete su lista de normales
    vector<normalHolder*>* normales=new vector<normalHolder*>;
    for(movHolder* mh:movs){
        normales->push_back(static_cast<normalHolder*>(mh));
        clk.push_back(new Clicker(normales));
    }
}


void Holder::procesar(vector<v>& pisados){  //vectores que potencialmente tocaron triggers
    for(movHolder* mh:movs)
        mh->procesar(pisados);
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
    triggs.reserve(org->conds.size()*sizeof(v));
}
void normalHolder::procesar(vector<v>& pisados){
    for(v trig:triggs)
        for(v pis:pisados)
            if(trig==pis){
                op->operar(this,h);
                return;
            }
}
void normalHolder::generar(){
    op->operar(this,h);
}
void normalHolder::accionar()
{
    for(acct* ac:accs)
        ac->func(h);
}
void normalHolder::draw()
{
    for(colort* c:colors)
        c->draw();
}



