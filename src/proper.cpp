

board* getBoard(properState* ps){
  return (board*)ps->gameState.firstBlock->data;
}

int dt=0;
int clickI=0;
bool confirm;
void humanTurn(bool bando,board* brd){
    /*
    dt++;//se podría mover adentro del if?
    if(!clickers.empty()){
        if(dt>20){
            dt=0;
            clickI++;
            for(Clicker* cli:clickers)
                cli->activacion(clickI);
        }
        drawScreen();
        confirm=false;
        if(input->click()){
            bool any=false;
            for(Clicker* cli:clickers)
                if(cli->update()){
                    any=true;
                    break;
                }
            if(confirm){
                drawScreen();
                return false;
            }
            clickers.clear();
            if(!any)
                drawScreen();
            return any;
        }
    }
    */
    while(true){
      sleep(milliseconds(20));
      input.check();
#if debugMode
      if(window.hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
        properGameInit((properState*)stateMem,true);
        while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
        throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
      }
#endif
      if(input.click()&&input.inGameRange(brd->dims)){
        v posClicked=input.get();
        for(Clicker& cli:clickers){
          ///@todo @optim esto se pregunta 60hz
          ///Lo mejor seria hacer que se bloquee hasta recibir otro click, hacerlo bien cuando
          ///vuelva a meter solapamiento
          if(posClicked==cli.clickPos){
            executeClicker(&cli,brd);//accionar
            drawScreen(properDraw);
            clearClickers();
            return;
          }
        }
        clearClickers();
        printf("(%d,%d)\n",input.get().x,input.get().y);

        Holder* act=tile(brd,input.get())->holder;
        if(act&&act->bando==bando){
          makeCli(act);
          debugPrintClickers(brd);
        }
        drawScreen(properDraw);
      }
    }
}

void skipTurn(){
  actualHolder.ps->turno++;
}





void properUpdate();
void properInit(char* mem,int boardId,int player1Id,int player2Id,bool forTest){
  properState* ps=new(mem)properState();
  ps->boardId=boardId;
  ps->player1=player1Id;
  ps->player2=player2Id;

  if(player1Id!=1&&player2Id!=1)
    fpsLock=0.;

  ps->turnoBlanco.setTexture(image.get("tiles.png"));
  ps->turnoNegro.setTexture(image.get("tiles.png"));
  ps->turnoBlanco.setTextureRect(IntRect(0,0,32,32));
  ps->turnoNegro.setTextureRect(IntRect(32,0,32,32));
  ps->turnoBlanco.setScale(22,16);
  ps->turnoNegro.setScale(22,16);
  ps->turnoBlanco.setPosition(510,0);
  ps->turnoNegro.setPosition(510,0);

#if debugMode
  posPiece.setFillColor(sf::Color(250,240,190,150));
  posActGood.setFillColor(sf::Color(180,230,100,100));
  posActBad.setFillColor(sf::Color(240,70,40,100));
  posMem.setFillColor(sf::Color(0,0,200,100));
  textDebug.setFont(font);
  textDebug.setPosition(520,465);
  backgroundMem.setFillColor(sf::Color(240,235,200));
  backgroundMem.setOutlineColor(sf::Color(195,195,175));
  backgroundMem.setOutlineThickness(4);
  backgroundMem.setSize(Vector2f(40,40));
  backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
  backgroundMemDebug.setOutlineColor(sf::Color(195,195,175));
  backgroundMemDebug.setOutlineThickness(4);
  backgroundMemDebug.setSize(sf::Vector2f(40,40));
  textValMem.setColor(sf::Color::Black);
  textValMem.setFont(font);
  textValMem.setPosition(570,10);
  localMemorySeparator.setFillColor(sf::Color(150,150,150));
  localMemorySeparator.setOutlineColor(sf::Color::Red);
  localMemorySeparator.setOutlineThickness(2);
  localMemorySeparator.setSize(sf::Vector2f(1,40));

  init(&debugDrawChannel);
#endif


  init(&ps->pieces);
  initParser(&ps->pd);

  init(&colores);
  init(&coloresImp);

  actualHolder.ps=ps;

  init(&normales);
  init(&clickers);
  init(&pisados);
  init(&trigsActivados);
  init(&reciclaje);
  init(&justSpawned);

  if(!forTest){
    actualStateUpdate=properUpdate;
    properGameInit(ps);
  }
}

void resetBucket(bucket* b,int size=bucketSize){
  clearBucket(b);
  delete b->firstBlock;
  initBucket(b,size);
}

