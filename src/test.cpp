

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
  int holderBucketBuckets;
  int opBucketSize;
  int opBucketBuckets;
  double promSec;
  double prom;
  double minProm;
  double delta;
};

struct{
  vector<testPrintData> before;
  vector<testPrintData> after;//guardo al final en vez de por test para no guardar en caso de crash
  char const* name;
}testPrint;

void sSkipLine(char** ss){
  char* s=*ss;
  /*do{s++;
    if(*s=='+'||*s=='-'){
      do{
        s++;
      }while(*s!='\n');
      continue;
    }
    }while(*s>'9'||*s<'0');*/
  do{s++;
  }while(*s!='\n');
  s++;
  *ss=s;
}

template<typename T>
void sGetNumber(char** ss,T* var){
  char* s=*ss;
  do{s++;
  }while(*s>'9'||*s<'0');

  char format[4]="%  ";
  if constexpr(std::is_same<T,int>::value){
    format[1]='d';
  }else{
    static_assert(std::is_same<T,double>::value);
    format[1]='l';
    format[2]='f';
  }

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
  defer(str);
  FILE* file=fopen("benchmarks","r");//medio bizarro reabrir pero bueno
  fseek(file,0,2);
  int size=ftell(file);
  fclose(file);

  if(size>10){
    int i;
    for(i=size-40;i>0;i--){
      if(str[i]=='#'){
        i+=2;//saltar # y \n
        break;
      }
    }
    assert(str[i]=='@');
    i++;
    while(str[i]!='@') i++;
    i+=2;//saltar @ y \n

    char* s=str+i;
    
    do{
      testPrintData* oldBench=newElem(&testPrint.before);
      char* b=s;
      do{s++;
      }while(*s!=':');
      oldBench->name=new char[s-b+1];//podria tener un char[256] sino no importa
      memcpy((char*)oldBench->name,b,s-b);//podria no copiar y mantener str, pero lo libero por las dudas de que tenga un impacto, no creo igual
      *(char*)(oldBench->name+(s-b))=0;//que rompe bola son los const
      
      sGetNumber(&s,&oldBench->holderBucketSize);
      sGetNumber(&s,&oldBench->holderBucketBuckets);sSkipLine(&s);
      sGetNumber(&s,&oldBench->opBucketSize);
      sGetNumber(&s,&oldBench->opBucketBuckets);sSkipLine(&s);
      sGetNumber(&s,&oldBench->promSec);sSkipLine(&s);
      sGetNumber(&s,&oldBench->prom);sSkipLine(&s);
      sGetNumber(&s,&oldBench->minProm);sSkipLine(&s);

      do{s++;}while(*s!='-'||*(s+1)!='-');//frenar en el ----, ignorar posibles menos
      do{s++;}while(*s=='-');
      s++;
    }while(*s!='#');
  }
}

