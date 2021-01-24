


void makeBoard(properState* p){

  parseData* pd=&p->pd;
  brd=new(p->gameState.data)board;
  brd->dims=pd->dims;
  actualHolder.brd=brd;

  escala=16*(1/(float)(brd->dims.x>brd->dims.y?brd->dims.x:brd->dims.y));
  brd->b.setTexture(image.get("tiles.png"));
  brd->n.setTexture(image.get("tiles.png"));
  brd->b.setScale(escala,escala);
  brd->n.setScale(escala,escala);
  brd->b.setTextureRect(IntRect(0,0,32,32));
  brd->n.setTextureRect(IntRect(32,0,32,32));

  for(Piece* piece:p->pieces){
    piece->spriteb.setScale(escala,escala);
    piece->spriten.setScale(escala,escala);
  }
  for(RectangleShape* rs:colores){
    rs->setSize(sf::Vector2f(32*escala,32*escala));
  }

  init(&brd->ts);
  brd->memGlobalSize=pd->memGlobalSize;
  brd->memTileSlots=pd->memTileSlots;
  brd->memGlobals=(memData*)(brd->tiles+brd->dims.x*brd->dims.y);
  memset(brd->memGlobals,0,pd->memGlobalSize*sizeof(memData));
  brd->memTiles=brd->memGlobals+pd->memGlobalSize;
  memset(brd->memTiles,0,pd->memTileSlots*brd->dims.x*brd->dims.y*sizeof(memData));

  p->gameState.head+=sizeof(board)
    + pd->dims.x*pd->dims.y*(sizeof(Tile)+pd->memTileSlots*sizeof(memData))
    + pd->memGlobalSize*sizeof(memData);


  memset(brd->tiles,0,sizeof(Tile)*pd->dims.x*pd->dims.y);
  for(int i=0; i<brd->dims.x*brd->dims.y; i++){
    Tile* tile=&brd->tiles[i];
    tile->pos=v(i%brd->dims.x,i/brd->dims.x);
    int id=pd->boardInit[i];
    if(id!=0){
      int sind=getCodedPieceIndexById(&pd->ids,id);
      tile->holder=initHolder(p->pieces[std::abs(sind)-1],sign(sind),tile,&p->gameState);
    }
  }

#if debugMode
  initDebugSystem();
#endif

  //assertf(p->gameState.head==p->gameState.data+p->hsSize,"antes de generar %d %d\n",p->gameState.head-p->gameState.data,p->hsSize);
  //TODO cuando haga el codigo general tendría que generar primero un bando, correr el codigo, despues el otro
  for(int i=0;i<brd->dims.x*brd->dims.y;i++){
    Holder* hAct=brd->tiles[i].holder;
    if(hAct)
      generar(hAct);
  }
  //assertf(p->gameState.head==p->gameState.data+p->hsSize,"despues de generar %d %d\n",p->gameState.head-p->gameState.data,p->hsSize);
}

Tile* tile(board* brd,v pos){
  assert(pos.x>=0);
  assert(pos.y>=0);
  assert(pos.x<brd->dims.x);
  assert(pos.y<brd->dims.y);
  return &brd->tiles[pos.x+pos.y*brd->dims.x];
}

void drawTiles(board* brd){
  for(int i=0; i<brd->dims.x; i++)
    for(int j=0; j<brd->dims.y; j++){
      if((i+j)&1){
        brd->b.setPosition(i*escala*32,j*escala*32);
        window.draw(brd->b);
      }else{
        brd->n.setPosition(i*escala*32,j*escala*32);
        window.draw(brd->n);
      }
    }
}

void drawHolder(Holder*);
void drawPieces(board* brd){
  for(int i=0; i<brd->dims.x; i++)
    for(int j=0; j<brd->dims.y; j++){
      Holder* p;
      if((p=brd->tiles[i+j*brd->dims.x].holder))
        drawHolder(p);
    }
}

