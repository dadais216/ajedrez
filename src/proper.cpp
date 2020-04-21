

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
        properGameInit((properState*)stateMem);
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
            return;
          }
        }
        clickers.size=0;
        printf("(%d,%d)\n",input.get().x,input.get().y);

        Holder* act=tile(brd,input.get())->holder;
        if(act&&act->bando==bando){
          makeCli(act);
        }
        drawScreen(properDraw);
      }
    }
}

void skipTurn(){
  actualHolder.ps->turno++;
}



double sProm=0;
int cProm=0;
double minV=10000;
double maxV=0;
void randomTurn(bool bando,properState* ps){
  board* brd=getBoard(ps);
  if(window.hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
    properGameInit(ps);
    while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
    throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
  }
  bool alive=false;
  for(int i=0; i<brd->dims.x; i++)
    for(int j=0; j<brd->dims.y; j++){
      Holder* act=tile(brd,v(i,j))->holder;
      if(act&&act->bando==bando){
        alive=true;
        makeCli(act);
      }
    }
  //drawScreen();
  if(!alive)
    while(true){
      if(window.hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
        properGameInit(ps);///@leaks
        while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
        throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
      }
    }
  if(clickers.size>0){
    //sleep(milliseconds(120));
    
    
    clock_t t=clock();
    executeClicker(&clickers[rand()%clickers.size],brd);
    double val=clock()-t;
    sProm+=val;
    if(val>maxV)
      maxV=val;
    if(val<minV)
      minV=val;
    cProm++;
    if(cProm==100){
      std::cout<<"normalSize  "<<sizeof(normalHolder)
               <<"\nbucketMovSize  "<<ps->gameState.head - ps->gameState.data - (intptr)(getBoard(ps))->tiles
               <<"\nbucketOpSize  "<<ps->pieceOps.head-ps->pieceOps.data
               <<"\npromedio: "<<std::fixed<<sProm/(double)cProm/CLOCKS_PER_SEC<<" segundos"
               <<"\nmin: "<<minV/CLOCKS_PER_SEC
               <<"\nmax: "<<maxV/CLOCKS_PER_SEC<<std::endl;
      exit(0);
    }
    
    clickers.size=0;
  }
}


void properUpdate();
void properInit(char* mem,int boardId,int player1Id,int player2Id){
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
  ps->turnoBlanco.setScale(12,16);
  ps->turnoNegro.setScale(12,16);
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
  backgroundMem.setSize(Vector2f(20,40));
  backgroundMemDebug.setFillColor(sf::Color(163,230,128,150));
  backgroundMemDebug.setOutlineColor(sf::Color(195,195,175));
  backgroundMemDebug.setOutlineThickness(4);
  backgroundMemDebug.setSize(Vector2f(20,40));
  textValMem.setColor(Color::Black);
  textValMem.setFont(font);
  textValMem.setPosition(570,10);

  ps->debugFirstRun=true;
  init(&debugDrawChannel);
#endif


  init(&ps->pieces);
  initParser(&ps->pd);

  init(&colores);

  actualHolder.ps=ps;

  init(&normales);
  init(&clickers);
  init(&pisados);
  init(&trigsActivados);
  init(&reciclaje);
  init(&justSpawned);

  actualStateUpdate=properUpdate;
  properGameInit(ps);
}

void initOrDebugResetBucket(properState* ps,bucket* b,int size=bucketSize){
#if debugMode
  if(!ps->debugFirstRun){
    clearBucket(b);
    delete[] b->firstBlock;
    initBucket(b,size);
  }else
#endif
    initBucket(b,size);
}

void properGameInit(properState* ps){
#if debugMode
  //cosas para resetear
  parseData* pd=&ps->pd;

  pd->lastGlobalMacro=tlast;
  pd->lastTangledGroup=0;
  init(&pd->memLocalSize,4);
  pd->memLocalSizeMax=0;
  pd->memPieceSize=0;
  pd->memGlobalSize=0;
  pd->memTileSlots=0;

  pd->spawner=false;

  init(&pd->boardInit);
  init(&pd->ids);
  init(&pd->macros);

  ps->pieces.size=0;

  colores.size=0;

  normales.size=0;
  clickers.size=0;
  pisados.size=0;
  trigsActivados.size=0;
  reciclaje.size=0;
  justSpawned.size=0;

  for(int i=tlast;i<ps->pd.lastLocalMacro;i++){
    ps->pd.wordToToken.erase(ps->pd.tokenToWord[i]);
  }
  ps->pd.lastLocalMacro=ps->pd.lastGlobalMacro=tlast;
#endif
  initOrDebugResetBucket(ps,&ps->pieceOps);
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
  initOrDebugResetBucket(ps,&ps->gameState,ps->hsSize);
  makeBoard(ps);

  /*
  for(int i=0; i<tablptr->tam.y; i++){
    for(int j=0; j<tablptr->tam.x; j++){
      cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
    }
    cout<<endl;
    }*/

  drawScreen(properDraw);

#if debugMode
  ps->debugFirstRun=false;
#endif
}


int debugGet(auto* v,int i){
  return v->operator[](i);
}
int debugGet(memData* v,int i){
  return (v+i)->val;
}
void debugDrawMemory(int memSize, int yOffset,auto memory){
  for(int i=0;i<memSize;i++){
    backgroundMem.setPosition(sf::Vector2f(530+25*(i%4),yOffset+45*(i/4-memSize/4)));
    window.draw(backgroundMem);
  }
  for(int i=0;i<memSize;i++){
    textValMem.setPosition(530+25*(i%4),yOffset+5+45*(i/4-memSize/4));
    textValMem.setString(std::to_string(debugGet(memory,i)));
    window.draw(textValMem);
  }
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

  textValMem.setString(std::to_string(ps->turno));
  textValMem.setPosition(sf::Vector2f(600,20));
  window.draw(textValMem);
#if debugMode
  if(drawDebug){
    window.draw(*tileActDebug);
  
    window.draw(posPiece);
    window.draw(textDebug);

    debugDrawMemory(actualHolder.nh->base->memLocalSize,405,&memMov);
    debugDrawMemory(actualHolder.h->piece-> memPieceSize,305,&actualHolder.h->memPiece);
    debugDrawMemory(brd->memGlobalSize,205,brd->memGlobals);
    debugDrawMemory(brd->memTileSlots,105,getTileMd(0,brd));

    bool cteInThisIteration=false;
    for(int i=0;i<debugDrawChannel.size;i++){
      switch(debugDrawChannel[i]){
      case tdebugSetIndirectColor:
        backgroundMemDebug.setFillColor(sf::Color(178,135,221,150));
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
          backgroundMemDebug.setPosition(Vector2f(530+25*(ind%4),
                                                  drawOffset+45*(ind/4-memSize/4)));
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
  case 2: randomTurn(bando,ps);srand(time(NULL));break;
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








