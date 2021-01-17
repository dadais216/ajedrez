



#if debugMode

RectangleShape backgroundMem;
RectangleShape backgroundMemDebug;
RectangleShape localMemorySeparator;
Text textValMem;
Text textIndexMem;

Text textMov;
visualWindow memDrawWindows[4];
visualWindow moveW;

void initDebugSystem(){//se llama despues de construir el tablero
  posPiece.setSize(sf::Vector2f(32*escala,32*escala));
  posActGood.setSize(sf::Vector2f(32*escala,32*escala));
  posActBad.setSize(sf::Vector2f(32*escala,32*escala));
  posMem.setSize(sf::Vector2f(32*escala,32*escala));

  textMov.setScale(.7,.7);
  textMov.setFont(font);

  //agrego getters
  funcToWord[(void(*)())localg]="l";
  funcToWord[(void(*)())localAccg]="l";
  funcToWord[(void(*)())localgi]="l";
  funcToWord[(void(*)())localAccgi]="l";
  funcToWord[(void(*)())pieceg]="p";
  funcToWord[(void(*)())piecegi]="p";
  funcToWord[(void(*)())globalRead]="g";
  funcToWord[(void(*)())globalReadi]="g";
  funcToWord[(void(*)())globalReadNTi]="g";
  funcToWord[(void(*)())tileRead]="t";
  funcToWord[(void(*)())tileReadi]="t";
  funcToWord[(void(*)())tileReadNT]="t";
  funcToWord[(void(*)())tileReadNTi]="t";
  funcToWord[(void(*)())cteRead]="";
  funcToWord[(void(*)())posXRead]="x";
  funcToWord[(void(*)())posYRead]="y";
  funcToWord[(void(*)())posSYRead]="sy";

  funcToWord[(void(*)())msetC]="mset";
  funcToWord[msetP]="mset";
  funcToWord[msetG]="mset";
  funcToWord[msetGi]="mset";
  funcToWord[msetT]="mset";
  funcToWord[msetTi]="mset";

  funcToWord[(void(*)())maddC]="madd";

  posPiece.setFillColor(sf::Color(250,240,190,150));
  posActGood.setFillColor(sf::Color(180,230,100,100));
  posActBad.setFillColor(sf::Color(240,70,40,100));
  posMem.setFillColor(sf::Color(0,0,200,100));
  backgroundMem.setFillColor(sf::Color(240,235,200));
  backgroundMem.setOutlineColor(sf::Color(195,195,175));
  backgroundMem.setOutlineThickness(4);
  backgroundMem.setSize(Vector2f(40,35));
  backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
  backgroundMemDebug.setOutlineColor(sf::Color(195,195,175));
  backgroundMemDebug.setOutlineThickness(4);
  backgroundMemDebug.setSize(sf::Vector2f(40,35));
  textValMem.setColor(sf::Color::Black);
  textValMem.setFont(font);
  //textValMem.setPosition(570,10);
  textIndexMem.setColor(sf::Color(200,200,200,200));
  textIndexMem.setFont(font);
  textIndexMem.setScale(sf::Vector2f(.6,.6));
  localMemorySeparator.setFillColor(sf::Color(150,150,150));
  localMemorySeparator.setOutlineColor(sf::Color::Red);
  localMemorySeparator.setOutlineThickness(2);
  localMemorySeparator.setSize(sf::Vector2f(1,40));

  init(&debugDrawChannel);

  debugMultiParameterBegin=-1;

  memDrawWindows[0].cells=brd->memGlobalSize;
  memDrawWindows[1].cells=brd->memTileSlots;

}

void debugShowAndWait(bool val){
  while(true){
    //si quiero evitar redibujados podría hacer que beg solo se limpie cuando cambia el movimiento,
    //y hacer el segundo dibujado solo cuando se actualize beg
    drawScreen([=](){
                 properDraw(stateMem);
                 debugDrawMemories();
                 debugShowMove(val);
               });
    /*
      textValMem.setString(std::to_string(ps->turno));
      textValMem.setPosition(sf::Vector2f(600,20));
      window.draw(textValMem);
    */
    handleSystemEvents();
    if(Input.c||Input.x||(Input.z))
      break;
    sleep(milliseconds(20));
  }
  if(Input.x)
    sleep(milliseconds(175));
  debugMultiParameterBegin=-1;
}

