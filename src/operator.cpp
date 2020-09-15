
int pop(parseMovData* p){
  return p->tokens[p->ind++];
}

int peek(parseMovData* p){
  return p->tokens[p->ind];
}

void makePiece(parseData* pd,int id,int sn,vector<int>* tokens,
               vector<Piece*>* pieces,bucket* operatorBucket){
  Piece* piece=alloc<Piece>(operatorBucket);
  piece->sn=sn;

  piece->spriteb.setTexture(image.get("piezas.png"));
  piece->spriteb.setTextureRect(IntRect(sn*64%384,(sn*64/384)*32,32,32));
  //piece->spriteb.setScale(escala,escala);
  piece->spriten.setTexture(image . get("piezas.png"));
  piece->spriten.setTextureRect(IntRect(sn*64%384+32,(sn*64/384)*32,32,32));
  //piece->spriten.setScale(escala,escala);

  piece->memPieceSize=pd->memPieceSize;

  alloc(operatorBucket,&piece->movs,pd->movQ);

  piece->hsSize=0;
  parseMovData p{operatorBucket,pd,*tokens,0,0,0,false};//writeInLocalMem base nunca se lee
  for(int i=0;i<pd->movQ;i++){
    p.movSize=0;
    p.clickExplicit=false;

    failIf(pd->memLocal[i].resetUntil>pd->memLocal[i].size,"reset size %d bigger than size %d",pd->memLocal[i].resetUntil,pd->memLocal[i].size);//prefiero fallar a poner el maximo, queda medio raro sino
    piece->movs[i].memLocal.size=pd->memLocal[i].size;
    piece->movs[i].memLocal.resetUntil=pd->memLocal[i].resetUntil==-1?pd->memLocal[i].size:pd->memLocal[i].resetUntil;
    p.memLocal=piece->movs[i].memLocal;

    piece->movs[i].raiz=parseOp(&p);
    piece->movs[i].size=p.movSize;
    piece->hsSize+=p.movSize;
    failIf(pop(&p)!=tmovEnd,"missing ;");
  }
  assert(p.ind==tokens->size);
  piece->spawner=pd->spawner;
  //piece->kamikase=p.kamikase;
  piece->hsSize+=sizeof(Holder)
    +pd->memPieceSize*sizeof(int)
    +pd->movQ*(sizeof(movHolder*)+sizeof(Base))
    +(pd->spawner?(sizeof(movHolder*)+sizeof(Base)+sizeof(spawnerGen)):0);
  piece->ind=pieces->size;
  push(pieces,piece);
}

operador* parseOp(parseMovData* p,bool fromNormal){//=false
  operador* op;
  switch(p->tokens[p->ind]){
  case tdesliz: p->ind++;op=parseDesliz(p);break;
  case texc:    p->ind++;op=parseExc(p);break;
  case tisol:   p->ind++;op=parseIsol(p);break;
  case tdesopt: p->ind++;op=parseDesopt(p);break;
  case tfail:   p->ind++;op=parseFail(p);break;
  case tend:
  case tmovEnd:
  case tseparator:       op=nullptr;break;
  default:
    if(fromNormal){
      fail("%s out of place",tokenToWord(p->pd,p->tokens[p->ind]));
    }else{
      op=parseNormal(p);
    }
  }
  return op;
}

//esto esta aca porque no hay lambdas con templates
template<typename T>
void gatherCte(vector<T>* vec,int tok,bool setCteReader=false){
  switch(tok){
  case tposX: push(vec,(T)posXRead);break;
  case tposY: push(vec,(T)posYRead);break;
  case tposSY:push(vec,(T)posSYRead);break;
  default:
    if(setCteReader)
      push(vec,(T)cteRead);
    push(vec,(T)((intptr)tok-2048));break;
  }
}

