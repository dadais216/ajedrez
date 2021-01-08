



#if debugMode

int debugGetCellVal(memData* v,int i){
  return (v+i)->val;
}
int debugGetCellVal(auto* v,int i){
  return *(v+i);
}
const int cellsPerRow=16;
const int cellSpacing=39;
void debugDrawMemoryCells(int memSize, int yOffset,auto memory){
  for(int i=0;i<memSize;i++){
    backgroundMem.setPosition(sf::Vector2f(530+cellSpacing*(i%cellsPerRow),yOffset+45*(i/cellsPerRow-memSize/cellsPerRow)));
    window.draw(backgroundMem);
  }
  for(int i=0;i<memSize;i++){
    textValMem.setPosition(530+cellSpacing*(i%cellsPerRow),yOffset+5+45*(i/cellsPerRow-memSize/cellsPerRow));
    textValMem.setString(std::to_string(debugGetCellVal(memory,i)));
    window.draw(textValMem);
  }
};

void debugDrawMemories(){
  board* brd=getBoard((properState*)stateMem);

  window.draw(*tileActDebug);
  
  window.draw(posPiece);
  window.draw(textDebug);

  debugDrawMemoryCells(actualHolder.nh->base->memLocal.size,405,memMov.data);
  debugDrawMemoryCells(actualHolder.h->piece-> memPieceSize,305,actualHolder.h->memPiece.beg);
  debugDrawMemoryCells(brd->memGlobalSize,205,brd->memGlobals);
  debugDrawMemoryCells(brd->memTileSlots,105,getTileMd(0,brd));

  if(actualHolder.nh->base->memLocal.size!=actualHolder.nh->base->memLocal.resetUntil){
    int resetUntil=actualHolder.nh->base->memLocal.resetUntil;
    localMemorySeparator.setPosition(sf::Vector2f(530+cellSpacing*(resetUntil%cellsPerRow),405+45*(resetUntil/cellsPerRow-actualHolder.nh->base->memLocal.size/cellsPerRow)));
    window.draw(localMemorySeparator);
  }
    

  bool cteInThisIteration=false;
  for(int i=0;i<debugDrawChannel.size;i++){
    switch(debugDrawChannel[i]){
    case tdebugSetIndirectColor:
      backgroundMemDebug.setFillColor(sf::Color(168,35,221,150));
      break;
    case tdebugUnsetIndirectColor:
      backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
      break;
    case tdebugDrawCte:
      {
        int val=debugDrawChannel[++i];
        textValMem.setPosition(610,cteInThisIteration?480:455);
        textValMem.setString(std::to_string(val));
        window.draw(textValMem);
        cteInThisIteration=true;
      }
      break;
    case tdebugDrawPos://esta siempre antes de posX,posY y los accesos a tile
      posMem.setPosition(sf::Vector2f(32*escala*actualHolder.nh->pos.x,32*escala*actualHolder.nh->pos.y));
      window.draw(posMem);
      break;
    case tdebugDrawPosX:
    case tdebugDrawPosY:
      textValMem.setPosition(610,cteInThisIteration?480:455);
      textValMem.setString(debugDrawChannel[i]==tdebugDrawPosX?"X":"Y");
      window.draw(textValMem);
      cteInThisIteration=true;
      break;
    default:
      {
        //se podría reescribir esto para que no dependa de memSize
        cteInThisIteration=false;
        int ind=debugDrawChannel[i++];
        int memSize=debugDrawChannel[i++];
        int drawOffset=debugDrawChannel[i];
        backgroundMemDebug.setPosition(Vector2f(530+cellSpacing*(ind%cellsPerRow),
                                                drawOffset+45*(ind/cellsPerRow-memSize/cellsPerRow)));
        window.draw(backgroundMemDebug);
      }
    }
  }
  debugDrawChannel.size=0;
}

void debugShowAndWait(char const* name,bool val){
  textDebug.setString(name);
  
  v posAct=actualHolder.nh->pos;
  if(val){
    posActGood.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActGood;
    textDebug.setColor(sf::Color(78,84,68,100));
  }else{
    posActBad.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActBad;
    textDebug.setColor(sf::Color(240,70,40,240));
  }
  posPiece.setPosition(actualHolder.h->tile->pos.x*32*escala,actualHolder.h->tile->pos.y*32*escala);

  drawScreen([](){
               properDraw(stateMem);
               debugDrawMemories();
             });
  
        
  stall();
}
void debugShowAndWaitMem(char const* name,bool val){
  textDebug.setString(name);

  tileActDebug=&posPiece;
  posPiece.setPosition(-32*escala,-32*escala);

  if(val)
    textDebug.setColor(sf::Color(78,84,68,100));
  else
    textDebug.setColor(sf::Color(240,70,40,240));

  drawScreen([](){
               properDraw(stateMem);
               debugDrawMemories();
             });
  
  stall();
}

RectangleShape rect;
Text operatorLetter;
Text textLabel;

bool clickPressed=false;
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

int mouseX,mouseY;
bool rightMouse;

void debugUpdateAndDrawBuckets(){

  clickPressed=false;
  rightMouse=false;
  //TODO mover a eventos cuando haya
  sf::Event event;
  while(window.pollEvent(event)){
    if(event.type == sf::Event::EventType::MouseWheelScrolled){
      bucketDraw.windowBeg-=event.mouseWheelScroll.delta;
      if(bucketDraw.windowBeg<0)
        bucketDraw.windowBeg=0;
      else if(bucketDraw.windowBeg>bucketDraw.maxHeight)
        bucketDraw.windowBeg=bucketDraw.maxHeight;
    }else if(event.type==sf::Event::EventType::MouseButtonPressed){
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        clickPressed=true;
        auto mousePos=sf::Mouse::getPosition(window);
        mouseX=mousePos.x;
        mouseY=mousePos.y;
      }
      if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
        rightMouse=true;
      }
      //if(sf::Mouse::isButtonPressed(sf::Mouse::XButton1)){
      //  rightB=true;
      //}
      //if(sf::Mouse::isButtonPressed(sf::Mouse::XButton2)){
      //  leftB=true;
      //}
      //}else if(event.type==sf::Event::EventType::MouseButtonReleased){
      //if(!sf::Mouse::isButtonPressed(sf::Mouse::Left)){
      //  clickPressed=false;
      //}
    }
  }

  stateSelectorButton.setPosition(650,20);
  stateSelectorButton.setFillColor(debugState==1?sf::Color::Red:sf::Color(150,150,150));
  window.draw(stateSelectorButton);
  stateSelectorButton.setPosition(690,20);
  stateSelectorButton.setFillColor(debugState==2?sf::Color::Red:sf::Color(150,150,150));
  window.draw(stateSelectorButton);

  if(clickPressed&&mouseY>=20&&mouseY<=50){
    if(debugState!=1&&mouseX>=650&&mouseX<=680){
      debugState=1;
      bucketDraw.windowBeg=0;
    }
    else if(debugState!=2&&mouseX>=690&&mouseX<=720){
      debugState=2;
      bucketDraw.windowBeg=0;
    }
  }

  properState* ps=(properState*)stateMem;
  board* brd=getBoard(ps);


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
        drawBucketOperator(pb->raiz,pieceColors[i]);
      }
    }
  }else{
    int bucketQ=getBucketSizeData(bucket).usedBuckets;
    char* dataTemp=bucket->data;

    textLabel.setPosition(800,20);
    textLabel.setColor(sf::Color(90,90,90));
    textLabel.setString(std::to_string(bucketDraw.actualBucket+1)+"/"+std::to_string(bucketQ));
    window.draw(textLabel);

    if(rightMouse){
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











