
//TODO no me termina de convencer tirar triggers por moverse nomas, me gustaria volver a separar la idea de esp de trigger.
//por ejemplo desliz w isol mover end end pone un trigger en el desliz en vez de adentro del isol, y hace recalcular toda la
//cadena al pedo. Se podría hacer un sistema mas intelgente, que tambien en caso de cosas como
//w exc mover or captura end ponga el trigger antes del exc

inline v getActualPos(v relPos,v offset){
  bool negate=actualHolder.h->bando;
  relPos.y=(relPos.y^-negate)+negate;
  return relPos+offset;
}
inline v getOffset(v relPos,v pos){
  bool negate=actualHolder.h->bando;
  relPos.y=(relPos.y^-negate)+negate;
  return pos-relPos;
}

void initMovH(movHolder* m,operador* op,int baseInd){
  Base* base=gameVector<Base>(baseInd);
  if(!base->movRoot)
    base->movRoot=indGameVectorInc(m);
  m->base=baseInd;
  m->bools=op->bools;//setea makeClick, hasClick y doEsp en normalh
}


movHolder* movH(int ind){
  return gameVector<movHolder>(ind);
}

#define getPtr(type,name,ind)                   \
  type* name gameVectorInc<type>(ind);

void generarSig(movHolder* m){
  //se llama cuando valor == true
  if(m->sig){
    m->bools&=~(valorCadena|valorFinal);
    movHolder* mh=movH(m->sig);
    mh->table->generar(m->sig);
    if(m->bools&hasClick){
      m->bools|=valorCadena;//TODO esto debería depender de propagar valorCadena, idenpendientemente de hasClick?
    }else
      m->bools|=mh->bools&valorCadena;
    m->bools|=mh->bools&valorFinal;
  }else
    m->bools|=valorFinal|valorCadena;
}

//para manejar templates
void reaccionarOverload(int m,int nh){
  gameVector<movHolder>(m)->table->reaccionar(m,nh);
}
void reaccionarOverload(int m,nhBuffer* nh){
  gameVector<movHolder>(m)->table->reaccionarVec(m,nh);
}

void reaccionarSig(movHolder* m,auto nhs){
  if(m->sig){
    reaccionarOverload(m->sig,nhs);
    if(switchToGen){
      m->bools&=~(valorCadena|valorFinal);
      if(m->bools&hasClick)
        m->bools|=valorCadena;
      else
        m->bools|=movH(m->sig)->bools&valorCadena;
      m->bools|=movH(m->sig)->bools&valorFinal;
    }
  }
}

inline void generarProperNormalH(normalHolder* n){
  int i;
  normal* op=opVector<normal>(n->op);
  actualHolder.buffer=(void(**)(void))varrayOpElem(&op->conds,0);
  actualHolder.bufferPos=&i;



  for(i=0;i<elems(op->conds);i++){
    bool ret=(*varrayOpElem(&op->conds,i))();
#if debugMode
    debugShowAndWait(ret);
#endif
    if(!ret){
      n->bools&=~(valorFinal|valorCadena|valor);
      return;
    }
  }
  offset=n->pos;

  n->bools|=valor;
  generarSig(n);
}
bool espFail(v pos){
  return pos.x<0||pos.x>=brd->dims.x
       ||pos.y<0||pos.y>=brd->dims.y;
}
void generarNormalH(int mInd){
  getPtr(normalHolder,n=,mInd);

  actualHolder.nh=n;
  memcpy(gameVectorInc(n->memAct.beg),memMov.data,gameVector<Base>(n->base)->memLocal.size*sizeof(int));

  n->pos=getActualPos(opVector<normal>(n->op)->relPos,offset);//pos se calcula siempre porque se usa para actualizar offset
  if(n->bools&doEsp){
    if(espFail(n->pos)){
      n->bools&=~(valorFinal|valorCadena|valor);
      return;
    }
    Tile* tile=tileGet(n->pos); //se me hace raro que tile solo se actualice si es esp, hay que mirar todo el tema de esp TODO
    actualHolder.tile=indGameVector(tile);

    //los triggers se ponen aca y se reponen en la reaccion. Las normales esp ahora son
    //las normales que se originan despues de un movimiento. (o hacen miran memoria de tile)
    //antes eran las normales que contenian un chequeo posicional, como vacio o memoria de tile,
    //lo que tiene mas sentido. Decidi ir por este otro camino porque la gran mayoria de normales
    //que siguen un movimiento hacen un chequeo posicional (creo que las que no se podrian reescribir siempre),
    //y hacerlo por posicion tiene el problema de que en cosas como w exc mover or captura end, que son comunes,
    //se esta poniendo un trigger en las dos ramas de exc en vez de antes de este, lo que no aprovecha la normal
    //de w que va a estar ahi de todas formas y hace que se tengan multiples triggers, lo que es mas caro.
    //Puede que se pueda hacer un sistema mejor en la version compilada, aunque no sé si lo vale.
    
    pushTrigger(&tile->triggersUsed,&tile->firstTriggerBox);
  }
  generarProperNormalH(n);
}
void reaccionarNormalH(int mInd,int nhInd){
  getPtr(normalHolder,self=,mInd);

  Base* base=gameVector<Base>(self->base);
  normal* op=opVector<normal>(self->op);

  if(nhInd==mInd){
    actualHolder.nh=self;

    memcpy(memMov.data,gameVectorInc<void>(self->memAct.beg),base->memLocal.size*sizeof(int));
    switchToGen=true;

    Tile* tile=tileGet(self->pos);
    actualHolder.tile=indGameVector(tile);
    pushTrigger(&tile->triggersUsed,&tile->firstTriggerBox);
    generarProperNormalH(self);
    if(!(self->bools&valorFinal)){
      offset=getOffset(op->relPos,self->pos);
      memcpy(memMov.data,gameVectorInc<void>(self->memAct.beg),base->memLocal.size*sizeof(int));
      //esta restauracion esta para que el operador que contenga reciba el offset y mem local correcta
      //la alternativa a hacer esto es que todos los operadores contenedores guarden offset y memoria local
      //para cada iteración/rama, lo que tambien tiene sus desventajas.
    }
  }else if(self->bools&valor){
    reaccionarSig(self,nhInd);
    if(!(self->bools&valorFinal)){
      offset=getOffset(op->relPos,self->pos);
      memcpy(memMov.data,gameVectorInc<void>(self->memAct.beg),base->memLocal.size*sizeof(int));
    }
  }
}
void reaccionarNormalH(int mInd,nhBuffer* nhs){
  getPtr(normalHolder,self=,mInd);
  assert(nhs->size>0);

  Base* base=gameVector<Base>(self->base);
  normal* op=opVector<normal>(self->op);
  for(int i=0;i<nhs->size;++i){
    int nhInd=nhs->buf[i];
    if(nhInd==mInd){
      #if debugMode
      if(i!=0){
        printf("normal fuera de orden\n");
      }
      #endif

      actualHolder.nh=self;

      memcpy(memMov.data,gameVectorInc<void>(self->memAct.beg),base->memLocal.size*sizeof(int));
      switchToGen=true;

      Tile* tile=tileGet(self->pos);
      actualHolder.tile=indGameVector(tile);
      pushTrigger(&tile->triggersUsed,&tile->firstTriggerBox);
      generarProperNormalH(self);
      if(!(self->bools&valorFinal)){
        offset=getOffset(op->relPos,self->pos);
        memcpy(memMov.data,gameVectorInc<void>(self->memAct.beg),base->memLocal.size*sizeof(int));
      }
      nhs->beg++;//faltaria reordenar si no es el primero pero si hago lo de filtrar no es necesario
      return;
    }
  }
  if(self->bools&valor){
    reaccionarSig(self,nhs);
    if(!(self->bools&valorFinal)){
      offset=getOffset(op->relPos,self->pos);
      memcpy(memMov.data,gameVectorInc<void>(self->memAct.beg),base->memLocal.size*sizeof(int));
    }
  }
}
/*
  como las normales estan en orden en memoria, y se van descartando conforme se activan o no se encuentran,
  la normal que se activaría en reaccionarNormalH nhs debería ser siempre la primera.
  El unico caso para el que no se cumple esto es desopt, porque desopt puede evaluar cosas yendo y viniendo en la memoria.
  Osea, la parte estatica tiene primero la primera iteracion y despues la segunda, y como hace un recorrido depth first salta
  hacia adelante y hacia atras. Esto no puede arreglarse haciendo que la estructura estatica se guarde en memoria de forma distinta, porque
  pasa el mismo problema entre la segunda iteracion y la parte dinamica. La unica solución a esto es hacer que toda la memoria de desopt sea
  dinamica, y reserve haciendo depth first en vez de reservar todas las ramas de un nodo juntas, lo que seria un poco mas lento pero es mas simple y tiene esta ventaja.
  La velocidad que aporta sacar ese bucle for esta adentro del ruido de variaciones entre mediciones, asi que no me parece que valga el esfuerzo
  hacer el cambio, ademas de que implica un costo en desopt. Pero hay que tener en cuenta que ese bucle for en una parte bastante usada del codigo
  esta ahi solo por un caso niche de desopt (2 triggers en ramas distintas, donde la rama que esta mas lejos en memoria se ejecuta primero por culpa
  de las ramas estaticas)
 */

