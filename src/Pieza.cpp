
int movSize;
Pieza::Pieza(int _id,int _sn){
    //recibe la lista de tokens y el tamaño de la memoria de pieza y las memorias locales
    id=_id;
    sn=_sn;
    spriteb.setTexture(image.get("piezas.png"));
    spriteb.setTextureRect(IntRect(sn*64%384,(sn*64/384)*32,32,32));
    spriteb.setScale(escala,escala);
    spriten.setTexture(image . get("piezas.png"));
    spriten.setTextureRect(IntRect(sn*64%384+32,(sn*64/384)*32,32,32));
    spriten.setScale(escala,escala);

    actualBucket=bucketPiezas;
    lastBucket=&bucketPiezas;

    spawner=false;
    kamikase=false;

//    if(id==1||id==-1)
//        spawner=true;///@hack

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
        movsTemp.push_back({op,memLocalSizeAct,movSize});
    }

    alloc(&movs,(int)movsTemp.size());
    copy(movs,movsTemp.data());

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

void crearMovHolder(operador* op,Base* base,char** place=&actualBucket->head){
  fromCast(placeBack,*place,movHolder*);
  switch(op->tipo){
    case NORMAL:
      initNormalH((normal*)op,base,place); break;
    case DESLIZ:
      initDeslizH((desliz*)op,base,place); break;
    case EXC:
      initExcH((exc*)op,base,place); break;
    case ISOL:
      initIsolH((isol*)op,base,place); break;
    case DESOPT:
      initDesoptH((desopt*)op,base,place); break;
    }
    if(op->sig){
        placeBack->sig=(movHolder*)*place;
        crearMovHolder(op->sig,base,place);
    }
    else
        placeBack->sig=nullptr;
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
    if(id==1){
        p->spawner=true;
    }

    alloc(&movs,count(p->movs)+(p->kamikase?1:0)+(p->spawner?1:0));
    int i=0;

    alloc(&memPieza,p->memPiezaSize);
    memset(memPieza.beg,0,p->memPiezaSize*sizeof(int));

    alloc(&memPiezaTrigs,p->memPiezaSize);
    for(int j=0;j<count(memPiezaTrigs);j++)
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
      allocInit(Base,base,{this,nullptr,0});
      spawnerGen* sp=alloc<spawnerGen>();
      initSpawner(sp,base);
      *movs[i++]=(movHolder*)sp;
    }
    if(pieza->kamikase){
      allocInit(Base,base,{this,nullptr,0});
      kamikaseCntrl* ka=alloc<kamikaseCntrl>();
      initKamikase(ka,base);
      *movs[i++]=(movHolder*)ka;
    }

    for(Pieza::base& b:pieza->movs){
        ensureSpace(b.size+sizeof(Base));//asegurar que un movimiento este contenido en un mismo bucket

        allocInitNC(Base,base,{this,nullptr,b.memLocalSize});

        *movs[i++]=(movHolder*)actualBucket->head;
        crearMovHolder(b.raiz,base);
    }
}
void Holder::draw()
{
    //todo el sprite debería actualizarse cada vez que se mueve en lugar de cada vez que se dibuja, pero bueno
    if(bando==1)
    {
        pieza->spriten.setPosition(tile->pos.x*escala*32,tile->pos.y*escala*32);
        window.draw(pieza->spriten);
    }
    else
    {
        pieza->spriteb.setPosition(tile->pos.x*escala*32,tile->pos.y*escala*32);
        window.draw(pieza->spriteb);
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
    window.draw(*sp);
    sp->setScale(escala,escala);
}
vector<normalHolder*> normales;
void Holder::makeCli(){
    for(movHolder* b:movs){
        if(!(b->bools&valorCadena)) continue;
        b->table->cargar(b,&normales);
        normales.clear();
    }
    Clicker::drawClickers=true;
}

void Holder::generar(){
    actualHolder.h=this;
    for(movHolder* m:movs){
        offset=tile->pos;
        memset(memMov.data(),0,m->base->memLocalSize*sizeof(int));
        m->table->generar(m);
    }
}

