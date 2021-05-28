



#if debugMode

RectangleShape backgroundMem;
RectangleShape backgroundMemDebug;
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
  backgroundMem.setOutlineColor(sf::Color(195,195,175));
  backgroundMem.setOutlineThickness(4);
  backgroundMem.setSize(sf::Vector2f(36,35));
  backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
  backgroundMemDebug.setOutlineColor(sf::Color(195,195,175));
  backgroundMemDebug.setOutlineThickness(4);
  backgroundMemDebug.setSize(sf::Vector2f(36,35));
  textValMem.setColor(sf::Color::Black);
  textValMem.setFont(font);
  //textValMem.setPosition(570,10);
  textIndexMem.setColor(sf::Color(200,200,200,200));
  textIndexMem.setFont(font);
  textIndexMem.setScale(sf::Vector2f(.4,.4));
  
  init(&debugDrawChannel);

  moveW.jumpToWord=true;
  debugMultiParameterBegin=-1;

  memDrawWindows[0].cells=brd->memGlobalSize;
  memDrawWindows[1].cells=brd->memTileSlots;

}

sf::RectangleShape stateSelectorButton;
void handleModeSelectors(){
  for(int i=0;i<3;i++){
    stateSelectorButton.setPosition(650+40*i,20);
    stateSelectorButton.setFillColor(debugState==i?sf::Color::Red:sf::Color(150,150,150));
    window.draw(stateSelectorButton);

    if(Input.leftClick&&Input.mouse.y>=20&&Input.mouse.y<=50&&
       debugState!=i&&Input.mouse.x>=650+40*i&&Input.mouse.x<=680+40*i){
      debugState=i;
      bvectorDraw.windowBeg=0;
    }
  }
}