//uso un vector porque necesito espacio ilimitado
//en un principio pensaba intentar meter todo en un bloque
//limitando la cantidad de triggers posibles o usando buckets por si hay mas.
//pero probablemente no haya una ventaja en tenerlo todo junto ahi, porque
//los datos del tablero estan lejos de todas formas y probablemente el acceso cueste lo mismo
/*
struct triggerSpace{
  int firstFree=-1;
  int lastFree=-1;
  vector<triggerBox> mem;
};

int newTriggerBox(triggerSpace* ts){
  if(ts->firstFree==-1){
    ts->mem.emplace_back();
    return ts->mem.size()-1;
  }
  int ret=ts->firstFree;
  if(ts->firstFree==ts->lastFree){
    ts->firstFree=ts->lastFree=-1;
  }else{
    ts->firstFree=(int)ts->mem[ts->firstFree];
  }
  return ret;
}

void freeTriggerBox(triggerSpace* ts,int ind){
  if(ts->firstFree==-1){
    ts->firstFree=ind;
    ts->lastFree=ind;
  }else{
    ts->mem[ts->lastFree]=(triggerBox)ind;
    ts->lastFree=ind;
  }
}
*/

void init(triggerSpace* ts){
  ts->size=16;
  ts->mem=new triggerBox[16];
  for(int i=0;i<16;i++){
    ts->mem[i].nextFree=i+1;
  }
  ts->firstFree=0;
  ts->lastFree=15;
}
int newTriggerBox(triggerSpace* ts){
  int ret=ts->firstFree;
  if(ts->firstFree==ts->lastFree){
    int sizeBefore=ts->size;
    ts->size*=2;//por ahi convendría crecer una cantidad fija para que no haya mucha dispersion, no sé
    triggerBox* newMem=new triggerBox[ts->size];
    memcpy(newMem,ts->mem,sizeBefore*sizeof(triggerBox));
    delete[] ts->mem;
    ts->mem=newMem;
    for(int i=sizeBefore;i<ts->size;i++){
      ts->mem[i].nextFree=i+1;
    }
    ts->firstFree=sizeBefore;
    ts->lastFree=ts->size-1;
  }else{
    ts->firstFree=ts->mem[ts->firstFree].nextFree;
  }
  return ret;
}
void freeTriggerBox(triggerSpace* ts,int ind){
  ts->mem[ts->lastFree].nextFree=ind;
  ts->lastFree=ind;
}


//@optim cuando pueda visualizar y medir bien probar hacer que arranquen todos con una caja

void pushTrigger(int* used,int* pushTo){
  triggerSpace* ts=&actualHolder.brd->ts;
  normalHolder* n=actualHolder.nh;

  int ind=*used;
  if(ind==0){//caso inicial. Si decido hacer que todos los tiles arranquen con 1 triggerBox no es necesario
    *pushTo=newTriggerBox(ts);
    ts->mem[*pushTo].triggers[0]=Trigger({n,n->base->holder->step});
    *used=1;
    return;
  }

  int tb=*pushTo;
  while(ind>triggersPerBox){
    ind-=triggersPerBox;
    tb=ts->mem[tb].next;
  }
  if(ind==triggersPerBox){
    int newTb=newTriggerBox(ts);
    ts->mem[tb].next=newTb;
    tb=newTb;
    ind=0;
  }
  ts->mem[tb].triggers[ind]=Trigger({n,n->base->holder->step});
  (*used)++;
}

vector<normalHolder*> trigsActivados; //para llamar a todos los mh una vez, despues de procesar pisados y limpiar
void chargeTriggers(int* used,int* source){
  triggerSpace* ts=&actualHolder.brd->ts;

  int tb=*source;
  int tu=*used;

  auto evalTrig=[&](int ind)->void{
                  Trigger trig=ts->mem[tb].triggers[ind];

                  int stepCheck=trig.nh->base->holder->step;

                  if(trig.step==stepCheck){
                    push(&trigsActivados,trig.nh);
                  }
                };


  while(tu>triggersPerBox){
    for(int i=0;i<triggersPerBox;i++){
      evalTrig(i);
    }
    tb=ts->mem[tb].next;
    tu-=triggersPerBox;
  }
  for(int i=0;i<tu;i++){
    evalTrig(i);
  }

  //clear
  tb=*source;
  int triggerBoxToClear=(*used+triggersPerBox-1)/triggersPerBox;
  for(;triggerBoxToClear;triggerBoxToClear--){
    int ctb=tb;
    tb=ts->mem[tb].next;
    freeTriggerBox(ts,ctb);
  }
  *used=0;
}


