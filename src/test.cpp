

struct{
  double sProm;
  int cProm;
  double minV;
  double maxV;
  bool cutEarly;
}testData;

void runTest(properState* ps,char const* name,int map,int turns,int times,bool player2Random,bool firstTest=false){
  for(int i=0;i<times;i++){
    ps->boardId=map;
    ps->player2=player2Random?2:4;
    properGameInit(ps,i!=0||!firstTest);//mas adelante debería hacer un copy paste del estado inicial en vez de recrear todo

    testData.sProm=0;
    testData.cProm=0;
    testData.minV= DBL_MAX;
    testData.maxV=0;
    testData.cutEarly=false;

    while(testData.cProm<turns&&!testData.cutEarly){
      testData.cProm++;
      properUpdate((char*)ps);
    }
    printf("%s:\nholder bucket %d\nop bucket %d\npromedio s %f\npromedio %f\nmin %f\nmax %f\n",
           name,
           (int)(intptr)(ps->gameState.head - (intptr)getBoard(ps)),
           (int)(intptr)(ps->pieceOps.head-ps->pieceOps.data),
           (testData.sProm/(double)testData.cProm)/1e9,
           testData.sProm/(double)testData.cProm,
           testData.minV,
           testData.maxV);
  }
}


void doTests(char* mem){
  properInit(mem,0,2,2,true);
  properState* ps=(properState*)mem;
  srand(time(NULL));

  timespec beg,end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&beg);

  runTest(ps,"simple",14,1000,2,false,true);
  runTest(ps,"tiles",21,400,60,false);
  runTest(ps,"growin",20,50,200,false);
  runTest(ps,"puzzle",15,1000,2,false);
  runTest(ps,"germen",19,399,100,false);
  runTest(ps,"reina",18,500,100,false);
  //runTest(ps,"emperadores",22,300,10,false); no anda porque isol en desliz no esta bien hecho y hay 2 formas de solucionarlo
  //runTest(ps,"rebote",23,300,10,false); no anda porque no hay normales no esp por ahora
  runTest(ps,"caballos",17,300,100,true);
  runTest(ps,"normal",16,80,1000,true);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);
  double elapsed=(end.tv_sec-beg.tv_sec)+(end.tv_nsec-beg.tv_nsec)/1e9;
  printf("\ntotal time %f\n",elapsed);
}


void randomTurnTestPlayer(bool bando,properState* ps){
  board* brd=getBoard(ps);
  
  for(int i=0; i<brd->dims.x; i++)
    for(int j=0; j<brd->dims.y; j++){
      Holder* act=tile(brd,v(i,j))->holder;
      if(act&&act->bando==bando){
        makeCli(act);
      }
    }
  //drawScreen();
  if(clickers.size>0){
    //sleep(milliseconds(120));
    timespec beg,end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&beg);
    executeClicker(&clickers[rand()%clickers.size],brd);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);

    double elapsed=(end.tv_sec-beg.tv_sec)*1e9+(end.tv_nsec-beg.tv_nsec);

    testData.sProm+=elapsed;
    if(elapsed>testData.maxV)
      testData.maxV=elapsed;
    if(elapsed<testData.minV)
      testData.minV=elapsed;

    clickers.size=0;
  }else{
    testData.cutEarly=true;
    testData.cProm--;
  }
  drawScreen(properDraw);
}

