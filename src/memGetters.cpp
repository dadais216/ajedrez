


//triggers globales se crean en la creacion de la pieza y son fijos
//triggers de memoria tile e indirectos funcionan los posicionales, se crean dinamicamente y solo son validos los generados en un mismo turno


//lo unico malo de esta version es que el debug quedo ligado a la ejecucion
//si por algun motivo quiero separarlos va a ser un asco
//una solucion sería cambiar las funciones por un switch, porque en ese caso
//debug seria un segundo switch aparte que no le molesta a nadie.
//usar switch podría tener una ventaja ademas de eso, podría probar si tengo ganas (podría ser inherentemente mas rapido?, int en vez de punteros en el buffer, puedo reutilizar codigo en el switch)
void debugDrawMem(int ind,int memSize,int drawOffset){  
#if debugMode
  backgroundMemDebug.setPosition(Vector2f(530+25*(ind%4),
                                          305+drawOffset+45*(ind/4-memSize/4)));
  window.draw(backgroundMemDebug);
#endif
}
void debugSetIndirectColor(){
#if debugMode
  backgroundMemDebug.setFillColor(sf::Color(178,235,221));
#endif
}

void debugUnsetIndirectColor(){
#if debugMode
  backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
#endif
}




//seguro que se puede abstraer en un macro pero no me aportaria nada eso

//los accesos indirectos son una optimizacion

int* localg(){
  intptr ind=(intptr)getNextInBuffer();;
  debugDrawMem(ind,actualHolder.nh->base->memLocalSize,100);
  return &memMov[ind];
}

int* localAccg(){
  intptr ind=(intptr)getNextInBuffer();;
  return actualHolder.nh->memAct[ind];
}

int* localgi(){
  getter g=(getter)getNextInBuffer();
  debugSetIndirectColor();
  int ind=*g();
  debugUnsetIndirectColor();;
  debugDrawMem(ind,actualHolder.nh->base->memLocalSize,100);
  return &memMov[ind];
}

int* localAccgi(){//aparece en cadenas de 3 o mas
  getter g=(getter)getNextInBuffer();
  int ind=*g();
  return actualHolder.nh->memAct[ind];
}

int* pieceg(){
  intptr ind=(intptr)getNextInBuffer();
  debugDrawMem(ind,size(actualHolder.h->memPiece),400);
  return actualHolder.h->memPiece[ind];
}

int* pieceAccg(){//unico motivo de existir es no tirar codigo debug en accion
  intptr ind=(intptr)getNextInBuffer();
  return actualHolder.h->memPiece[ind];
}

int* piecegi(){
  getter g=(getter)getNextInBuffer();
  debugSetIndirectColor();
  int ind=*g();
  debugUnsetIndirectColor();
  debugDrawMem(ind,size(actualHolder.h->memPiece),400);
  return actualHolder.h->memPiece[ind];
}

int* pieceAccgi(){//unico motivo de existir es no tirar codigo debug en accion
  getter g=(getter)getNextInBuffer();
  int ind=*g();
  return actualHolder.h->memPiece[ind];
}


//no hay globalWrite porque esta inlineado directamente en msetG, porque
//solo se usa ahi. Lo mismo para tileWrite con msetT. Esto evita tener que
//hacer una comunicacion rara tambien.

int* globalRead(){
  intptr ind=(intptr)getNextInBuffer();;
  debugDrawMem(ind,actualHolder.brd->memGlobalSize,0);

  memData* md=&actualHolder.brd->memGlobals[ind];
  pushTrigger(&md->triggersUsed,&md->firstTriggerBox);//probar triggers fijos despues
  return &md->val;
}

//no me acuerdo porque hice que step este en el tile donde esta parada la pieza en lugar de meterlo en la pieza
//osea en vez de avanzar el step cada vez que una pieza se mueve, podría avanzarlo en cada generacion y captura.
//es practicamente lo mismo, es mas simple y eso manejaría el caso de movimientos que no se mueven sin hacer
//el hack de avanzar el step en un if @todo