normal* parseNormal(parseMovData* p){
  normal* n=alloc<normal>(p->b);

  vector<void(*)(void)> accsTemp;init(&accsTemp);defer(&accsTemp);
  vector<bool(*)(void)> condsTemp;init(&condsTemp);defer(&condsTemp);
  vector<int> colorsTemp;init(&colorsTemp);defer(&colorsTemp);

  auto getNum=[&]()->int{
                int tok=pop(p);
                failIf(tok<1024,"%s requires number");
                return tok-2048;
              };

  auto setupBarrays=[&](){
/*cargo listas de punteros despues del resto de memoria de la normal.
Lo pongo al final en vez de al principio para evitar tener que hacer un analisis previo a la carga para saber hasta donde llega la normal y que cosas tiene. Supongo que que este antes o despues tiene el mismo efecto en la cache
TODO probar haciendo un memcpy al final de todo. Si resulta ser mas rapido deberia replantearme el parseo, en la etapa anterior debería anotar cuantas operaciones de que tipo tiene cada normal, lo que implica hacer el procesado de memoria antes, y de paso se podría ya tener el arbol de ops armado. Igual seria un re quilombo, ver. Si resulta que no importa podría probar mover las bases al final tambien, para no tener el hueco de spawner y kamikase.*/
                      allocCopy(p->b,&n->accs,accsTemp.size,accsTemp.data);
                      allocCopy(p->b,&n->conds,condsTemp.size,condsTemp.data);
                      allocCopy(p->b,&n->colors,colorsTemp.size,colorsTemp.data);
                    };

  p->movSize+=sizeof(normalHolder)+p->memLocal.size*sizeof(int);//+ mov de spawner
  n->tipo=NORMAL;
  n->bools=0;
  n->relPos=v(0,0);
  bool writeInLocalMem=false;
  while(true){
    int tok=pop(p);
    switch(tok){
    case tW:
    case tS:
    case tD:
    case tA:

      //antes nomas cortaba en cuando hubo una cond posicional antes. Ahora lo hago por cualquiera
      //porque sino cosas como mcmp p0 1 w mover ponen un trigger en w aun cuando mcmp es falso
      //por ahi no vale la pena cortar por eso igual.
      if(!condsTemp.size==0||!accsTemp.size==0){
        p->ind--;
        setupBarrays();
        n->sig=parseNormal(p);
        return n;
      }else{
        switch(tok){
        case tW: n->relPos.y--;break;
        case tS: n->relPos.y++;break;
        case tD: n->relPos.x++;break;
        case tA: n->relPos.x--;break;
        }
        n->bools|=doEsp;
        //TODO mirar el tema de esp. Por ahi hacer que se explicite cuando se usa y
        //tener una normal aparte, sacar el if? probar.
        //esa normal prohibiria todos los movimientos posicionales. La construccion podría hacerse
        //como un filtro sobre esta normal para no reescribir
      }
      break;
#define cond(TOKEN) case t##TOKEN:  push(&condsTemp,TOKEN) ;break
      cond(vacio);
      cond(piece);
      cond(enemigo);
      cond(aliado);
      cond(self);
      cond(pass);
    case tesp:
      n->bools|=doEsp;
      break;
    case tassert: push(&condsTemp,langAssert); break;
#undef cond
#define acc(TOKEN) case t##TOKEN: push(&accsTemp,TOKEN);break
      acc(mov);
      acc(pausa);
#undef acc
    case tcapt:
      push(&accsTemp,capt);
      //if(n->relPos==v(0,0))
      //  p->kamikase=true;
      break;
    case tspwn:
      {
        intptr id=getNum();
        addIdIfMissing(p->pd,std::abs(id));
        push(&accsTemp,spwn);
        push(&accsTemp,(actionBuffer)(intptr)getCodedPieceIndexById(&p->pd->ids,id));
        p->pd->spawner=true;
      }
      break;
      //spwn n con n positivo quiere decir mismo bando, negativo bando enemigo
    case tcolor:
      {
        int r=getNum();
        int g=getNum();
        push(&colorsTemp,crearColor(r,g,getNum()));
      }
      break;
      //       colorr(sprt);
      //       colorr(numShow);
    case tmcmp:
    case tmset:
    case tmadd:
    case tmless:
    case tmmore:
    case tmdist:
    case tmgoto:
      {
        //mset l0 4 mset g0 l0 <-en este caso necesito cortar en 2 normales, para que el segundo set tenga registrado el primero

        //TODO en vez de cortar la normal no podría escribir el numero directamente en el buffer?
        //seria un poco complejo porque tendría que agregar una instruccion que actualice el buffer en cada local de accion, por ej
        //mset l0 4 mset g0 l0 => mset l0 4 __updateLocalAcc__ 16 0 mset g0 0 <- donde este 0 es sobreescrito por updateLocalAcc en tiempo de condicion
        //esto esta bueno porque saca la necesidad de meter otra normal, lo que probablemente lo haga mas rapido y le saca complejidad

        auto isCte=[](int tok)->bool{
                     return tok>1024||
                       tok==tposX||
                       tok==tposY||
                       tok==tposSY;
                   };

        int op=tok;
        bool write=op==tmset||op==tmadd;
        tok=peek(p);
#define badParameter default: fail("memory operator %s has bad parameter %d as %s",tokenToWord(p->pd,op),i+1,tokenToWord(p->pd,tok));

        if(write&&isCte(tok)){
          fail("write on constant");
        }
        bool action=write&&(tok==tmglobal||tok==tmtile||tok==tmpiece);
        if(action){
          tok=pop(p);
          int i=0;
          int nextTok=peek(p);
          if(isCte(nextTok)){
            switch(tok){//manejo sets nomas
            case tmglobal: push(&accsTemp,msetG);break;
            case tmtile:   push(&accsTemp,msetT);break;
            case tmpiece:  push(&accsTemp,msetP);push(&accsTemp,(actionBuffer)pieceg);break;
            badParameter;
            }
            tok=pop(p);
            gatherCte(&accsTemp,tok);
            i++;
          }else{
            switch(tok){
            case tmglobal: push(&accsTemp,msetGi);break;
            case tmtile:   push(&accsTemp,msetTi);break;
            case tmpiece:  push(&accsTemp,msetP);push(&accsTemp,(actionBuffer)piecegi);break;
            badParameter;
            }
          }
          for(;i<2;i++){
            tok=pop(p);
            if(i==1&&isCte(tok)){
              gatherCte(&accsTemp,tok,true);
              break;
            }
            while(true){
              nextTok=peek(p);
              if(isCte(nextTok)){
                switch(tok){
                case tmglobal: push(&accsTemp,(actionBuffer)globalRead);break;
                case tmtile:   push(&accsTemp,(actionBuffer)tileReadNT);break;
                case tmlocal:  push(&accsTemp,(actionBuffer)localAccg);
                  if(writeInLocalMem) goto splitNormal;
                  break;
                case tmpiece:  push(&accsTemp,(actionBuffer)pieceg);break;
                badParameter;
                }
                tok=pop(p);
                gatherCte(&accsTemp,tok);
                break;
              }else{
                switch(tok){
                case tmglobal: push(&accsTemp,(actionBuffer)globalReadNTi);break;
                case tmtile:   push(&accsTemp,(actionBuffer)tileReadNTi);break;
                case tmlocal:  push(&accsTemp,(actionBuffer)localAccgi);
                  if(writeInLocalMem) goto splitNormal;
                  break;
                case tmpiece:  push(&accsTemp,(actionBuffer)piecegi);break;
                badParameter;
                }
              }
              tok=pop(p);
            }
          }
        }else{

          conditionBuffer cond;
          switch(op){
          case tmset: cond=msetC;break;
          case tmadd: cond=maddC;break;
          case tmdist:cond=mdist;break;
          case tmcmp: cond=mcmp;break;
          case tmless:cond=mless;break;
          case tmmore:cond=mmore;break;
          case tmgoto:cond=mgoto;break;
          default: fail("bad condition");
          }
          push(&condsTemp,cond);
          for(int i=0;i<2;i++){
            tok=pop(p);
            if(isCte(tok)){
              gatherCte(&condsTemp,tok,true);
              continue;
            }
            while(true){
              int nextTok=peek(p);
              if(isCte(nextTok)){
                switch(tok){
                case tmglobal: push(&condsTemp,(conditionBuffer)globalRead);/*setUpMemTriggersPerNormalHolderTemp.push_back({0,tg[j]});*/break;
                case tmtile:   push(&condsTemp,(conditionBuffer)tileRead);break;
                case tmlocal:  push(&condsTemp,(conditionBuffer)localg);
                  if(i==0&&write){
                    writeInLocalMem=true;
                    p->writeInLocalMem=true;
                  }break;
                case tmpiece:  push(&condsTemp,(conditionBuffer)pieceg);break;
                badParameter;
                }
                tok=pop(p);
                gatherCte(&condsTemp,tok);
                break;
              }else{
                switch(tok){
                case tmglobal: push(&condsTemp,(conditionBuffer)globalReadi);break;
                case tmtile:   push(&condsTemp,(conditionBuffer)tileReadi);break;
                case tmlocal:  push(&condsTemp,(conditionBuffer)localgi);break;
                case tmpiece:  push(&condsTemp,(conditionBuffer)piecegi);break;
                badParameter;
                }
              }
              tok=pop(p);
            }
          }
        }
        break;
        {
        splitNormal:
          int tok;
          do{
            p->ind--;
            tok=p->tokens[p->ind];
          }while(tok!=tmset&&tok!=tmadd);
          void(*cmd)(void);
          do{
            accsTemp.size--;
            cmd=accsTemp[accsTemp.size-1];
          }while(cmd!=msetG  && cmd!=msetT  &&
                 cmd!=msetGi && cmd!=msetTi &&
                 cmd!=msetP);
          accsTemp.size--;
          setupBarrays();
          n->sig=parseNormal(p);
          return n;
          //medio choto esto porque tengo que reconstruir el comando,
          //por ahi vale la pena dejarlo terminar y copiarlo terminado.
          //el tema es que no hay forma segura de copiarlo, debería
          //parametrizarlo y esta funcion ya es un quilombo y este es un caso
          //oscuro
        }
        #undef badParameter
        break;
      }
    case tclick:
      p->clickExplicit=true;
      n->bools|=hasClick|makeClick;
      setupBarrays();
      n->sig=parseOp(p);
      ///TODO prohibir dos clicks seguidos en el preprocesado
      return n;
    case tmovEnd:
      if(!p->clickExplicit)
        n->bools|=hasClick|makeClick;
    default:
      p->ind--;
      setupBarrays();
      n->sig=parseOp(p,true);
      return n;
      }
    }
}

