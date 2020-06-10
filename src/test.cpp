

struct{
  double nProm;
  int dProm;
  bool cutEarly;

  double sProm;
  double minProm;
}testData;

struct testPrintData{
  char const* name;
  int holderBucketSize;
  int opBucketSize;
  double promSec;
  double prom;
  double minProm;
};

struct{
  vector<testPrintData> before;
  vector<testPrintData> after;//guardo al final en vez de por test para no guardar en caso de crash
}testPrint;

template<typename T>
void sGetNext(char** ss,T* var){
  char* s=*ss;
  do{s++;
    if(*s=='+'||*s=='-'){
      do{
        s++;
      }while(*s!='\n');
      continue;
    }
  }while(*s>'9'||*s<'0');

  char format[4]="%  ";
  if(typeid(T)==typeid(int))
    format[1]='d';
  else if(typeid(T)==typeid(double)){
    format[1]='l';
    format[2]='f';
  }
  else
    fail("bad metaprogramming");

  sscanf(s,format,var);
  do{s++;
  }while((*s<='9'&&*s>='0')||*s=='.');
  *ss=s;
}

void getOldStats(){
  init(&testPrint.before);
  init(&testPrint.after);

  char* str=loadFile("benchmarks");//no sé si se puede hacer algo mejor que esto, nomas necesito lo que este al final del archivo pero el offset no es fijo
  //(supongo que podria calcular el offset haciendo los datos const y teniendo una funcion constexpr aca pero eh)
  FILE* file=fopen("benchmarks","r");//medio bizarro reabrir pero bueno
  fseek(file,0,2);
  int size=ftell(file);
  fclose(file);

  int i;
  for(i=size-40;i>0;i--){
    if(str[i]=='#'){
      i+=2;//saltar # y \n
      break;
    }
  }
  char* s=str+i;

  do{
    testPrintData* oldBench=newElem(&testPrint.before);
    char* b=s;
    do{s++;
    }while(*s!=':');
    oldBench->name=new char[s-b+1];//podria tener un char[256] sino no importa
    memcpy((char*)oldBench->name,b,s-b);//podria no copiar y mantener str, pero lo libero por las dudas de que tenga un impacto, no creo igual
    *(char*)(oldBench->name+(s-b))=0;//que rompe bola son los const

    sGetNext(&s,&oldBench->holderBucketSize);
    sGetNext(&s,&oldBench->opBucketSize);
    sGetNext(&s,&oldBench->promSec);
    sGetNext(&s,&oldBench->prom);
    sGetNext(&s,&oldBench->minProm);

    s+=5;//saltar el posible - en el ultimo delta, medio garca
    do{s++;}while(*s!='-');
    do{s++;}while(*s=='-');
    s++;
  }while(*s!='#');

  delete[] str;
}

void saveStats(){
  FILE* file;
  if(saveBenchmark){
    file=fopen("benchmarks","a");
  }
  for(int i=0;i<testPrint.after.size;i++){
    testPrintData* actual=&testPrint.after[i];
    int holderBucketSizeDelta=0,opBucketSizeDelta=0;
    double promSecDelta=0,promDelta=0,minPromDelta=0;

    for(int j=0;j<testPrint.before.size;j++){
      testPrintData* before=&testPrint.before[j];
      if(strcmp(before->name,actual->name)==0){
        holderBucketSizeDelta=actual->holderBucketSize-before->holderBucketSize;
        opBucketSizeDelta=actual->opBucketSize-before->opBucketSize;
        promSecDelta=actual->promSec-before->promSec;
        promDelta=actual->prom-before->prom;
        minPromDelta=actual->minProm-before->minProm;
      }
    }

    auto charSign=[](double val)->char{
                    return val>=0?'+':'-';
                  };

    printf("%s:\nholder bucket %d\t%c%d\nop bucket %d\t\t%c%d\npromedio s %f\t%c%f\npromedio %f\t%c%f\nmejor %f\t%c%f\n---------------------------\n",
           actual->name,
           actual->holderBucketSize,charSign(holderBucketSizeDelta),std::abs(holderBucketSizeDelta),
           actual->opBucketSize,charSign(opBucketSizeDelta),std::abs(opBucketSizeDelta),
           actual->promSec,charSign(promSecDelta),std::abs(promSecDelta),
           actual->prom,charSign(promDelta),std::abs(promDelta),
           actual->minProm,charSign(minPromDelta),std::abs(minPromDelta));
    if(saveBenchmark){
      fprintf(file,"%s:\nholder bucket %d\t%c%d\nop bucket %d\t\t%c%d\npromedio s %f\t%c%f\npromedio %f\t%c%f\nmejor %f\t%c%f\n---------------------------\n",
              actual->name,
              actual->holderBucketSize,charSign(holderBucketSizeDelta),std::abs(holderBucketSizeDelta),
              actual->opBucketSize,charSign(opBucketSizeDelta),std::abs(opBucketSizeDelta),
              actual->promSec,charSign(promSecDelta),std::abs(promSecDelta),
              actual->prom,charSign(promDelta),std::abs(promDelta),
              actual->minProm,charSign(minPromDelta),std::abs(minPromDelta));
    }
  }
  if(saveBenchmark){
    fprintf(file,"###########################\n");
    fclose(file);
  }
}

void runTest(properState* ps,char const* name,int map,int turns,int times,bool player2Random,bool firstTest=false){
  printf("running %s\n",name);

  testData.sProm=0;
  testData.minProm=DBL_MAX;
  for(int i=0;i<times;i++){
    ps->boardId=map;
    ps->player2=player2Random?2:4;
    properGameInit(ps,i!=0||!firstTest);//mas adelante debería hacer un copy paste del estado inicial en vez de recrear todo

    testData.nProm=0;
    testData.dProm=0;
    testData.cutEarly=false;

    while(testData.dProm<turns&&!testData.cutEarly){
      properUpdate((char*)ps);
    }
    testData.sProm+=testData.nProm/testData.dProm;
    testData.minProm=std::min(testData.minProm,testData.nProm/testData.dProm);
  }


  testPrintData* result=newElem(&testPrint.after);
  result->name=name;
  result->holderBucketSize=(intptr)(ps->gameState.head-(intptr)getBoard(ps));
  result->opBucketSize=(intptr)(ps->pieceOps.head-ps->pieceOps.data);
  result->promSec=(testData.sProm/(double)times)/1e9;
  result->prom=testData.sProm/(double)times;
  result->minProm=testData.minProm;
}


void doTests(char* mem){
  properInit(mem,0,2,2,true);
  properState* ps=(properState*)mem;
  srand(time(NULL));
  getOldStats();

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
  runTest(ps,"desopt",23,1600,10,false);
  runTest(ps,"desopt a manopla",24,1600,10,false);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);
  double elapsed=(end.tv_sec-beg.tv_sec)+(end.tv_nsec-beg.tv_nsec)/1e9;
  printf("\ntotal time %f\n",elapsed);

  saveStats();
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

    testData.dProm++;
    testData.nProm+=elapsed;

    clickers.size=0;
  }else{
    testData.cutEarly=true;
  }
  drawScreen(properDraw);
}