void updateScrolling(int* beg,int max){
  *beg-=Input.wheelDelta;
  if(*beg<0)
    *beg=0;
  else if(*beg>max)
    *beg=max;
}

int debugGetCellVal(memData* v,int i){
  return (v+i)->val;
}
int debugGetCellVal(auto* v,int i){
  return *(v+i);
}
const int cellsPerRow=16;
const int cellSpacing=39;
void debugDrawMemoryCells(visualWindow* vw,auto memory){
  int yOffset=vw->height*45+30+(vw-&memDrawWindows[0])*10;
  if(Input.wheelDelta!=0&&
     Input.mouse.x>530&&Input.mouse.x<530+cellSpacing*cellsPerRow&&
     Input.mouse.y>yOffset&&Input.mouse.y<yOffset+45*vw->size){
    updateScrolling(&vw->beg,vw->rows-vw->size);
  }

  int beg=vw->beg*cellsPerRow;
  int end=std::min((vw->beg+vw->size)*cellsPerRow,vw->cells);

  for(int i=beg;i<end;i++){
    backgroundMem.setPosition(sf::Vector2f(530+cellSpacing*(i%cellsPerRow),yOffset+45*((i-beg)/cellsPerRow)));
    window.draw(backgroundMem);
  }
  for(int i=beg;i<end;i++){
    textValMem.setPosition(530+cellSpacing*(i%cellsPerRow),yOffset+5+45*((i-beg)/cellsPerRow));
    textValMem.setString(std::to_string(debugGetCellVal(memory,i)));
    textIndexMem.setPosition(530+cellSpacing*(i%cellsPerRow)+22,yOffset+5+45*((i-beg)/cellsPerRow)+17);
    textIndexMem.setString(std::to_string(i));
    window.draw(textValMem);
    window.draw(textIndexMem);
  }
};

normalHolder* nhLastFrame=nullptr;
void computeWindowHeights(){
  if(actualHolder.nh==nhLastFrame)
    return;
  nhLastFrame=actualHolder.nh;

  moveW.beg=0;
  memDrawWindows[0].beg=0;
  memDrawWindows[1].beg=0;
  memDrawWindows[2].beg=0;
  memDrawWindows[3].beg=0;

  memDrawWindows[2].cells=actualHolder.nh->base->memLocal.size;
  memDrawWindows[3].cells=actualHolder.h->piece->memPieceSize;

  for(visualWindow& w:memDrawWindows){
    w.rows=(w.cells+cellsPerRow-1)/cellsPerRow;
    w.fixed=false;
  }

  int available=8;
  int leftToFix=4,leftToFixBefore;
  int parcel;

  do{
    parcel=available/leftToFix;
    leftToFixBefore=leftToFix;
    for(visualWindow& w:memDrawWindows){
      if(!w.fixed){
        if(w.rows<=parcel){
          w.fixed=true;
          available-=w.rows;
          leftToFix--;
        }
      }
    }
  }while(leftToFix && leftToFix!=leftToFixBefore);

  int sum=0;
  for(visualWindow& w:memDrawWindows){
    w.height=sum;
    if(w.fixed){
      w.size=w.rows;
      sum+=w.rows;
    }else{
      if(leftToFix==1){
        //puede que al ultimo no fijo le quede mas que parcel porque parcel
        //puede ser mas chico por redondeos de division cuando hay mas de uno
        //no fixed
        w.size=available;
        sum+=available;
        available=0;
      }else{
        w.size=parcel;
        sum+=parcel;

        leftToFix--;
        available-=parcel;
      }
    }
  }
  moveW.height=sum;
  moveW.size=available+3;
  return;
}

