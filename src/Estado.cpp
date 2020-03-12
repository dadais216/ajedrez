
struct properState{
  Jugador player1,player2;
  Sprite turnoBlanco,turnoNegro;
  debug(
        v posPieza;
        v posActGood;
        v posActBad;
        Text textDebug;
        RectangleShape backgroundMem;
        RectangleShape backgroundMemDebug;
        Text textValMem;
        )
}


properUpdate();
void properInit(int tableroId_,int player1Id,int player2Id){
  resetBucket(&stateBucket);
  getStruct(properState,ps,stateBucket);

  int nonHumans=0;

  int sel=player1Id;
  bool bando=false;
 createPlayer:
  switch(sel){
  case 0:nonHumans++;nadieInit(bando);break;
  case 1:humanoInit(bando);break;
  case 2:nonHumans++;aleatorioInit(bando);break;
    //case 3: return new IA(bando,tablero);
  }
  if(!bando){
    bando=true;
    sel=player2Id;
    goto createPlayer;
  }
  if(nonHumans==2)
    fpsLock=0.;

  ps->turnoBlanco.setTexture(imagen->get("tiles.png"));
  ps->turnoNegro.setTexture(imagen->get("tiles.png"));
  ps->turnoBlanco.setTextureRect(IntRect(0,0,32,32));
  ps->turnoNegro.setTextureRect(IntRect(32,0,32,32));
  ps->turnoBlanco.setScale(12,16);
  ps->turnoNegro.setScale(12,16);
  ps->turnoBlanco.setPosition(510,0);
  ps->turnoNegro.setPosition(510,0);

  debug(
        posPieza.setFillColor(sf::Color(250,240,190,150));
        posActGood.setFillColor(sf::Color(180,230,100,100));
        posActBad.setFillColor(sf::Color(240,70,40,100));
        textDebug.setFont(j->font);
        textDebug.setPosition(520,465);

        backGroundMem.setFillColor(sf::Color(240,235,200));
        backGroundMem.setOutlineColor(sf::Color(195,195,175));
        backGroundMem.setOutlineThickness(4);
        backGroundMem.setSize(Vector2f(20,40));
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setOutlineColor(sf::Color(195,195,175));
        backGroundMemDebug.setOutlineThickness(4);
        backGroundMemDebug.setSize(Vector2f(20,40));
        textValMem.setColor(Color::Black);
        textValMem.setFont(j->font);
        )
  lastBucket=&bucketPiezas;new Bucket();
  lastBucket=&bucketHolders;new Bucket();

  properGameInit();
}

void properGameInit(){
  clickers.clear();
  memMov.clear();
  maxMemMovSize=0;
  memGlobal.clear();
  memGlobalTriggers.clear();
  memGlobalSize=0;
  memTileSize=0;
  turnoTrigs[0].clear();
  turnoTrigs[1].clear();
  turnoAct=2;
  turno=1;

  if(lect.archPiezas.is_open())
    lect.archPiezas.close();
  lect.archPiezas.open("piezas.txt");

  lect.generarIdsTablero(id);

  tablero.armar(v(lect.matriz[0].size(),lect.matriz.size()));

  lect.cargarDefs();

  //esto esta aca porque escala se setea en armar
  posPieza.setSize(Vector2f(32*escala,32*escala));
  debug(
        posActGood.setSize(Vector2f(32*escala,32*escala));
        posActBad.setSize(Vector2f(32*escala,32*escala));
        )


  piezas.clear();
  cout<<"-----"<<endl;
  for(uint i=0; i<lect.matriz.size(); i++){
    for(uint j=0; j<lect.matriz[0].size(); j++){
      int n=lect.matriz[i][j];
      v pos(j,i);
      //cout<<pos<<"  "<<tablero.tam<<endl;
      if(n)
        tablero.tile(pos)->holder=lect.crearPieza(n,pos);
      else
        tablero.tile(pos)->holder=nullptr;
    }
  }

  memset(memGlobal.data(),0,memGlobalSize*sizeof(int));

  for(uint i=0; i<lect.matriz.size(); i++){
    for(uint j=0; j<lect.matriz[0].size(); j++){
      Holder* hAct=tablero.tile(v(j,i))->holder;
      if(hAct){
        turno1=hAct->bando;
        hAct->generar();
      }
    }
  }
  cout<<endl;
  for(int i=0; i<tablptr->tam.y; i++){
    for(int j=0; j<tablptr->tam.x; j++){
      cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
    }
    cout<<endl;
  }
  turno1=true;
  drawScreen();
}

void properDraw(){
  tablero.drawTiles();
  if(Clicker::drawClickers)
    for(Clicker& cli:clickers)
      cli.draw();
  tablero.drawPieces();
  if(turno1)
    window.draw(turnoBlanco);
  else
    window.draw(turnoNegro);

  debug(
        textValMem.setPosition(570,10);
        textValMem.setString(to_string(turno));
        window.draw(textValMem);

          window.draw(*tileActDebug);
          if(drawAsterisco){
            window.draw(asterisco);
            drawAsterisco=false;
          }
        
          window.draw(posPieza);
          window.draw(textDebug);
          int memSize=actualHolder.nh->base->memLocalSize;
          for(int i=0;i<memSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),405+45*(i/4-memSize/4)));
            window.draw(backGroundMem);
          }
          for(int i=0;i<memGlobalSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),305+45*(i/4-memGlobalSize/4)));
            window.draw(backGroundMem);
          }
          int memPiezaSize=actualHolder.h->memPieza.count();
          for(int i=0;i<memPiezaSize;i++){
            backGroundMem.setPosition(Vector2f(530+25*(i%4),205+45*(i/4-memPiezaSize/4)));
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
          for(int i=0;i<memPiezaSize;i++){
            textValMem.setPosition(530+25*(i%4),210+45*(i/4-memPiezaSize/4));
            textValMem.setString(to_string(*actualHolder.h->memPieza[i]));
            window.draw(textValMem);
          }
          for(int i=0;i<memTileSize;i++){
            textValMem.setPosition(530+25*(i%4),110+45*(i/4-memTileSize/4));
            textValMem.setString(to_string(tablptr->tile(posDebugTile)->memTile[i]));
            window.draw(textValMem);
          }
        )
}

void properUpdate(){
  try{
    primero->turno();
    segundo->turno();
  }catch(...){}
}

/*
bool properInRange(v a)
{
  return a.x>=0&&a.x<=tablero.tam.x-1&&a.y>=0&&a.y<=tablero.tam.y-1;
}
*/








