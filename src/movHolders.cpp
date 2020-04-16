
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
      if(m->bools&hasClick)
        m->bools|=valorCadena;
      else
        m->bools|=m->sig->bools&valorCadena;
      m->bools|=m->sig->bools&valorFinal;
    }
  }
}

inline void generarProperNormalH(normalHolder* n){
  n->bools&=~(valorFinal|valorCadena|valor);
  int i=0;
  actualHolder.buffer=(void(**)(void))n->op->conds.beg;
  actualHolder.bufferPos=&i;
  for(bool(**c)(void)=n->op->conds.beg;
      c+i != n->op->conds.after;
      i++)
    if(!(*(c+i))()){
      return;
    }
  offset=n->pos;

  n->bools|=valor;
  generarSig(n);
}
void generarNormalH(movHolder* m){
  normalHolder* n=(normalHolder*) m;
  actualHolder.nh=n;
  memcpy(n->memAct.beg,memMov.data,n->base->memLocalSize*sizeof(int));
  n->pos=getActualPos(n->relPos,offset);//pos se calcula siempre porque se usa para actualizar offset
  if(n->bools&doEsp){
    if(n->pos.x<0||n->pos.x>=actualHolder.brd->dims.x||n->pos.y<0||n->pos.y>=actualHolder.brd->dims.y){
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

    memcpy(memMov.data,self->memAct.beg,self->base->memLocalSize*sizeof(int));
    switchToGen=true;

    actualHolder.tile=tile(actualHolder.brd,self->pos);
    pushTrigger(&actualHolder.tile->triggersUsed,&actualHolder.tile->firstTriggerBox);
    generarProperNormalH(self);
    if(!(self->bools&valorFinal)){
      offset=getOffset(self->relPos,self->pos);
      memcpy(memMov.data,self->memAct.beg,self->base->memLocalSize*sizeof(int));
      //esta restauracion esta para que el operador que contenga reciba el offset y mem local correcta
      //la alternativa a hacer esto es que todos los operadores contenedores guarden offset y memoria local
      //para cada iteración/rama, lo que tambien tiene sus desventajas.
    }
  }else if(valor){ //@check esto tiene sentido?
    reaccionarSig(self,nh);
    if(!(self->bools&valorFinal)){
      offset=getOffset(self->relPos,self->pos);
      memcpy(memMov.data,self->memAct.beg,self->base->memLocalSize*sizeof(int));
    }
  }
}
void reaccionarNormalH(movHolder* m,vector<normalHolder*>* nhs){
  normalHolder* s=(normalHolder*) m;

  for(int i=0;i<nhs->size;++i){
    normalHolder* nh=(*nhs)[i];
    if(nh==s){
      memcpy(memMov.data,s->memAct.beg,s->base->memLocalSize*sizeof(int));
      switchToGen=true;
      actualHolder.tile=tile(actualHolder.brd,s->pos);
      pushTrigger(&actualHolder.tile->triggersUsed,&actualHolder.tile->firstTriggerBox);
      generarProperNormalH(s);
      if(!(s->bools&valorFinal)){
        offset=getOffset(s->relPos,s->pos);
        memcpy(memMov.data,s->memAct.beg,s->base->memLocalSize*sizeof(int));
      }
      unorderedErase(nhs,i);
      return;
    }
  }
  if(valor){
    reaccionarSig(s,nhs);
    if(!(s->bools&valorFinal)){
      offset=getOffset(s->relPos,s->pos);
      memcpy(memMov.data,s->memAct.beg,s->base->memLocalSize*sizeof(int));
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
    push(&clickers,makeClicker(norms,n->base->h));
  if(n->sig)
    n->sig->table->cargar(n->sig,norms);
}
void drawNormalH(normalHolder* n){
  actualHolder.nh=n;
  for(colort* d:n->op->colors){
    d->draw();
  }
}

 virtualTableMov normalTable={generarNormalH,reaccionarNormalH,reaccionarNormalH,cargarNormalH};

void initNormalH(normal* org,Base* base_,char** head)
{
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
  n->memAct.after=((int*)*head)+base_->memLocalSize;
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
    push(&clickers,makeClicker(norms,d->base->h));
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




void generarExcH(movHolder* m){
  fromCast(e,m,excHolder*);

  int i;
  v offsetOrg=offset;
  for(i=0;i<count(e->ops);i++){
    movHolder* branch=*e->ops[i];
    branch->table->generar(branch);
    if(branch->bools&valorCadena){
      //si una rama con un clicker puso el clicker y despues fallo se toma como rama buena,
      //pero la pos vuelve al origen. Medio raro. Por ahora lo dejo asi porque asi esta el sistema,
      //si se quiere hacer algo distinto no sería problema agregarlo en la version compilada
      e->bools|=valor;
      e->actualBranch=i; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que valorCadena==true
      generarSig(e);
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
    movHolder* nextBranch=*e->ops[i];
    if(nextBranch>nh){
      branch=*e->ops[i-1];
      goto branchFound;
    }
  }
  branch=*e->ops[e->actualBranch];
 branchFound:
  branch->table->reaccionar(branch,nh);
  if(switchToGen){//solo falso si la nh es innaccesible, por ejemplo esta en la parte invalida de un desliz
    if(!(branch->bools&valorCadena)){ //si el ab al recalcularse se invalida generar todo devuelta, saltandolo
      int j;
      for(j=0;j<count(e->ops);j++){
        if(i-1!=j){
          movHolder* brancj=*e->ops[j];
          brancj->table->generar(brancj);
          if(brancj->bools&valorCadena){
            e->bools|=valor;
            e->actualBranch=j;
            generarSig(e);
            return;
          }
        }
      }
      e->bools&=~(valorCadena|valor);
      e->actualBranch=j-1;
    }else{ //se valido una rama que era invalida
      e->actualBranch=i-1;
      e->bools|=valor;
      generarSig(e);
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
  if(!valorCadena) return;
  movHolder* branch=*e->ops[e->actualBranch];
  branch->table->cargar(branch,norms);
  if(e->bools&makeClick)
    push(&clickers,makeClicker(norms,e->base->h));
  if(e->sig)
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




void generarIsolH(movHolder* m){
  fromCast(s,m,isolHolder*);

  v tempPos=offset;
  void* memTemp=alloca(s->base->memLocalSize*sizeof(int));
  memcpy(memTemp,memMov.data,s->base->memLocalSize*sizeof(int));
  s->inside->table->generar(s->inside);
  offset=tempPos;
  memcpy(memMov.data,memTemp,s->base->memLocalSize*sizeof(int));
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
      ///@optim podria hacerse un lngjmp
      switchToGen=false;
    }
  }
  if(s->sig)
    s->sig->table->reaccionarVec(s->sig,nhs);
}
void cargarIsolH(movHolder*m,vector<normalHolder*>* norms){
  fromCast(s,m,isolHolder*);
  int sizeBefore=norms->size;
  s->inside->table->cargar(s->inside,norms);
  if(s->bools&makeClick&&!norms->size==0)//evitar generar clickers sin normales
    push(&clickers,makeClicker(norms,s->base->h));
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
  s->bools|=valorFinal|valorCadena;
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

  v offsetOrg=offset;

  int localMemSize=d->base->memLocalSize*sizeof(int);
  void* memTemp=alloca(localMemSize);
  memcpy(memTemp,memMov.data,localMemSize);

  void* memTemp2=alloca(localMemSize);

  for(int tam:d->op->movSizes){
    desoptHolder::node* firstIter=(desoptHolder::node*)(desoptMov+clusterOffset);
    //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
    movHolder* actualMov=(movHolder*)(firstIter+1);

    actualMov->table->generar(actualMov);
    if(actualMov->bools&valorFinal){
      firstIter->iter=(desoptHolder::node*)correspondingCluster;
      int clusterOffset2=0;
      v offsetOrg2=offset;
      memcpy(memTemp2,memMov.data,localMemSize);

      for(int tam:d->op->movSizes){
        desoptHolder::node* secondIter=(desoptHolder::node*)((char*)firstIter->iter+clusterOffset2);
        movHolder* actualMov2=(movHolder*)(secondIter+1);

        actualMov2->table->generar(actualMov2);
        if(actualMov2->bools&valorFinal){
          secondIter->iter=(desoptHolder::node*)d->dinamClusterHead;
          construirYGenerarNodo(d,localMemSize);
        }else
          secondIter->iter=nullptr;
        clusterOffset2+=tam;
        offset=offsetOrg2;
        memcpy(memMov.data,memTemp2,localMemSize);
      }
    }else
      firstIter->iter=nullptr;
    clusterOffset+=tam;
    correspondingCluster+=d->op->clusterSize;
    offset=offsetOrg;
    memcpy(memMov.data,memTemp,localMemSize);
  }
}
void construirYGenerarNodo(desoptHolder* d,int localMemSize){
  char* clusterBeg=d->dinamClusterHead;
  assert(d->dinamClusterHead<desoptMov+d->op->desoptInsideSize);
  for(operador* opos:d->op->ops){
    *(movHolder**)d->dinamClusterHead=nullptr;
    d->dinamClusterHead+=sizeof(desoptHolder::node);
    crearMovHolder(opos,d->base,&d->dinamClusterHead);
  }
  int clusterOffset=0;
  v offsetOrg=offset;
  void* memTemp=alloca(localMemSize);
  memcpy(memTemp,memMov.data,localMemSize);

  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)(clusterBeg+clusterOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);

    actualMov->table->generar(actualMov);
    if(actualMov->bools&valorFinal){
      nextIter->iter=(desoptHolder::node*)d->dinamClusterHead;
      construirYGenerarNodo(d,localMemSize);
    }
    clusterOffset+=tam;
    offset=offsetOrg;
    memcpy(memMov.data,memTemp,localMemSize);
  }
}
void generarNodo(desoptHolder* d,desoptHolder::node* iter,int localMemSize){//iter valido
  int clusterOffset=0;
  v offsetOrg=offset;

  void* memTemp=alloca(localMemSize);
  memcpy(memTemp,memMov.data,localMemSize);
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+clusterOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    actualMov->table->generar(actualMov);
    if(actualMov->bools&valorFinal){
      if(nextIter->iter){
        generarNodo(d,nextIter->iter,localMemSize);
      }else{
        nextIter->iter=(desoptHolder::node*)d->dinamClusterHead;
        construirYGenerarNodo(d,localMemSize);
      }
    }
    clusterOffset+=tam;
    offset=offsetOrg;
    memcpy(memMov.data,memTemp,localMemSize);
  }
}
void reactIfNh(normalHolder* nh,movHolder* actualMov,int tam){
  if((char*)nh>=(char*)actualMov&&(char*)nh<(char*)actualMov+tam){
    actualMov->table->reaccionar(actualMov,nh);
  }
}
void reactIfNh(vector<normalHolder*>* nhs,movHolder* actualMov,int tam){
  for(normalHolder* nh:*nhs){
    reactIfNh(nh,actualMov,tam);
  }
}
template<typename T>
void reaccionarProperDesoptH(desoptHolder* d,T nh,desoptHolder::node* iter){
  int offset=0;
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+offset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    offset+=tam;

    reactIfNh(nh,actualMov,tam);

    if(actualMov->bools&valorFinal){
      if(switchToGen){
        if(nextIter->iter)
          generarNodo(d,nextIter->iter,d->base->memLocalSize*sizeof(int));
        else{
          nextIter->iter=(desoptHolder::node*)d->dinamClusterHead;
          construirYGenerarNodo(d,d->base->memLocalSize*sizeof(int));
        }
      }else
        reaccionarProperDesoptH(d,nh,nextIter->iter);
    }
    switchToGen=false;
  }
}
void reaccionarDesoptH(movHolder* m,normalHolder* n){
  fromCast(d,m,desoptHolder*);
  reaccionarProperDesoptH(d,n,d->movs);
}
void reaccionarDesoptH(movHolder* m,vector<normalHolder*>* n){
  fromCast(d,m,desoptHolder*);
  reaccionarProperDesoptH(d,n,d->movs);
}
void cargarNodos(movHolder* m,desoptHolder::node* iter,vector<normalHolder*>* norms){
  fromCast(d,m,desoptHolder*);
  int res=norms->size;
  int offset=0;
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+offset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    if(actualMov->bools&valorFinal){
      actualMov->table->cargar(actualMov,norms);
      assert(nextIter->iter);
      cargarNodos(d,nextIter->iter,norms);
    }else if(makeClick&&norms->size!=0){
      push(&clickers,makeClicker(norms,d->base->h));
    }
    offset+=tam;
    norms->size=res;
  }
}
void cargarDesoptH(movHolder*m,vector<normalHolder*>* norms){
  fromCast(d,m,desoptHolder*);
  cargarNodos(d,(desoptHolder::node*)desoptMov,norms);
  if(m->sig)//puede que decida prohibir poner cosas despues de un desopt, obligar a envolver en un isol si se quiere
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

 virtualTableMov desoptTable={generarDesoptH,reaccionarDesoptH,
                              reaccionarDesoptH,cargarDesoptH};
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




void generarNewlySpawned(movHolder* m){
  fromCast(s,m,spawnerGen*);

  if(justSpawned.size==0)
    return;

  vector<Holder*> justSpawnedC;//evitar bucles infinitos si spawneo un spawner
  justSpawnedC.data=justSpawned.data;
  justSpawnedC.size=justSpawned.size;
  justSpawned.size=0;

  for(Holder* h:justSpawnedC)
    if(s->h!=h)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
      generar(h);
  if(s->kamikaseNext)
    kamikaseCheckAlive(m);
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


