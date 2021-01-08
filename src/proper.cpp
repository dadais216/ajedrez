

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
        properGameInit<true>((properState*)stateMem);
        while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
        throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
      }

      //por ahora este codigo esta aca, eventualmente voy a tener un thread que maneje eventos y lo voy a tirar ahi
      drawScreen([&](){properDraw(stateMem);
          debugUpdateAndDrawBuckets();
                 });
      
#endif


      if(input.click()&&input.inGameRange(brd->dims)){
        v posClicked=input.get();
        for(Clicker& cli:clickers){
          ///@todo @optim esto se pregunta 60hz
          ///Lo mejor seria hacer que se bloquee hasta recibir otro click, hacerlo bien cuando
          ///vuelva a meter solapamiento
          if(posClicked==cli.clickPos){
            executeClicker(&cli,brd);//accionar
            drawScreen([&](){properDraw(stateMem);});
            clearClickers();
            return;
          }
        }
        clearClickers();
        printf("(%d,%d)\n",input.get().x,input.get().y);

        Holder* act=tile(brd,input.get())->holder;
        if(act&&act->bando==bando){
          makeCli(act);
          //debugPrintClickers(brd);
        }
        //drawScreen([&](){properDraw(stateMem);});
      }

    }
}

void skipTurn(){
  actualHolder.ps->turno++;
}




void properUpdate();
template<bool forTest>
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

  initBucket(&ps->pieceOps);


  if constexpr(forTest){
    ps->gameState.size=0;
    //marcar para no borrar la primera vez en test, porque no hay nada que borrar
    //podría alocar algo al pedo para borrar, pero poner un if es mas simple y
    //esto no es algo importante
  }else{
    actualStateUpdate=properUpdate;
    properGameInit<false>(ps);
  }
}

void resetBucket(bucket* b,int size=bucketSize){
  clearBucket(b);
  delete b->firstBlock;
  initBucket(b,size);
}

template<bool reset>
void properGameInit(properState* ps,bool firstTestIteration){
  if constexpr(reset){
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

    if(ps->gameState.size!=0){
      board* brd=getBoard(ps);
      delete[] brd->ts.mem;

      free(&memMov);
      if(firstTestIteration)//no termino de entender esto, la primera vez quien lo aloca? TODO
        resetBucket(&ps->pieceOps);
      else
        clearBucketNoFree(&ps->pieceOps);
    }
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

  if constexpr(reset){
    if(firstTestIteration){
      if(ps->gameState.size!=0){
        resetBucket(&ps->gameState,ps->hsSize);
      }else{
        initBucket(&ps->gameState,ps->hsSize);
      }
    }else{
      clearBucketNoFree(&ps->gameState);
    }
  }else
    initBucket(&ps->gameState,ps->hsSize);

  makeBoard(ps);

#if debugMode
  debugUpdateAndDrawBucketsInit(reset);
#endif
  drawScreen([](){properDraw(stateMem);});
}





void properDraw(char* mem,bool drawDebug){
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

  /*
  board* brd=getBoard(ps);
  for(int i=0; i<brd->dims.y; i++){
    for(int j=0; j<brd->dims.x; j++){
      printf("%d ",tile(brd,v(j,i))->triggersUsed);
    }
    printf("\n");
  }
  */
}

/*
bool properInRange(v a)
{
  return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}
*/