/*
  hay 3 cosas que pueden pasar cuando se llama a una funcion reaccion de un operador mas complejos:
  - que la normal se active, no importa si es valida o no, lleva a una generacion.
  - que se determine que la normal es innacesible, si es la ultima o unica se hace un lngjmp porque no hay nada mas que hacer
  - que se determine que la normal es innacesible, pero hay mas y se sigue la reaccion
  operadores que corten la generacion para volver a una etapa de reaccion, si ven que no hay mas normales, hacen el lngjmp.
 */

void accionarNormalH(normalHolder* n){
  actualHolder.h=gameVector<Holder>(gameVector<Base>(n->base)->holder);
  actualHolder.nh=n;
  actualHolder.tile=indGameVector(tileGet(n->pos));

  normal* op=opVector<normal>(n->op);
  int i;
  actualHolder.buffer=varrayOpElem(&op->accs,0);
  actualHolder.bufferPos=&i;

  for(i=0;i<elems(op->accs);i++){
    (*varrayOpElem(&op->accs,i))();
  }
}
void cargarNormalH(movHolder* m,vector<normalHolder*>* norms){
  normalHolder* n=(normalHolder*)m;
  if(!(n->bools&valorCadena)) return;
  push(norms,n);
  if(n->bools&makeClick)
    makeClicker(norms,n->base);
  if(n->sig)
    movH(n->sig)->table->cargar(movH(n->sig),norms);
}
void drawNormalH(normalHolder* nh){
  actualHolder.nh=nh;
  normal* n=opVector<normal>(nh->op);
  for(int i=0;i<elems(n->colors);i++){
    int c=*varrayOpElem(&n->colors,i);
    coloresImp[c].draw();
  }
}

virtualTableMov normalTable={generarNormalH,reaccionarNormalH,reaccionarNormalH,cargarNormalH};

void initNormalH(normal* org,int base_,bigVector* hv){
  fromCast(n,head(hv),normalHolder*);

  initMovH(n,org,base_);
  n->table=&normalTable;
  
  hv->size+=sizeof(normalHolder);

  n->op=indOpVector(org);
  /*for(auto trigInfo:n->op->setUpMemTriggersPerNormalHolder)
    switch(trigInfo.type){
    case 0:
      memGlobalTriggers[trigInfo.ind].perma.push_back(n);
      break;case 1:
      base_->h->memPiezaTrigs[trigInfo.ind]->perma.push_back(n);
      break;case 2:
      turnoTrigs[base_->h->bando].push_back({base_->h,n}); ///@check
      }*/
  n->memAct.beg=hv->size;
  n->memAct.size=gameVector<Base>(base_)->memLocal.size*sizeof(int);
  hv->size+=n->memAct.size;
}