jmp_buf jmpReaccion;
int contador=0;
void activateTriggers(){
    //los triggers duplicados (por dos condiciones poniendo dos triggers a un mismo normalHolder, o por
    //dos lecturas a una memoria dinamica en distintos turnos) no son un problema, no causan calculos extra.
    //Van a mandarse juntos en la misma activacion, el primero va a causar la generacion y el segundo va a quedar colgado
    if(trigsActivados.size==0) return;
    if(trigsActivados.size==1){
        switchToGen=false;
        Base* base=trigsActivados[0]->base;
        actualHolder.h=base->holder;

        if(!setjmp(jmpReaccion))
          base->root->table->reaccionar(base->root,trigsActivados[0]);
    }
    else{
      //@test no sería suficiente con ordenar segun nh desde el principio?

        ///@optim supongo que volcarlo a una matriz es mas rapido que ordenarlo y trocearlo
      std::sort(&trigsActivados[0],&trigsActivados.data[trigsActivados.size],[](normalHolder* a,normalHolder* b)->bool
             {return a->base->root<b->base->root;});
        int i=0;
        while(i<trigsActivados.size){
            switchToGen=false;
            movHolder* base=trigsActivados[i]->base->root;
            int j=i+1;
            while(j<trigsActivados.size&&trigsActivados[j]->base->root==base)
                j++;
            actualHolder.h=base->base->holder;

            if(!setjmp(jmpReaccion)){
              if(j==i+1)
                base->table->reaccionar(base,trigsActivados[i]);
              else{
                nhBuffer nb;
                nb.size=j-i;
                nb.beg=0;
                nb.buf=(normalHolder**)alloca(nb.size*sizeof(normalHolder*));
                memcpy(nb.buf,&trigsActivados[i],(j-i)*sizeof(normalHolder*));

                std::sort(&nb.buf[0],&nb.buf[nb.size],[](normalHolder* a,normalHolder* b)->bool{return a<b;});
                //TODO si apunto al otro vector en vez de hacer la copia debería andar igual
                base->table->reaccionarVec(base,&nb);
              }
            }

            i=j;
        }
    }
    trigsActivados.size=0;
}

/*
MULTIPLES TRIGGERS
cuando una pieza se mueve hacia una torre le activa dos triggers, primero el mas lejano por salir de ese espacio,
y despues el mas cercano por llegar a ese espacio.
Si se reacciona cada trigger individualmente en el orden que llegan primero se va a recalcular el del espacio vacio,
que va a recalcular todo el recorrido despues de la pieza. Y esto es al pedo, porque el segundo trigger va a invalidar
todo.
Por ahora este problema se soluciona procesando los triggers en orden inverso, asi primero invalida y el segundo no se
procesa. Pero esto es una heuristica, no va a servir para todos los casos.


Un problema distinto pasa cuando se activan dos triggers en una pieza con un desliz exc, estando los dos triggers en ramas
distintas.
desliz exc tiene el mismo problema que desliz, pero a demas tiene un otro especifico
Tenemos un desliz exc P , S end end
la rama actual generada es SP...
Una pieza activa el trigger que invalida P1, y tambien el trigger de P2.
Si se recalcula primero P2 se va a armar todo un recorrido que despues se va a invalidar por el segundo
trigger, que hace S1->P1 y recalcula todo el recorrido.

Si se recalcula en el otro orden pasa algo mas raro. Se hace S1->P1, se arma todo el recorrido. Cuando se hace la segunda
corrida buscando P2, que es de un trigger que pertenencia al recorrido que fue reemplazado, no deberia pasar nada. Pero
como se busca por punteros, el nuevo P2 tiene la misma posicion en memoria que P2 viejo, y se va a encontrar y se va a
recalcular al pedo.

Las dos formas de recorrer hacen recalculos innecesarios. El segundo caso se puede solucionar preguntando por mas cosas
a demas de el puntero, solo en exc.

Pero esta pensando que estos dos casos surgen del mismo problema, tener mas de un trigger en un mismo movimiento, y recalcular
uno por uno.
Una solucion sería recalcular preguntando por todos los triggers al mismo tiempo. Cuando se encuentre alguno se pasa a
generar, y el resto se ignora. Si hay partes del movimiento que no se regeneran y podrian tener triggers se revisan. Triggers
en zonas que fueron regeneradas no se activan dos veces, triggers que hayan quedado sueltos se ignoran. Creo que no romperia
nada.

En casos de multiples triggers seria mas rapido porque solo se recorre una vez, y no tiene errores de regenerar al pedo.
En casos de un solo trigger sería mas lento porque agrega manejo de vectores cuando no se necesita. Pero este caso podria
separarse del otro con un if y listo.

Lo unico malo de agregar esto es que hace al codigo mas bizarro, porque agregaria una version vector de reaccionar. Pero lo vale
*/