void debugDrawMemories(){
  //window.draw(textDebug);

  computeWindowHeights();

  debugDrawMemoryCells(&memDrawWindows[0],brd->memGlobals);
  debugDrawMemoryCells(&memDrawWindows[1],getTileMd(0,brd));
  debugDrawMemoryCells(&memDrawWindows[2],actualHolder.h->memPiece.beg);
  debugDrawMemoryCells(&memDrawWindows[3],memMov.data);

  if(actualHolder.nh->base->memLocal.size!=actualHolder.nh->base->memLocal.resetUntil){
    int resetUntil=actualHolder.nh->base->memLocal.resetUntil;
    localMemorySeparator.setPosition(sf::Vector2f(530+cellSpacing*(resetUntil%cellsPerRow),405+45*(resetUntil/cellsPerRow-actualHolder.nh->base->memLocal.size/cellsPerRow)));
    window.draw(localMemorySeparator);
  }
  for(int i=0;i<debugDrawChannel.size;i++){
    switch(debugDrawChannel[i]){
    case tdebugSetIndirectColor:
      backgroundMemDebug.setFillColor(sf::Color(168,35,221,150));
      break;
    case tdebugUnsetIndirectColor:
      backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
      break;
    case tdebugDrawPos://esta siempre antes de posX,posY y los accesos a tile
      posMem.setPosition(sf::Vector2f(32*escala*actualHolder.nh->pos.x,32*escala*actualHolder.nh->pos.y));
      window.draw(posMem);
      break;
    default:
      {
        //se podría reescribir esto para que no dependa de memSize
        int ind=debugDrawChannel[i++];
        int memType=debugDrawChannel[i];
        backgroundMemDebug.setPosition(Vector2f(530+cellSpacing*(ind%cellsPerRow),
                                                memDrawWindows[memType].height*45+30+45*(ind/cellsPerRow)+memType*10));
        window.draw(backgroundMemDebug);
      }
    }
  }
  debugDrawChannel.size=0;
}


const int xSpace=600;
const int yInterleave=30;


void debugShowMove(bool val){
  v posAct=actualHolder.nh->pos;
  if(val){
    posActGood.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActGood;
  }else{
    posActBad.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActBad;
  }
  posPiece.setPosition(actualHolder.h->tile->pos.x*32*escala,actualHolder.h->tile->pos.y*32*escala);
  window.draw(posPiece);
  window.draw(*tileActDebug);

  normalHolder* nh=actualHolder.nh;
  movHolder* root=nh->base->root;
  int mi=0;
  for(movHolder** hRoot=actualHolder.h->movs.beg;;hRoot++){
    assert(hRoot!=actualHolder.h->movs.after);
    if(root==*hRoot){
      break;
    }
    mi++;
  }
  operador* rootOp=(*(actualHolder.h->piece->movs.beg+mi)).root;

  moveW.x=moveW.y=0;
  moveW.madeIt=val;
  drawMoveText(rootOp);

  if(moveW.size<moveW.y){
    int yOffset=moveW.height*45+30+(4)*10;
    if(Input.wheelDelta!=0&&
       Input.mouse.x>530&&Input.mouse.x<530+cellSpacing*cellsPerRow&&
       Input.mouse.y>yOffset&&Input.mouse.y<yOffset+45*moveW.size){
      updateScrolling(&moveW.beg,moveW.y-moveW.size);
    }
  }
}



