


void armarTablero(board* brd, v dims){
  t->dims=dims;

  for(int i=0; i<dims.x; i++)
    for(int j=0; j<dims.y; j++){
      Tile* tile=&brd->tiles[i+j*dims.x];
      memset(tile,0,sizeof(Tile));
      tile->pos=v(i,j);
      tile->holder=nullptr;//poner puntero a holder aca
      tile->triggersUsed=0;
      tile->step=0;
    }

  escala=16*(1/(float)(dims.x>dims.y?dims.x:dims.y));
  brd->b.setTexture(image.get("tiles.png"));
  brd->n.setTexture(image.get("tiles.png"));
  brd->b.setScale(escala,escala);
  brd->n.setScale(escala,escala);
  brd->b.setTextureRect(IntRect(0,0,32,32));
  brd->n.setTextureRect(IntRect(32,0,32,32));

  init(&brd->ts);

  brd->memGlobal=(memData*)(brd->tiles+brd.dims.x*brd.dims.y);
  brd->memTile=brd->memGlobal+parser->globalQuantity;
}

Tile* tile(board* brd,v pos){
  return &brd->tile[pos.x+pos.y*brd->dim.x];
}

void draw(board* brd){
  for(int i=0; i<tam.x; i++)
    for(int j=0; j<tam.y; j++){
      if((i+j)&1){
        b.setPosition(i*escala*32,j*escala*32);
        window.draw(b);
      }else{
        n.setPosition(i*escala*32,j*escala*32);
        window.draw(n);
      }
      Holder* p;
      if((p=brd->tiles[i+j*brd->dims.x]->holder))
        draw(p);
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
    ts->mem[i]=(triggerBox)(i+1);
  }
  firstFree=0;
  lastFree=15;
}
int newTriggerBox(triggerSpace* ts){
  int ret=ts->firstFree;
  if(ts->firstFree==ts->lastFree){
    int sizeBefore=ts->size;
    ts->size*=2;//por ahi convendría crecer una cantidad fija para que no haya mucha dispersion, no sé
    triggerBox* newMem=new triggerBox[ts->size];
    memcpy(newMem,ts->mem,sizeBefore);
    delete ts->mem;
    ts->mem=newMem;
    for(int i=sizeBefore;i<ts->size;i++){
      ts->mem[i]=(triggerBox)(i+1);
    }
    ts->firstFree=sizeBefore;
    ts->lastFree=ts->size-1;
  }else{
    ts->firstFree=(int)ts->mem[ts->firstFree];
  }
  return ret;
}
void freeTriggerBox(triggerSpace* ts,int ind){
  ts->mem[ts->lastFree]=(triggerBox)ind;
  ts->lastFree=ind;
}


//@optim cuando pueda visualizar y medir bien probar hacer que arranquen todos con una caja

//@optim se da la casualidad de que used y pushTo siempre son contiguos, podría probar pasarlos como un struct
void pushTrigger(triggerSpace* ts,int* used,int* pushTo,Tile* tile,normalHolder* n){
  int ind=*used;
  if(ind==0){//caso inicial. Si decido hacer que todos los tiles arranquen con 1 triggerBox no es necesario
    *pushTo=newTriggerBox(ts);
    ts->mem[*pushTo]->triggers[0]=Trigger({n,&tile->step,tile->step});
    *used=1;
    return;
  }

  int tb=*pushTo;
  while(ind>triggersPerBox){
    ind-=triggersPerBox;
    tb=ts->mem[tb]->next;
  }
  if(ind==triggersBox){
    int newTb=newTriggerBox(ts);
    ts->mem[tb]->next=newTb;
    tb=newTb;
    ind=0;
  }
  ts->mem[tb]->triggers[ind]=Trigger({n,&tile->step,tile->step});
  *used++;
}

vector<normalHolder*> trigsActivados; //para llamar a todos los mh una vez, despues de procesar pisados y limpiar
void chargeTriggers(triggerSpace* ts,int* used,int* source){
  int tb=*source;
  int tu=*used;

  auto evalTrig=[&](int ind)->void{
                  Trigger trig=ts->mem[tb]->triggers[ind];
                  if(trig.step==*trig.stepCheck){
                    trigsActivados.push_back(trig.nh);
                  }
                };


  while(tu>triggersPerBox){
    for(int i=0;i<triggersPerBox;i++){
      evalTrig(i);
    }
    tb=ts->mem[tb]->next;
    tu-=triggersPerBox;
  }
  for(int i=0;i<tu;i++){
    evalTrig(i);
  }

  //clear
  tb=*source;
  *used=0;
  do{
    triggerBox* temp=tb->next;
    freeTriggerBox(tb);
    tb=temp;
  }while(tb);
  //tile->triggerBox->next=nullptr;
}

int contador=0;
void activateTriggers(){
    //los triggers duplicados (por dos condiciones poniendo dos triggers a un mismo normalHolder, o por
    //dos lecturas a una memoria dinamica en distintos turnos) no son un problema, no causan calculos extra.
    //Van a mandarse juntos en la misma activacion, el primero va a causar la generacion y el segundo va a quedar colgado
    if(trigsActivados.size()==0) return;
    if(trigsActivados.size()==1){
        switchToGen=false;
        Base* base=trigsActivados[0]->base;
        actualHolder.h=base->h;

        base->beg->table->reaccionar(base->beg,trigsActivados[0]);
    }
    else{
      //@test no sería suficiente con ordenar segun nh desde el principio?

        ///@optim supongo que volcarlo a una matriz es mas rapido que ordenarlo y trocearlo
        sort(trigsActivados.begin(),trigsActivados.end(),[](normalHolder* a,normalHolder* b)->bool
             {return a->base->beg<b->base->beg;});
        int i=0;
        while(i<trigsActivados.size()){
            switchToGen=false;
            movHolder* base=trigsActivados[i]->base->beg;
            int j=i+1;
            while(j<trigsActivados.size()&&trigsActivados[j]->base->beg==base)
                j++;
            actualHolder.h=base->base->h;
            if(j==i+1)
              base->table->reaccionar(base,trigsActivados[i]);
            else{
                vector<normalHolder*> nhs(&trigsActivados[i],&trigsActivados[j]);
                sort(nhs.begin(),nhs.end(),[](normalHolder* a,normalHolder* b)->bool{return a<b;});
                //estaria bueno no hacer una copia, no es muy importante igual
                base->table->reaccionarVec(base,&nhs);
            }
            i=j;
        }
    }
    trigsActivados.clear();
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
