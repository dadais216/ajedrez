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
    spriteb.setTexture(imagen->get("piezas.png"));
    spriteb.setTextureRect(IntRect(sn*64%384,(sn*64/384)*32,32,32));
    spriteb.setScale(escala,escala);
    spriten.setTexture(imagen->get("piezas.png"));
    spriten.setTextureRect(IntRect(sn*64%384+32,(sn*64/384)*32,32,32));
    spriten.setScale(escala,escala);

    bandoAct=sgn(_id);
    actualBucket=bucketPiezas;
    lastBucket=&bucketPiezas;

    spawner=false;
    kamikase=false;

    if(id==1||id==-1)
        spawner=true;///@hack

    vector<Pieza::base> movsTemp;
    int i=0;
    while(!tokens.empty()){
//        for(int tok:tokens){
//            cout<<tok<<"-";
//        }
//        cout<<endl;

        clickExplicit=false;
        movSize=0;
        memLocalSizeAct=lect.memLocalSizes[i++];
        operador* op=tomar();
        assert(movSize<bucketSize);
        /*
        if(debugMode){
            normal* n=bucketAdd<normal>(false);
            n->conds.init(1,bucketAdd<debugInicial>());
            n->sig=op;
            movsTemp.push_back({n,memLocalSizeAct});
        }else*/
        movsTemp.push_back({op,memLocalSizeAct,movSize});
    }
    movs.init(movsTemp.size(),movsTemp.data());

    memPiezaSize=lect.memPiezaSize;

    function<void(operador*)> showOp=[&showOp](operador* op)->void{
        switch(op->tipo){
        case NORMAL:
            cout<<"NORMAL";
            if(((normal*)op)->bools&doEsp)
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

void crearMovHolder(char** place,operador* op,Base* base){
    char* placeBack=*place;
    switch(op->tipo){
    case NORMAL:
        *place+=sizeof(normalHolder);
        new(placeBack)normalHolder(static_cast<normal*>(op),base,place);
        break;
    case DESLIZ:
        *place+=sizeof(deslizHolder);
        new(placeBack)deslizHolder(((desliz*)op),base,place);
        break;
    case EXC:
        *place+=sizeof(excHolder);
        new(placeBack)excHolder(static_cast<exc*>(op),base,place);
        break;
    case ISOL:
        *place+=sizeof(isolHolder);
        new(placeBack)isolHolder(static_cast<isol*>(op),base,place);
        break;
    case DESOPT:
        *place+=sizeof(desoptHolder);
        new(placeBack)desoptHolder(static_cast<desopt*>(op),base,place);
    }
    if(op->sig){
        ((movHolder*)placeBack)->sig=(movHolder*)*place;
        crearMovHolder(place,op->sig,base);
    }
    else
        ((movHolder*)placeBack)->sig=nullptr;
}



Holder::Holder(int _bando,Pieza* p,v pos_){
    bando=_bando;
    id=p->id;
    pieza=p;
    tile=tablptr->tile(pos_);
    inPlay=true;

    actualBucket=bucketHolders;
    lastBucket=&bucketHolders;

    ///@todo sacar temporal
    movs.reserve(p->movs.count()+(p->kamikase?1:0)+(p->spawner?1:0));
    int i=0;

    memPieza.reserve(p->memPiezaSize);
    memset(memPieza.begptr,0,p->memPiezaSize*sizeof(int));

    memPiezaTrigs.reserve(p->memPiezaSize);
    for(int j=0;j<memPiezaTrigs.count();j++)
        new(memPiezaTrigs[j])memTriggers();
    //por ahora decidi hacer que los triggers usen memoria dinamica, porque es dificil determinar cuantos van a ser
    //se podria pedir al usuario que marque eso, pero es una idea muy compleja para dejarla al aire,
    //causando crashes si no se toca. Como default se podria inferir que es la cantidad de others que se usen,
    //pero no puedo saber eso en esta etapa porque no proceso todos los operadores antes de arrancar con los holders.
    //y esto romperia con triggers dinamicos y spawns.
    //Supongo que si cambio mi opinion sobre esto va a ser recien en la version compilada, donde si tengo ganas
    //podria hacer todo un sistema para inferir cuales son los triggers potencialmente usados en cualquier tipo de
    //memoria. De todas formas no es algo muy importante, causaria un solo fallo de cache, y al menos en caso de
    //pieza, nomas se usa cuando hay others y son bastante niche

    if(pieza->spawner){
        new(actualBucket->head)Base({this,nullptr,0});
        Base* base=(Base*)actualBucket->head;
        actualBucket->head+=sizeof(Base);
        *movs[i++]=(movHolder*)actualBucket->head;
        new(actualBucket->head)spawnerGen(base);
        actualBucket->head+=sizeof(spawnerGen);
    }
    if(pieza->kamikase){
        new(actualBucket->head)Base({this,nullptr,0});
        Base* base=(Base*)actualBucket->head;
        actualBucket->head+=sizeof(Base);
        *movs[i++]=(movHolder*)actualBucket->head;
        new(actualBucket->head)kamikaseCntrl(base);
        actualBucket->head+=sizeof(kamikaseCntrl);
    }

    for(Pieza::base& b:pieza->movs){
        actualBucket->enoughSize(b.size+sizeof(Base));//asegurar que un movimiento este contenido en un mismo bucket

        new(actualBucket->head)Base({this,nullptr,b.memLocalSize});
        Base* base=(Base*)actualBucket->head;
        actualBucket->head+=sizeof(Base);

        *movs[i++]=(movHolder*)actualBucket->head;
        crearMovHolder(&actualBucket->head,b.raiz,base);
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
vector<normalHolder*> normales;
void Holder::makeCli(){
    for(movHolder* b:movs){
        if(!(b->bools&valorCadena)) continue;
        b->cargar(&normales);
        normales.clear();
    }
    Clicker::drawClickers=true;
}

void Holder::generar(){
    for(movHolder* m:movs){
        offset=tile->pos;
        memset(memMov.data(),0,m->base->memLocalSize*sizeof(int));
        m->generar();
    }
}