void drawText(char* name,int colorType){
  textMov.setString(name);

  int len=textMov.getGlobalBounds().width;
  if(moveW.x+len>xSpace){
    moveW.x=0;
    moveW.y++;
  }

  bool inWindowRange=moveW.y>=moveW.beg && moveW.y<moveW.beg+moveW.size;
  bool wordInFocus=colorType==2;

  //despues se puede acomodar para moverse antes de salirse de foco si quiero, voy a necesitar
  //una variable para mirar si hay mas lineas abajo para no bajar si no hay nada
  if(wordInFocus&&!inWindowRange){
    moveW.beg=moveW.y-moveW.size+1;
    //esto es para cambiar el focus si la palabra actual esta fuera de rango
    //no puedo cambiarla inmediatemente porque lo anterior ya se dibujo (y se necesita para calcular el offset),
    //si quisiera hacerlo bien tendría que diferir el dibujado para que sea opcional o actualizar el offset
    //cuando la palabra focus este en la ultima linea, si hay mas lineas despues (este me gusta pero es mas complejo,
    //y no andaria si solo hay una linea visible)
    //esta solucion es medio garca porque se basa en que el sistema debug se dibuja muchas veces,
    //pero bueno es lo mas simple asi que es mejor. El sistema debug se va a dibujar muchas veces igual por el
    //tema de que se va a poder mover las ventanas y eso, esto es mover la ventana forzosamente. Antes mantenia
    //el windowBeg y lo actualizaba cuando se terminaba de dibujar, pero como voy a pisar todo dibujando devuelta
    //no me preocupa que se dibuje un estado incorrecto, asi que ahora actualizo directamente
  }

  /*
  if(wordInFocus){
    printf("%d %d %d \n",moveW.y,dText.beg,dText.begNextFrame);
  }
  */

  if(inWindowRange){
    int yOffset=moveW.height*45+30+(4)*10;

    textMov.setPosition(580+moveW.x,yOffset+(moveW.y-moveW.beg)*yInterleave);

    sf::Color color;
    switch(colorType){
    case 0: color=sf::Color(153, 153, 102);break;
    case 1: color=sf::Color(245, 153, 0);break;
    case 2: color=moveW.madeIt?
        sf::Color(78,184,68):sf::Color(240,70,40);break;
    case 3: color=sf::Color(110,110,170);break;
    }
    textMov.setColor(color);

    window.draw(textMov);
  }
  moveW.x+=len+(*name==0?0:10);
}

char* bufferElementToString(void(*func)(), char* buffer){
  if(auto it=funcToWord.find(func);it!=funcToWord.end()){
    return (*it).second;
  }
  sprintf(buffer,"%d",(int)(intptr)func);
  return buffer;
}

void drawNormalText(operador* op){
  normal* an=actualHolder.nh->op;
  int i=0;

  v relPos=((normal*)op)->relPos;
  char dirs[255];//no va a ser exactamente igual a como esta escrito por el usuario pero el resultado es el mismo
  int j=0;
  for(int i=relPos.y;i!=0;){
    if(i>0){
      i--;
      dirs[j++]='s';
    }else{
      i++;
      dirs[j++]='w';
    }
  }
  for(int i=relPos.x;i!=0;){
    if(i>0){
      i--;
      dirs[j++]='d';
    }else{
      i++;
      dirs[j++]='a';
    }
  }
  dirs[j]=0;
  drawText(dirs,an==op?3:0);

  normal* n=(normal*)op;
  for(bool(**c)(void)=n->conds.beg;
      c != n->conds.after;
      c++){
    void(*cast)(void)=(void(*)(void))*c;
    if(n==an&&
       ((*actualHolder.bufferPos==i)||
       (debugMultiParameterBegin!=-1 && debugMultiParameterBegin<=i && *actualHolder.bufferPos>=i))){
      drawText(bufferElementToString(cast,dirs),2);
    }else{
      drawText(bufferElementToString(cast,dirs),0);
    }
    i++;
  }
  for(void(**a)(void)=n->accs.beg;
      a!=n->accs.after;
      a++){
    drawText(bufferElementToString(*a,dirs),an==op?3:0);
  }
}

void drawMoveTextInsideOp(char* name,auto insideOp,bool inRange){
  //mirar si n esta adentro usando rangos
  drawText(name,inRange?1:0);
  insideOp();
  drawText("end",inRange?1:0);
}