//TODO se me hace raro que se creen en el momento, no sería costoso armarlos todos al principio
void maybeAddIteration(deslizHolder*d,int i){
  if(d->cantElems==i){
    bigVector* hv=&actualHolder.ps->gameState;
    int sizeBefore=hv->size;
    hv->size=d->beg+d->elemSize*i;
    crearMovHolder(opVector<desliz>(d->op)->inside,d->base,hv);
    d->cantElems++;
    assert(d->cantElems<=(d->after-d->beg)/d->elemSize);
    assert(hv->size<=sizeBefore);
    hv->size=sizeBefore;
  }
}
void generarDeslizH(int mInd){
  getPtr(deslizHolder,d=,mInd);

  movHolder* act;
  int i=0;
  for(;;){
    v offsetOrg=offset;
    act=movH(d->beg+d->elemSize*i);
    act->table->generar(indGameVector(act));
    if(!(act->bools&valorFinal)){
      offset=offsetOrg;
      break;
    }
    i++;
    maybeAddIteration(d,i);
  }
  if(act->bools&valorCadena)
    d->bools|=lastNotFalse;
  else
    d->bools&=~lastNotFalse;
  d->f=i;
  generarSig(d);
}
int getFirstNormal(int nh){
  return nh;
}
int getFirstNormal(nhBuffer* nb){
  return nb->buf[nb->beg];
}

void deleteInnacesibleNormalsMaybeJump(int nh,auto cond){
  longjmp(jmpReaccion,1);
}
void deleteInnacesibleNormalsMaybeJump(nhBuffer* nb,auto cond){
  for(;
      nb->beg<nb->size;
      nb->beg++){
    if(!cond(gameVector<normalHolder>(nb->buf[nb->beg]))){
      return;
    }
  }
  longjmp(jmpReaccion,1);
}

movHolder* deslizElem(deslizHolder* d,int i){
  return gameVector<movHolder>(d->beg+d->elemSize*i);
}

template<typename T>
void reaccionarDeslizH(int mInd,T tnh){
  getPtr(deslizHolder,d=,mInd);
  
  constexpr bool singleNh=std::is_same<T,int>::value;
  int nhInd=getFirstNormal(tnh);

  desliz* des=opVector<desliz>(d->op);

  if(nhInd>=mInd+des->insideSize){
    reaccionarSig(d,tnh);
    return;
  }

  assert(nhInd>=d->beg);
  int diff=nhInd-d->beg;
  int iter=diff/des->iterSize;
  for(int i=0;i<iter;++i){
    movHolder* m=deslizElem(d,i);
    if(!(m->bools&valorFinal)){
      deleteInnacesibleNormalsMaybeJump(tnh,[=](normalHolder* inh)->bool{
                                                     return (char*)inh<(char*)d+des->insideSize;
                                                    });
      reaccionarSig(d,tnh);
      return;
    }
  }
  movHolder* act=deslizElem(d,iter);
  reaccionarOverload(indGameVector(act),tnh);

  if constexpr(!singleNh){
    if(!switchToGen){
      //esto pasa en caso de que la nh estaba en un isol, desopt o region innacesible de operador interior
      //y hay mas normales en el buffer, que pueden estar en proximas iteraciones o despues
      reaccionarDeslizH(mInd,tnh);
      return;
    }
  }
  assert(switchToGen==true);

  for(;act->bools&valorFinal;){
    iter++;
    maybeAddIteration(d,iter);
    act=deslizElem(d,iter);
    act->table->generar(indGameVector(act));
  }
  if(act->bools&valorCadena)
    d->bools|=lastNotFalse;
  else
    d->bools&=~lastNotFalse;
  d->f=iter;
  generarSig(d);
}
void cargarDeslizH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(d,m,deslizHolder*);
  if(!(d->bools&valorCadena)) return;
  for(int i=0;i<(d->bools&lastNotFalse?d->f+1:d->f);i++){
    movHolder* mov=deslizElem(d,i);
    mov->table->cargar(mov,norms);
  }
  if(d->bools&makeClick&&!(norms->size==0)) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
    makeClicker(norms,d->base);
  if(d->sig)
    movH(d->sig)->table->cargar(movH(d->sig),norms);
}

virtualTableMov deslizTable={generarDeslizH,reaccionarDeslizH<int>,reaccionarDeslizH<nhBuffer*>,cargarDeslizH};
//esta version del desliz retrocede a la posicion de inicio de una iteracion si la iteracion falla
//esto la hace mas consistente en casos donde se usen multiples normales y cosas asi. Pero para
//casos comunes como la torre no se usa, agrega codigo y almacenamiento, y lo mas importante es
//que genera multiples triggers. No vale la pena agregar otra version ahora para no hacer bloat,
//pero en la version compilada podria estar. Tambien se podria agregar instrucciones bizarras
//solo de desliz sin mucho problema, como cambiar la pos de retorno o breaks.
void initDeslizH(desliz* org,int base_,bigVector* hv){
  getPtr(deslizHolder,d=,hv->size);

  initMovH(d,org,base_);
  d->table=&deslizTable;

  hv->size+=sizeof(deslizHolder);
  d->op=indOpVector(org);
  d->beg=hv->size;
  d->after=d->beg+org->insideSize;
  d->elemSize=org->iterSize;

  crearMovHolder(org->inside,base_,hv);//crear primera iteracion

  hv->size=d->after;
  d->cantElems=1;
}


void generarSigExc(movHolder* e,movHolder* validBranch){
  assert(validBranch->bools&valorCadena);
  if(validBranch->bools&valorFinal){
    generarSig(e);
  }else{
    e->bools|=valorCadena;
    e->bools&=~valorFinal;
    //una rama a medias es valida pero corta el flujo, mantiene la posicion
  }
}

