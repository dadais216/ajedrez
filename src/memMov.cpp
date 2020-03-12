

//como lo planteaba antes tenia una funcion mcond (y macc) que se traia el cond y lo llamaba. Como medio al pedo usar polimorfismo 2 veces, ahora tengo un macro al principio 

#undef CONDRET
#if debugMode
#define CONDRET(VAL) debugShowAndWaitMem(__func__,VAL); return VAL
#else
#define CONDRET(VAL) return VAL 
#endif
//prefiero cargar las cosas debug especificas de los getters desde los getters
//hacerlo de otra forma implicaria pasar los getters a debugShowAndWaitMem, que los llamaria devuelta con el buffer reseteado. Para eso tengo que guardar el estado inicial del buffer. Ademas de esto no tendría la informacion de a que memoria pertenece lo que acabo de hacer, a menos que haga un switch con los punteros de funcion o algo asi. 

bool mcmp(){
  get g=getterInfoFromBuffer();
  CONDRET(*g.a==*g.b);
}
bool msetC(){
  get g=getterInfoFromBuffer();
  *g.a=*g.b;
  CONDRET(true);
}
bool maddC(){
  get g=getterInfoFromBuffer();
  *g.a+=*g.b;
  CONDRET(true);
}
bool mless(){
  get g=getterInfoFromBuffer();
  CONDRET(*g.a<*g.b);
}
bool mmoreCond(){
  get g=getterInfoFromBuffer();
  CONDRET(g.a>g.b);
}
bool mdistCond(){
  get g=getterInfoFromBuffer();
  CONDRET(g.a!=g.b);
}


//las acciones de memoria tienen una version global y tile, porque ahora son los unicos buffers globales y necesitan hacer cosas distintas
//piden un getter solo porque el primero siempre es write

///una escritura que cambie el valor activa todos los triggers asociados que no sean del mismo holder.
///tile tiene una condicion mas (que no haya variado el step), y other tambien (que no hayan variado dos steps)
void activateGlobalMemTriggers(int ind){
  for(normalHolder* nh:memGlobalTriggers[ind]->perma)
    if(nh->base->h!=actualHolder.h&&nh->base->h->inPlay)
      trigsActivados.push_back(nh);
  for(normalHolder* nh:memGlobalTriggers[ind]->dinam)
    if(nh->base->h!=actualHolder.h&&nh->base->h->inPlay)
      trigsActivados.push_back(nh);
  memGlobalTriggers[ind]->dinam.clear();
  //si se sacan los ifs se harian recalculos al pedo en algunos casos, pero puede que termine siendo mas rapido asi
}

void activateTileMemTriggers(int ind){
  for(Tile::tileTrigInfo& tti:actualTile->memTileTrigs[ind])
    if(tti.nh->base->h!=actualHolder.h&&tti.step==*tti.stepCheck)
      trigsActivados.push_back(tti.nh);
  memTile->clear();
}


//tecnicamente con usar set bastaria, siempre se puede hacer una lectura y calcular algo complejo con getters normales, y al final hacer la escritura. Como tengo pensado hacer el parser distinto al pedo agregar add aca
#define setCheck(ind,memory,triggers)                      \
  int* val1=&memory[ind];                                  \
  int val2=getterMemDataInBuffer1();                       \
  if(*val1!=val2){                                         \
    *val1=val2;                                            \
    triggers(ind);                                         \
  }

void msetG(){
  int ind=getNextInBuffer();
  setCheck(ind,memGlobal,activateGlobalMemTriggers);
}
void msetGi(){
  getter g=getNextInBuffer();
  int ind=*g();
  setCheck(ind,memGlobal,activateGlobalMemTriggers);
}

void msetT(){
  int ind=getNextInBuffer();
  setCheck(ind,actualTile->memTile,activateTileMemTriggers);
}
void msetTi(){
  getter g=getNextInBuffer();
  int ind=*g();
  setCheck(ind,actualTile->memTile,activateTileMemTriggers);
}