void drawMoveText(operador* op){
  char* obj=(char*)actualHolder.nh->op;
  bool inRange=obj>(char*)op && (op->sig?(obj<(char*)op->sig):true);

  switch(op->tipo){
  case NORMAL:{
    drawNormalText(op);
  }break;
  case DESLIZ:{
    desliz* d=(desliz*)op;
    drawMoveTextInsideOp("desliz",[=](){drawMoveText(d->inside);},inRange);
  }break;
  case EXC:{
    exc* e=(exc*)op;
    drawMoveTextInsideOp("exc",[=](){
                                 operador** excOp;
                                  for(excOp=e->ops.beg;excOp!=e->ops.after-1;excOp++){
                                    drawMoveText(*excOp);
                                    drawText("or",inRange?1:0);
                                  }
                                  drawMoveText(*excOp);
                               },inRange);
  }break;
  case ISOL:{
    drawMoveTextInsideOp("isol",[=](){drawMoveText(((isol*)op)->inside);},inRange);
  }break;
  case DESOPT:{
    desopt* d=(desopt*)op;
    drawMoveTextInsideOp("desopt",[=](){
                                     for(operador** desOp=d->ops.beg;desOp!=d->ops.after;desOp++){
                                       drawMoveText(*desOp);
                                     }
                                  },inRange);
  }break;
  }
  if(op->sig!=nullptr)
    drawMoveText(op->sig);
}





RectangleShape rect;
Text operatorLetter;
Text textLabel;

int debugState=2;//0 movHolder 1 opBucket 2 movHolderBucket
sf::RectangleShape stateSelectorButton;

//en vez de inyectar codigo en la creacion de operadores/movHolders
//preferi rerecorrer el arbol aca. La ventaja de esto es que no ensucio
//el otro codigo, la desventaja es que puede que lo que se muestre no sea
//correcto. Igual si es incorrecto se va a notar porque no van a encajar los espacios
//no puedo inyectar codigo en bucket porque no tendría toda la informacion

vector<sf::Color> pieceColors;
struct HolderColor{
  Holder* h;
  sf::Color color;
};
vector<HolderColor> holderColors;//tipo un hash pero a lo perro con un for

struct{
  bucket* bkt;
  int end;//para manejar poner la cola de exc, medio hack pero prefiero hacer eso a arrastrar calculos de tamaño
  int windowBeg;
  int maxHeight;
  int actualBucket;//cuando hay mas de un bucket, por ahora en movHolders nomas
}bucketDraw;
int lineSize=512;
int windowSize=22;

sf::Color newRandomColor(){
  int r=56+rand()%200;
  int g=56+rand()%200;
  int b=56+rand()%200;
  return sf::Color(r,g,b,190);
}

sf::Color darkenColor(sf::Color color,int darkness){
  sf::Color darkerColor;
  darkerColor.r=std::max(0,color.r-darkness);
  darkerColor.g=std::max(0,color.g-darkness);
  darkerColor.b=std::max(0,color.b-darkness);
  return darkerColor;
}

void debugUpdateAndDrawBucketsInit(bool reset){
  properState* ps=(properState*)stateMem;

  if(reset){
    pieceColors.size=0;
    holderColors.size=0;
  }else{
    init(&pieceColors,ps->pieces.size);
    init(&holderColors,ps->pieces.size*2);
  }
  for(int i=0;i<ps->pieces.size;i++){
    push(&pieceColors,newRandomColor());
  }

  bucketDraw.actualBucket=0;

  operatorLetter.setScale(.7,.7);
  operatorLetter.setFont(font);

  textLabel.setColor(sf::Color(90,90,90));
  textLabel.setScale(.7,.7);
  textLabel.setFont(font);

  stateSelectorButton.setSize(sf::Vector2f(32,32));
}