int iterations=0;
void generarExcH(int mInd){
  getPtr(excHolder,e=,mInd);

  iterations++;
  //if(iterations==19){
  //  int fafsd=423;
  //}

  int i;
  v offsetOrg=offset;
  for(i=0;i<elems(e->movs);i++){
    movHolder* branch=movH(*varrayGameElem(&e->movs,i));
    branch->table->generar(indGameVector(branch));
    if(branch->bools&valorCadena){
      e->bools|=valor;
      e->actualBranch=i; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que valorCadena==true
      generarSigExc(e,branch);
      return;
    }
    offset=offsetOrg;
  }
  e->bools&=~(valorFinal|valorCadena|valor);
  e->actualBranch=i-1;
}
template<typename T>
void reaccionarExcH(int mInd,T tnh){
  getPtr(excHolder,e=,mInd);

  constexpr bool singleNh=std::is_same<T,int>::value;
  int nhInd=getFirstNormal(tnh);

  int i=1;
 loop:

  if(nhInd-mInd>=e->size){
    reaccionarSig(e,tnh);
    return;
  }

  movHolder* branch;
  for(;i<=e->actualBranch;++i){
    int nextBranch=indGameVector(varrayGameElem(&e->movs,i));
    if(nextBranch>nhInd){
      branch=gameVector<movHolder>(*varrayGameElem(&e->movs,i-1));
      goto branchFound;
    }
  }
  if(e->actualBranch!=e->movs.size-1
     && nhInd>=*varrayGameElem(&e->movs,e->actualBranch+1)){//si la actual no es la ultima existe la posibilidad de que el nh sea de una rama invalida
    deleteInnacesibleNormalsMaybeJump(tnh,[e](normalHolder* inh)->bool{
                                                           return (char*)inh<(char*)e+e->movs.size;
                                                         });
    reaccionarSig(e,tnh);
    return;
  }

  branch=gameVector<movHolder>(*varrayGameElem(&e->movs,e->actualBranch));
 branchFound:
  branch->table->reaccionar(indGameVector(branch),nhInd);
  if(switchToGen){
    if(!(branch->bools&valorCadena)){ //si el ab al recalcularse se invalida generar todo devuelta, saltandolo
      int j;
      for(j=i;j<elems(e->movs);j++){
        movHolder* brancj=gameVector<movHolder>(*varrayGameElem(&e->movs,j));
        brancj->table->generar(indGameVector(brancj));
        if(brancj->bools&valorCadena){
          e->bools|=valor;
          e->actualBranch=j;
          generarSigExc(e,brancj);
          return;
        }
      }
      e->bools&=~(valor|valorCadena|valorFinal);
      e->actualBranch=j-1;
    }else{ //se valido una rama que era invalida
      e->actualBranch=i-1;
      e->bools|=valor;
      generarSigExc(e,branch);
    }
  }else{
    assert(!singleNh);
    assert(((nhBuffer*)(intptr)tnh)->size>0);
    goto loop;
  }
}
void cargarExcH(movHolder* m,vector<normalHolder*>* norms){
  excHolder* e=(excHolder*)m;
  if(!(e->bools&valorCadena)) return;
  movHolder* branch=gameVector<movHolder>(*varrayGameElem(&e->movs,e->actualBranch));
  branch->table->cargar(branch,norms);
  if(e->bools&makeClick)
    makeClicker(norms,e->base);
  if(e->sig&&(branch->bools&valorFinal))
    movH(e->sig)->table->cargar(movH(e->sig),norms);
}

virtualTableMov excTable={generarExcH,reaccionarExcH<int>,reaccionarExcH<nhBuffer*>,cargarExcH};
void initExcH(exc* org,int base_,bigVector* hv){
  excHolder* e=(excHolder*)head(hv);
  initMovH(e,org,base_);
  e->table=&excTable;
  hv->size+=sizeof(excHolder);

  e->movs.beg=hv->size;
  e->movs.size=org->ops.size;
  hv->size+=org->ops.size;

  e->size=org->insideSize+sizeof(excHolder);
  for(int i=0;i<elems(org->ops);i++){
    *varrayGameElem(&e->movs,i)=hv->size;
    crearMovHolder(*varrayOpElem<int>(&org->ops,i),base_,hv);
  }
}

//es un macro por alloca. Pido resetSize porque desopt lo guarda, para no buscarlo a la base cada vez. No creo que tenga un impacto eso igual
#define storeState(resetSize)                      \
  v tempPos=offset;                               \
  void* tempLocalMem=alloca(resetSize);           \
  memcpy(tempLocalMem,memMov.data,resetSize);


#define restoreState(resetSize)                 \
  offset=tempPos;                               \
  memcpy(memMov.data,tempLocalMem,resetSize);



void generarIsolH(int mInd){
  getPtr(isolHolder,s=,mInd);

  int resetSize=gameVector<Base>(s->base)->memLocal.resetUntil*sizeof(int);
  storeState(resetSize);
  movH(s->inside)->table->generar(s->inside);
  restoreState(resetSize);

  if(s->sig){
    movH(s->sig)->table->generar(s->sig);
    s->bools&=~valorFinal;
    s->bools|=movH(s->sig)->bools&valorFinal;
  }
}

template<typename T>
void reaccionarIsolH(int mInd,T tnh){
  getPtr(isolHolder,s=,mInd);

  int nhInd=getFirstNormal(tnh);
  if(nhInd-mInd<s->size){
    reaccionarOverload(s->inside,tnh);
    deleteInnacesibleNormalsMaybeJump(tnh,[s](normalHolder* inh)->bool{
                                            return (char*)inh<(char*)s+s->size;
                                          });
    switchToGen=false; //aca la memoria tiene basura que se cargo en la generacion interna, no importa porque se volvio a modo reaccion y se va a pisar si se activa un proximo trigger
  }
  if(s->sig)
    reaccionarOverload(s->sig,tnh);
}

void cargarIsolH(movHolder* m,vector<normalHolder*>* norms){
  isolHolder* s=(isolHolder*)m;
  int sizeBefore=norms->size;
  movH(s->inside)->table->cargar(movH(s->inside),norms);
  if(s->bools&makeClick&&!norms->size==0)//evitar generar clickers sin normales
    makeClicker(norms,s->base);
  norms->size=sizeBefore;
  if(s->sig)
    movH(s->sig)->table->cargar(movH(s->sig),norms);
}

virtualTableMov isolTable={generarIsolH,reaccionarIsolH<int>,reaccionarIsolH<nhBuffer*>,cargarIsolH};
void initIsolH(isol* org,int base_,bigVector* hv){
  isolHolder* s=(isolHolder*)head(hv);

  initMovH(s,org,base_);
  s->table=&isolTable;

  hv->size+=sizeof(isolHolder);
  s->inside=hv->size;
  crearMovHolder(org->inside,base_,hv);
  s->bools|=valor|valorFinal|valorCadena;
  s->size=org->size;
}



#define desoptMov (mInd+sizeof(desoptHolder))

void construirYGenerarNodo(desoptHolder*,int);

