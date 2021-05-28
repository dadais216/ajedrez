
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



int initHolder(Piece* p,int bando,int posInd,bigVector* hb){
  reserve(hb,p->hsSize);
  Holder* h=allocNC<Holder>(hb);

  h->bando=bando==1;
  h->piece=indOpVector(p);
  h->tile=posInd;
  h->inPlay=true;
  
  allocNC(hb,&h->movs,elems(p->movs)/*+(p->spawner||p->kamikase?1:0)*/);

  allocNC(hb,&h->memPiece,p->memPieceSize);
  memset(&actualHolder.gameState->data[h->memPiece.beg],0,p->memPieceSize*sizeof(int));

  /*
  int i=0;
  if(p->spawner||p->kamikase){
    allocInitNC(hb,Base,base,{h,nullptr,{0,0}}); //para algunas cosas especificas que necesitan tratar a todos los movholders por igual, no se usa directamente
    //puntualmente creo que es solo el memset que limpia la memoria local
    if(p->spawner){
      spawnerGen* sp =alloc<spawnerGen>(hb);
      initSpawner(sp,h,base,falseh->piece->kamikase);
      h->movs[0]=(movHolder*)sp;
      //b->size=sizeof(spawnerGen);
    }else{
      kamikaseCntrl* kc=alloc<kamikaseCntrl>(hb);
      initKamikase(kc,h);
      *h->movs[0]=(movHolder*)kc;
      //b->size=sizeof(kamikaseCntrl);
      }
    i=1;
  }*/

  for(int j=0;j<elems(p->movs);j++){
    pBase* pb=varrayOpElem(&p->movs,j);
    //TODO lo de que root arranque en null y lo setee el primer movimiento se me hace raro, por que no lo marco desde aca?
    Base* base=new(head(hb))Base{indGameVector(h),0,pb->memLocal};
    hb->size+=sizeof(Base);

    *varrayGameElem(&h->movs,j)=hb->size;
    crearMovHolder(pb->root,indGameVector(base),hb);
  }

  //para cantidades grandes de movimientos el real mide 72 menos que el declarado, el tamaño de una normalHolder
  //este bucle itera la cantidad de veces correcta, si fuera que faltara una vez la diferencia seria normalHolder+base

  assertf((char*)head(hb)-(char*)h==p->hsSize,"real %d == declared %d\n",(char*)head(hb)-(char*)h,p->hsSize);
  return indGameVector(h);
}

void crearMovHolder(int opInd,int baseInd,bigVector* hv){
  operador* op=opVector<operador>(opInd);
  movHolder* thisMov=(movHolder*)head(hv);
  switch(op->tipo){
  case NORMAL:
    initNormalH((normal*)op,baseInd,hv); break;
  case DESLIZ:
    initDeslizH((desliz*)op,baseInd,hv); break;
  case EXC:
    initExcH((exc*)op,baseInd,hv); break;
  case ISOL:
    initIsolH((isol*)op,baseInd,hv); break;
  case ISOLNRM:
    initIsolNonResetMemH((isol*)op,baseInd,hv); break;
  case DESOPT:
    initDesoptH((desopt*)op,baseInd,hv); break;
  case DESOPTNRM:
    initDesoptNonResetMemH((desopt*)op,baseInd,hv);break;
  case FAILOP:
    initFailH(hv);break;
  default:
    assert(false);
  }
  if(op->sig){
    thisMov->sig=hv->size;
    crearMovHolder(op->sig,baseInd,hv);
  }
  else
    thisMov->sig=0;
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
  for(int i=0;i<elems(h->movs);i++){
    movHolder* b=gameVector<movHolder>(*varrayGameElem(&h->movs,i));
    if(!(b->bools&valorCadena)) continue;
    b->table->cargar(b,&normales);
    normales.size=0;
  }
  Clicker::drawClickers=true;
}

void generar(Holder* h){
    actualHolder.h=h;
    for(int i=0;i<elems(h->movs);i++){
      int* mInd=varrayGameElem(&h->movs,i);
      movHolder* m=gameVector<movHolder>(*mInd);

      offset=gameVector<Tile>(h->tile)->pos;
      memset(memMov.data,0,gameVector<Base>(m->base)->memLocal.size*sizeof(int));
      m->table->generar(*mInd);
    }
}

