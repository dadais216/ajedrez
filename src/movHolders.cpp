
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

void initMovH(movHolder* m,operador* op,Base* base_){
  if(!base_->root)
    base_->root=m;
  m->base=base_;
  m->bools=op->bools;//setea makeClick, hasClick y doEsp en normalh
}

void generarSig(movHolder* m){
  //se llama cuando valor == true
  if(m->sig){
    m->bools&=~(valorCadena|valorFinal);
    m->sig->table->generar(m->sig);
    if(m->bools&hasClick)
      m->bools|=valorCadena;
    else
      m->bools|=m->sig->bools&valorCadena;
    m->bools|=m->sig->bools&valorFinal;
  }else
    m->bools|=valorFinal|valorCadena;
}

//para manejar templates
void reaccionarOverload(movHolder* m,normalHolder* nh){
  m->table->reaccionar(m,nh);
}
void reaccionarOverload(movHolder* m,vector<normalHolder*>* nh){
  m->table->reaccionarVec(m,nh);
}

void reaccionarSig(movHolder*m,auto nhs){
  if(m->sig){
    reaccionarOverload(m->sig,nhs);
    if(switchToGen){
      m->bools&=~(valorCadena|valorFinal);
      if(m->bools&hasClick)
        m->bools|=valorCadena;
      else
        m->bools|=m->sig->bools&valorCadena;
      m->bools|=m->sig->bools&valorFinal;
    }
  }
}

inline void generarProperNormalH(normalHolder* n){
  int i=0;
  actualHolder.buffer=(void(**)(void))n->op->conds.beg;
  actualHolder.bufferPos=&i;
  for(bool(**c)(void)=n->op->conds.beg;
      c+i != n->op->conds.after;
      i++)
    if(!(*(c+i))()){
      n->bools&=~(valorFinal|valorCadena|valor);
      return;
    }
  offset=n->pos;

  n->bools|=valor;
  generarSig(n);
}
bool espFail(v pos){
  return pos.x<0||pos.x>=actualHolder.brd->dims.x
       ||pos.y<0||pos.y>=actualHolder.brd->dims.y;
}
void generarNormalH(movHolder* m){
  normalHolder* n=(normalHolder*) m;
  actualHolder.nh=n;
  memcpy(n->memAct.beg,memMov.data,n->base->memLocal.size*sizeof(int));
  n->pos=getActualPos(n->relPos,offset);//pos se calcula siempre porque se usa para actualizar offset
  if(n->bools&doEsp){
    if(espFail(n->pos)){
      n->bools&=~(valorFinal|valorCadena|valor);
      return;
    }
    actualHolder.tile=tile(actualHolder.brd,n->pos); //se me hace raro que tile solo se actualice si es esp, hay que mirar todo el tema de esp TODO

    //los triggers se ponen aca y se reponen en la reaccion. Las normales esp ahora son
    //las normales que se originan despues de un movimiento. (o hacen miran memoria de tile)
    //antes eran las normales que contenian un chequeo posicional, como vacio o memoria de tile,
    //lo que tiene mas sentido. Decidi ir por este otro camino porque la gran mayoria de normales
    //que siguen un movimiento hacen un chequeo posicional (creo que las que no se podrian reescribir siempre),
    //y hacerlo por posicion tiene el problema de que en cosas como w exc mover or captura end, que son comunes,
    //se esta poniendo un trigger en las dos ramas de exc en vez de antes de este, lo que no aprovecha la normal
    //de w que va a estar ahi de todas formas y hace que se tengan multiples triggers, lo que es mas caro.
    //Puede que se pueda hacer un sistema mejor en la version compilada, aunque no sé si lo vale.
    pushTrigger(&actualHolder.tile->triggersUsed,&actualHolder.tile->firstTriggerBox);
  }
  generarProperNormalH(n);
}
void reaccionarNormalH(movHolder* m,normalHolder* nh){
  normalHolder* self=(normalHolder*) m;

  if(nh==self){
    actualHolder.nh=nh;

    memcpy(memMov.data,self->memAct.beg,self->base->memLocal.size*sizeof(int));
    switchToGen=true;

    actualHolder.tile=tile(actualHolder.brd,self->pos);
    pushTrigger(&actualHolder.tile->triggersUsed,&actualHolder.tile->firstTriggerBox);
    generarProperNormalH(self);
    if(!(self->bools&valorFinal)){
      offset=getOffset(self->relPos,self->pos);
      memcpy(memMov.data,self->memAct.beg,self->base->memLocal.size*sizeof(int));
      //esta restauracion esta para que el operador que contenga reciba el offset y mem local correcta
      //la alternativa a hacer esto es que todos los operadores contenedores guarden offset y memoria local
      //para cada iteración/rama, lo que tambien tiene sus desventajas.
    }
  }else if(self->bools&valor){
    reaccionarSig(self,nh);
    if(!(self->bools&valorFinal)){
      offset=getOffset(self->relPos,self->pos);
      memcpy(memMov.data,self->memAct.beg,self->base->memLocal.size*sizeof(int));
    }
  }
}
void reaccionarNormalH(movHolder* m,vector<normalHolder*>* nhs){
  normalHolder* s=(normalHolder*) m;

  for(int i=0;i<nhs->size;++i){
    normalHolder* nh=(*nhs)[i];
    if(nh==s){
      actualHolder.nh=nh;

      memcpy(memMov.data,s->memAct.beg,s->base->memLocal.size*sizeof(int));
      switchToGen=true;
      actualHolder.tile=tile(actualHolder.brd,s->pos);
      pushTrigger(&actualHolder.tile->triggersUsed,&actualHolder.tile->firstTriggerBox);
      generarProperNormalH(s);
      if(!(s->bools&valorFinal)){
        offset=getOffset(s->relPos,s->pos);
        memcpy(memMov.data,s->memAct.beg,s->base->memLocal.size*sizeof(int));
      }
      unorderedErase(nhs,i);
      return;
    }
  }
  if(s->bools&valor){
    reaccionarSig(s,nhs);
    if(!(s->bools&valorFinal)){
      offset=getOffset(s->relPos,s->pos);
      memcpy(memMov.data,s->memAct.beg,s->base->memLocal.size*sizeof(int));
    }
  }
}