int* globalReadi(){
  getter g=(getter)getNextInBuffer();
  debugSetIndirectColor();
  int ind=*g();
  debugUnsetIndirectColor();
  debugDrawMem(ind,actualHolder.brd->memGlobalSize,0);
  memData* md=&actualHolder.brd->memGlobals[ind];
  pushTrigger(&md->triggersUsed,&md->firstTriggerBox);
  return &md->val;
}

int* globalReadNTi(){
  getter g=(getter)getNextInBuffer();
  debugSetIndirectColor();
  int ind=*g();
  debugUnsetIndirectColor();;
  debugDrawMem(ind,actualHolder.brd->memGlobalSize,0);
  return &actualHolder.brd->memGlobals[ind].val;
}


v posDebugTile(0,0);

memData* getTileMd(int ind,board* b){
  return &b->memTiles[ind+actualHolder.nh->pos.x*b->dims.y+actualHolder.nh->pos.y*b->dims.x*b->memTileSlots];
}

int* tileRead(){
  intptr ind=(intptr)getNextInBuffer();
  debugDrawMem(ind,actualHolder.brd->memTileSize,200);

  memData* md=getTileMd(ind,actualHolder.brd);
  pushTrigger(&md->triggersUsed,&md->firstTriggerBox);
  return &md->val;
}
int* tileReadNT(){  
  intptr ind=(intptr)getNextInBuffer();
  return &getTileMd(ind,actualHolder.brd)->val;
;
}

int* tileReadi(){
  getter g=(getter)getNextInBuffer();
  debugSetIndirectColor();
  int ind=*g();
  debugUnsetIndirectColor();
  debugDrawMem(ind,actualHolder.brd->memTileSlots,200);

  memData* md=getTileMd(ind,actualHolder.brd);
  pushTrigger(&md->triggersUsed,&md->firstTriggerBox);
  return &md->val;
}

int* tileReadNTi(){
  getter g=(getter)getNextInBuffer();
  int ind=*g();
  return &getTileMd(ind,actualHolder.brd)->val;
}

//cteRead == (int)(*)()getNextInBuffer

int* cteRead(){
  int* cte=(int*)&actualHolder.buffer[*actualHolder.bufferPos];//cast void** to int* 
  (*actualHolder.bufferPos)++;
#if debugMode
  //debería haber un if para no mostar durante accion supongo
  //TODO si hago esto evito duplicar algunas cosas de pieza y local
  textValMem.setPosition(610,470);
  textValMem.setString(std::to_string(*cte));
  window.draw(textValMem);
#endif
  return cte;
}

//pos se trata como una cte porque no puede variar desde la generacion hasta la accion inclusive
int* posXRead(){
  //para evitar usar una variable global (porque podría ser mas lenta, no creo pero ni idea) se podría dejar un int en el buffer que se escribe aca
  static int x=offset.x+actualHolder.nh->relPos.x;
#if debugMode
  //
#endif
  return &x;
}

int* posYRead(){
  static int y;
  y=offset.y+actualHolder.nh->relPos.y;
  if(actualHolder.nh->base->h->bando){
    y=actualHolder.brd->dims.y-1-y;
      }
  return &y;
}

//por ahi algo para acceder al step de la pieza seria util?

