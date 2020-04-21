
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
  parseMovData p{operatorBucket,pd,*tokens,0,0,0,false};
  for(int i=0;i<pd->movQ;i++){
    p.movSize=0;
    p.memLocalSize=pd->memLocalSize[i];
    p.clickExplicit=false;

    piece->movs[i].memLocalSize=pd->memLocalSize[i];
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
  default:
    if(setCteReader)
      push(vec,(T)cteRead);
    push(vec,(T)((intptr)tok-2048));break;
  }
}

normal* parseNormal(parseMovData* p){
  normal* n=alloc<normal>(p->b);

  vector<void(*)(void)> accsTemp;init(&accsTemp);defer(&accsTemp);
  vector<bool(*)(void)> condsTemp;init(&condsTemp);defer2(&condsTemp);
  vector<colort*> colorsTemp;init(&colorsTemp);defer3(&colorsTemp);

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

  p->movSize+=sizeof(normalHolder)+p->memLocalSize*sizeof(int);//+ mov de spawner
  n->tipo=NORMAL;
  n->bools&=~(hasClick|makeClick|doEsp);
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
      cond(pass);
    case tesp:
      n->bools|=doEsp;
      break;
#define acc(TOKEN) case t##TOKEN: push(&accsTemp,TOKEN);break
      acc(mov);
      acc(pausa);
    case tcapt:
      push(&accsTemp,capt);
      //if(n->relPos==v(0,0))
      //  p->kamikase=true;
      break;
    case tspwn:
      {
        intptr id=getNum();
        push(&accsTemp,spwn);
        push(&accsTemp,(actionBuffer)(id));
        p->pd->spawner=true;
        addIdIfMissing(p->pd,id);
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
      {
        //mset l0 4 mset g0 l0 <-en este caso necesito cortar en 2 normales, para que el segundo set tenga registrado el primero

        //TODO en vez de cortar la normal no podría escribir el numero directamente en el buffer?
        //seria un poco complejo porque tendría que agregar una instruccion que actualice el buffer en cada local de accion, por ej
        //mset l0 4 mset g0 l0 => mset l0 4 __updateLocalAcc__ 16 0 mset g0 0 <- donde este 0 es sobreescrito por updateLocalAcc en tiempo de condicion
        //esto esta bueno porque saca la necesidad de meter otra normal, lo que probablemente lo haga mas rapido y le saca complejidad

        auto isCte=[](int tok)->bool{
                     return tok>1024||
                       tok==tposX||
                       tok==tposY;
                   };


        int op=tok;
        bool write=op==tmset||op==tmadd;
        tok=peek(p);

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
            defaultAssert;
            }
            tok=pop(p);
            gatherCte(&accsTemp,tok);
            i++;
          }else{
            switch(tok){
            case tmglobal: push(&accsTemp,msetGi);break;
            case tmtile:   push(&accsTemp,msetTi);break;
            case tmpiece:  push(&accsTemp,msetP);push(&accsTemp,(actionBuffer)piecegi);break;
            defaultAssert;
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
                  if(writeInLocalMem) goto splitNormal;break;
                case tmpiece:  push(&accsTemp,(actionBuffer)pieceg);break;
                defaultAssert;
                }
                tok=pop(p);
                gatherCte(&accsTemp,tok);
                break;
              }else{
                switch(tok){
                case tmglobal: push(&accsTemp,(actionBuffer)globalReadNTi);break;
                case tmtile:   push(&accsTemp,(actionBuffer)tileReadNTi);break;
                case tmlocal:  push(&accsTemp,(actionBuffer)localAccgi);
                  if(writeInLocalMem) goto splitNormal;break;
                case tmpiece:  push(&accsTemp,(actionBuffer)piecegi);break;
                defaultAssert;
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
                  if(i==0&&write){writeInLocalMem=true;}break;
                case tmpiece:  push(&condsTemp,(conditionBuffer)pieceg);break;
                defaultAssert;
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
                defaultAssert;
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
  d->bools&=~makeClick;

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

  bool clickExplicitBefore=p->clickExplicit;
  p->clickExplicit=false;

  i->tipo=ISOL;
  i->bools|=hasClick;
  i->bools&=~makeClick;

  int movSizeBefore=p->movSize;

  i->inside=parseOp(p);
  failIf(pop(p)!=tend,"isol with no end");

  p->movSize+=sizeof(isolHolder);
  i->size=p->movSize-movSizeBefore;


  if(!p->clickExplicit)
    i->bools|=makeClick;
  p->clickExplicit=clickExplicitBefore;

  i->sig=parseOp(p);

  return i;
}

desopt* parseDesopt(parseMovData* p){
  desopt* d=alloc<desopt>(p->b);
  d->tipo=DESOPT;

  int slots=0;
  if(peek(p)>2048){
    slots=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;
  vector<operador*> opsTemp;init(&opsTemp);defer(&opsTemp);
  vector<int> sizesTemp;init(&sizesTemp);defer2(&sizesTemp);
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
