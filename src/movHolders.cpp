

void initMovH(movHolder* m,operador* op,Base* base_){
  if(!base_->beg)
    base_->beg=m;
  m->base=base_;
  m->bools|=op->bools;//setea makeClick, hasClick y doEsp en normalh
}

void generarSig(movHolder* m){
  //se llama cuando valor == true
  if(m->sig){
    m->sig->table.generar(m->sig);
    if(m->bools&hasClick||m->sig->bools&valorCadena)//puede que haya una forma de no usar el if?
      m->bools|=valorCadena;
    else
      m->bools&=~valorCadena;
    m->bools|=m->sig->bools&valorFinal;
  }else
    m->bools|=valorFinal|valorCadena;
}
void reaccionarSig(movHolder*m,auto nhs){
  if(m->sig){
    m->sig->reaccionar(m->sig,nhs);
    if(switchToGen){
      if(m->bools&hasClick||m->sig->bools&valorCadena)
        m->bools|=valorCadena;
      else
        m->bools&=~valorCadena;
      m->bools|=m->sig->bools&valorFinal;
    }
  }
}

inline void generarProperNormalH(normalHolder* n){
  actualHolder.nh=n;//lo usan algunas cosas de memoria y debug
  for(condt* c:n->op->conds)
    if(!c->check()){
      n->bools&=~(valorFinal|valorCadena|valor);
      return;
    }
  offset=n->pos;

  n->bools|=valor;
  generarSig(n);
}
void generarNormalH(movHolder* m){
  normalHolder* n=(normalHolder*) m;

  memcpy(n->memAct.begptr,memMov.data(),n->base->memLocalSize*sizeof(int));
  n->pos=getActualPos(n->relPos,offset);//pos se calcula siempre porque se usa para actualizar offset
  if(n->bools&doEsp){
    if(n->pos.x<0||n->pos.x>=tablptr->tam.x||n->pos.y<0||n->pos.y>=tablptr->tam.y){
      n->bools&=~(valorFinal|valorCadena|valor);
      return;
    }
    actualTile=tablptr->tile(n->pos);
    //los triggers se ponen aca y se reponen en la reaccion. Las normales esp ahora son
    //las normales que se originan despues de un movimiento. (o hacen miran memoria de tile)
    //antes eran las normales que contenian un chequeo posicional, como vacio o memoria de tile,
    //lo que tiene mas sentido. Decidi ir por este otro camino porque la gran mayoria de normales
    //que siguen un movimiento hacen un chequeo posicional (creo que las que no se podrian reescribir siempre),
    //y hacerlo por posicion tiene el problema de que en cosas como w exc mover or captura end, que son comunes,
    //se esta poniendo un trigger en las dos ramas de exc en vez de antes de este, lo que no aprovecha la normal
    //de w que va a estar ahi de todas formas y hace que se tengan multiples triggers, lo que es mas caro.
    //Puede que se pueda hacer un sistema mejor en la version compilada, aunque no sé si lo vale.
    actualTile->triggers.push_back(Trigger{n->base->h->tile,n,n->base->h->tile->step});
  }
  generarProperNormalH(n);
}
void reaccionarNormalH(movHolder* m,normalHolder* nh){
  normalHolder* self=(normalHolder*) m;

  if(nh==self){
    memcpy(memMov.data(),self->memAct.begptr,self->base->memLocalSize*sizeof(int));
    switchToGen=true;
    actualTile=tablptr->tile(self->pos);
    actualTile->triggers.push_back(Trigger{self->base->h->tile,self,self->base->h->tile->step});
    normalHGenerarProper(self);
    if(!(self->bools&valorFinal)){
      offset=getOffset(self->relPos,self->pos);
      memcpy(memMov.data(),self->memAct.begptr,self->base->memLocalSize*sizeof(int));
      //esta restauracion esta para que el operador que contenga reciba el offset y mem local correcta
      //la alternativa a hacer esto es que todos los operadores contenedores guarden offset y memoria local
      //para cada iteración/rama, lo que tambien tiene sus desventajas.
    }
  }else if(valor){ //@check esto tiene sentido?
    reaccionarSig(self,nh);
    if(!(self->bools&valorFinal)){
      offset=getOffset(self->relPos,self->pos);
      memcpy(memMov.data(),self->memAct.begptr,self->base->memLocalSize*sizeof(int));
    }
  }
}
void reaccionarNormalH(movHolder* m,vector<normalHolder*>* nhs){
  normalHolder* s=(normalHolder*) m;

  for(int i=0;i<nhs->size();++i){
    normalHolder* nh=(*nhs)[i];
    if(nh==s){
      memcpy(memMov.data(),s->memAct.begptr,s->base->memLocalSize*sizeof(int));
      switchToGen=true;
      actualTile=tablptr->tile(pos);
      actualTile->triggers.push_back(Trigger{s->base->h->tile,s,s->base->h->tile->step});
      generarProperNormalH(s);
      if(!(s->bools&valorFinal)){
        offset=getOffset(s->relPos,s->pos);
        memcpy(memMov.data(),s->memAct.begptr,s->base->memLocalSize*sizeof(int));
      }
      nhs->erase(nhs->begin()+i);//puede que no lo valga
      return;
    }
  }
  if(valor){
    reaccionarSig(s,nhs);
    if(!(s->bools&valorFinal)){
      offset=getOffset(s->relPos,s->pos);
      memcpy(memMov.data(),s->memAct.begptr,s->base->memLocalSize*sizeof(int));
    }
  }
}

