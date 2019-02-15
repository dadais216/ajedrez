#include "../include/global.h"
#include "../include/Pieza.h"
#include "../include/Clicker.h"
#include "../include/operador.h"

vector<Pieza*> piezas;

int movSize;
int bandoAct;
Pieza::Pieza(int _id,int _sn){
    //recibe la lista de tokens y el tamaño de la memoria de pieza y las memorias locales
    id=_id;//signo indica bando
    sn=_sn;
    spriteb.setTexture(imagen->get("sprites.png"));
    spriteb.setTextureRect(IntRect(64+sn*64,0,32,32));
    spriteb.setScale(escala,escala);
    spriten.setTexture(imagen->get("sprites.png"));
    spriten.setTextureRect(IntRect(64+sn*64+32,0,32,32));
    spriten.setScale(escala,escala);

    bandoAct=sgn(_id);
    actualBucket=bucketPiezas;
    int i=0;
    while(!tokens.empty()){
//        for(int tok:tokens){
//            cout<<tok<<"-";
//        }
//        cout<<endl;

        clickExplicit=false;
        movSize=0;
        operador* op=tomar();
        int memLocalSize=lect.memLocalSizes[i++];
        if(debugMode){
            normal* n=bucketAdd<normal>(false);
            n->conds.init(1,bucketAdd<debugInicial>());
            n->sig=op;
            movs.push_back(Pieza::base{n,memLocalSize});
        }else
            movs.push_back(Pieza::base{op,memLocalSize});
    }
    memPiezaSize=lect.memPiezaSize;

    function<void(operador*)> showOp=[&showOp](operador* op)->void{
        switch(op->tipo){
        case NORMAL:
            cout<<"NORMAL";
            if(((normal*)op)->doEsp)
                cout<<"e";
            cout<<" ";
            break;
        case DESLIZ:
            cout<<"DESLIZ (";
            showOp(static_cast<desliz*>(op)->inside);
            cout<<") ";
            break;
        case EXC:
            cout<<"EXC (";
            for(operador* opi:static_cast<exc*>(op)->ops){
                showOp(opi);
                cout<<" |";
            }
            cout<<"x)";
            break;
        case ISOL:
            cout<<"ISOL (";
            showOp(static_cast<isol*>(op)->inside);
            cout<<") ";
            break;
        case DESOPT:
            cout<<"DESOPT (";
            for(operador* opi:static_cast<desopt*>(op)->ops){
                showOp(opi);
                cout<<" |";
            }
            cout<<"x) ";
        }
        if(op->sig)
            showOp(op->sig);
    };
    cout<<endl;
    for(Pieza::base& b:movs){
        showOp(b.raiz);
        cout<<endl;
    }

    piezas.push_back(this);
}
movHolder* crearMovHolder(Holder* h,operador* op,Base* base){
    movHolder* m;
    switch(op->tipo){
    case NORMAL:
        m=bucketAdd<normalHolder>(h,static_cast<normal*>(op),base);break;
    case DESLIZ:
        m=bucketAdd<deslizHolder>(h,static_cast<desliz*>(op),base);break;
    case EXC:
        m=bucketAdd<excHolder>(h,static_cast<exc*>(op),base);break;
    case ISOL:
        m=bucketAdd<isolHolder>(h,static_cast<isol*>(op),base);break;
    case DESOPT:
        m=bucketAdd<desoptHolder>(h,static_cast<desopt*>(op),base);
    }
    if(op->sig)
        m->sig=crearMovHolder(h,op->sig,base);
    else
        m->sig=nullptr;
    return m;
}
Holder::Holder(int _bando,Pieza* p,v pos_){
    bando=_bando;
    id=p->id;
    pieza=p;
    tile=tablptr->tile(pos_);

    actualBucket=bucketHolders;

    movs.reserve(sizeof(movHolder*)*pieza->movs.size());
    memPieza.resize(p->memPiezaSize);
    memPiezaTrigs.resize(p->memPiezaSize);
    for(Pieza::base& b:pieza->movs){
        Base* base=new Base;
        base->beg=nullptr;
        base->memLocalSize=b.memLocalSize;
        movs.push_back(crearMovHolder(this,b.raiz,base));
        delete base;
    }
    inPlay=true;
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
vector<normalHolder*> normales;
void Holder::makeCli(){
    for(movHolder* b:movs){
        if(!b->valorCadena) continue;
        b->cargar(&normales);
        normales.clear();
    }
    Clicker::drawClickers=true;
}

void Holder::generar(){
    for(movHolder* m:movs){
        offset=tile->pos;
        ///@optim ver si resulta comodo para el lenguaje hacer que la memoria de movimiento arranque en 0
        memset(memMov.data(),0,m->base.memLocalSize*sizeof(int));
        m->generar();
    }
}