void drawRegionLine(int height,int beg,int end,sf::Color color){
  if(height<bucketDraw.windowBeg||height>=bucketDraw.windowBeg+windowSize)
    return;

  sf::Color borderColor(std::max(color.r-30,0),std::max(color.g-30,0),std::max(color.b-30,0),255);
  rect.setSize(sf::Vector2f(end-beg,24));
  rect.setPosition(550+beg,60+(height-bucketDraw.windowBeg)*20);
  rect.setFillColor(borderColor);
  window.draw(rect);

  rect.setSize(sf::Vector2f(end-beg-8,16));
  rect.setPosition(550+beg+4,60+(height-bucketDraw.windowBeg)*20+4);
  rect.setFillColor(color);
  window.draw(rect);

  if(height%2==0){//se va a dibujar varias veces pero bueno
    textLabel.setPosition(520,60+(height-bucketDraw.windowBeg)*20);
    textLabel.setString(std::to_string(height/2)+"k");
    window.draw(textLabel);
  }
}

void drawBucketElement(void* ptr,int size,sf::Color color,char letter=' '){
  int beg=(int)((char*)ptr-bucketDraw.bkt->data);
  assert(beg>=0&&beg<bucketDraw.bkt->size);

  int by=beg/lineSize;
  int bx=beg%lineSize;

  int end=beg+size;
  int ey=end/lineSize;
  int ex=end%lineSize;

  if(by==ey){
    drawRegionLine(by,bx,ex,color);
  }else{
    int y=by;
    drawRegionLine(y,bx,lineSize,color);
    y++;
    while(y!=ey){
      drawRegionLine(y,0,lineSize,color);
      y++;
    }
    drawRegionLine(ey,0,ex,color);
  }

  if(by>=bucketDraw.windowBeg&&by<bucketDraw.windowBeg+windowSize){
    operatorLetter.setColor(sf::Color(std::max(color.r-40,0),std::max(color.g-40,0),std::max(color.b-40,0),255));
    operatorLetter.setString(letter);
    if(bx+20<lineSize)
      operatorLetter.setPosition(555+bx,59+(by-bucketDraw.windowBeg)*20);
    else
      operatorLetter.setPosition(555,59+(by-bucketDraw.windowBeg+1)*20);
    window.draw(operatorLetter);
  }
  bucketDraw.end=end;
}

void drawBucketOperator(operador* op,sf::Color color){
  sf::Color colorFade=darkenColor(color,10);
  switch(op->tipo){
  case NORMAL:{
    normal* n=(normal*)op;
    drawBucketElement(op,sizeof(normal)+size(n->accs)+size(n->conds)+size(n->colors),color,'n');
  }
    break;
  case DESLIZ:{
    drawBucketElement(op,sizeof(desliz),color,'d');
    drawBucketOperator(((desliz*)op)->inside,colorFade);
  }break;
  case EXC:{
    exc* e=(exc*)op;
    drawBucketElement(op,sizeof(exc),color,'e');
    for(operador** excOp=e->ops.beg;excOp!=e->ops.after;excOp++){
      drawBucketOperator(*excOp,colorFade);
    }
    drawBucketElement(bucketDraw.bkt->data+bucketDraw.end,size(e->ops),color,' ');
  }break;
  case ISOL:{
    drawBucketElement(op,sizeof(isol),color,'i');
    drawBucketOperator(((isol*)op)->inside,colorFade);
  }break;
  case DESOPT:{
    desopt* d=(desopt*)op;
    drawBucketElement(op,sizeof(desopt)+size(d->ops)+size(d->movSizes),color,'f');
    for(operador** desOp=d->ops.beg;desOp!=d->ops.after;desOp++){
      drawBucketOperator(*desOp,colorFade);
    }
  }break;
  }
  if(op->sig!=nullptr)
    drawBucketOperator(op->sig,color);
}