void accionarNormalH(normalHolder* n){
  actualHolder.h=n->base->h;
  actualHolder.nh=n;
  actualTile=tablptr->tile(n->pos);
  for(acct* ac:n->op->accs)
    ac->func();
}
void cargarNormalH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(n,m,normalHolder*);
  if(!(n->bools&valorCadena)) return;
  norms->push_back(n);
  if(n->bools&makeClick)
    clickers.emplace_back(norms,n->base->h);
  if(n->sig)
    n->sig->cargar(norms);
}
void drawNormalH(normalHolder* n){
  actualPosColor=n->pos;
  for(colort* c:n->op->colors)
    c->draw();
}

const virtualTableMov normalTable={generarNormalH,reaccionarNormalH,reaccionar,cargarNormalH};

void initNormalH(normalHolder* n,normal* org,Base* base_,char** head)
{
  initMovH(n,op,base_);

  n->table=normalTable;

  n->op=org;
  for(auto trigInfo:op->setUpMemTriggersPerNormalHolder)
    switch(trigInfo.type){
    case 0:
      memGlobalTriggers[trigInfo.ind].perma.push_back(this);
      break;case 1:
      base->h->memPiezaTrigs[trigInfo.ind]->perma.push_back(this);
      break;case 2:
      turnoTrigs[base->h->bando].push_back({base->h,this}); ///@check
    }
  memAct.begptr=(int*)*head;
  memAct.endptr=((int*)*head)+base_->memLocalSize;
  *head=(char*)memAct.endptr;

  relPos=op->relPos;
}