void peekClick(operador* op,parseMovData* p){
  if(peek(p)==tclick){
    pop(p);
    op->bools|=makeClick;
  }
  if(!p->clickExplicit&&peek(p)==tmovEnd){
    op->bools|=makeClick;
  }
}

desliz* parseDesliz(parseMovData* p){
  desliz* d=alloc<desliz>(p->b);

  d->tipo=DESLIZ;
  d->bools=0;

  int iters=0;
  if(peek(p)>2048){
    iters=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;

  d->inside=parseOp(p);

  failIf(pop(p)!=tend,"desliz with no end");

  d->iterSize=p->movSize-movSizeBefore;
  if(iters==0){
    v dims=p->pd->dims;
    d->insideSize=d->iterSize*((dims.x>dims.y?dims.x:dims.y));
  //por default iteraciones necesarias para recorrer el tablero en linea recta
  }else{
    d->insideSize=d->iterSize*iters;
  }
  p->movSize+=sizeof(deslizHolder)-d->iterSize+d->insideSize;
  
  peekClick((operador*)d,p);
  d->sig=parseOp(p);

  if(d->bools&makeClick)
    d->bools|=hasClick;
  else
    for(operador* op=d->inside;op!=nullptr;op=op->sig)
      if(op->bools&hasClick){
        d->bools|=hasClick;
        break;
      }
  return d;
}


exc* parseExc(parseMovData* p){
  exc* e=alloc<exc>(p->b);

  e->tipo=EXC;
  e->bools=0;
  int movSizeBefore=p->movSize;

  int finalTok;
  vector<operador*> opsTemp;init(&opsTemp);defer(&opsTemp);
  do{
    operador* op=parseOp(p);
    push(&opsTemp,op);
    finalTok=pop(p);
  }while(finalTok==tseparator);
  failIf(finalTok!=tend,"exc with no end");

  allocCopy(p->b,&e->ops,opsTemp.size,opsTemp.data);
  p->movSize+=size(e->ops);

  e->insideSize=p->movSize-movSizeBefore;
  p->movSize+=sizeof(excHolder);

  peekClick((operador*)e,p);
  e->sig=parseOp(p);
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

isol* parseIsol(parseMovData* p){
  isol* i=alloc<isol>(p->b);

  //bool clickExplicitBefore=p->clickExplicit;
  //p->clickExplicit=false;
  bool writeInLocalMemBefore=p->writeInLocalMem;//en caso de anidar operadores
  p->writeInLocalMem=false;

  i->bools=0;
  i->bools|=hasClick;

  int movSizeBefore=p->movSize;

  i->inside=parseOp(p);
  failIf(pop(p)!=tend,"isol with no end");

  p->movSize+=sizeof(isolHolder);
  i->size=p->movSize-movSizeBefore;

  if(p->writeInLocalMem&&p->memLocal.size!=p->memLocal.resetUntil)
    i->tipo=ISOLNRM;
  else
    i->tipo=ISOL;
  p->writeInLocalMem=writeInLocalMemBefore;

  //al final decidi que isol ponga clickers automaticamente si no se explicitan
  //porque en movimientos complejos hay casos donde se usan isoles sin clickers y es molesto recordar cancelar la actitud default
  //podría hacerse que no influencie clickExplicit para el resto del movimiento, pero dejar eso haria que por ejemplo el emperador
  //ponga un clicker en la esquina inferior izquierda, y a primera vista no esta claro por que
  //if(!p->clickExplicit)
  //  i->bools|=makeClick;
  //p->clickExplicit=clickExplicitBefore;

  i->sig=parseOp(p);

  return i;
}

desopt* parseDesopt(parseMovData* p){
  desopt* d=alloc<desopt>(p->b);
  d->bools=0;

  int slots=0;
  if(peek(p)>2048){
    slots=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;
  bool writeInLocalMemBefore=p->writeInLocalMem;
  vector<operador*> opsTemp;init(&opsTemp);defer(&opsTemp);
  vector<int> sizesTemp;init(&sizesTemp);defer(&sizesTemp);
  int branches=0;
  do{
    int movSizeBefore=p->movSize;
    operador* op=parseOp(p);
    push(&opsTemp,op);
    push(&sizesTemp,p->movSize-movSizeBefore);
    branches++;
  }while(pop(p)==tseparator);
  failIf(p->tokens[p->ind-1]!=tend,"desopt with no end");

  allocCopy(p->b,&d->ops,opsTemp.size,opsTemp.data);

  for(int i=0;i<sizesTemp.size;i++)
    sizesTemp[i]+=sizeof(desoptHolder::node*);


  allocCopy(p->b,&d->movSizes,branches,sizesTemp.data);

  p->movSize+=branches*sizeof(desoptHolder::node*);
  d->clusterSize=p->movSize-movSizeBefore;
  d->dinamClusterBaseOffset=d->clusterSize+d->clusterSize*branches;

  if(slots==0) slots=branches*6;
  d->desoptInsideSize=d->clusterSize+d->clusterSize*branches+d->clusterSize*slots;

  p->movSize+=sizeof(desoptHolder)+d->desoptInsideSize-d->clusterSize;

  d->bools&=~makeClick;

  if(p->writeInLocalMem&&p->memLocal.size!=p->memLocal.resetUntil)
    d->tipo=DESOPTNRM;
  else
    d->tipo=DESOPT;
  p->writeInLocalMem=writeInLocalMemBefore;
  
  peekClick((operador*)d,p);//que significa que desopt sea click?
  d->sig=parseOp(p);
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

operador* parseFail(parseMovData* p){
  operador* failOp=alloc<operador>(p->b);
  failOp->tipo=FAILOP;
  failOp->sig=nullptr;
  failOp->bools=0;
  p->movSize+=sizeof(movHolder);
  return failOp;
}