void debugShowAndWait(bool val){
  while(true){
    //si quiero evitar redibujados podría hacer que beg solo se limpie cuando cambia el movimiento,
    //y hacer el segundo dibujado solo cuando se actualize beg
    drawScreen([=](){
                 properDraw(stateMem);

                 handleModeSelectors();
                 if(debugState==2){
                   debugDrawMemories();
                   debugShowMove(val);
                 }else{
                   debugUpdateAndDrawBvectors();
                 }
               });
    /*
      textValMem.setString(std::to_string(ps->turno));
      textValMem.setPosition(sf::Vector2f(600,20));
      window.draw(textValMem);
    */
    handleSystemEvents();
    if(Input.c||Input.x||(Input.z)){
      moveW.jumpToWord=true;
      break;
    }
    sleep(milliseconds(20));
  }
  if(Input.x)
    sleep(milliseconds(175));
  debugMultiParameterBegin=-1;
  debugDrawChannel.size=0;
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
void debugDrawMemoryCells(visualWindow* vw,auto memory, int resetBar=INT_MAX){
  int yOffset=vw->height*45+30+(vw-&memDrawWindows[0])*10;
  if(Input.wheelDelta!=0&&
     Input.mouse.x>530&&Input.mouse.x<530+cellSpacing*cellsPerRow&&
     Input.mouse.y>yOffset&&Input.mouse.y<yOffset+45*vw->size){
    updateScrolling(&vw->beg,vw->rows-vw->size);
  }

  int beg=vw->beg*cellsPerRow;
  int end=std::min((vw->beg+vw->size)*cellsPerRow,vw->cells);

  for(int i=beg;i<end;i++){
    if(i>=resetBar){
      backgroundMem.setFillColor(sf::Color(255,225,190));
    }else{
      backgroundMem.setFillColor(sf::Color(240,235,200));
    }
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
  if(actualHolder.nh!=nhLastFrame){
    nhLastFrame=actualHolder.nh;

    moveW.beg=0;
    memDrawWindows[0].beg=0;
    memDrawWindows[1].beg=0;
    memDrawWindows[2].beg=0;
    memDrawWindows[3].beg=0;

    memDrawWindows[2].cells=opVector<Piece>(actualHolder.h->piece)->memPieceSize;
    memDrawWindows[3].cells=gameVector<Base>(actualHolder.nh->base)->memLocal.size;
  }
  for(visualWindow& w:memDrawWindows){
    w.rows=(w.cells+cellsPerRow-1)/cellsPerRow;
    w.fixed=false;
  }

  int available=9-std::min(4,moveW.y);
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
  moveW.size=available+3+std::min(4,moveW.y);
  return;
}

void debugDrawMemories(){
  //window.draw(textDebug);
  computeWindowHeights();

  debugDrawMemoryCells(&memDrawWindows[0],brd->memGlobals);
  debugDrawMemoryCells(&memDrawWindows[1],getTileMd(0));
  debugDrawMemoryCells(&memDrawWindows[2],gameVector<int>(actualHolder.h->memPiece.beg));
  debugDrawMemoryCells(&memDrawWindows[3],memMov.data,gameVector<Base>(actualHolder.nh->base)->memLocal.resetUntil);

  for(int i=0;i<debugDrawChannel.size;i++){
    switch(debugDrawChannel[i]){
    case tdebugSetIndirectColor:
      backgroundMemDebug.setFillColor(sf::Color(8,35,221,120));
      break;
    case tdebugUnsetIndirectColor:
      backgroundMemDebug.setFillColor(sf::Color(163,230,128,120));
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

        visualWindow* vw=&memDrawWindows[memType];
        int beg=vw->beg*cellsPerRow;
        int end=std::min((vw->beg+vw->size)*cellsPerRow,vw->cells);

        if(ind>=beg&&ind<end){
          backgroundMemDebug.setPosition(Vector2f(530+cellSpacing*(ind%cellsPerRow),
                                                  memDrawWindows[memType].height*45+30+45*(ind/cellsPerRow)+memType*10));
          window.draw(backgroundMemDebug);
        }
      }
    }
  }
}


const int xSpace=600;
const int yInterleave=30;


void debugShowMove(bool val){
  v posAct=actualHolder.nh->pos;
  Tile* tile=gameVector<Tile>(actualHolder.h->tile);

  if(val){
    posActGood.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActGood;
  }else{
    posActBad.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActBad;
  }
  posPiece.setPosition(tile->pos.x*32*escala,tile->pos.y*32*escala);
  window.draw(posPiece);
  window.draw(*tileActDebug);

  normalHolder* nh=actualHolder.nh;
  movHolder* root=gameVector<movHolder>(gameVector<Base>(nh->base)->movRoot);
  int mi;
  for(mi=0;;mi++){
    assert(mi<actualHolder.h->movs.size);
    movHolder* hRoot=gameVector<movHolder>(*varrayGameElem(&actualHolder.h->movs,mi));
    if(root==hRoot){
      break;
    }
  }
  operador* rootOp=opVector<operador>(varrayOpElem(&opVector<Piece>(actualHolder.h->piece)->movs,mi)->root);

  moveW.x=moveW.y=0;
  moveW.madeIt=val;
  drawMoveText(rootOp);

  if(moveW.size<moveW.y){
    int yOffset=moveW.height*45+30+(4)*10;
    if(Input.wheelDelta!=0&&
       Input.mouse.x>530&&Input.mouse.x<530+cellSpacing*cellsPerRow&&
       Input.mouse.y>yOffset&&Input.mouse.y<yOffset+45*moveW.size){
      updateScrolling(&moveW.beg,moveW.y-moveW.size+1);//no estoy seguro de donde sale el +1
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
  if(moveW.jumpToWord&&wordInFocus&&!inWindowRange){
    moveW.beg=moveW.y-moveW.size+1;
    moveW.jumpToWord=false;
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
  normal* an=opVector<normal>(actualHolder.nh->op);

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
  forVOp(n->conds){
    if(n==an&&
       ((*actualHolder.bufferPos==indv)||
       (debugMultiParameterBegin!=-1 && debugMultiParameterBegin<=indv && *actualHolder.bufferPos>=indv))){
      drawText(bufferElementToString((void(*)())*el,dirs),2);
    }else{
      drawText(bufferElementToString((void(*)())*el,dirs),0);
    }
  }
  forVOp(n->accs){
    drawText(bufferElementToString(*el,dirs),an==op?3:0);
  }
}

void drawMoveTextInsideOp(char* name,auto insideOp,bool inRange){
  //mirar si n esta adentro usando rangos
  drawText(name,inRange?1:0);
  insideOp();
  drawText("end",inRange?1:0);
}

void drawMoveText(operador* op){
  int obj=actualHolder.nh->op;
  bool inRange=obj>indOpVector(op) && (op->sig?(obj<op->sig):true);

  switch(op->tipo){
  case NORMAL:{
    drawNormalText(op);
  }break;
  case DESLIZ:{
    desliz* d=(desliz*)op;
    drawMoveTextInsideOp("desliz",[=](){drawMoveText(opVector<operador>(d->inside));},inRange);
  }break;
  case EXC:{
    exc* e=(exc*)op;
    drawMoveTextInsideOp("exc",[=](){
                                 int i;
                                 for(i=0;i<elems(e->ops)-1;i++){
                                   drawMoveText(opVector<operador>(*varrayOpElem(&e->ops,i)));
                                   drawText("or",inRange?1:0);
                                 }
                                 drawMoveText(opVector<operador>(*varrayOpElem(&e->ops,i)));
                               },inRange);
  }break;
  case ISOL:
  case ISOLNRM:{
    drawMoveTextInsideOp("isol",[=](){
                                  drawMoveText(opVector<operador>(((isol*)op)->inside));
                                },inRange);
  }break;
  case DESOPT:
  case DESOPTNRM:{
    desopt* d=(desopt*)op;
    drawMoveTextInsideOp("desopt",[=](){
                                    int i;
                                    for(i=0;i<elems(d->ops)-1;i++){
                                      drawMoveText(opVector<operador>(*varrayOpElem(&d->ops,i)));
                                      drawText("or",inRange?1:0);
                                    }
                                    drawMoveText(opVector<operador>(*varrayOpElem(&d->ops,i)));
                                  },inRange);
  }break;
  }
  if(op->sig!=0)
    drawMoveText(opVector<operador>(op->sig));
}





RectangleShape rect;
Text operatorLetter;
Text textLabel;


//en vez de inyectar codigo en la creacion de operadores/movHolders
//preferi rerecorrer el arbol aca. La ventaja de esto es que no ensucio
//el otro codigo, la desventaja es que puede que lo que se muestre no sea
//correcto. Igual si es incorrecto se va a notar porque no van a encajar los espacios
//no puedo inyectar codigo en bucket porque no tendría toda la informacion

vector<sf::Color> pieceColors;
struct HolderColor{
  int hI;
  sf::Color color;
};
vector<HolderColor> holderColors;//tipo un hash pero a lo perro con un for

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

void debugUpdateAndDrawBvectorsInit(bool reset){
  properState* ps=(properState*)stateMem;

  if(reset){
    pieceColors.size=0;
    holderColors.size=0;
  }else{
    init(&pieceColors,20);
    init(&holderColors,40);//no uso ps->pieces porque debuggeando test no lo tengo antes de correr el primero
  }
  for(int i=0;i<ps->pieces.size;i++){
    push(&pieceColors,newRandomColor());
  }

  operatorLetter.setScale(.7,.7);
  operatorLetter.setFont(font);

  textLabel.setColor(sf::Color(90,90,90));
  textLabel.setScale(.7,.7);
  textLabel.setFont(font);

  stateSelectorButton.setSize(sf::Vector2f(32,32));
}

void drawRegionLine(int height,int beg,int end,sf::Color color){
  if(height<bvectorDraw.windowBeg||height>=bvectorDraw.windowBeg+windowSize)
    return;

  sf::Color borderColor(std::max(color.r-30,0),std::max(color.g-30,0),std::max(color.b-30,0),255);
  rect.setSize(sf::Vector2f(end-beg,22));
  rect.setPosition(550+beg,60+(height-bvectorDraw.windowBeg)*20);
  rect.setFillColor(borderColor);
  window.draw(rect);

  rect.setSize(sf::Vector2f(end-beg-8,14));
  rect.setPosition(550+beg+4,60+(height-bvectorDraw.windowBeg)*20+4);
  rect.setFillColor(color);
  window.draw(rect);

  if(height%2==0){//se va a dibujar varias veces pero bueno
    textLabel.setPosition(520,60+(height-bvectorDraw.windowBeg)*20);
    textLabel.setString(std::to_string(height/2)+"k");
    window.draw(textLabel);
  }
}

void drawBvectorDelimitator(int beg){
  assert(beg>=0&&beg<bvectorDraw.bv->size);

  int by=beg/lineSize;
  int bx=beg%lineSize;

  if(by<bvectorDraw.windowBeg||by>=bvectorDraw.windowBeg+windowSize)
    return;

  rect.setSize(sf::Vector2f(2,24));
  rect.setPosition(550+bx,60+(by-bvectorDraw.windowBeg)*20);
  rect.setFillColor(sf::Color(210,210,210));
  window.draw(rect);
}

void drawBvectorElement(int beg,int size,sf::Color color,char letter=' '){
  assert(beg>=0&&beg<bvectorDraw.bv->size);

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

  if(by>=bvectorDraw.windowBeg&&by<bvectorDraw.windowBeg+windowSize){
    operatorLetter.setColor(sf::Color(std::max(color.r-40,0),std::max(color.g-40,0),std::max(color.b-40,0),255));
    operatorLetter.setString(letter);
    if(bx+20<lineSize)
      operatorLetter.setPosition(555+bx,59+(by-bvectorDraw.windowBeg)*20);
    else
      operatorLetter.setPosition(555,59+(by-bvectorDraw.windowBeg+1)*20);
    window.draw(operatorLetter);
  }
  bvectorDraw.end=end;
}

void drawBvectorOperator(int opInd,sf::Color color){
  operador* op=opVector<operador>(opInd);

  sf::Color colorFade=darkenColor(color,10);
  switch(op->tipo){
  case NORMAL:{
    normal* n=(normal*)op;
    drawBvectorElement(opInd,sizeof(normal)+n->accs.size+n->conds.size+n->colors.size,color,'n');
  }
    break;
  case DESLIZ:{
    drawBvectorElement(opInd,sizeof(desliz),color,'d');
    drawBvectorOperator(((desliz*)op)->inside,colorFade);
  }break;
  case EXC:{
    exc* e=(exc*)op;
    drawBvectorElement(opInd,sizeof(exc),color,'e');
    forVOp(e->ops){
      drawBvectorOperator(*el,colorFade);
    }
    //for(int i=0;i<e->ops.size;i++){
    //  drawBvectorOperator(*varrayOpElem(&e->ops,i),colorFade);
    //}
    drawBvectorElement(bvectorDraw.end,e->ops.size,color,' ');
  }break;
  case ISOL:
  case ISOLNRM:{
    drawBvectorElement(opInd,sizeof(isol),color,'i');
    drawBvectorOperator(((isol*)op)->inside,colorFade);
  }break;
  case DESOPT:
  case DESOPTNRM:{
    desopt* d=(desopt*)op;
    drawBvectorElement(opInd,sizeof(desopt),color,'f');
    forVOp(d->ops){
      drawBvectorOperator(*el,colorFade);
    }
    drawBvectorElement(bvectorDraw.end,d->ops.size+d->movSizes.size,color,' ');
  }break;
  case FAILOP:
    drawBvectorElement(opInd,sizeof(operador),color,'F');
    break;
  defaultAssert;
  }
  if(op->sig!=0)
    drawBvectorOperator(op->sig,color);
}


void drawBvectorMovholder(int mhInd,sf::Color color){
  sf::Color colorFade=darkenColor(color,20);
  movHolder* mh=gameVector<movHolder>(mhInd);

  if(mh->table==&normalTable){
    normalHolder* nh=(normalHolder*)mh;
    drawBvectorElement(mhInd,sizeof(normalHolder)+nh->memAct.size,color,'n');
  }else if(mh->table==&deslizTable){
    deslizHolder* dh=(deslizHolder*)mh;
    desliz* op=opVector<desliz>(dh->op);
    drawBvectorElement(mhInd,sizeof(deslizHolder),color,'d');
    for(int i=0;i<dh->cantElems;i++){
      drawBvectorMovholder(dh->beg+op->iterSize*i,colorFade);
    }
    int vacantSpace=op->insideSize-op->iterSize*dh->cantElems;
    if(vacantSpace)
      drawBvectorElement(dh->beg+dh->cantElems*op->iterSize,vacantSpace,darkenColor(colorFade,40));
  }else if(mh->table==&excTable){
    excHolder* eh=(excHolder*)mh;
    drawBvectorElement(mhInd,sizeof(excHolder)+eh->movs.size,color,'e');
    forVGame(eh->movs){
      drawBvectorMovholder(*el,colorFade);
    }
  }else if(mh->table==&isolTable || mh->table==&isolNRMTable){
    isolHolder* ih=(isolHolder*)mh;
    drawBvectorElement(mhInd,sizeof(isolHolder),color,'i');
    drawBvectorMovholder(ih->inside,color);
  }else if(mh->table==&desoptTable || mh->table==&desoptNRMTable){
    desoptHolder* dh=(desoptHolder*)mh;
    drawBvectorElement(mhInd,sizeof(desoptHolder)+opVector<desopt>(dh->op)->desoptInsideSize,darkenColor(colorFade,40));
    drawBvectorElement(mhInd,sizeof(desoptHolder),color,'f');

    //    printf("%p %p %d %d\n",(char*)dh->movs,(char*)dh+sizeof(desoptHolder),indGameVector(dh->movs),indGameVector((char*)dh+sizeof(desoptHolder)));

    drawBvectorDesoptHNodes(dh,indGameVector(dh->movs),color);
  }/*else if(mh->table==&spawnerTable){
    drawBvectorElement(mh,sizeof(spawnerGen),color,'s');
    }*/else if(mh->table==&failTable){
    drawBvectorElement(mhInd,sizeof(movHolder),color,'F');
  }else{
    fail("bad movHolder in debug\n");
  }
  if(mh->sig!=0)
    drawBvectorMovholder(mh->sig,color);
}
void drawBvectorDesoptHNodes(desoptHolder* d,int clusterIter,sf::Color color){
  int branchOffset=0;
  desopt* op=opVector<desopt>(d->op);
  forVOp(op->movSizes){
    int tam=*el;
    int movIter=clusterIter+branchOffset;
    movHolder* actualMov=gameVector<movHolder>(movIter+sizeof(int));
    branchOffset+=tam;

    drawBvectorElement(movIter,sizeof(int),sf::Color(std::min(color.r+30,255),std::min(color.g+30,255),std::min(color.b+30,255),color.a));
    drawBvectorMovholder(indGameVector(actualMov),color);

    if(actualMov->bools&valorFinal){
      drawBvectorDesoptHNodes(d,*gameVector<int>(movIter),darkenColor(color,4));
    }
  }
}

void drawHolderData(Holder* h,int x,int y){
  sf::Color color;
  for(int i=0;i<elems(holderColors);i++){
    if(holderColors[i].hI==indGameVector(h)){
      color=holderColors[i].color;
      goto cont;
    }
  }
  color=newRandomColor();
  push(&holderColors,HolderColor{indGameVector(h),color});
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

  Piece* piece=opVector<Piece>(h->piece);
  drawBvectorElement(indGameVector(h),sizeof(Holder)+piece->movs.size,color,'H');
  if(piece->memPieceSize)
    drawBvectorElement(indGameVector(h)+sizeof(Holder)+elems(piece->movs),piece->memPieceSize*sizeof(int),color,'p');

  int firstMov=h->movs.beg;
  assert(firstMov<=bvectorDraw.bv->size);

  forVGame(h->movs){
    int mh=*el;
    drawBvectorElement(mh-sizeof(Base),sizeof(Base),color,'b');
    drawBvectorMovholder(mh,color);
    drawBvectorDelimitator(mh);
  }
}

void debugUpdateAndDrawBvectors(){
  updateScrolling(&bvectorDraw.windowBeg,bvectorDraw.maxScrollingHeight);

  properState* ps=(properState*)stateMem;

  bigVector* bv=debugState==1?&ps->pieceOps:&ps->gameState;

  bvectorDraw.bv=bv;
  bvectorDraw.maxScrollingHeight=std::max(bvectorDraw.bv->cap/lineSize - windowSize,0);

  //recorro el arbol cada vez porque la eficiencia no importa aca
  //ademas como guardar la informacion para que retomarla sea comodo no es obvio,
  //podría guardarla por linea teniendo cada linea una cantidad variable de bloques
  //con informacion extra, o podría tener la lista de bloques y trocearlos dinamicamente
  //aca. Las 2 tienen sus problemas, hacer todo dinamico es lo mas simple

  drawBvectorElement(0,bvectorDraw.bv->cap,sf::Color(150,150,150));
  drawBvectorElement(0,bvectorDraw.bv->size,sf::Color::Black);

  if(debugState==1){
    for(int x=0;x<brd->dims.x;x++){
      for(int y=0;y<brd->dims.y;y++){
        int hI=tileGet(v(x,y))->holder;
        if(hI){
          Holder* h=gameVector<Holder>(hI);
          rect.setSize(sf::Vector2f(32*escala,32*escala));
          rect.setPosition(32*escala*x,32*escala*y);
          rect.setFillColor(pieceColors[opVector<Piece>(h->piece)->ind]);
          window.draw(rect);
        }
      }
    }
    for(int i=0;i<elems(ps->pieces);i++){
      Piece* p=ps->pieces[i];
      int pieceSize=sizeof(Piece)+p->movs.size;
      drawBvectorElement(indOpVector(p),pieceSize,pieceColors[i]);

      for(int j=0;j<elems(p->movs);j++){
        pBase* pb=varrayOpElem(&p->movs,j);
        drawBvectorOperator(pb->root,pieceColors[i]);
        drawBvectorDelimitator(pb->root);
      }
    }
  }else{
    parseData* pd=&ps->pd;
    drawBvectorElement(0,sizeof(board),sf::Color(204, 102, 0),'B');
    int memTileSize=pd->dims.x*pd->dims.y*(sizeof(Tile)+pd->memTileSlots*sizeof(memData));
    if(memTileSize)
      drawBvectorElement(sizeof(board),memTileSize,sf::Color(153, 153, 102),'T');
    int memGlobalSize=pd->memGlobalSize*sizeof(memData);
    if(memGlobalSize)
      drawBvectorElement(sizeof(board)+memTileSize,memGlobalSize,sf::Color(255, 102, 102),'G');

    for(int x=0;x<brd->dims.x;x++){
      for(int y=0;y<brd->dims.y;y++){
        int hI=tileGet(v(x,y))->holder;
        if(hI){
          Holder* h=gameVector<Holder>(hI);
          drawHolderData(h,x,y);
        }
      }
    }
    for(int i=0;i<elems(reciclaje);i++){
      drawHolderData(gameVector<Holder>(reciclaje[i]),-1,-1);
    }
  }

  static int vel;
  vel+=346;
  if(vel/2<bvectorDraw.bv->size)
    drawBvectorElement(vel/2,vel,sf::Color::Red,'X');

}


#endif