void saveStats(){
  printf("@%s@\n",testPrint.name);
  FILE* file;
  if(saveBenchmark){
    file=fopen("benchmarks","a");
    fprintf(file,"@%s@\n",testPrint.name);
  }

  for(int i=0;i<testPrint.after.size;i++){
    testPrintData* actual=&testPrint.after[i];
    int holderBucketSizeDelta=0,holderBucketBucketsDelta=0,opBucketSizeDelta=0,opBucketBucketsDelta=0;
    double promSecDelta=0,promDelta=0,minPromDelta=0;

    for(int j=0;j<testPrint.before.size;j++){
      testPrintData* before=&testPrint.before[j];
      if(strcmp(before->name,actual->name)==0){
        holderBucketSizeDelta=actual->holderBucketSize-before->holderBucketSize;
        holderBucketBucketsDelta=actual->holderBucketBuckets-before->holderBucketBuckets;
        opBucketSizeDelta=actual->opBucketSize-before->opBucketSize;
        opBucketBucketsDelta=actual->opBucketBuckets-before->opBucketBuckets;
        promSecDelta=actual->promSec-before->promSec;
        promDelta=actual->prom-before->prom;
        minPromDelta=actual->minProm-before->minProm;
      }
    }

    auto charSign=[](double val)->char{
                    return val>=0?'+':'-';
                  };

    printf("%s:\nholder bucket %d  %d\t%c%d   %c%d\nop bucket %d  %d\t%c%d   %c%d\npromedio s %f\t%c%f\npromedio %f\t%c%f\nmejor %f\t%c%f  (/%f = %c%f)\n---------------------------\n",
           actual->name,
           actual->holderBucketSize,actual->holderBucketBuckets,
           charSign(holderBucketSizeDelta),std::abs(holderBucketSizeDelta),charSign(holderBucketBucketsDelta),std::abs(holderBucketBucketsDelta),
           actual->opBucketSize,actual->opBucketBuckets,
           charSign(opBucketSizeDelta),std::abs(opBucketSizeDelta),charSign(opBucketBucketsDelta),std::abs(opBucketBucketsDelta),
           actual->promSec,charSign(promSecDelta),std::abs(promSecDelta),
           actual->prom,charSign(promDelta),std::abs(promDelta),
           actual->minProm,charSign(minPromDelta),std::abs(minPromDelta),
           actual->delta,charSign(minPromDelta),std::abs(minPromDelta)/actual->delta);
    if(saveBenchmark){
      fprintf(file,"%s:\nholder bucket %d  %d\t%c%d   %c%d\nop bucket %d  %d\t%c%d   %c%d\npromedio s %f\t%c%f\npromedio %f\t%c%f\nmejor %f\t%c%f  (/%f = %c%f)\n---------------------------\n",
              actual->name,
              actual->holderBucketSize,actual->holderBucketBuckets,
              charSign(holderBucketSizeDelta),std::abs(holderBucketSizeDelta),charSign(holderBucketBucketsDelta),std::abs(holderBucketBucketsDelta),
              actual->opBucketSize,actual->opBucketBuckets,
              charSign(opBucketSizeDelta),std::abs(opBucketSizeDelta),charSign(opBucketBucketsDelta),std::abs(opBucketBucketsDelta),
              actual->promSec,charSign(promSecDelta),std::abs(promSecDelta),
              actual->prom,charSign(promDelta),std::abs(promDelta),
              actual->minProm,charSign(minPromDelta),std::abs(minPromDelta),
              actual->delta,charSign(minPromDelta),std::abs(minPromDelta)/actual->delta);
    }
  }
  if(saveBenchmark){
    fprintf(file,"###########################\n");
    fclose(file);
  }
}

//en la version vieja corria todos los test en forma circular n veces para que haya mas variabilidad, y promediaba
//pero justamente lo que quiero es que haya menos, asi que ahora corro un test las veces que necesite para converger
//podría darse que justo este pasando algo en la computadora que haga variar al test, y como pasan todos juntos me 
//mueva la convergencia. Pero no sé si es algo que pueda llegar a pasar, y ensuciar todo el codigo con manejo de 
//una cola circular donde los elementos se eliminan al azar, pero la impresion final debe ser ordenada, me parece mucho
//quilombo solo por eso. Y ese problema se va a presentar de todas formas al final, cuando la cola tenga pocos elementos