void accionarNormalH(normalHolder* n){
  actualHolder.h=n->base->h;
  actualHolder.nh=n;
  actualHolder.tile=tile(actualHolder.brd,n->pos);

  int i=0;
  actualHolder.buffer=n->op->accs.beg;
  actualHolder.bufferPos=&i;

  for(void(**func)(void)=n->op->accs.beg;
      func+i != n->op->accs.after;
      i++)
    (*(func+i))();
}
void cargarNormalH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(n,m,normalHolder*);
  if(!(n->bools&valorCadena)) return;
  push(norms,n);
  if(n->bools&makeClick)
    makeClicker(norms,n->base->h);
  if(n->sig)
    n->sig->table->cargar(n->sig,norms);
}
void drawNormalH(normalHolder* n){
  actualHolder.nh=n;
  for(int i:n->op->colors){
    coloresImp[i].draw();
  }
}

 virtualTableMov normalTable={generarNormalH,reaccionarNormalH,reaccionarNormalH,cargarNormalH};

void initNormalH(normal* org,Base* base_,char** head){
  fromCast(n,*head,normalHolder*);
  initMovH(n,org,base_);
  n->table=&normalTable;
  
  *head+=sizeof(normalHolder);

  n->op=org;
  /*for(auto trigInfo:n->op->setUpMemTriggersPerNormalHolder)
    switch(trigInfo.type){
    case 0:
      memGlobalTriggers[trigInfo.ind].perma.push_back(n);
      break;case 1:
      base_->h->memPiezaTrigs[trigInfo.ind]->perma.push_back(n);
      break;case 2:
      turnoTrigs[base_->h->bando].push_back({base_->h,n}); ///@check
      }*/
  n->memAct.beg=(int*)*head;
  n->memAct.after=((int*)*head)+base_->memLocal.size;
  *head=(char*)n->memAct.after;

  n->relPos=n->op->relPos;
}