void drawBucketMovholder(movHolder* mh,sf::Color color){
  sf::Color colorFade=darkenColor(color,20);

  if(mh->table==&normalTable){
    normalHolder* nh=(normalHolder*)mh;
    drawBucketElement(mh,sizeof(normalHolder)+size(nh->memAct),color,'n');
  }else if(mh->table==&deslizTable){
    deslizHolder* dh=(deslizHolder*)mh;
    drawBucketElement(mh,sizeof(deslizHolder),color,'d');
    for(int i=0;i<dh->cantElems;i++){
      drawBucketMovholder((movHolder*)(dh->movs.beg+dh->op->iterSize*i),colorFade);
    }
    drawBucketElement((movHolder*)(dh->movs.beg+dh->cantElems*dh->op->iterSize),dh->op->insideSize-dh->op->iterSize*dh->cantElems,darkenColor(colorFade,40));
  }else if(mh->table==&excTable){
    excHolder* eh=(excHolder*)mh;
    drawBucketElement(mh,sizeof(excHolder)+size(eh->movs),color,'e');
    for(movHolder** mi=eh->movs.beg;mi!=eh->movs.after;mi++){
      drawBucketMovholder(*mi,colorFade);
    }
  }else if(mh->table==&isolTable || mh->table==&isolNRMTable){
    isolHolder* ih=(isolHolder*)mh;
    drawBucketElement(mh,sizeof(isolHolder),color,'i');
    drawBucketMovholder(ih->inside,color);
  }else if(mh->table==&desoptTable || mh->table==&desoptNRMTable){
    desoptHolder* dh=(desoptHolder*)mh;
    drawBucketElement(dh,sizeof(desoptHolder)+dh->op->desoptInsideSize,darkenColor(colorFade,40));
    drawBucketElement(dh,sizeof(desoptHolder),color,'f');
    drawBucketdesoptHNodes(dh,dh->movs,color);
  }
  if(mh->sig!=nullptr)
    drawBucketMovholder(mh->sig,color);
}
void drawBucketdesoptHNodes(desoptHolder* d,desoptHolder::node* iter,sf::Color color){
  int branchOffset=0;
  for(int tam:d->op->movSizes){
    desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+branchOffset);
    movHolder* actualMov=(movHolder*)(nextIter+1);
    branchOffset+=tam;

    drawBucketElement(nextIter,sizeof(desoptHolder::node*),sf::Color(std::min(color.r+30,255),std::min(color.g+30,255),std::min(color.b+30,255),color.a));
    drawBucketMovholder(actualMov,color);

    if(actualMov->bools&valorFinal){
      drawBucketdesoptHNodes(d,nextIter->iter,darkenColor(color,4));
    }
  }
}

void drawHolderData(Holder* h,int x,int y){
  sf::Color color;
  for(int i=0;i<holderColors.size;i++){
    if(holderColors[i].h==h){
      color=holderColors[i].color;
      goto cont;
    }
  }
  color=newRandomColor();
  push(&holderColors,HolderColor{h,color});
 cont:

  bool capturado=x==-1;
  if(!capturado){
    rect.setSize(sf::Vector2f(32*escala,32*escala));
    rect.setPosition(32*escala*x,32*escala*y);
    rect.setFillColor(color);
    window.draw(rect);
  }else{
    color=darkenColor(color,150);
  }

  char* firstMov=(char*)*h->movs.beg;
  if(firstMov>=bucketDraw.bkt->data&&firstMov<bucketDraw.bkt->data+bucketDraw.bkt->size){
    //por ahora cada holder o esta entero o no esta en el bucket
    for(movHolder** mh=h->movs.beg;mh!=h->movs.after;mh++){
      drawBucketMovholder(*mh,color);
      drawBucketElement(*mh,0,sf::Color::White);
    }
  }
}