/*probando boludeces template v1
template<typename A,int size,typename B,B A::* m,typename lt>
B foldInArrayMember(A* objArray,lt lambda){
  B acc=(objArray[0].*m);
  for(int i=1;i<size;i++){
    acc=lambda(acc,(objArray[i].*m));
  }
  return acc;
}

template<typename A,int size,typename B,B A::* m, typename lt>
bool forAllPairsInArrayMember(A* objArray,lt lambda){
  assert(size>1);
  for(int i=0;i<size-1;i++){
    for(int j=i+1;j<size;j++){
      if(!lambda((objArray[i].*m),(objArray[j].*m)))
        return false;
    }
  }
  return true;
}
//se llama haciendo
//forAllPairsInArrayMember<testPrintData,runsToConverge,double,&testPrintData::prom>(runData,[](double a,double b)->bool{return std::abs(a-b)<100;})
//after->holderBucketSize=foldInArrayMember<testPrintData,runsToConverge,int,&testPrintData::holderBucketSize>(runData,[](int a,int b)->int{return std::max(a,b);})
//intente hacer que no sea necesario especializar la lambda cuando es generica (usar std::max y std::plus en vez de hacer las lambdas esas)
//usando algo como template<typename,typename> typename lt, pero parece que eso no anda con funciones template? no estoy seguro
//tambien se podría mover la lambda al template y especializar el tipo ahi, pero es de c++20
//tambien quise sacar el size apartir del array, pasandolo por decltype. pero no anda porque necesito el tipo no array para el metodo de acceso,
// y B (std::remove_all_extents<A>::type)::* m no anda
//se podría haber hecho con todas las boludeces, pero sin tipos, pasando un offset por template

template<typename A,typename B,typename lt>
B foldInArrayMember(A objArray,B first,lt lambda){
  B acc=first;
  for(int i=0;i<(int)std::extent<A>::value;i++){
    acc=lambda(acc,objArray[0]);
  }
  return acc;
}

template<typename A,typename lt>
bool forAllPairsInArrayMember(A objArray,lt lambda){
  constexpr int size=std::extent<A>::value;
  static_assert(size>1);
  for(int i=0;i<size-1;i++){
    for(int j=i+1;j<size;j++){
      if(!lambda(objArray[i],objArray[j]))
        return false;
    }
  }
  return true;
}
//la version anterior esta buena porque las lambdas manejan el tipo base y no necesita valor default
//,pero todo el quilombo que hay que hacer para eso no lo vale

    if(run>=runsToConverge-1){
      if(forAllPairsInArrayMember<decltype(runData)>(runData,[](testPrintData& a,testPrintData& b)->bool{
                                                               bool ret=std::abs(a.prom-b.prom)<runsAcceptedDeviation;
                                                               if(!ret)
                                                                 printf("  %f",a.prom-b.prom);
                                                               return ret;
                                                             })){
        testPrintData* after=newElem(&testPrint.after);
        after->name=name;
        after->holderBucketSize=foldInArrayMember<decltype(runData)>(runData,0,[](int a,testPrintData& b)->int{return std::max(a,b.holderBucketSize);});
        after->opBucketSize=foldInArrayMember<decltype(runData)>(runData,0,[](int a,testPrintData& b)->int{return std::max(a,b.opBucketSize);});
        after->prom=foldInArrayMember<decltype(runData)>(runData,0,[](double a,testPrintData& b)->double{return a+b.prom;})/runsToConverge;
        after->promSec=foldInArrayMember<decltype(runData)>(runData,0,[](double a,testPrintData& b)->double{return a+b.promSec;})/runsToConverge;
        after->minProm=foldInArrayMember<decltype(runData)>(runData,1<<30,[](double a,testPrintData& b)->double{return std::min(a,b.minProm);});
        break;
      }
    }
al final no use nada de esto porque termine corriendo hasta conseguir un minimo en vez de correr n veces buscando convergencia
*/


const int conscBiggerReq=100;//200
void runTest(properState* ps,char const* name,int map,int turns,int times,bool player2Random){
  int run=0;
  int conscBigger=0;
  double maxDelta=0;
  testPrintData minimo,actual;

  while(true){
    printf("\nrunning %s #%d",name,run+1);

    testData.sProm=0;
    testData.minProm=DBL_MAX;
    for(int i=0;i<times;i++){
      ps->boardId=map;
      ps->player2=player2Random?2:4;
      properGameInit<true>(ps,i==0&&run==0);
      //podria hacer un copy paste del estado inicial en vez de recrear todo
      //ahora se reutiliza la misma memoria bucket para no tener problemas de variabilidad.
      //en test que necesiten reservar mas bloques, tambien se conservan estos. Para hacer eso
      //se agregó mas cosas a bucket, atras de un macro. 

      testData.nProm=0;
      testData.dProm=0;
      testData.cutEarly=false;
      
      while(testData.dProm<turns&&!testData.cutEarly){
        properUpdate((char*)ps);
      }
      testData.sProm+=testData.nProm/testData.dProm;
      testData.minProm=std::min(testData.minProm,testData.nProm/testData.dProm);
    }

    testPrintData* result=run==0?&minimo:&actual;


    auto holderBucketSizeData=getBucketSizeData(&ps->gameState);
    auto opBucketSizeData=getBucketSizeData(&ps->pieceOps);
    result->holderBucketSize=holderBucketSizeData.usedSize;
    result->holderBucketBuckets=holderBucketSizeData.usedBuckets;
    result->opBucketSize=opBucketSizeData.usedSize;
    result->opBucketBuckets=opBucketSizeData.usedBuckets;
    result->promSec=(testData.sProm/(double)times)/1e9;
    result->prom=testData.sProm/(double)times;
    result->minProm=testData.minProm;

    if(run>0){
      if(actual.minProm>=minimo.minProm){
        double delta=actual.minProm-minimo.minProm;
        printf("  min %f     delt %f",minimo.minProm,delta);
        conscBigger++;
        maxDelta=std::max(maxDelta,delta);
        //la idea es que si los n ultimos fueron mas grandes, tengo el minimo. No pruebo con uno solo porque
        //tengo miedo de que un fluke me haga cortar temprano. Si hay un fluke que hace que consiga un minimo
        //muy bajo, no comun, me puede mover las mediciones. Por eso imprimo las diferencias, si hay un resultado
        //sospechoso lo debería poder comprobar si fue por esto, aunque creo que eso no debería pasar
        if(conscBigger==conscBiggerReq){
          testPrintData* after=newElem(&testPrint.after);
          *after=minimo;
          after->name=name;
          after->delta=maxDelta;
          break;
          //la idea de promediar los deltas es tener un valor de "error esperado" entre corridas iguales, con lo que
          //poder normalizar la diferencia entre corridas distintas. Es una heuristica
        }
      }else{
        printf("  %f < %f   ",actual.minProm,minimo.minProm);
        conscBigger=0;
        maxDelta=0;
        minimo=actual;
      }
    }
    run++;
  }
}