void maybeAddIteration(deslizHolder*d,int i){
  if(d->cantElems==i){
    char* place=(char*)movs.begptr+movs.elem.size()*i;//crearMovHolder necesita **
    crearMovHolder(d->op->inside,d->base,&place);
    d->cantElems++;
    assert(d->cantElems<=d->movs.count());
  }
}
void generarDeslizH(movHolder* m){
  fromCast(d,m,deslizHolder*);
  movHolder* act;
  int i=0;
  for(;;){
    v offsetOrg=offset;
    act=(movHolder*)d->movs[i];
    act->table.generar(act);
    if(!(act->bools&valorFinal)){
      offset=offsetOrg;
      break;
    }
    i++;
    maybeAddIteration(b,i);
  }
  if(act->bools&valorCadena)
    bools|=lastNotFalse;
  else
    bools&=~lastNotFalse;
  f=i;
  generarSig(d);
}
void reaccionarDeslizH(movHolder* m,normalHolder* nh){
  deslizHolder* d=(deslizHolder*)m;
  if((char*)nh<(char*)d+d->op->insideSize)
    reaccionarNh(d,nh);
  else
    reaccionarSig(d,nh);
}
void reaccionarDeslizH(movHolder*m,vector<normalHolder*>* nhs){
  deslizHolder* d=(deslizHolder*)m;
  if((char*)(*nhs)[0]<(char*)d+d->op->insideSize)
    reaccionarNh(d,(*nhs)[0]);
  else
    reaccionarSig(d,nhs);
}
inline void reaccionarNhDesliz(deslizHolder* d,normalHolder* nh){
  assert((char*)nh>(char*)d);
  intptr_t diff=(char*)nh-(char*)d;
  int iter=diff/d->op->iterSize;
  for(int i=0;i<iter;++i){
    if(!(((movHolder*)d->movs[i])->bools&valorFinal)){
      return;
    }
  }
  movHolder* act=(movHolder*)b->movs[iter];
  act->reaccionar(nh);
  assert(switchToGen);
  for(;b->act->bools&valorFinal;){
    iter++;
    maybeAddIteration(b,iter);
    act=(movHolder*)b->movs[iter];
    act->generar();
  }
  if(act->bools&valorCadena)
    bools|=lastNotFalse;
  else
    bools&=~lastNotFalse;
  b->f=iter;
  generarSig(b);
}
void cargarDeslizH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(m,d,deslizHolder*);
  if(!(d->bools&valorCadena)) return;
  for(int i=0;i<(d->bools&d->lastNotFalse?d->f+1:d->f);i++){
    fromCast(mov,d->movs[i],movHolder*);
    mov->table.cargar(mov,norms);
  }
  if(mov->bools&makeClick&&!norms->empty()) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
    clickers.emplace_back(norms,base->h);
  if(d->sig)
    d->sig->cargar(norms);
}

const virtualTableMov deslizTable={generarDeslizH,reaccionarDeslizH,reaccionarDeslizH,cargarDeslizH};
//esta version del desliz retrocede a la posicion de inicio de una iteracion si la iteracion falla
//esto la hace mas consistente en casos donde se usen multiples normales y cosas asi. Pero para
//casos comunes como la torre no se usa, agrega codigo y almacenamiento, y lo mas importante es
//que genera multiples triggers. No vale la pena agregar otra version ahora para no hacer bloat,
//pero en la version compilada podria estar. Tambien se podria agregar instrucciones bizarras
//solo de desliz sin mucho problema, como cambiar la pos de retorno o breaks.
void initdeslizH(deslizHolder* d,desliz* org,Base* base_,char** head){
  initMovH(d,org,base_);
  d->table=deslizTable;

  d->op=org;
  d->movs.begptr=*head;
  d->movs.endptr=(char*)movs.begptr+org->insideSize;
  d->movs.elem.setSize(org->iterSize);
  crearMovHolder(org->inside,base,head);//crear primera iteracion
  *head=(char*)movs.endptr;
  d->cantElems=1;
}




void generarExcH(movHolder* m){
  fromCast(e,m,excHolder*);

  int i;
  v offsetOrg=offset;
  for(i=0;i<e->ops.count();i++){
    movHolder* branch=*e->ops[i];
    branch->generar();
    if(branch->bools&valorCadena){
      //si una rama con un clicker puso el clicker y despues fallo se toma como rama buena,
      //pero la pos vuelve al origen. Medio raro. Por ahora lo dejo asi porque asi esta el sistema,
      //si se quiere hacer algo distinto no sería problema agregarlo en la version compilada
      bools|=valor;
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
      branch=*ops[i-1];
      goto branchFound;
    }
  }
  branch=*e->ops[actualBranch];
 branchFound:
  branch->table.reaccionar(branch,nh);
  if(switchToGen){//solo falso si la nh es innaccesible, por ejemplo esta en la parte invalida de un desliz
    if(!(branch->bools&valorCadena)){ //si el ab al recalcularse se invalida generar todo devuelta, saltandolo
      int j;
      for(j=0;j<e->ops.count();j++){
        if(i-1!=j){
          movHolder* brancj=*e->ops[j];
          brancj->table.generar(brancj);
          if(brancj->bools&valorCadena){
            bools|=valor;
            actualBranch=j;
            generarSig();
            return;
          }
        }
      }
      bools&=~(valorCadena|valor);
      actualBranch=j-1;
    }else{ //se valido una rama que era invalida
      actualBranch=i-1;
      bools|=valor;
      generarSig();
    }
  }
}
void reaccionarExcH(movHolder* m,normalHolder* nh){
  fromCast(e,m,excHolder*);
  if((char*)nh-(char*)this<size){
    reaccionarNh(nh);
  }else if(bools&valor)
    reaccionarSig(nh);
}
void reaccionarExcH(movHolder*m,vector<normalHolder*>* nhs){
  //no puedo asumir que la primera normal es la que va a causar el cambio a generacion
  //porque puede que esta este en un lugar innaccesible
  fromCast(e,m,excHolder*);
  for(normalHolder* nh:*e->nhs){
    if((char*)nh-(char*)e<size){
      reaccionarNh(e,nh);
      if(switchToGen){
        break;
      }
    }else{
      reaccionarSig(nh);
      return;
    }
  }
}
void cargarExcH(movHolder* m,vector<normalHolder*>* norms){
  fromCast(e,m,excHolder*);
  if(!valorCadena) return;
  movHolder* branch=*e->ops[e->actualBranch];
  branch->table.cargar(branch,norms);
  if(e->makeClick)
    clickers.emplace_back(norms,e->base->h);
  if(sig)
    sig->table.cargar(sig,norms);
}

