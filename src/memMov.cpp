

//como lo planteaba antes tenia una funcion mcond (y macc) que se traia el cond y lo llamaba. Como medio al pedo usar polimorfismo 2 veces, ahora tengo un macro al principio 

#undef CONDRET
#if debugMode
#define CONDRET(VAL) debugShowAndWaitMem(__func__,VAL); return VAL
#else
#define CONDRET(VAL) return VAL 
#endif
//prefiero cargar las cosas debug especificas de los getters desde los getters
//hacerlo de otra forma implicaria pasar los getters a debugShowAndWaitMem, que los llamaria devuelta con el buffer reseteado. Para eso tengo que guardar el estado inicial del buffer. Ademas de esto no tendría la informacion de a que memoria pertenece lo que acabo de hacer, a menos que haga un switch con los punteros de funcion o algo asi. 

struct get{
  int* a;
  int* b;
};
int* getGetter1(){
  getter g=(getter)getNextInBuffer();
  return g();
}
get getGetter2(){
  get g;
  g.a=getGetter1();
  g.b=getGetter1();
  return g;
}

bool mcmp(){
  get g=getGetter2();
  CONDRET(*g.a==*g.b);
}
bool msetC(){
  get g=getGetter2();
  *g.a=*g.b;
  CONDRET(true);
}
bool maddC(){
  get g=getGetter2();
  *g.a+=*g.b;
  CONDRET(true);
}
bool mless(){
  get g=getGetter2();
  CONDRET(*g.a<*g.b);
}
bool mmore(){
  get g=getGetter2();
  CONDRET(*g.a>*g.b);
}
bool mdist(){
  get g=getGetter2();
  CONDRET(*g.a!=*g.b);
}


//las acciones de memoria tienen una version global y tile, porque ahora son los unicos buffers globales y necesitan hacer cosas distintas
//piden un getter solo porque el primero siempre es write

///una escritura que cambie el valor activa todos los triggers asociados que no sean del mismo holder.
///tile tiene una condicion mas (que no haya variado el step), y other tambien (que no hayan variado dos steps)



//tecnicamente con usar set bastaria, siempre se puede hacer una lectura y calcular algo complejo con getters normales, y al final hacer la escritura. Como tengo pensado hacer el parser distinto al pedo agregar add aca

void setCheck(memData* md){
  int* val1=&md->val;
  int val2=*getGetter1();
  if(*val1!=val2){
    *val1=val2;
    chargeTriggers(&md->triggersUsed,&md->firstTriggerBox);
  }
}

void msetG(){
  int ind=(intptr)getNextInBuffer();
  memData* md=&actualHolder.brd->memGlobals[ind];
  setCheck(md);
}
void msetGi(){
  getter g=(getter)getNextInBuffer();
  int ind=*g();
  memData* md=&actualHolder.brd->memGlobals[ind];
  setCheck(md);
}

void msetT(){
  int ind=(intptr)getNextInBuffer();
  memData* md=getTileMd(ind,actualHolder.brd);
  setCheck(md);
}
void msetTi(){
  getter g=(getter)getNextInBuffer();
  int ind=*g();
  memData* md=getTileMd(ind,actualHolder.brd);
  setCheck(md);
}


//set normal, pieza no necesita poner triggers. Igual a msetC pero sin las cosas debug
//TODO podría tener un bool inAction para hacer un if en codigo debug
void msetP(){
  get g=getGetter2();
  *g.a=*g.b;
}