void segmentationHandler(int sig){
  //undefined behavior pero debería andar
  int res=system("cpupower frequency-set --governor powersave");
  if(res!=0)
    printf("check cpu governor\n");
  if(sig==SIGSEGV)
    fail("crash on test\n");
  else{
    printf("stopped\n");
    abort();
  }
}

void doTests(char* mem){
  properInit<true>(mem,0,2,2);
  properState* ps=(properState*)mem;
  srand(time(NULL));

#if debugMode
  for(int i=0;i<20;i++)
    printf("in debug mode!!\n");
  init(&testPrint.after);
#else
  getOldStats();

  std::signal(SIGSEGV,segmentationHandler);
  std::signal(SIGINT,segmentationHandler);
  int res=system("cpupower frequency-set --governor performance");
  failIf(res!=0,"testing needs sudo\n");
#endif

  timespec beg,end;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&beg);

  testPrint.name="cerrando mediciones por el momento";

  runTest(ps,"simple",14,300,160,false);
  runTest(ps,"tiles",21,600,90,false);
  runTest(ps,"growin",20,600,250,false);
  runTest(ps,"puzzle",15,1000,80,false);
  runTest(ps,"germen",19,399,60,false);
  runTest(ps,"desliz",18,300,60,false);
  runTest(ps,"emperadores",22,100,240,false);
  runTest(ps,"caballos",17,150,70,true);
  runTest(ps,"normal",16,80,200,true);
  runTest(ps,"desopt",23,500,80,false);
  runTest(ps,"desopt a manopla",24,500,140,false);
  runTest(ps,"damas",25,30,80,false);
  //si corre por muchos turnos el promedio se hace mas lento porque acumula triggers, y se recorren como lista
  //cuando agrega mas. Esto no es un bug pero podría manejarse por las dudas, ver 

  //runTest(ps,"rebote",23,300,10,false); no anda porque no hay normales no esp por ahora

#if !debugMode
  res=system("cpupower frequency-set --governor powersave");
  if(res!=0)
    printf("check cpu governor\n");
#endif

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);
  double elapsed=(end.tv_sec-beg.tv_sec)+(end.tv_nsec-beg.tv_nsec)/1e9;
  printf("\ntotal time %f\n",elapsed);

#if !debugMode
  saveStats();
#endif
}


void randomTurnTestPlayer(bool bando,properState* ps){
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

    Clicker* clickerToProcess=&clickers[rand()%clickers.size];

    timespec beg,end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&beg);
    executeClicker(clickerToProcess,brd);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);

    double elapsed=(end.tv_sec-beg.tv_sec)*1e9+(end.tv_nsec-beg.tv_nsec);

    testData.dProm++;
    testData.nProm+=elapsed;

    clearClickers();
  }else{
    testData.cutEarly=true;
  }
  //drawScreen(properDraw);
}