void generarDesoptH(int mInd){
  //se tiene la base y primera iteracion construidos, rondas mas alla de estas usan el resto de memoria
  //de forma dinamica. Es un punto intermedio entre tener todo construido y reservar mucha memoria que probablemente
  //no se use, y hacer todo dinamico y estar reconstruyendo operadores en cada generacion
  //Seria lo mas optimizado para la dama

  //con limpiar la caja de movHolders de la primera iteracion antes de la generacion total
  //y sobreescribir el espacio dinamico no deberia haber ningun problema de construcciones kakeadas y cosas asi
  getPtr(desoptHolder,d=,mInd);

  desopt* op=opVector<desopt>(d->op);
  Base* base=gameVector<Base>(d->base);

  d->dinamClusterHead=desoptMov+op->dinamClusterBaseOffset;

  int clusterOffset=0;
  int correspondingCluster=desoptMov+op->clusterSize;

  int resetSize=base->memLocal.resetUntil*sizeof(int);

  storeState(resetSize);
  
  void* memTemp2=alloca(resetSize);

  forVOp(op->movSizes){
    int tam=*el;
    int firstIter=desoptMov+clusterOffset;
    //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
    movHolder* actualMov= gameVector<movHolder>(firstIter+sizeof(int));

    actualMov->table->generar(indGameVector(actualMov));
    if(actualMov->bools&valorFinal){
      *gameVector<int>(firstIter)=correspondingCluster;
      int clusterOffset2=0;
      v offsetOrg2=offset;
      memcpy(memTemp2,memMov.data,resetSize);

      forVOp(op->movSizes){
        int tam=*el;
        int secondIter=*gameVector<int>(firstIter)+clusterOffset2;
        movHolder* actualMov2=gameVector<movHolder>(secondIter+sizeof(int));

        actualMov2->table->generar(indGameVector(actualMov2));
        if(actualMov2->bools&valorFinal){
          *gameVector<int>(secondIter)=d->dinamClusterHead;
          construirYGenerarNodo(d,resetSize);
        }else
          *gameVector<int>(secondIter)=0;
        clusterOffset2+=tam; //@optim por ahi vale la pena saltarse esto en la ultima iteracion? en el bucle exterior dependeria de si hay sig
        offset=offsetOrg2;
        memcpy(memMov.data,memTemp2,resetSize);
      }
    }else
      *gameVector<int>(firstIter)=0;
    clusterOffset+=tam;
    correspondingCluster+=op->clusterSize;
    restoreState(resetSize);
  }
  if(d->sig)//TODO deberia propagar el valorFinal como isol
    movH(d->sig)->table->generar(d->sig);
}
//TODO es necesario contruir en el momento? no debería dar lo mismo construir todo al principio, si son siempre los mismos bloques?
void construirYGenerarNodo(desoptHolder* d,int resetSize){
  desopt* op=opVector<desopt>(d->op);
  int clusterBeg=d->dinamClusterHead;

  assert(d->dinamClusterHead<indGameVector(d)+(int)sizeof(desoptHolder)+op->desoptInsideSize);
  forVOp(op->ops){
    operador* opos=gameVector<operador>(*el);
    *gameVector<int>(d->dinamClusterHead)=0;
    d->dinamClusterHead+=sizeof(int);

    bigVector* hv=&actualHolder.ps->gameState;
    int sizeBefore=hv->size;
    hv->size=d->dinamClusterHead;

    crearMovHolder(indGameVector(opos),d->base,hv);

    assert(hv->size<=sizeBefore);
    d->dinamClusterHead=hv->size;
    hv->size=sizeBefore;
  }
  int clusterOffset=0;
  storeState(resetSize);

  forVOp(op->movSizes){
    int tam=*el;

    int nextIter=clusterBeg+clusterOffset;
    movHolder* actualMov=gameVector<movHolder>(nextIter+sizeof(int));

    assert(*gameVector<int>(nextIter)==0);
    actualMov->table->generar(nextIter+sizeof(int));
    if(actualMov->bools&valorFinal){
      *gameVector<int>(nextIter)=d->dinamClusterHead;
      construirYGenerarNodo(d,resetSize);
    }
    clusterOffset+=tam;
    restoreState(resetSize);
  }
}
void generarNodo(desoptHolder* d,int nextIter,int resetSize){
  int iter=*gameVector<int>(nextIter);
  if(iter){
    desopt* op=opVector<desopt>(d->op);

    int clusterOffset=0;
    storeState(resetSize);

    forVOp(op->movSizes){
      int tam=*el;
      int nextIter=iter+clusterOffset;
      movHolder* actualMov=gameVector<movHolder>(nextIter+sizeof(int));
      actualMov->table->generar(indGameVector(actualMov));
      if(actualMov->bools&valorFinal){
        generarNodo(d,nextIter,resetSize);
      }
      clusterOffset+=tam;
      restoreState(resetSize);
    }
  }else{
    *gameVector<int>(nextIter)=d->dinamClusterHead;
    construirYGenerarNodo(d,resetSize);
  }
}
/*
  no siempre se quiere que se resetee la memoria local en cada rama. Si se esta haciendo algo como crecer un arbol hasta alcanzar n espacios
  si se quiere, porque haces un madd en cada rama y un mcmp cte para cortar. Si no fuera asi tendrías que tener una segunda variable que guarde
  una idea de en que parte del arbol se esta, que es un quilombo.

  Hay varios casos igual donde mantener memoria entre ramas es util, como la dama. Poder marcar si en una rama se encontró una pieza enemiga capturable
  es util para saber si se pueden hacer movimientos basicos. Tambien porque en la dama se guarda un monton de informacion entre ramas que despues se analiza.

  Osea que sería util tener las 2 funcionalidades. Podría tener 2 versiones de desopt ahora, pero complicaria el codigo al pedo. Lo voy a dejar para
  la version compilada, que sería un if durante la escritura nomas. Debería haber algo parecido para isol tambien.

  Por ahora podría implementarlo como un if de un flag adentro de desopt, pero como por ahora nomas me interesa la segunda funcionalidad voy a comentar
  lo que guarda y restaura memoria y listo.

  Otra forma de tener las 2 funcionalidades es dejar que la memoria local se restaure y usar la memoria de pieza con mset* como memoria que no se restaura.
  No me termina de convencer porque la memoria de pieza es persistente entre movimientos y tiene otros fines, medio hack

  Otra forma de tener las 2 funcionalidades es marcar cierta region de la memoria local como no restaurable. No agregaria mucha complicacion al codigo porque
  no es un nuevo tipo de memoria, es nomas un int para cada movimiento que solo sería leido por desopt e isol al momento de restaurar. Por ejemplo,
  si un movimiento tiene marcado memResl 4 la memoria local de 0 a 3 se restauraria y de 4 para arriba no. No debería ser mas lento porque ese agregado
  de memoria no deberia importar. No sé si me convence porque agrega una cosa mas al lenguaje, tener 2 desopts e isols parece mas simple y seria igual de
  eficiente. La ventaja que tiene esto es que podrías tener memoria restaurada y no restaurada en un mismo operador. Si se quiere eso se puede usar la
  version que no restaura y restaurar manualmente igual, tecnicamente se puede implementar desopt con desliz, excs y memoria tambien (exc(CODE,pass) para encargarse de la funcionalidad de seguir cargando aunque falle lo de adentro)

  TODO?
 */