virtualTableMov excTable={generarExcH,reaccionarExcH,reaccionarExcH,cargarExcH};
initExcHolder(excHolder* e,exc* org,Base* base_,char** head){
  initMovH(org,base);
  e->table=excTable;

  e->ops.begptr=(movHolder**)*head;
  e->ops.endptr=(movHolder**)(*head+org->ops.size());
  *head=(char*)e->ops.endptr;
  e->size=org->insideSize+sizeof(excHolder);
  int i=0;
  for(operador* opos:org->ops){
    *(e->ops.begptr+i++)=(movHolder*)*head;
    crearMovHolder(opos,base,head);
  }
}




void generarIsolH(movHolder* m){
  fromCast(s,m,isolHolder*);

  v tempPos=offset;
  void* memTemp=alloca(s->base->memLocalSize*sizeof(int));
  memcpy(memTemp,memMov.data(),s->base->memLocalSize*sizeof(int));
  s->inside->generar();
  offset=tempPos;
  memcpy(memMov.data(),memTemp,s->base->memLocalSize*sizeof(int));
  if(s->sig){
    s->sig->table.generar(s->sig);
    s->bools&=~valorFinal;
    s->bools|=s->sig->bools&valorFinal;
  }
}
void reaccionarIsolH(movHolder* m,normalHolder* nh){
  fromCast(s,m,isolHolder*);
  if((char*)nh-(char*)s<size){
    inside->table.reaccionar(inside,nh);
    if(switchToGen){
      ///@optim podria hacerse un lngjmp
      switchToGen=false;
    }
  }
  else if(s->sig)
    s->sig->table.reaccionar(s->sig,nh);
}
void reaccionarIsolH(movHolder* m,vector<normalHolder*>* nhs){
  fromCast(s,m,isolHolder*);
  if((char*)(*nhs)[0]-(char*)s<size){
    s->inside->table.reaccionar(s->inside,(*nhs)[0]);
    if(switchToGen){
      ///@optim podria hacerse un lngjmp
      switchToGen=false;
    }
  }
  if(s->sig)
    s->sig->table.reaccionar(s->sig,nhs);
}
void cargarIsolH(movHolder*m,vector<normalHolder*>* norms){
  fromCast(s,m,isolHolder*);
  vector<normalHolder*> normExt=*norms; ///@optim agregar y cortar en lugar de copiar. O copiar aca y no en clicker devuelta
  s->inside->table.cargar(s->inside,&normExt);
  if(s->bools&makeClick&&!normExt.empty())//evitar generar clickers sin normales
    clickers.emplace_back(norms,s->base->h);
  if(s->sig)
    s->sig->table.cargar(s->sig,norms);
}

const virtualTableMov isolTable={generarIsolH,reaccionarIsolH,reaccionarIsolH,cargarIsolH};
initIsolH(isolHolder* s,isol* org,Base* base_,char** head){
  initMovH(s,org,base_);
  s->table=isolTable;

  s->inside=(movHolder*)*head;
  crearMovHolder(org->inside,base_,head);
  s->bools|=valorFinal|valorCadena;
  s->size=org->size;
}




