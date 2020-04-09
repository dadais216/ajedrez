


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
        //static_cast<Proper*>(j->actual)->init();///@leaks
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
          drawScreen(properDraw);
        }
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
  board* brd=(board*)ps->gameState.data;
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
               <<"\nbucketMovSize  "<<ps->gameState.head - ps->gameState.data - (intptr)(((board*)ps->gameState.data)->tiles)
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
  auto color=sf::Color(250,240,190,150);
  posPiece.setFillColor(color);
  posActGood.setFillColor(sf::Color(180,230,100,100));
  posActBad.setFillColor(sf::Color(240,70,40,100));
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
#endif


  init(&ps->pieces);
  initParser(&ps->pd);

  init(&colores);

  actualHolder.ps=ps;
  initBucket(&ps->pieceOps);

  init(&normales);
  init(&clickers);
  init(&pisados);
  init(&trigsActivados);

  actualStateUpdate=properUpdate;
  properGameInit(ps);
}


void properGameInit(properState* ps){
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

void properDraw(char* mem){
  properState* ps=(properState*)mem;
  board* brd=(board*)ps->gameState.data;
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
  window.draw(textValMem);
#if debugMode
  if(drawDebugTiles){
    window.draw(*tileActDebug);
  
    window.draw(posPiece);
    window.draw(textDebug);
  /*int memSize=actualHolder.nh->base->memLocalSize;
    for(int i=0;i<memSize;i++){
    backGroundMem.setPosition(Vector2f(530+25*(i%4),405+45*(i/4-memSize/4)));
    window.draw(backGroundMem);
    }
    for(int i=0;i<memGlobalSize;i++){
    backGroundMem.setPosition(Vector2f(530+25*(i%4),305+45*(i/4-memGlobalSize/4)));
    window.draw(backGroundMem);
    }
    int memPieceSize=actualHolder.h->memPiece.count();
    for(int i=0;i<memPieceSize;i++){
    backGroundMem.setPosition(Vector2f(530+25*(i%4),205+45*(i/4-memPieceSize/4)));
    window.draw(backGroundMem);
    }
    for(int i=0;i<memTileSize;i++){
    backGroundMem.setPosition(Vector2f(530+25*(i%4),105+45*(i/4-memTileSize/4)));
    window.draw(backGroundMem);
    }
    if(getterMemDebug1){
    getterMemDebug1->drawDebugMem();
    getterMemDebug2->drawDebugMem();
    }
    for(int i=0;i<memSize;i++){
    textValMem.setPosition(530+25*(i%4),410+45*(i/4-memSize/4));
    textValMem.setString(to_string(memMov[i]));
    window.draw(textValMem);
    }
    for(int i=0;i<memGlobalSize;i++){
    textValMem.setPosition(530+25*(i%4),310+45*(i/4-memGlobalSize/4));
    textValMem.setString(to_string(memGlobal[i]));
    window.draw(textValMem);
    }
    for(int i=0;i<memPieceSize;i++){
    textValMem.setPosition(530+25*(i%4),210+45*(i/4-memPieceSize/4));
    textValMem.setString(to_string(*actualHolder.h->memPiece[i]));
    window.draw(textValMem);
    }
    for(int i=0;i<memTileSize;i++){
    textValMem.setPosition(530+25*(i%4),110+45*(i/4-memTileSize/4));
    textValMem.setString(to_string(tablptr->tile(posDebugTile)->memTile[i]));
    window.draw(textValMem);
    }*/
  }
#endif
}

void doTurn(properState* ps,int player,bool bando){
  switch(player){
  case 1: humanTurn(bando,(board*)ps->gameState.data);break;
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