void reactIfNh(int nh,movHolder* actualMov,int tam){
  int indM=indGameVector(actualMov);
  if(nh>=indM&&nh<indM+tam){
    actualMov->table->reaccionar(indM,nh);
  }
}
void reactIfNh(nhBuffer* nhs,movHolder* actualMov,int tam){
  for(int i=0;i<nhs->size;){
    int beforeNh=nhs->buf[i];
    reactIfNh(beforeNh,actualMov,tam);
    if(switchToGen) break;
    if(nhs->buf[i]==beforeNh) i++;//para manejar el caso donde el reaccionar consume el actual, y por ahi proximos, ahi no se avanza
  }
}
template<typename T>
void reaccionarProperDesoptH(desoptHolder* d,T nh,int iter){
  int branchOffset=0;
  desopt* op=opVector<desopt>(d->op);

  forVOp(op->movSizes){
    int tam=*el;

    int nextIter=iter+branchOffset;
    movHolder* actualMov=gameVector<movHolder>(nextIter+sizeof(int));
    branchOffset+=tam;

    reactIfNh(nh,actualMov,tam);

    if(switchToGen){
      if(actualMov->bools&valorFinal){
        generarNodo(d,nextIter,gameVector<Base>(d->base)->memLocal.resetUntil*sizeof(int));
      }
      if constexpr(std::is_same<T,int>::value){
        longjmp(jmpReaccion,1);
      }else{
        if(nh->size==0)
          longjmp(jmpReaccion,1);
      }
    }else{
      if(actualMov->bools&valorFinal)
        reaccionarProperDesoptH(d,nh,*gameVector<int>(nextIter));
    }

    switchToGen=false;
  }
}

template<typename T,void(*reaccionarProper)(desoptHolder*,T,int)>
void reaccionarDesoptH(int mInd,T tnh){
  getPtr(desoptHolder,d=,mInd);
  if(getFirstNormal(tnh)<(int)desoptMov+opVector<desopt>(d->op)->desoptInsideSize){
    reaccionarProper(d,tnh,indGameVector(d->movs));
  }else{
    reaccionarOverload(indGameVector(d),tnh);
  }
}



/*
  como se reutiliza espacios de memoria puede que una rama ponga un trigger, y en una regeneracion por un trigger anterior la rama
  quede invalidada, y se reutilice para otra memoria. Ahora este trigger invalido, a diferencia de desliz, no apunta a un lugar que
  no se va a encontrar nunca. Apunta a un lugar que esta siendo usado para otra cosa, y esto va a causar una regeneracion al pedo. No
  va a crear comportamientos erroneos, es calculos al pedo nomas. No creo que pase muy seguido igual, y si agrego la optimizacion de no
  seguir generando si la normal que se activó no cambio su valor de validez, el impacto de esto se reduce a solo recalcular una normal.
 */
void cargarNodos(movHolder* m,int iter,vector<normalHolder*>* norms){
  fromCast(d,m,desoptHolder*);
  desopt* op=opVector<desopt>(d->op);

  int res=norms->size;
  int offset=0;
  forVOp(op->movSizes){
    int tam=*el;
    int nextIter=iter+offset;
    movHolder* actualMov=gameVector<movHolder>(nextIter+sizeof(int));
    if(actualMov->bools&valorFinal){//TODO falta manejar estado intermedio no?
      actualMov->table->cargar(actualMov,norms);
      assert(*gameVector<int>(nextIter));
      cargarNodos(d,*gameVector<int>(nextIter),norms);
    }else if((d->bools&makeClick)&&norms->size!=0){
      makeClicker(norms,d->base);
    }
    offset+=tam;
    norms->size=res;
  }
}
void cargarDesoptH(movHolder* m,vector<normalHolder*>* norms){
  desoptHolder* d=(desoptHolder*)m;
  cargarNodos(d,indGameVector(d)+sizeof(desoptHolder),norms);
  if(m->sig)
    movH(m->sig)->table->cargar(movH(m->sig),norms);
}
/*
  desopt actua como un isol respecto a lo que esta antes y despues.
  De no hacerlo todo lo que venga despues tendria que agregarse al final de cada rama, lo que no
  es obvio por la forma en que se escribe

  Si se quiere hacer algo como desliz A optar B , C end D c end se tiene que escribir
  A desopt BD c A , CD c A end
  Esto refleja lo que se hace y no oscurece la cantidad de calculos distintos que se hacen
  (de la otra forma se podría pensar que D y A se calculan una vez en lugar de por cada rama)
  @testarVelocidad con A a la izquierda, sin partir con c
*/

virtualTableMov desoptTable={generarDesoptH,reaccionarDesoptH<int,reaccionarProperDesoptH>,
                             reaccionarDesoptH<nhBuffer*,reaccionarProperDesoptH>,cargarDesoptH};
void initDesoptH(desopt* org,int base_,bigVector* hv){
  desoptHolder* d=(desoptHolder*)head(hv);
  initMovH(d,org,base_);
  d->table=&desoptTable;

  d->op=indOpVector(org);

  int clusterSize=org->clusterSize;
  hv->size+=sizeof(desoptHolder);

  int nextIteration=hv->size;
  int headFirst=hv->size;
  forVOp(org->ops){//armar base
    operador* opos=opVector<operador>(*el);
    nextIteration+=clusterSize;
    *gameVectorInc<int>(hv->size)=nextIteration;//head apunta a puntero al proximo cluester de movimientos,
    hv->size+=sizeof(int);//que corresponde a la proxima iteracion de este movHolder
    crearMovHolder(indOpVector(opos),base_,hv);
  }
  for(int i=0;i<elems(org->ops);i++){//armar primer iteracion
    for(int j=0;j<elems(org->ops);j++){
      *gameVectorInc<int>(hv->size)=0;//cuando se usa apunta a un espacio dinamico
      hv->size+=sizeof(int);
      crearMovHolder(*varrayOpElem(&org->ops,j),base_,hv);
    }
  }

  hv->size=headFirst+org->desoptInsideSize;
  d->bools|=valorCadena|valorFinal;
}