/*
  sobre tipos de memoria


  En un tiempo considere borrar tile:
  tile guardaba informacion en una memoria global en el casillero, por ejemplo mset t1 3 guardaba 3 en el slot 1 del casillero donde este parada la pieza.
  El sistema de triggers era más complejo que en global, porque necesitaba tener un chequeo de mas para no hacer reaccionar una pieza por un trigger viejo.

  Por ejemplo w mcmp t1 0 mover debería activarse si se escribe el casillero que esta arriba, pero solo si la pieza que puso ese trigger no se movio.
  Un trigger viejo activado permitiría a la pieza moverse hacia arriba cuando la condicion, para la posicion en la que esta ahora, es falsa.

  Para evitar esto guardaba un step de la pieza en el trigger. Cuando se activa el trigger, se mira que el step de la pieza no haya variado.

  Esto en principio no se puede hacer desde el lenguaje, no hay forma de guardar informacion en los triggers.
  Se puede aproximar usando un slot de la tile como memoria del trigger, usando escrituras en tiempo de condicion para mantener
  un step actualizado (en un movimiento sin condiciones, para que solo se actualice cuando se hace un movimiento y no por triggers)
  y escribirlo cuando se hace la lectura del slot vecino. Despues de cada lectura debería haber un segundo chequeo que mire que el step no haya variado.
  Esto mas o menos andaría pero rompe si una pieza hace una segunda lectura, porque estaría pisando el step con su step (lo que ademas de romper
  la logica crearía un bucle infinito). Tambien esta el tema de que al usar este segundo slot se estan agregando triggers, que estan al pedo.
  Esto ultimo se podría solucionar agregando un slot de control por cada pieza que lee, y mejor todavia los slots de control podrían estar en la memoria de
  la pieza que los usa, por lo que no tendrían triggers. 

  Tambien se podría hacer que una pieza guarde una lista de posiciones y en que instancia de movimiento piso en su ultimo turno,
  y frente a un trigger verifique que venga de una de esas posiciones y se este en la instancia de movimiento correcta. Tambien es compleja y lenta.

  La unica solucion que se me ocurre es tener memoria en el trigger que se pueda escribir desde el lenguaje. Agregar un int a cada trigger no debería tener
  un costo, aunque puede que sea mas ineficiente de todas formas porque el chequeo con el trigger se hace en el movimiento de la pieza, mientras antes
  se antes de acceder (puede que esto no importe mucho en la version compilada) (el chequeo se haría solo en respuesta a un trigger, no durante
  la generacion inicial).
  Tambien saca la posibilidad de poder hacer limpiezas de triggers falsos generales, porque ahora los triggers son parte del sistema.
  Por ahi estaría bueno hacer la memoria de trigger algo explicito, pero es bastante niche, y creo que solo haría ciertas cosas raras (como una pieza que
  pueda reaccionar a triggers que dejo en los ultimos n turnos) un poco más accesibles, aunque se podrían programar de otra forma (que la pieza re-haga
  los ultimos n movimientos cada turno, que es mas ineficiente pero cumple la misma funcionalidad, y como es algo raro no me preocupa)

  Asi que decidi dejar la memoria de tile. Acceder a una memoria de trigger sacaría la necesidad de tenerla, pero agrega complejidad al lenguaje, y hace
  las cosas mas ineficientes para el caso mas comun, a cambio de dar la posibilidad de escribir piezas muy raras un poco mas facil.
  Piezas raras podrían implementarse de ser posible abusando los mecanismos de tile, y sino usando cosas como las 2 soluciones anteriores.

  La primera solucion no me parece totalmente mala, dudo que sea mas rapida pero podría probarse. Si no se usan tiles el codigo de tiles no debería
  tener un costo relevante (y se puede comentar eso), asi que se podría probar facil.

  borrar other:
  permite a una pieza acceder a memoria otra pieza, en una posicion. Lo saque porque no era muy usado, agregaba triggers a la memoria de pieza,
  y usarlo era incomodo.
  para usarlo primero hay que checkear que en tal casillero hay una pieza y que es del tipo necesario. Sin hacer estos chequeos se accede a un
  espacio de memoria que puede existir o no y explota todo.
  La misma funcionalidad, haciendo los mismos chequeos, se puede conseguir haciendo que las piezas escriban la memoria de tile

  pense borrar pieza:
  Pero no, no podría implementarse en la memoria global porque no habría forma de direccionarlas a lugares distintos, porque para eso necesitarias
  tener una memoria en la pieza que le diga a donde mirar, y ya que estas ahi podrías meter toda la memoria y listo. Se podría crear harcodeando el
  direccionamiento, haciendo un tipo de pieza para cada pieza del juego (ej 16 tipos de peon)
  Ademas la memoria de pieza no tiene triggers, y el lenguaje se los pondrían innecesariamente en la memoria global. 


 */