void debugUpdateAndDrawBuckets(){
  updateScrolling(&bucketDraw.windowBeg,bucketDraw.maxHeight);

  stateSelectorButton.setPosition(650,20);
  stateSelectorButton.setFillColor(debugState==1?sf::Color::Red:sf::Color(150,150,150));
  window.draw(stateSelectorButton);
  stateSelectorButton.setPosition(690,20);
  stateSelectorButton.setFillColor(debugState==2?sf::Color::Red:sf::Color(150,150,150));
  window.draw(stateSelectorButton);

  if(Input.leftClick&&Input.mouse.y>=20&&Input.mouse.y<=50){
    if(debugState!=1&&Input.mouse.x>=650&&Input.mouse.x<=680){
      debugState=1;
      bucketDraw.windowBeg=0;
    }
    else if(debugState!=2&&Input.mouse.x>=690&&Input.mouse.x<=720){
      debugState=2;
      bucketDraw.windowBeg=0;
    }
  }

  properState* ps=(properState*)stateMem;

  bucket* bucket=debugState==1?&ps->pieceOps:&ps->gameState;

  bucketDraw.bkt=bucket;
  bucketDraw.maxHeight=bucket->size/lineSize - windowSize;

  //recorro el arbol cada vez porque la eficiencia no importa aca
  //ademas como guardar la informacion para que retomarla sea comodo no es obvio,
  //podría guardarla por linea teniendo cada linea una cantidad variable de bloques
  //con informacion extra, o podría tener la lista de bloques y trocearlos dinamicamente
  //aca. Las 2 tienen sus problemas, hacer todo dinamico es lo mas simple

  drawBucketElement(bucket->data,bucket->size,sf::Color::Black);

  if(debugState==1){
    for(int x=0;x<brd->dims.x;x++){
      for(int y=0;y<brd->dims.y;y++){
        Holder* h=tile(brd,v(x,y))->holder;
        if(h){
          rect.setSize(sf::Vector2f(32*escala,32*escala));
          rect.setPosition(32*escala*x,32*escala*y);
          rect.setFillColor(pieceColors[h->piece->ind]);
          window.draw(rect);
        }
      }
    }
    for(int i=0;i<ps->pieces.size;i++){
      Piece* p=ps->pieces[i];
      int pieceSize=sizeof(Piece)+size(p->movs);
      drawBucketElement(p,pieceSize,pieceColors[i]);

      for(pBase* pb=p->movs.beg;pb!=p->movs.after;pb++){
        drawBucketOperator(pb->root,pieceColors[i]);
      }
    }
  }else{
    int bucketQ=getBucketSizeData(bucket).usedBuckets;
    char* dataTemp=bucket->data;

    textLabel.setPosition(800,20);
    textLabel.setColor(sf::Color(90,90,90));
    textLabel.setString(std::to_string(bucketDraw.actualBucket+1)+"/"+std::to_string(bucketQ));
    window.draw(textLabel);

    if(Input.rightClick){
      bucketDraw.actualBucket=(bucketDraw.actualBucket+1)%bucketQ;
    }

    bucket::block* block=bucket->firstBlock;
    for(int i=0;i<bucketDraw.actualBucket;i++){
      block=block->next;
    }
    bucket->data=block->data;

    if(bucketDraw.actualBucket==0){
      parseData* pd=&ps->pd;
      drawBucketElement(bucket->data,sizeof(board),sf::Color(204, 102, 0),'B');
      int memTileSize=pd->dims.x*pd->dims.y*(sizeof(Tile)+pd->memTileSlots*sizeof(memData));
      if(memTileSize)
        drawBucketElement(bucket->data+sizeof(board),memTileSize,sf::Color(153, 153, 102),'T');
      int memGlobalSize=pd->memGlobalSize*sizeof(memData);
      if(memGlobalSize)
        drawBucketElement(bucket->data+sizeof(board)+memTileSize,memGlobalSize,sf::Color(255, 102, 102),'G');
    }

    for(int x=0;x<brd->dims.x;x++){
      for(int y=0;y<brd->dims.y;y++){
        Holder* h=tile(brd,v(x,y))->holder;
        if(h){
          drawHolderData(h,x,y);
        }
      }
    }
    for(int i=0;i<reciclaje.size;i++){
      drawHolderData(reciclaje[i],-1,-1);
    }

    bucket->data=dataTemp;
  }

  static int vel;
  vel+=118;
  if(vel/2<bucket->size)
    drawBucketElement(bucket->data+vel/2,vel,sf::Color::Red,'X');

}


#endif