//fail tambien se podría implementar retornando falso en todo y forzando el operador anterior a tener hasClick,
void generarFail(int m){
  movH(m)->bools=valorCadena; //valorFinal=false, el resto no importa
  return;
}
void reaccionarFail(int m, int nh){}
void reaccionarFail(int m, nhBuffer* nhs){}
void cargarFail(movHolder* m, vector<normalHolder*>* norms){}
virtualTableMov failTable={generarFail,reaccionarFail,reaccionarFail,cargarFail};

void initFailH(bigVector* hv){
  movHolder* f=(movHolder*)head(hv);
  f->table=&failTable;
  f->base=0;
  f->bools=0;
  hv->size+=sizeof(movHolder);
}



#define storeFirstNormalState(movHolder)              \
  normalHolder* tempNh=getNextNormalH(movHolder);     \
  v tempPos=getOffset(opVector<normal>(tempNh->op)->relPos,tempNh->pos); \
  memcpy(tempLocalMem,gameVectorInc<void>(tempNh->memAct.beg),resetSize);
normalHolder* getNextNormalH(movHolder*);

template<typename T>
void reaccionarIsolNonResetMemH(int mInd,T tnh){
  getPtr(isolHolder,s=,mInd);
  constexpr bool singleNh=std::is_same<T,int>::value;
  if(getFirstNormal(tnh)-mInd<s->size){
    int resetSize=gameVector<Base>(s->base)->memLocal.resetUntil*sizeof(int);
    void* tempLocalMem=alloca(resetSize);
    storeFirstNormalState(movH(s->inside));

    reaccionarOverload(s->inside,tnh);
    assert(singleNh?switchToGen:true);
    if(singleNh||switchToGen){
      restoreState(resetSize);
      if(s->sig) movH(s->sig)->table->generar(s->sig); //se sigue generando porque potencialmente se cambio memoria que no se resetea
      //no se limpian normales extra que estan contenidas en este isol, si las hay, porque se sigue
      //generando y no son relevantes. El isol comun si lo hace porque se vuelve a reaccionar.
      return;
    }
  }
  if(s->sig)
    reaccionarOverload(s->sig,tnh);
}


virtualTableMov isolNRMTable={generarIsolH,reaccionarIsolNonResetMemH,reaccionarIsolNonResetMemH,cargarIsolH};
void initIsolNonResetMemH(isol* org,int base_,bigVector* hv){
  isolHolder* s=(isolHolder*)head(hv);
  initIsolH(org,base_,hv);
  s->table=&isolNRMTable;
}



#define storeStateNoAlloc(resetSize)            \
  v tempPos=offset;                             \
  memcpy(tempLocalMem,memMov.data,resetSize);
//desoptNRM se usa si alguna rama usa memoria no reseteable. Aunque una rama no toque esa memoria, que se haya disparado su trigger implica
//que potencialmente va a invalidar o crear nodos futuros, y por lo menos uno de esos nodos si toca la memoria.
template<typename T,bool firstReaccionar=true>
void reaccionarProperDesoptNonResetMemH(desoptHolder* d,T nh,int iter){
  int branchOffset=0;
  int resetSize=gameVector<Base>(d->base)->memLocal.resetUntil*sizeof(int);
  void* tempLocalMem=alloca(resetSize);
  //printf("desopt NRM\n");
  desopt* op=opVector<desopt>(d->op);
  forVOp(op->movSizes){
    int tam=*el;
    int nextIter=iter+branchOffset;
    int actualMovInd=nextIter+sizeof(int);
    movHolder* actualMov=gameVector<movHolder>(actualMovInd);
    branchOffset+=tam;

    if(switchToGen){
      //tengo que volver a generar todos los nodos que estan despues en el orden de generacion, no solo los siguientes al nodo que disparo el trigger
      storeStateNoAlloc(resetSize);
      actualMov->table->generar(actualMovInd);
      if(actualMov->bools&valorFinal){
        generarNodo(d,nextIter,resetSize);
      }
      restoreState(resetSize);
    }else{
      storeFirstNormalState(actualMov);
      reactIfNh(nh,actualMov,tam);
      if(switchToGen){
        if(actualMov->bools&valorFinal){
          generarNodo(d,nextIter,resetSize);
        }
      }else if(actualMov->bools&valorFinal){
        reaccionarProperDesoptNonResetMemH<T,false>(d,nh,*gameVector<int>(nextIter));
      }
      restoreState(resetSize);
    }
  }
  if constexpr(firstReaccionar){
      if(switchToGen&&d->sig){
        movH(d->sig)->table->generar(d->sig);
      }
    }
}

virtualTableMov desoptNRMTable={generarDesoptH,reaccionarDesoptH<int,reaccionarProperDesoptNonResetMemH>,
                                reaccionarDesoptH<nhBuffer*,reaccionarProperDesoptNonResetMemH>,cargarDesoptH};
void initDesoptNonResetMemH(desopt* org,int base_,bigVector* hv){
  //printf("gbdfgbdf\n");
  getPtr(desoptHolder,d=,hv->size);
  initDesoptH(org,base_,hv);
  d->table=&desoptNRMTable;
}