vector<int> memGenStack;//copia de memoria actual para reestablecer en el cambio de rama en generaciones. Es un stack de arrays aplanado

#define desoptMovs ((char*)d+sizeof(desoptHolder))
void generarDesoptH(desoptHolder* d){
  //se tiene la base y primera iteracion construidos, rondas mas alla de estas usan el resto de memoria
  //de forma dinamica. Es un punto intermedio entre tener todo construido y reservar mucha memoria que probablemente
  //no se use, y hacer todo dinamico y estar reconstruyendo operadores en cada generacion
  //Seria lo mas optimizado para la dama

  //con limpiar la caja de movHolders de la primera iteracion antes de la generacion total
  //y sobreescribir el espacio dinamico no deberia haber ningun problema de construcciones kakeadas y cosas asi
  d->dinamClusterHead=desoptMovs+d->op->dinamClusterBaseOffset;


  int clusterOffset=0;
  char* correspondingCluster=desoptMovs+d->op->clusterSize;

  v offsetOrg=offset;

  int localMemSize=d->base->memLocalSize*sizeof(int);
  void* memTemp=alloca(localMemSize);
  memcpy(memTemp,memMov.data(),localMemSize);

  void* memTemp2=alloca(localMemSize);

  for(int tam:d->op->movSizes){
    node* firstIter=(node*)(desoptMovs+clusterOffset);
    //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
    movHolder* actualMov=(movHolder*)(firstIter+1);

    actualMov->generar();
    if(actualMov->bools&valorFinal){
      firstIter->iter=(node*)correspondingCluster;
      int clusterOffset2=0;
      v offsetOrg2=offset;
      memcpy(memTemp2,memMov.data(),localMemSize);

      for(int tam:d->op->movSizes){
        node* secondIter=(node*)((char*)firstIter->iter+clusterOffset2);
        movHolder* actualMov2=(movHolder*)(secondIter+1);

        actualMov2->generar();
        if(actualMov2->bools&valorFinal){
          secondIter->iter=(node*)dinamClusterHead;
          construirYGenerarNodo(localMemSize);
        }else
          secondIter->iter=nullptr;
        clusterOffset2+=tam;
        offset=offsetOrg2;
        memcpy(memMov.data(),memTemp2,localMemSize);
      }
    }else
      firstIter->iter=nullptr;
    clusterOffset+=tam;
    correspondingCluster+=op->clusterSize;
    offset=offsetOrg;
    memcpy(memMov.data(),memTemp,localMemSize);
  }
}
void construirYGenerarNodo(desoptHolder* d,int localMemSize){
  char* clusterBeg=d->dinamClusterHead;
  assert(d->dinamClusterHead<desoptMovs+d->op->desoptInsideSize);
  for(operador* opos:d->op->ops){
    *(movHolder**)d->dinamClusterHead=nullptr;
    d->dinamClusterHead+=sizeof(node);
    crearMovHolder(opos,base,&dinamClusterHead);
  }
  int clusterOffset=0;
  v offsetOrg=offset;
  void* memTemp=alloca(localMemSize);
  memcpy(memTemp,memMov.data(),localMemSize);

  for(int tam:op->movSizes){
    node* nextIter=(node*)(clusterBeg+clusterOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);

    actualMov->table.generar(actualMov);
    if(actualMov->bools&valorFinal){
      nextIter->iter=(node*)d->dinamClusterHead;
      construirYGenerarNodo(d,localMemSize);
    }
    clusterOffset+=tam;
    offset=offsetOrg;
    memcpy(memMov.data(),memTemp,localMemSize);
  }
}
void generarNodo(desoptHolder* d,node* iter,int localMemSize){//iter valido
  int clusterOffset=0;
  v offsetOrg=offset;

  void* memTemp=alloca(localMemSize);
  memcpy(memTemp,memMov.data(),localMemSize);
  for(int tam:d->op->movSizes){
    node* nextIter=(node*)((char*)iter+clusterOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    actualMov->table.generar(actualMov);
    if(actualMov->bools&valorFinal){
      if(nextIter->iter){
        generarNodo(d,nextIter->iter,localMemSize);
      }else{
        nextIter->iter=(node*)d->dinamClusterHead;
        construirYGenerarNodo(d,localMemSize);
      }
    }
    clusterOffset+=tam;
    offset=offsetOrg;
    memcpy(memMov.data(),memTemp,localMemSize);
  }
}
void reactIfNh(normalHolder* nh,movHolder* actualMov,int tam){
  if((char*)nh>=(char*)actualMov&&(char*)nh<(char*)actualMov+tam){
    actualMov->reaccionar(nh);
  }
}
void reactIfNh(vector<normalHolder*>* nhs,movHolder* actualMov,int tam){
  for(normalHolder* nh:*nhs){
    reactIfNh(nh,actualMov,tam);
  }
}
template<typename T>
void reaccionarDesoptH(desoptHolder* d,T nh,desoptHolder::node* iter){
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
        reaccionarDesoptH(d,nh,nextIter->iter);
    }
    switchToGen=false;
  }
}
void cargarNodos(desoptHolder* d,node* iter,vector<normalHolder*>* norms){
  int res=norms->size();
  int offset=0;
  for(int tam:d->op->movSizes){
    node* nextIter=(node*)((char*)iter+offset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    if(actualMov->bools&valorFinal){
      actualMov->table.cargar(actualMov,norms);
      assert(nextIter->iter);
      cargarNodos(d,nextIter->iter,norms);
    }else if(makeClick&&!norms->empty()){
      clickers.emplace_back(norms,base->h);
    }
    offset+=tam;
    norms->resize(res);
  }
}
void cargarDesoptH(movHolder*m,vector<normalHolder*>* norms){
  cargarNodos((desoptHolder*)m,(node*)movs(),norms);
  if(m->sig)//puede que decida prohibir poner cosas despues de un desopt, obligar a envolver en un isol si se quiere
    m->sig->table.cargar(m->sig,norms);
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

const virtualTableMov desoptTable={generarDesoptH,reaccionarDesoptH<normalHolder*>,
                             reaccionarDesoptH<vector<normalHolder*>*>,cargarDesoptH};
initDesoptH(desoptHolder* d,desopt* org,Base* base_,char** head){
  initMovH(d,org,base_);
  d->table=desoptTable;

  d->op=org;
  char* nextIteration=*head;
  char* headFirst=*head;
  for(operador* opos:org->ops){//armar base
    assert(sizeof(movHolder*)==sizeof(char*));//debe haber una forma menos sospechosa de hacer esto
    **((movHolder***)head)=(movHolder*)(nextIteration+=op->clusterSize);//head apunta a puntero al proximo cluester de movimientos,
    *head+=sizeof(node*);//que corresponde a la proxima iteracion de este movHolder
    crearMovHolder(opos,base,head);
  }
  for(int i=0;i<org->ops.count();i++){//armar primer iteracion
    for(operador* opos:org->ops){
      **(movHolder***)head=nullptr;//cuando se use apunta a un espacio dinamico
      *head+=sizeof(node*);
      crearMovHolder(head,opos,base);
    }
  }
  *head=headFirst+org->desoptInsideSize;
  d->bools|=valorCadena|valorFinal;
}




void generarNewlySpawned(){
  if(justSpawned.empty())
    return;

  vector<Holder*> justSpawnedL(justSpawned);//@optim estaria bueno usar el stack para estas cosas, o un move
  justSpawned.clear();//evitar bucles infinitos

  for(Holder* s:justSpawnedL)
    if(base->h!=s)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
      s->generar();
}

void cargarNothing(movHolder*m,vector<normalHolder*>* nh){
}

const virtualTableMov spawnerTable={generarNewlySpawned,nullptr,nullptr,cargarNothing};
void initSpawner(spawner* s,base* base_){
  s->table=spawnerTable;
  s->base=base_;
}

void kamikaseCheckAlive(){
  if(!base->h->inPlay)
    throw nullptr;
}
//si spawnea y muere en el mismo turno no pasa nada porque spawn corre antes de kamikase
const virtualTableMov kamikaseTable={kamikaseCheckAlive,nullptr,nullptr,cargarNothing};
void kamikaseInit(kamikase* k,base* base_){
  k->table=kamikaseTable;
  k->base=base_;
}


