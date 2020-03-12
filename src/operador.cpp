
int memLocalSizeAct;

template<typename T>
bool gatherMemCte(vector<T>& vec,int tok){
                   switch(tok){
                   case lector::posX: vec.push_back((T)posXRead);break;
                   case lector::posY: vec.push_back((T)posYRead);break;
                   case lector::turno:vec.push_back((T)turnoRead);break;
                   default:           vec.push_back((T)(tok-1000));break;
                   }
}

//creo que lo de make se usaba solo para lo de *
normal* initNormal(bool make){
  normal* n=alloc<normal>();

  vector<void(*)(void)> accsTemp;
  vector<bool(*)(void)> condsTemp;
  vector<colort*> colorsTemp;
  vector<normal::setupTrigInfo> setUpMemTriggersPerNormalHolderTemp;
  auto setupBarrays=[&](){
                      //cargo listas de punteros despues del resto de memoria de la normal.
                      //Lo pongo al final en vez de al principio para evitar tener que hacer un analisis
                      //previo a la carga para saber hasta donde llega la normal y que cosas tiene.
                      //supongo que que este antes o despues tiene el mismo efecto en la cache
                      init(n->accs,accsTemp.size(),accsTemp.data());
                      init(n->conds,condsTemp.size(),condsTemp.data());
                      init(n->colors,colorsTemp.size(),colorsTemp.data());
                      init(n->setUpMemTriggersPerNormalHolder,setUpMemTriggersPerNormalHolderTemp.size(),setUpMemTriggersPerNormalHolderTemp.data());
                    };

  movSize+=sizeof(normalHolder)+memLocalSizeAct*sizeof(int);
  n->tipo=NORMAL;
  n->bools&=~(hasClick|makeClick|doEsp);
  n->sig=nullptr;
  n->relPos=v(0,0);
  if(make){
    bool changeInLocalMem=false;
    while(true){
      if(tokens.empty()) return nullptr;///@optim creo que no se activa nunca, no deberia
      int tok=tokens.front();
      tokens.pop_front();
      switch(tok){
      case lector::W:
      case lector::S:
      case lector::D:
      case lector::A:

        //antes nomas cortaba en cuando hubo una cond posicional antes. Ahora lo hago por cualquiera
        //porque sino cosas como mcmp p0 1 w mover ponen un trigger en w aun cuando mcmp es falso
        //por ahi no vale la pena cortar por eso igual.
        if(!condsTemp.empty()||!accsTemp.empty()){
          tokens.push_front(tok);
          setupBarrays();
          n->sig=initNormal(true);//nueva normal
          return n;
        }else{
          switch(tok){
          case lector::W: n->relPos.y--;break;
          case lector::S: n->relPos.y++;break;
          case lector::D: n->relPos.x++;break;
          case lector::A: n->relPos.x--;break;
          }
        }
        break;
        //cout<<#TOKEN<<endl;
#define cond(TOKEN) case lector::TOKEN:                           \
        condsTemp.push_back(TOKEN) ;break
        cond(vacio);
        cond(pieza);
        cond(enemigo);
        cond(pass);
      case lector::esp:
        bools|=doEsp;
        break;
#define acc(TOKEN) case lector::TOKEN: accsTemp.push_back(TOKEN);break
        acc(mov);
        acc(capt);
        acc(pausa);
      case lector::spwn:
        accsTemp.push_back(spwn);
        accsTemp.push_back(void(*)(void)(tokens.front()-1000));
        tokens.pop_front();break;
        //spwn n con n positivo quiere decir mismo bando, negativo bando enemigo
      case lector::color:
        colorsTemp.push_back(crearColor());
        break;
        //       colorr(sprt);
        //       colorr(numShow);
      case lector::mcmp:
      case lector::mset:
      case lector::madd:
      case lector::mless:
      case lector::mmore:
      case lector::mdist:
        {
          //mset l0 4 mset g0 l0 <-en este caso necesito cortar en 2 normales, para que el segundo set tenga registrado el primero
          //esto no se maneja ahora porque prefiero delegar eso al parser cuando lo haga bien. Igual se puede manejar aca tambien, antes lo hacia

          auto isCte=[](int tok)->bool{
                       return isNum(tok)||
                         tok==lector::posX||
                         tok==lector::posY||
                         tok==lector::turno;
                     };


          int op=tok;
          bool write=op==lector::mset||op==lector::madd;
          tok=tokens.front();tokens.pop_front();

          assert(!(write&&isCte(tok)));

          bool action=write&&(tok==lector::global||lector::tile);
          if(action){
            int i=0;
            int nextTok=tokens.front();
            if(isCte(nextTok)){
              switch(tok){//manejo sets nomas
              case lector::global: accsTemp.push_back(msetG);break;
              case lector::tile:   accsTemp.push_back(msetT);break;
              }
              tok=tokens.front();tokens.pop_front();
              gatherCte(accsTemp,tok);
              i++;
            }else{
              switch(tok){
              case lector::global: accsTemp.push_back(msetGi);break;
              case lector::tile:   accsTemp.push_back(msetTi);break;
              }
            }
            for(;i<2;i++){
              tok=tokens.front();tokens.pop_front();
              if(i==1&&isCte(tok)){
                gatherCte(accsTemp,tok);
                break;
              }
              while(true){
                nextTok=tokens.front();
                if(isCte(nextTok)){
                  switch(tok){
                  case lector::global: accsTemp.push_back(globalRead);break;
                  case lector::tile:   accsTemp.push_back(tileReadNT);break;
                  case lector::local:  accsTemp.push_back(localAccg);break;
                  case lector::pieza:  accsTemp.push_back(piezaAccg);break;
                  }
                  tok=tokens.front();tokens.pop_front();
                  gatherCte(accsTemp,tok);
                  return;
                }else{
                  switch(tok){
                  case lector::global: accsTemp.push_back(globalReadNTi);break;
                  case lector::tile:   accsTemp.push_back(tileReadNTi);break;
                  case lector::local:  accsTemp.push_back(localAccgi);break;
                  case lector::pieza:  accsTemp.push_back(piezaAccgi);break;
                  }
                }
                tok=tokens.front();tokens.pop_front();
              }
            }
          }else{
            condsTemp.push_back(op);
            for(int i=0;i<2;i++){
              if(isCte(tok)){
                gatherCte(condsTemp,tok);
                continue;
              }
              while(true){
                int nextTok=tokens.front();
                if(isCte(nextTok)){
                  switch(tok){
                  case lector::global: condsTemp.push_back(globalRead);setUpMemTriggersPerNormalHolderTemp.push_back({0,tg[j]});break;
                  case lector::tile:   condsTemp.push_back(tileRead);break;
                  case lector::local:  condsTemp.push_back(localg);break;
                  case lector::pieza:  condsTemp.push_back(piezag);break;
                  }
                  tok=tokens.front();tokens.pop_front();
                  gatherCte(condsTemp,tok);
                  break;
                }else{
                  switch(tok){
                  case lector::global: condsTemp.push_back(globalReadi);break;
                  case lector::tile:   condsTemp.push_back(tileReadi);break;
                  case lector::local:  condsTemp.push_back(localgi);break;
                  case lector::pieza:  condsTemp.push_back(piezagi);break;
                  }
                }
                tok=tokens.front();tokens.pop_front();
              }
            }
          }











      case lector::sep:
        //cout<<"sepn->"<<endl;
        separator=true;
        setupBarrays();
        return n;
      case lector::eol:
        bools|=hasClick|makeClick;
        setupBarrays();
        return n;
      case lector::end:
        //cout<<"lim"<<endl;
        setupBarrays();
        return n;
      case lector::click:
        bools|=hasClick|makeClick;
        clickExplicit=true;
        setupBarrays();
        n->sig=tomar();
        ///@todo mirar casos raros como dos clicks seguidos
        return n;
      default:
        tokens.push_front(tok);
        sig=tomar();
        setupBarrays();
        n->sig=(operador*)actualBucket->head;
        initNormal(true);
        return n;
      }
    }
  }
}
desliz* initDesliz(){
  desliz* d=alloc<desliz>();

  d->tipo=DESLIZ;
  d->bools&=~makeClick;

  int movSizeTemp=movSize;
  movSize=0;
  d->inside=tomar();
  v& tam=tablptr->tam;
  //iteraciones necesarias para recorrer el tablero en linea recta.
  d->iterSize=movSize;
  d->insideSize=movSize*((tam.x>tam.y?tam.x:tam.y))*2;///@todo agregar posibilidad de elegir cuando se reserva
  movSize=movSizeTemp+sizeof(deslizHolder)+d->insideSize;

  sig=keepOn(&bools);

  if(bools&makeClick)
    bools|=hasClick;
  else
    for(operador* op=d->inside;op!=nullptr;op=op->sig)
      if(op->bools&hasClick){
        bools|=hasClick;
        break;
      }
  return d;
}
exc* initExc(){
  exc* e=alloc<exc>();

  e->tipo=EXC;
  int movSizeTemp=movSize;
  movSize=0;

  vector<operador*> opsTemp;
  do{
    separator=false;
    operador* op=tomar();
    opsTemp.push_back(op);
  }while(separator);
  e->ops.init(opsTemp.size(),opsTemp.data());
  movSize+=e->ops.size();

  e->insideSize=movSize;
  movSize=movSizeTemp+sizeof(excHolder)+e->insideSize;

  e->bools&=~makeClick;
  e->sig=keepOn(&e->bools);
  if(e->bools&makeClick)
    e->bools|=hasClick;
  else{
    e->bools&=~hasClick;
    for(operador* op:e->ops)
      if(op->bools&hasClick){
        e->bools|=hasClick;
        break;
      }
  }
  return e;
}
isol* initIsol(){
  isol* i=alloc<isol>();

  i->tipo=ISOL;
  i->bools|=hasClick;
  i->bools&=~makeClick;
  bool clickExplicitBack=clickExplicit;

  int movSizeTemp=movSize;
  movSize=sizeof(isolHolder);
  i->inside=tomar();
  i->size=movSize;
  movSize+=movSizeTemp;

  if(!clickExplicit)
    i->bools|=makeClick;
  clickExplicit=clickExplicitBack;
  sig=keepOn(&i->bools);

  return i;
}
desopt* initDesopt(){
  desopt* d=alloc<desopt>();

  d->tipo=DESOPT;
  int movSizeTemp=movSize;
  movSize=0;
  vector<operador*> opsTemp;
  vector<int> sizesTemp;
  int branches=0;
  do{
    int movSizeTemp=movSize;
    separator=false;
    operador* op=tomar();
    opsTemp.push_back(op);
    sizesTemp.push_back(movSize-movSizeTemp);
    branches++;
  }while(separator);
  ops.init(opsTemp.size(),opsTemp.data());
  for(int& i:sizesTemp) i+=sizeof(desoptHolder::node*);//sumar espacio puntero a cluster
  movSize+=branches*sizeof(desoptHolder::node*);

  d->movSizes.init(branches,sizesTemp.data());

  d->clusterSize=movSize;
  d->dinamClusterBaseOffset=d->clusterSize+d->clusterSize*branches;
  d->desoptInsideSize=d->clusterSize+d->clusterSize*branches+d->clusterSize*1024;//12 es la cantidad de slots del espacio dinamico
  //@todo hacerse pueda determinar otros valores como con desliz
  movSize=movSizeTemp+sizeof(desoptHolder)+d->desoptInsideSize;

  d->bools&=~makeClick;
  d->sig=keepOn(&d->bools);
  if(d->bools&makeClick)
    d->bools|=hasClick;
  else{
    d->bools&=~hasClick;
    for(operador* op:d->ops)
      if(op->bools&hasClick){
        d->bools|=hasClick;
        break;
      }
  }
  return d;
}

//mira si hay algun token adelante que genere un operador
operador* keepOn(int32_t* bools){
  if(tokens.empty())
    return nullptr;
  switch(tokens.front())
    {
    case lector::click:
      *bools|=makeClick;
      tokens.pop_front();
      return keepOn(bools);
    case lector::sep:
      separator=true;
      tokens.pop_front();
      return nullptr;
    case lector::eol:
      if(!clickExplicit)
        *bools|=makeClick;
    case lector::end:
      tokens.pop_front();
      return nullptr;
    }
  return tomar();
}

operador* tomar(){
  if(tokens.empty()) return nullptr;
  int tok=tokens.front();
  tokens.pop_front();
#define caseTomar(TOKEN) case lector::TOKEN: return init##TOKEN ()
  switch(tok)
    {
      caseTomar(desliz);
      caseTomar(exc);
      caseTomar(isol);
      caseTomar(desopt);
    case lector::sep:
      separator=true;
    case lector::eol:
    case lector::end:
      return nullptr;
    default:
      tokens.push_front(tok);
      return initNormal(true);
    }
}