//funcion que consigue la siguiente normal, lo usa isolNonResetMemH para obtener la posicion y memoria local para restaurarla despues
//se podría haber hecho que isolNonResetMemH guarde esta informacion, pero preferi hacerlo asi para no tener la memoria redudante y
//porque me parece mas simple 
//esta funcion podría estar en las tablas virtuales, pero no quise ensuciarlas por este caso que es bastante niche. No sé si hay una
//diferencia de performance por hacerlo de una forma u otra
normalHolder* getNextNormalH(movHolder* m){
  //se trae la informacion de la primera nh que se encuente, no importa si es la del camino valido, porque se sabe
  //que se ejecutó y se puede extraer de esta la memoria y el offset antes de que se modificaran


  //pense que se podia hacer un switch sobre punteros pero parece que no, asi que queda esto medio choto
  if(m->table==&normalTable)
    return (normalHolder*)m;
  if(m->table==&deslizTable)
    return getNextNormalH(gameVector<movHolder>(((deslizHolder*)m)->beg));
  if(m->table==&excTable)
    return getNextNormalH(gameVector<movHolder>(((excHolder*)m)->movs.beg));
  if(m->table==&isolTable || m->table==&isolNRMTable)
    return getNextNormalH(gameVector<movHolder>(((isolHolder*)m)->inside));
  if(m->table==&desoptTable || m->table==&desoptNRMTable){
    int firstIter=indGameVector(m)+sizeof(desoptHolder);
    return getNextNormalH(gameVector<movHolder>(firstIter+sizeof(int)));
  }
  fail("getNextNormalH didn't found nh\n");
  return nullptr;
}



/*
void generarNewlySpawned(movHolder* m){
  fromCast(s,m,spawnerGen*);

  if(justSpawned.size==0)
    return;

  vector<Holder*> justSpawnedC;//evitar bucles infinitos si spawneo un spawner
  justSpawnedC.data=justSpawned.data;
  justSpawnedC.size=justSpawned.size;
  justSpawned.size=0;

  Holder* spawnerH=actualHolder.h;

  for(Holder* h:justSpawnedC)
    if(s->h!=h)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
      generar(h);
  if(s->kamikaseNext)
    kamikaseCheckAlive(m);

  actualHolder.h=spawnerH;
}

void cargarNothing(movHolder*m,vector<normalHolder*>* nh){
}

 virtualTableMov spawnerTable={generarNewlySpawned,nullptr,nullptr,cargarNothing};
void initSpawner(spawnerGen* s,Holder* h,Base* b,bool kamikaseNext){
  s->table=&spawnerTable;
  s->h=h;
  s->base=b;
  s->kamikaseNext=kamikaseNext;
}

//kamikase son las piezas que hagan capt en posRel 0,0 o capt en una posicion no relativa (TODO), por las dudas
void kamikaseCheckAlive(movHolder* m){
  fromCast(k,m,kamikaseCntrl*);
  if(!k->h->inPlay)
    throw nullptr;
}
//si spawnea y muere en el mismo turno no pasa nada porque spawn corre antes de kamikase
 virtualTableMov kamikaseTable={kamikaseCheckAlive,nullptr,nullptr,cargarNothing};
void initKamikase(kamikaseCntrl* k,Holder* h){
  k->table=&kamikaseTable;
  k->h=h;
}
*/

/*
sobre isol/desopt y memoria no reseteable:
necesito una forma de poder transmitir informacion afuera del desopt para manejar cosas como la dama, y me imagino que hay casos donde saltar un isol tambien es util.
Por default isol y desopt resetean la memoria local y la posicion cuando terminan y en desopt cada vez que cambia de rama.

Lo que hice es marcar una region de memoria local para que no se resetee, lo que en generacion funciona y dentro de todo es simple y no tiene costo.
El problema esta en la regeneracion, antes podia cortar la regeneracion si esta se disparaba adentro de un isol, o limitarla a la propagacion de una rama en desopt. Pero ahora que estos pueden transmitir informacion mas alla de si la regeneracion se tiene que propagar afuera tambien. Creo que lo voy a hacer de todas formas porque es lo que se quiere y lo que se necesita, pero es algo que no tuve en cuenta al principio y me agarró de sorpresa ahora. La propagacion solo pasa cuando se usa memoria no reseteable, lo que se testea facil. Me hace un poco de ruido porque isol termina siendo un restaurador de posicion y memoria nomas, y desopt lo mismo pero trayendo una estructura de arbol. Pero bueno, son utiles y ya estan hechos. Creo que es el mejor compromiso que se me ocurrió, y dan lugar a optimizaciones a veces. Me estoy olvidando que isol y desopt tambien manejan el buffer de acciones, lo que en realidad era su funcion principal y es lo mas util.

Otra cosa que pensé es volar la memoria local no reseteable y usar la memoria de pieza como memoria persistente, con un operador asignacion-durante-condicion especial, aunque esto tiene problemas:
-Es una memoria que persiste entre movimientos, lo que puede ser un poco molesto pero se puede ignorar
-Es una memoria que persiste entre generaciones, lo que implica que al inicio de cada generacion se debe resetear manualmente.
Y hay que tener en cuenta que no es como la memoria local, que al reaccionar se resetea al estado que tenia en la generacion en ese momento. El estado es el estado del final de la generacion, y si eso no se maneja en el codigo puede tener comportamientos extraños.
Esto agrega un monton de complejidad y sigue teniendo el mismo problema que el anterior, en caso de reaccion regenera todo devuelta o tiene reglas muy raras sobre cuando saltar isols en regeneraciones. Asi que no.
-
Igual la idea de usar una memoria persistente entre regeneraciones para optimizar recalculos podría ser interesante. Por ejemplo en la dama, si una rama cambia no sería necesario recalcular las proximas ramas devuelta, porque no cambiaron y no dependen de este cambio (mas o menos, puede que sean la rama ganadora ahora). Lo unico que se necesita recalcular es el codigo que viene despues, y solo en caso de que el nuevo recalculo mueva el maximo de cadena que estaba. Supongo que se podrían agregar reglas al lenguaje para poder decir cosas como esta, pero no quiero agregar mas complejidad al lenguaje para optimizaciones. Recalcular todo siempre maneja todos los casos y es simple. Por ahí en la version compilada se podría agregar alguna optimizacion que vea que regiones de memoria se tocan y decida si recalcular otras ramas, o partes de estas, y cosas asi, estaria bueno. Pero no es algo del lenguaje, por ahora nada.

Otra opcion mas loca es borrar isol y desopt porque se hicieron muy bloated, y agregar comandos de bajo nivel para manejar la restauracion de posiciones, regiones de memoria local y el buffer de acciones. Lo malo de esto es que no se puede optimizar tanto como isol y desopt, ya que tendrían sus propias memorias y no aprovechan lo que se guarda en normales para restaurar en reacciones.  Y si aprovechan eso son basicamente lo mismo que tengo ahora, pero mas dificil de programar. Y tiene los mismos problemas, recalculan todo siempre. Asi que no
 */