void properGameInit(properState* ps,bool reset){
  if(reset){
    parseData* pd=&ps->pd;

    clearMacros(pd);
    pd->lastTangledGroup=0;
    pd->memLocal.size=0;
    pd->memLocalSizeMax=0;
    pd->memPieceSize=0;
    pd->memGlobalSize=0;
    pd->memTileSlots=0;
    pd->spawner=false;
    pd->boardInit.size=0;
    pd->ids.size=0;

    ps->pieces.size=0;
    
    //colores.size=0;
    coloresImp.size=0;

    normales.size=0;
    clickers.size=0;
    pisados.size=0;
    trigsActivados.size=0;
    reciclaje.size=0;
    justSpawned.size=0;

    board* brd=getBoard(ps);
    delete[] brd->ts.mem;

    free(&memMov);
    resetBucket(&ps->pieceOps);
  }else{
    initBucket(&ps->pieceOps);
  }
  ps->clickers.size=0;
  ps->turno=1;

  getBoardIds(&ps->pd,ps->boardId);
  ps->pieces.size=0;
  makePieces(&ps->pd,&ps->pieces,&ps->pieceOps);

  ps->hsSize=0;

  for(int& id:ps->pd.boardInit){
    if(id){
      ps->hsSize+=ps->pieces[getIndexById(&ps->pd.ids,id)]->hsSize;
    }
  }
  
  ps->hsSize+=sizeof(board)
            + ps->pd.dims.x*ps->pd.dims.y*(sizeof(Tile)+ps->pd.memTileSlots*sizeof(memData))
            + ps->pd.memGlobalSize*sizeof(memData);

  if(reset)
    resetBucket(&ps->gameState,ps->hsSize);
  else
    initBucket(&ps->gameState,ps->hsSize);

  makeBoard(ps);

  /*
  for(int i=0; i<tablptr->tam.y; i++){
    for(int j=0; j<tablptr->tam.x; j++){
      cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
    }
    cout<<endl;
    }*/

  drawScreen(properDraw);
}


int debugGet(memData* v,int i){
  return (v+i)->val;
}
int debugGet(auto* v,int i){
  return *(v+i);
}
const int cellsPerRow=16;
const int cellSpacing=39;
void debugDrawMemory(int memSize, int yOffset,auto memory){
#if debugMode
  for(int i=0;i<memSize;i++){
    backgroundMem.setPosition(sf::Vector2f(530+cellSpacing*(i%cellsPerRow),yOffset+45*(i/cellsPerRow-memSize/cellsPerRow)));
    window.draw(backgroundMem);
  }
  for(int i=0;i<memSize;i++){
    textValMem.setPosition(530+cellSpacing*(i%cellsPerRow),yOffset+5+45*(i/cellsPerRow-memSize/cellsPerRow));
    textValMem.setString(std::to_string(debugGet(memory,i)));
    window.draw(textValMem);
  }
#endif
};




void properDraw(char* mem){
  properState* ps=(properState*)mem;
  board* brd=getBoard(ps);
  drawTiles(brd);
  if(Clicker::drawClickers)
    for(Clicker& cli:clickers)
      drawClicker(&cli);
  drawPieces(brd);
  if(ps->turno&1)
    window.draw(ps->turnoBlanco);
  else
    window.draw(ps->turnoNegro);


#if debugMode
  textValMem.setString(std::to_string(ps->turno));
  textValMem.setPosition(sf::Vector2f(600,20));
  window.draw(textValMem);
  if(drawDebug){
    window.draw(*tileActDebug);
  
    window.draw(posPiece);
    window.draw(textDebug);

    debugDrawMemory(actualHolder.nh->base->memLocal.size,405,memMov.data);
    debugDrawMemory(actualHolder.h->piece-> memPieceSize,305,actualHolder.h->memPiece.beg);
    debugDrawMemory(brd->memGlobalSize,205,brd->memGlobals);
    debugDrawMemory(brd->memTileSlots,105,getTileMd(0,brd));

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
#endif
}

void doTurn(properState* ps,int player,bool bando){
  /*board* brd=getBoard(ps);
  if(!bando)
  for(int i=0;i<ps->pd.memTileSlots*brd->dims.x*brd->dims.y;i++){
    if(i%brd->dims.x*ps->pd.memTileSlots==0){
      printf("\n");
    }
    printf("%d ",brd->memTiles[i].val);
    }*/
  switch(player){
  case 1: humanTurn(bando,getBoard(ps));break;
  case 2: randomTurnTestPlayer(bando,ps);break;
  case 4: skipTurn();
  }
}

void properUpdate(char* mem){
  properState* ps=(properState*)mem;
  try{
    doTurn(ps,ps->player1,false);
    doTurn(ps,ps->player2,true);
  }catch(...){}
}

/*
bool properInRange(v a)
{
  return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}
*/








