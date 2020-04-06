
  /*
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
  */



//por el momento sigo usando buckets. Aunque se el tamaño de antes, voy llenandola progresivamente para poder reutilizar el codigo con piezas que aparezcan por spawn, y usen el otro bucket.
Holder* initHolder(Piece* p,int bando,Tile* pos,bucket* hb){
  Holder* h=alloc<Holder>(hb);

  h->bando=bando==1;
  h->piece=p;
  h->tile=pos;
  h->inPlay=true;
  
  alloc(hb,&h->movs,count(p->movs)+(p->spawner||p->kamikase?1:0));

  alloc(hb,&h->memPiece,p->memPieceSize);
  memset(h->memPiece.beg,0,p->memPieceSize*sizeof(int));

  int i=0;
  if(p->spawner||p->kamikase){
    //allocInitNC(hb,Base,base,{h,nullptr,0}); estos no ponen triggers, no necesitan acceder a una base
    if(p->spawner){
      spawnerGen* sp =alloc<spawnerGen>(hb);
      initSpawner(sp,h,h->piece->kamikase);
      *h->movs[0]=(movHolder*)sp;
      //b->size=sizeof(spawnerGen);
    }else{
      kamikaseCntrl* kc=alloc<kamikaseCntrl>(hb);
      initKamikase(kc,h);
      *h->movs[0]=(movHolder*)kc;
      //b->size=sizeof(kamikaseCntrl);
    }
    i=1;
  }

  for(pBase& pb:p->movs){
    //TODO lo de que root arranque en null y lo setee el primer movimiento se me hace raro, por que no lo marco desde aca?
    allocInitNC(hb,Base,base,{h,nullptr,pb.memLocalSize});
    *h->movs[i++]=(movHolder*)hb->head;
    crearMovHolder(pb.raiz,base,&hb->head);
  }

  //printf("real %d == declared %d\n",hb->head-(char*)h,p->hsSize);
  assert(hb->head-(char*)h==p->hsSize);
  return h;
}

void crearMovHolder(operador* op,Base* base,char** place){
  movHolder* thisMov=(movHolder*)*place;
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
    thisMov->sig=(movHolder*)*place;
    crearMovHolder(op->sig,base,place);
  }
  else
    thisMov->sig=nullptr;
}

void drawHolder(Holder* h){
    //TODO el sprite debería actualizarse cada vez que se mueve en lugar de cada vez que se dibuja, pero bueno
  Sprite* sprt=h->bando==1?&h->piece->spriten:&h->piece->spriteb;
  sprt->setPosition(h->tile->pos.x*escala*32,h->tile->pos.y*escala*32);
  window.draw(*sprt);
}
/*void Holder::draw(int n)  //pos en capturados
{
    Sprite* sp;
    if(bando==1)
        sp=&piece->spriten;
    else
        sp=&piece->spriteb;
    sp->setScale(1,1);
    sp->setPosition(515+(16*n)%112,20+(n/7)*10);
    window.draw(*sp);
    sp->setScale(escala,escala);
    }*/
vector<normalHolder*> normales;
void makeCli(Holder* h){
  for(movHolder* b:h->movs){
    if(!(b->bools&valorCadena)) continue;
    b->table->cargar(b,&normales);
    normales.size=0;
  }
  Clicker::drawClickers=true;
}

void generar(Holder* h){
    actualHolder.h=h;
    for(movHolder* m:h->movs){
        offset=h->tile->pos;
        memset(memMov.data,0,m->base->memLocalSize*sizeof(int));
        m->table->generar(m);
    }
}