//TODO se me hace raro que se creen en el momento, no sería costoso armarlos todos al principio
void maybeAddIteration(deslizHolder*d,int i){
  if(d->cantElems==i){
    char* place=(char*)d->movs.beg+d->movs.elemSize*i;//crearMovHolder necesita **
    crearMovHolder(d->op->inside,d->base,&place);
    d->cantElems++;
    assert(d->cantElems<=count(d->movs));
  }
}
void generarDeslizH(movHolder* m){
  fromCast(d,m,deslizHolder*);
  movHolder* act;
  int i=0;
  for(;;){
    v offsetOrg=offset;
    act=(movHolder*)d->movs[i];
    act->table->generar(act);
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
inline void reaccionarNhDesliz(deslizHolder* d,normalHolder* nh){
  assert((char*)nh>=(char*)d->movs.beg);
  intptr diff=(char*)nh-(char*)d->movs.beg;
  int iter=diff/d->op->iterSize;
  for(int i=0;i<iter;++i){
    if(!(((movHolder*)d->movs[i])->bools&valorFinal)){
      return;
    }
  }
  movHolder* act=(movHolder*)d->movs[iter];
  act->table->reaccionar(act,nh);
  //assert(switchToGen); TODO no es valido porque los isols pueden apagarlo. Si hago un lngjmp en isol si se podria volver a meter
  /*if(!switchToGen) return;//por isols nomas. Si hago lo del lngjmp no es necesario
    para que funcione correctamente debería ser return solo en la version simple, en la version vector
    debería llamarse a si misma, sacando el nh mas bajo. Por ahora no lo hago porque es una optimizacion
  */
  for(;act->bools&valorFinal;){
    iter++;
    maybeAddIteration(d,iter);
    act=(movHolder*)d->movs[iter];
    act->table->generar(act);
  }
  if(act->bools&valorCadena)
    d->bools|=lastNotFalse;
  else
    d->bools&=~lastNotFalse;
  d->f=iter;
  generarSig(d);
}
void reaccionarDeslizH(movHolder* m,normalHolder* nh){
  deslizHolder* d=(deslizHolder*)m;
  if((char*)nh<(char*)d+d->op->insideSize)
    reaccionarNhDesliz(d,nh);
  else
    reaccionarSig(d,nh);
}
void reaccionarDeslizH(movHolder*m,vector<normalHolder*>* nhs){
  deslizHolder* d=(deslizHolder*)m;
  if((char*)(*nhs)[0]<(char*)d+d->op->insideSize)
    reaccionarNhDesliz(d,(*nhs)[0]);
  else
    reaccionarSig(d,nhs);
}
void cargarDeslizH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(d,m,deslizHolder*);
  if(!(d->bools&valorCadena)) return;
  for(int i=0;i<(d->bools&lastNotFalse?d->f+1:d->f);i++){
    fromCast(mov,d->movs[i],movHolder*);
    mov->table->cargar(mov,norms);
  }
  if(d->bools&makeClick&&!norms->size==0) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
    makeClicker(norms,d->base->h);
  if(d->sig)
    d->sig->table->cargar(d->sig,norms);
}

virtualTableMov deslizTable={generarDeslizH,reaccionarDeslizH,reaccionarDeslizH,cargarDeslizH};
//esta version del desliz retrocede a la posicion de inicio de una iteracion si la iteracion falla
//esto la hace mas consistente en casos donde se usen multiples normales y cosas asi. Pero para
//casos comunes como la torre no se usa, agrega codigo y almacenamiento, y lo mas importante es
//que genera multiples triggers. No vale la pena agregar otra version ahora para no hacer bloat,
//pero en la version compilada podria estar. Tambien se podria agregar instrucciones bizarras
//solo de desliz sin mucho problema, como cambiar la pos de retorno o breaks.
void initDeslizH(desliz* org,Base* base_,char** head){
  fromCast(d,*head,deslizHolder*);

  initMovH(d,org,base_);
  d->table=&deslizTable;

  *head+=sizeof(deslizHolder);
  d->op=org;
  d->movs.beg=*head;
  d->movs.after=(char*)d->movs.beg+org->insideSize;
  d->movs.elemSize=org->iterSize;

  crearMovHolder(org->inside,base_,head);//crear primera iteracion
  *head=(char*)d->movs.after;
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

void generarExcH(movHolder* m){
  fromCast(e,m,excHolder*);

  int i;
  v offsetOrg=offset;
  for(i=0;i<count(e->ops);i++){
    movHolder* branch=e->ops[i];
    branch->table->generar(branch);
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
inline void reaccionarNhExcH(excHolder* e,normalHolder* nh){
  movHolder* branch;
  int i;
  for(i=1;i<=e->actualBranch;++i){
    movHolder* nextBranch=e->ops[i];
    if(nextBranch>nh){
      branch=e->ops[i-1];
      goto branchFound;
    }
  }
  branch=e->ops[e->actualBranch];
  //TODO no me cierra esto. Si el nh esta en una rama despues de la actual no se debería hacer nada, aca se hace reaccionar a la actual
 branchFound:
  branch->table->reaccionar(branch,nh);
  if(switchToGen){//solo falso si la nh es innaccesible, por ejemplo esta en la parte invalida de un desliz
    if(!(branch->bools&valorCadena)){ //si el ab al recalcularse se invalida generar todo devuelta, saltandolo
      int j;
      for(j=i;j<count(e->ops);j++){
        movHolder* brancj=e->ops[j];
        brancj->table->generar(brancj);
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
  }
}
void reaccionarExcH(movHolder* m,normalHolder* nh){
  fromCast(e,m,excHolder*);
  if((char*)nh-(char*)e<e->size){
    reaccionarNhExcH(e,nh);
  }else if(e->bools&valor)
    reaccionarSig(e,nh);
}
void reaccionarExcH(movHolder*m,vector<normalHolder*>* nhs){
  //no puedo asumir que la primera normal es la que va a causar el cambio a generacion
  //porque puede que esta este en un lugar innaccesible
  fromCast(e,m,excHolder*);
  for(normalHolder* nh:*nhs){
    if((char*)nh-(char*)e<e->size){
      reaccionarNhExcH(e,nh);
      if(switchToGen){
        break;
      }
    }else{
      reaccionarSig(e,nh);
      return;
    }
  }
}
void cargarExcH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(e,m,excHolder*);
  if(!(e->bools&valorCadena)) return;
  movHolder* branch=e->ops[e->actualBranch];
  branch->table->cargar(branch,norms);
  if(e->bools&makeClick)
    makeClicker(norms,e->base->h);
  if(e->sig&&(branch->bools&valorFinal))
    e->sig->table->cargar(e->sig,norms);
}

virtualTableMov excTable={generarExcH,reaccionarExcH,reaccionarExcH,cargarExcH};
void initExcH(exc* org,Base* base_,char** head){
  fromCast(e,*head,excHolder*);
  initMovH(e,org,base_);
  e->table=&excTable;

  *head+=sizeof(excHolder);
  e->ops.beg=(movHolder**)*head;
  e->ops.after=(movHolder**)(*head+size(org->ops));
  *head=(char*)e->ops.after;
  e->size=org->insideSize+sizeof(excHolder);
  int i=0;
  for(operador* opos:org->ops){
    *(e->ops.beg+i++)=(movHolder*)*head;
    crearMovHolder(opos,base_,head);
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



void generarIsolH(movHolder* m){
  fromCast(s,m,isolHolder*);

  int resetSize=s->base->memLocal.resetUntil*sizeof(int);
  storeState(resetSize);
  s->inside->table->generar(s->inside);
  restoreState(resetSize);

  if(s->sig){
    s->sig->table->generar(s->sig);
    s->bools&=~valorFinal;
    s->bools|=s->sig->bools&valorFinal;
  }
}
void reaccionarIsolH(movHolder* m,normalHolder* nh){
  fromCast(s,m,isolHolder*);
  if((char*)nh-(char*)s<s->size){
    s->inside->table->reaccionar(s->inside,nh);
    if(switchToGen){
      ///@optim podria hacerse un lngjmp
      switchToGen=false;
      //printf("s coming back\n");
    }
  }
  else if(s->sig)
    s->sig->table->reaccionar(s->sig,nh);
}
void reaccionarIsolH(movHolder* m,vector<normalHolder*>* nhs){
  fromCast(s,m,isolHolder*);
  if((char*)(*nhs)[0]-(char*)s<s->size){
    s->inside->table->reaccionar(s->inside,(*nhs)[0]);
    if(switchToGen){
      switchToGen=false;
      //printf("v coming back\n");
      //aclaracion: aca la memoria tiene basura que se cargo en la generacion interna, no importa porque se volvio a modo reaccion y se va a pisar si se activa un proximo trigger
    }
    //unorderedErase(nhs,0); TODO mirar esto
  }
  if(s->sig)
    s->sig->table->reaccionarVec(s->sig,nhs);
}
void cargarIsolH(movHolder*m,vector<normalHolder*>* norms){
  fromCast(s,m,isolHolder*);
  int sizeBefore=norms->size;
  s->inside->table->cargar(s->inside,norms);
  if(s->bools&makeClick&&!norms->size==0)//evitar generar clickers sin normales
    makeClicker(norms,s->base->h);
  norms->size=sizeBefore;
  if(s->sig)
    s->sig->table->cargar(s->sig,norms);
}

 virtualTableMov isolTable={generarIsolH,reaccionarIsolH,reaccionarIsolH,cargarIsolH};
void initIsolH(isol* org,Base* base_,char** head){
  fromCast(s,*head,isolHolder*);
  initMovH(s,org,base_);
  s->table=&isolTable;

  *head+=sizeof(isolHolder);
  s->inside=(movHolder*)*head;
  crearMovHolder(org->inside,base_,head);
  s->bools|=valor|valorFinal|valorCadena;
  s->size=org->size;
}



#define desoptMov ((char*)d+sizeof(desoptHolder))

void construirYGenerarNodo(desoptHolder*,int);

void generarDesoptH(movHolder* m){
  //se tiene la base y primera iteracion construidos, rondas mas alla de estas usan el resto de memoria
  //de forma dinamica. Es un punto intermedio entre tener todo construido y reservar mucha memoria que probablemente
  //no se use, y hacer todo dinamico y estar reconstruyendo operadores en cada generacion
  //Seria lo mas optimizado para la dama

  //con limpiar la caja de movHolders de la primera iteracion antes de la generacion total
  //y sobreescribir el espacio dinamico no deberia haber ningun problema de construcciones kakeadas y cosas asi
  fromCast(d,m,desoptHolder*);

  d->dinamClusterHead=desoptMov+d->op->dinamClusterBaseOffset;

  int clusterOffset=0;
  char* correspondingCluster=desoptMov+d->op->clusterSize;

  int resetSize=d->base->memLocal.resetUntil*sizeof(int);

  storeState(resetSize);
  
  void* memTemp2=alloca(resetSize);

  for(int tam:d->op->movSizes){
    desoptHolder::node* firstIter=(desoptHolder::node*)(desoptMov+clusterOffset);
    //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
    movHolder* actualMov=(movHolder*)(firstIter+1);

    actualMov->table->generar(actualMov);
    if(actualMov->bools&valorFinal){
      firstIter->iter=(desoptHolder::node*)correspondingCluster;
      int clusterOffset2=0;
      v offsetOrg2=offset;
      memcpy(memTemp2,memMov.data,resetSize);

      for(int tam:d->op->movSizes){
        desoptHolder::node* secondIter=(desoptHolder::node*)((char*)firstIter->iter+clusterOffset2);
        movHolder* actualMov2=(movHolder*)(secondIter+1);

        actualMov2->table->generar(actualMov2);
        if(actualMov2->bools&valorFinal){
          secondIter->iter=(desoptHolder::node*)d->dinamClusterHead;
          construirYGenerarNodo(d,resetSize);
        }else
          secondIter->iter=nullptr;
        clusterOffset2+=tam; //@optim por ahi vale la pena saltarse esto en la ultima iteracion? en el bucle exterior dependeria de si hay sig
        offset=offsetOrg2;
        memcpy(memMov.data,memTemp2,resetSize);
      }
    }else
      firstIter->iter=nullptr;
    clusterOffset+=tam;
    correspondingCluster+=d->op->clusterSize;
    restoreState(resetSize);
  }
  if(d->sig)//TODO deberia propagar el valorFinal como isol
    d->sig->table->generar(d->sig);
}
//TODO es necesario contruir en el momento? no debería dar lo mismo construir todo al principio, si son siempre los mismos bloques?
void construirYGenerarNodo(desoptHolder* d,int resetSize){
  char* clusterBeg=d->dinamClusterHead;
  assert(d->dinamClusterHead<desoptMov+d->op->desoptInsideSize);
  for(operador* opos:d->op->ops){
    *(movHolder**)d->dinamClusterHead=nullptr;
    d->dinamClusterHead+=sizeof(desoptHolder::node);
    crearMovHolder(opos,d->base,&d->dinamClusterHead);
  }
  int clusterOffset=0;
  storeState(resetSize);

  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)(clusterBeg+clusterOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);

    actualMov->table->generar(actualMov);
    if(actualMov->bools&valorFinal){
      nextIter->iter=(desoptHolder::node*)d->dinamClusterHead;
      construirYGenerarNodo(d,resetSize);
    }
    clusterOffset+=tam;
    restoreState(resetSize);
  }
}
void generarNodo(desoptHolder* d,desoptHolder::node** iter,int resetSize){
  if(*iter){
    int clusterOffset=0;
    storeState(resetSize);

    for(int tam:d->op->movSizes){
      desoptHolder::node* nextIter=(desoptHolder::node*)((char*)*iter+clusterOffset);
      movHolder* actualMov=(movHolder*)(nextIter+1);
      actualMov->table->generar(actualMov);
      if(actualMov->bools&valorFinal){
        generarNodo(d,&nextIter->iter,resetSize);
      }
      clusterOffset+=tam;
      restoreState(resetSize);
    }
  }else{
    *iter=(desoptHolder::node*)d->dinamClusterHead;
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
void reactIfNh(normalHolder* nh,movHolder* actualMov,int tam){
  if((char*)nh>=(char*)actualMov&&(char*)nh<(char*)actualMov+tam){
    actualMov->table->reaccionar(actualMov,nh);
  }
}
void reactIfNh(vector<normalHolder*>* nhs,movHolder* actualMov,int tam){
  for(normalHolder* nh:*nhs){
    reactIfNh(nh,actualMov,tam);
    if(switchToGen) break;
  }
}
template<typename T>
void reaccionarProperDesoptH(desoptHolder* d,T nh,desoptHolder::node* iter,bool* found){
  //@optim en vez de hacer esto se podría avanzar por la memoria, aprovechando que todos los bloques miden lo mismo,
  //y meterse directamente en el correcto. Aunque como no se estaria recorriendo se podría acceder a memoria muerta, no sé.
  int branchOffset=0;
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+branchOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    branchOffset+=tam;

    reactIfNh(nh,actualMov,tam);

    if(switchToGen){
      if constexpr(std::is_same<T,normalHolder*>::value){
        *found=true;
      }else{
        unorderedErase(nh,0);
        if(nh->size==0)
          *found=true;
      }
    }

    if(actualMov->bools&valorFinal){
      if(switchToGen){
        generarNodo(d,&nextIter->iter,d->base->memLocal.resetUntil*sizeof(int));
      }else
        reaccionarProperDesoptH(d,nh,nextIter->iter,found);
    }
    switchToGen=false;
    if(*found)
      return;
  }
}
template<void(*reaccionarProper)(desoptHolder*,normalHolder*,desoptHolder::node*,bool*)>
void reaccionarDesoptH(movHolder* m,normalHolder* n){
  fromCast(d,m,desoptHolder*);
  bool found=false;
  if((char*)n<(char*)d+sizeof(desoptHolder)+d->op->desoptInsideSize){
    reaccionarProper(d,n,d->movs,&found);
  }else{
    d->sig->table->reaccionar(d->sig,n);
  }
}
template<void(*reaccionarProper)(desoptHolder*,vector<normalHolder*>*,desoptHolder::node*,bool*)>
void reaccionarDesoptH(movHolder* m,vector<normalHolder*>* n){
  fromCast(d,m,desoptHolder*);
  bool found=false;
  if((char*)(n->operator[](0))<(char*)d+sizeof(desoptHolder)+d->op->desoptInsideSize){
    reaccionarProper(d,n,d->movs,&found);
  }else{
    d->sig->table->reaccionarVec(d->sig,n);
  }
}
/*
  como se reutiliza espacios de memoria puede que una rama ponga un trigger, y en una regeneracion por un trigger anterior la rama
  quede invalidada, y se reutilice para otra memoria. Ahora este trigger invalido, a diferencia de desliz, no apunta a un lugar que
  no se va a encontrar nunca. Apunta a un lugar que esta siendo usado para otra cosa, y esto va a causar una regeneracion al pedo. No
  va a crear comportamientos erroneos, es calculos al pedo nomas. No creo que pase muy seguido igual, y si agrego la optimizacion de no
  seguir generando si la normal que se activó no cambio su valor de validez, el impacto de esto se reduce a solo recalcular una normal.
 */
void cargarNodos(movHolder* m,desoptHolder::node* iter,vector<normalHolder*>* norms){
  fromCast(d,m,desoptHolder*);
  int res=norms->size;
  int offset=0;
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+offset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    if(actualMov->bools&valorFinal){//TODO falta manejar estado intermedio no?
      actualMov->table->cargar(actualMov,norms);
      assert(nextIter->iter);
      cargarNodos(d,nextIter->iter,norms);
    }else if((d->bools&makeClick)&&norms->size!=0){
      makeClicker(norms,d->base->h);
    }
    offset+=tam;
    norms->size=res;
  }
}
void cargarDesoptH(movHolder*m,vector<normalHolder*>* norms){
  fromCast(d,m,desoptHolder*);
  cargarNodos(d,(desoptHolder::node*)desoptMov,norms);
  if(m->sig)
    m->sig->table->cargar(m->sig,norms);
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

virtualTableMov desoptTable={generarDesoptH,reaccionarDesoptH<reaccionarProperDesoptH>,
                              reaccionarDesoptH<reaccionarProperDesoptH>,cargarDesoptH};
void initDesoptH(desopt* org,Base* base_,char** head){
  fromCast(d,*head,desoptHolder*);
  initMovH(d,org,base_);
  d->table=&desoptTable;

  d->op=org;

  *head+=sizeof(desoptHolder);
  char* nextIteration=*head;
  char* headFirst=*head;
  for(operador* opos:org->ops){//armar base
    assert(sizeof(movHolder*)==sizeof(char*));//debe haber una forma menos sospechosa de hacer esto
    **((movHolder***)head)=(movHolder*)(nextIteration+=d->op->clusterSize);//head apunta a puntero al proximo cluester de movimientos,
    *head+=sizeof(desoptHolder::node*);//que corresponde a la proxima iteracion de este movHolder
    crearMovHolder(opos,base_,head);
  }
  for(int i=0;i<count(org->ops);i++){//armar primer iteracion
    for(operador* opos:org->ops){
      **(movHolder***)head=nullptr;//cuando se use apunta a un espacio dinamico
      *head+=sizeof(desoptHolder::node*);
      crearMovHolder(opos,base_,head);
    }
  }
  *head=headFirst+org->desoptInsideSize;
  d->bools|=valorCadena|valorFinal;
}




//fail tambien se podría implementar retornando falso en todo y forzando el operador anterior a tener hasClick,
//las 2 cosas resultan en generarSig y simil de isol haciendo que se propague valorCadena true y valorFinal false
//elegi esta porque me parece mas simple
void generarFail(movHolder* m){
  m->bools=valorCadena; //valorFinal=false, el resto no importa
  return;
}
void reaccionarFail(movHolder* m, normalHolder* nh){}
void reaccionarFail(movHolder* m, vector<normalHolder*>* nhs){}
void cargarFail(movHolder* m, vector<normalHolder*>* norms){}
virtualTableMov failTable={generarFail,reaccionarFail,reaccionarFail,cargarFail};

void initFailH(char** head){
  fromCast(f,*head,movHolder*);
  f->table=&failTable;
  f->base=nullptr;
  f->bools=0;
  *head+=sizeof(movHolder);
}



normalHolder* getNextNormalH(movHolder*);
void reaccionarIsolNonResetMemH(movHolder* m,normalHolder* nh){
  fromCast(s,m,isolHolder*);
  //printf("isol NRM\n");
  if((char*)nh-(char*)s<s->size){
    normalHolder* nh=getNextNormalH(s->inside);
    v posBefore=getOffset(nh->relPos,nh->pos);
    int resetSize=s->base->memLocal.resetUntil*sizeof(int);
    void* memTemp=alloca(resetSize);
    memcpy(memTemp,nh->memAct.beg,resetSize);

    s->inside->table->reaccionar(s->inside,nh);
    if(switchToGen){
      offset=posBefore;
      memcpy(memMov.data,memTemp,resetSize);

      //se sigue generando porque potencialmente se cambio memoria que no se resetea
      s->sig->table->generar(s->sig);
      //switchToGen queda activado para que el contender empieze a generar
    }
  }
  else if(s->sig)
    s->sig->table->reaccionar(s->sig,nh);
}
#define storeFirstNormalState(movHolder)        \
  normalHolder* tempNh=getNextNormalH(movHolder);   \
  v tempPos=getOffset(tempNh->relPos,tempNh->pos);      \
  memcpy(tempLocalMem,tempNh->memAct.beg,resetSize);

void reaccionarIsolNonResetMemH(movHolder* m,vector<normalHolder*>* nhs){
  fromCast(s,m,isolHolder*);
  //printf("isol NRM\n");
  if((char*)(*nhs)[0]-(char*)s<s->size){
    int resetSize=s->base->memLocal.resetUntil*sizeof(int);
    void* tempLocalMem=alloca(resetSize);
    storeFirstNormalState(s->inside);

    s->inside->table->reaccionar(s->inside,(*nhs)[0]);
    if(switchToGen){
      restoreState(resetSize);
      s->sig->table->generar(s->sig);
      return;
    }
  }
  if(s->sig)
    s->sig->table->reaccionarVec(s->sig,nhs);
}


virtualTableMov isolNRMTable={generarIsolH,reaccionarIsolNonResetMemH,reaccionarIsolNonResetMemH,cargarIsolH};
void initIsolNonResetMemH(isol* org,Base* base_,char** head){
  fromCast(s,*head,isolHolder*);
  initIsolH(org,base_,head);
  s->table=&isolNRMTable;
}



#define storeStateNoAlloc(resetSize)            \
  v tempPos=offset;                             \
  memcpy(tempLocalMem,memMov.data,resetSize);
//desoptNRM se usa si alguna rama usa memoria no reseteable. Aunque una rama no toque esa memoria, que se haya disparado su trigger implica
//que potencialmente va a invalidar o crear nodos futuros, y por lo menos uno de esos nodos si toca la memoria.
template<typename T,bool firstReaccionar=true>
void reaccionarProperDesoptNonResetMemH(desoptHolder* d,T nh,desoptHolder::node* iter,bool* found){
  int branchOffset=0;
  int resetSize=d->base->memLocal.resetUntil*sizeof(int);
  void* tempLocalMem=alloca(resetSize);
  //printf("desopt NRM\n");
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+branchOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    branchOffset+=tam;


    if(switchToGen){
      //tengo que volver a generar todos los nodos que estan despues en el orden de generacion, no solo los siguientes al nodo que disparo el trigger
      storeStateNoAlloc(resetSize);
      actualMov->table->generar(actualMov);
      if(actualMov->bools&valorFinal){
        generarNodo(d,&nextIter->iter,resetSize);
      }
      restoreState(resetSize);
    }else{
      storeFirstNormalState(actualMov);
      reactIfNh(nh,actualMov,tam);
      if(switchToGen){
        if(actualMov->bools&valorFinal){
          generarNodo(d,&nextIter->iter,resetSize);
        }
      }else if(actualMov->bools&valorFinal){
        reaccionarProperDesoptNonResetMemH<T,false>(d,nh,nextIter->iter,found);
      }
      restoreState(resetSize);
    }
  }
  if constexpr(firstReaccionar){
      if(switchToGen&&d->sig){
        d->sig->table->generar(d->sig);
      }
    }
}

virtualTableMov desoptNRMTable={generarDesoptH,reaccionarDesoptH<reaccionarProperDesoptNonResetMemH>,
                                reaccionarDesoptH<reaccionarProperDesoptNonResetMemH>,cargarDesoptH};
void initDesoptNonResetMemH(desopt* org,Base* base_,char** head){
  //printf("gbdfgbdf\n");
  fromCast(d,*head,desoptHolder*);
  initDesoptH(org,base_,head);
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
    return getNextNormalH((movHolder*)((deslizHolder*)m)->movs[0]);
  if(m->table==&excTable)
    return getNextNormalH(((excHolder*)m)->ops[0]);
  if(m->table==&isolTable || m->table==&isolNRMTable)
    return getNextNormalH(((isolHolder*)m)->inside);
  if(m->table==&desoptTable || m->table==&desoptNRMTable){
    desoptHolder::node* firstIter=(desoptHolder::node*)((char*)m+sizeof(desoptHolder));
    return getNextNormalH((movHolder*)(firstIter+1));
  }
  fail("getNextNormalH didn't found nh\n");
  return nullptr;
}




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
