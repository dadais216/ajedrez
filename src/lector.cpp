

/*
  en un momento considere hacer todo el parseo en una pasada, osea ir de texto a tener los operadores hechos. Creo que es posible difiriendo algunas cosas como el tamaño de memoria local, y usando buffer temporales por todos lados. Pero sería un asco y probablemente no sea un incremento significativo de velocidad, y esta parte no necesita ser rapida.
  Tambien se podrían hacer cosas raras como mezclar tokens con no tokens, pero lo mismo, agrega complejidad y no aporta nada.

  Una idea tambien es que en la primera pasada en lugar de terminar con la lista de tokens termine con un arbol de operadores sobre tokens, con algunos datos como cantidad de acciones y condiciones. Pero hacer algo asi sería bastante complejo, porque los cortes entre normales no son obvios (se cortan despues de cada movimiento, y en una accion que use una memoria local que se escribio previamente), y detectar que son acciones y que condiciones en operaciones de memoria requiere hacer el analisis, lo que basicamente lleva a hacer todo lo de la segunda pasada a la primera y sería una version distinta de lo mismo.
 */
enum {
      tW,tA,tS,tD,tN,
      tmov,tcapt,tspwn,tpausa,
      tvacio,tpiece,tenemigo,taliado,tself, tpass,tesp,
      tmcmp,tmset,tmadd,tmless,tmmore,tmdist,tmgoto,
      tmsize,tmreset,
      tmlocal,tmglobal,tmpiece,tmtile,tposX,tposY,tposSY,
      tdesliz,texc,tisol,tdesopt,
      tclick,tfail,
      tcolor,tsprt,tnumShow,tassert,
      tmovEnd,tseparator,tend,
      tmacroSeparator,
      tlast
};
//el token de un macro se guarda de last en adelante
//los numeros se guardan en el mismo espacio, se les suma 2048 para diferenciarlos. Como hay numeros negativos, para verificar si un token en un numero se mira que sea mas grande que 1024. Suponiendo que el codigo sea correcto no sería necesario hacer esto, pero en caso de codigo incorrecto si no se hace un numero podría interpretarse como una instruccion valida.


void initParser(parseData* pd){
  init(&pd->tokenToWord,tlast);
  pd->tokenToWord.size=tlast;
#define rel(T)                                  \
  pd->wordToToken[stringForHash(#T)]=t##T;      \
  pd->tokenToWord[t##T]=stringForHash(#T);

#if debugMode
#define relF(T)                                 \
  rel(T);                                       \
  funcToWord[(void(*)(void))&T]=#T;
#else
#define relF(T)  rel(T);
#endif

  rel(sprt);
  rel(numShow);
  rel(color);

  relF(mov);
  relF(pausa);
  relF(spwn);
  relF(capt);
  relF(pass);

  rel(esp);
  relF(vacio);
  relF(enemigo);
  relF(piece);
  relF(aliado);
  relF(self);
  relF(pass);

  rel(desliz);
  rel(exc);
  rel(isol);
  rel(desopt);
  rel(end);

  pd->wordToToken["c"]=tclick;//TODO por que no uso stringForHash aca? igual tendria que volar todo eso
  pd->wordToToken["or"]=tseparator;
  
  relF(mcmp);
  rel(mset);
  rel(madd);
  relF(mless);
  relF(mmore);
  relF(mdist);

  pd->wordToToken["goto"]=tmgoto;
  pd->tokenToWord[tmgoto]="goto";

  rel(msize);
  rel(mreset);

  rel(fail);

  rel(assert);
#undef rel
  pd->wordToToken["=="]=tmcmp;
  pd->wordToToken["="]=tmset;
  pd->wordToToken["+="]=tmadd;
  pd->wordToToken["<"]=tmless;
  pd->wordToToken[">"]=tmmore;
  pd->wordToToken["!="]=tmdist;

#if debugMode
  pd->tokenToWord[tmovEnd]=stringForHash(";");
  pd->tokenToWord[tmacroSeparator]=stringForHash("|");
  pd->tokenToWord[tseparator]=stringForHash("or");
  pd->tokenToWord[tW]=stringForHash("w");
  pd->tokenToWord[tA]=stringForHash("a");
  pd->tokenToWord[tS]=stringForHash("s");
  pd->tokenToWord[tD]=stringForHash("d");
  pd->tokenToWord[tclick]=stringForHash("c");
  pd->tokenToWord[tmlocal]=stringForHash("l");
  pd->tokenToWord[tmpiece]=stringForHash("p");
  pd->tokenToWord[tmglobal]=stringForHash("g");
  pd->tokenToWord[tmtile]=stringForHash("t");
  pd->tokenToWord[tposX]=stringForHash("x");
  pd->tokenToWord[tposY]=stringForHash("y");
#endif
  pd->lastGlobalMacro=tlast;
  pd->lastTangledGroup=0;
  init(&pd->memLocal,4);
  pd->memLocalSizeMax=0;
  pd->memPieceSize=0;
  pd->memGlobalSize=0;
  pd->memTileSlots=0;

  pd->spawner=false;

  init(&pd->boardInit);
  init(&pd->ids);
  init(&pd->macros);
}

char const* tokenToWord(parseData* pd,int tok){
  if(tok>1024)
    return std::to_string(tok-2048).c_str();
  return pd->tokenToWord[tok].word;
}

int stringToInt(char** s){
  int a=0,sgn=1;
  if(**s=='-'){
    sgn=-1;
    (*s)++;
  }
  char* check=*s;
  for(;**s>='0'&&**s<='9';(*s)++){
    a=a*10+**s-'0';
  }
  if(check==*s)
    fail("bad number");
  return a*sgn;
}


int getIndexById(vector<int>* pieces,int id){
  for(int i=0;i<pieces->size;i++){
    if(pieces->data[i]==std::abs(id)){
      return i;
    }
  }
  exit(0);
}

//lo el index signado, pero con
//menos cero se hacen medias raras las cosas. Puede que andase igual
//pero por ahora hice una codificacion mas compleja, que sigue siendo
//mucho mejor que guardar el id y buscar en el vector durante spawn
int getCodedPieceIndexById(vector<int>* pieces,int id){
  return (getIndexById(pieces,id)+1)*sign(id);
}



void addIdIfMissing(parseData* pd,int id){
  for(int j=0;j<pd->ids.size;j++){
    if(pd->ids[j]==id)
      return;
  }
  push(&pd->ids,id);
}

//en un momento pense en guardar punteros al archivo durante la carga de nombres para evitar tener que volver a recorrer el archivo
//decidi no hacerlo porque no se bancaria hotloading. De todas formas cuando use el editor voy a tener todo en varios archivos supongo.
void getBoardIds(parseData* pd,int n){
  char* filePtr=loadFile("tableros.txt");
  defer(filePtr);
  char* s=filePtr;

  char c;
  int i=0;
  while((c=*(s++))){
    if(c=='"'){
      if(i++/2==n){// /2 por las 2 comillas
        while((*s++)!='\n');
        goto foundBoard;
      }
    }
  }
  fail("board not found");
 foundBoard:
  bool firstLine=true;
  int x=0;
  pd->dims=v(0,0);
  while(true){//en esta parte asumo que no hay un eof de golpe porque es algo que mas adelante va a escribir el programa
    while(*s==' ') s++;
    if(*s=='\n'){
      if(firstLine){
        firstLine=false;
        pd->dims.x=x;
      }else{
        assertf(pd->dims.x==x,"dims.x %d, x %d\n",pd->dims.x,x);
      }
      x=0;
      pd->dims.y++;

      s++;
      if(*s==' '||*s=='\n'||*s=='"'||*s=='\0') return;
    }
    int pieceId=stringToInt(&s);
    if(pieceId!=0){
      addIdIfMissing(pd,abs(pieceId));
      push(&pd->boardInit,pieceId);
    }else
      push(&pd->boardInit,0);

    x++;
  }
}

macro getMacro(parseData* pd,int token){
  int ind=token-tlast;
  return pd->macros[ind];
}
//TODO por que no borro la local del arbol aca?
void clearLocalMacros(parseData* pd){
  //si estuvieran todos las expansiones en un vector lo limpiaria y listo, ademas de tener menos accesos al heap. No sé si vale hacerlo ahora igual
  for(int i=pd->lastGlobalMacro+1;i<pd->lastLocalMacro;i++){
    macro m=getMacro(pd,i);
    free(&m.expansion);
  }
  pd->lastLocalMacro=pd->lastGlobalMacro;
  pd->macros.size=pd->lastGlobalMacro-tlast;
}

void clearMacros(parseData* pd){
  for(int i=tlast;i<pd->lastLocalMacro;i++){
    macro m=getMacro(pd,i);
    free(&m.expansion);
    pd->wordToToken.erase(pd->tokenToWord[i]);
  }
  pd->lastLocalMacro=pd->lastGlobalMacro=tlast;
  pd->macros.size=0;
}
//TODO en algun momento podría mirar de limpiar todas las cosas del parser despues de usarlo. Que queden no es un leak pero quedan ahi al pedo igual
//la funcion de arriba solo se usa para test pero debería usarse normalmente 

//consigo todas las pieces juntas para saber los tamaños de las memorias. Esto incluye piezas que no esten en el tablero, aparezcan por spawn
void makePieces(parseData* pd,vector<Piece*>* pieces,bigVector* b){
  char* filePtr=loadFile("piezas.txt");
  defer(filePtr);

  loadGlobalMacros(pd,filePtr);

  vector<int> tokens;init(&tokens);defer(&tokens);//algunos de estos vectores podrían ser globales y reutilizarse
  for(int i=0;i<pd->ids.size;i++){
    char* s=filePtr;
    char c;
    int id,sn;
    while((c=*(s++))){
      if(c=='#'){
        do{s++;}while(*s!='\n'); s++;
      }
      if(c==':'){
        id=stringToInt(&s);
        if(pd->ids[i]==id){
          while(*s==' ') s++;
          sn=stringToInt(&s);
          while((*s++)!='\n');
          goto pieceFound;
        }
      }
    }
    fail("piece not found");
  pieceFound:
    generateTokens(pd,&tokens,s);//tokenizar piece
    processTokens(pd,&tokens);//verificar errores, contar memoria, aplicar macros y expandir llaves
    makePiece(pd,id,sn,&tokens,pieces,b);
    for(int i=pd->lastGlobalMacro;i<pd->lastLocalMacro;i++){
      pd->wordToToken.erase(pd->tokenToWord[i]);
    }
    clearLocalMacros(pd);
    tokens.size=0;
  }
  init(&memMov,pd->memLocalSizeMax);
  memMov.size=pd->memLocalSizeMax;
}

bool whiteSpace(char c){
  return c==' '||c=='\n';
}

bool centinel(char c){
  return c==';'||c=='#';
}


/*antes tenia esto, y lo especializaba para macros y codigo normal. Pero me parece medio al pedo ahora, porque es poco codigo y la especializacion mete
  redundacia en macros, ademas de que estaría bueno poder avanzar s
  template<bool(*cond)(char)>
void generateTokensWhile(vector<int>* tokens,char* s){
  char* b=nullptr;
  while(true){
    if(whiteSpace(*s)||centinel(*s)||cond(*s)){
      if(b!=nullptr){
        tokenWord(tokens,b,s);
        b=nullptr;
      }
      if(cond(*s)){
        break;
      }
      if(centinel(*s)){
        tokenCentinel(tokens,*s);
      }
    }else{
      if(b==nullptr){
        b=s;
      }
    }
    if(*s=='#') do{s++;}while(*s!='\n');
    s++;
  }
  }*/

bool wordIsMov(char* b,char* e){
  for(char* s=b;s!=e;s++){
    if(*s!='w'&&*s!='a'&&*s!='s'&&*s!='d'){
      return false;
    }
  }
  return true;
}

bool wordIsNum(char* b,char* e){
  for(char* s=b;s!=e;s++){
    if(*s>'9'||*s<'0'){
      return false;
    }
  }
  return true;
}

bool wordIsGetter(char* b,char* e){
  for(char* s=b;s!=e;s++){
    if(*s!='g'&&*s!='t'&&*s!='p'&&*s!='l'){
      if(s+1==e){
        if(*s=='x'||*s=='y'||*s=='Y') return true;
      }
      return wordIsNum(s,e);
    }
  }
  return false;
}


void tokenWord(parseData* pd,vector<int>* tokens,char* b,char* e){
  stringForHash sh(b,e);
  //printf("%s ",sh.word);
  if(pd->wordToToken.find(sh)!=pd->wordToToken.end()){
    push(tokens,pd->wordToToken[sh]);
    return;
  }

  if(wordIsMov(b,e)){
    for(char* s=b;s!=e;s++){
      int tok;
      switch(*s){
      case 'w': tok=tW;break;
      case 'a': tok=tA;break;
      case 's': tok=tS;break;
      case 'd': tok=tD;break;
      }
      push(tokens,tok);
    }
    return;
  }
  if(wordIsNum(b,e)){
    push(tokens,stringToInt(&b)+2048);
    return;
  }
  if(wordIsGetter(b,e)){
    for(char* s=b;s!=e;s++){
      int tok;
      switch(*s){
      case 'g': tok=tmglobal;break;
      case 't': tok=tmtile;break;
      case 'p': tok=tmpiece;break;
      case 'l': tok=tmlocal;break;
      case 'x': tok=tposX;break;
      case 'y': tok=tposY;break;
      case 'Y': tok=tposSY;break;//TODO cambiar Y por sy? puede que sea medio molesto tener un getter con 2 letras, ver
      default:
        push(tokens,stringToInt(&s)+2048);
        return;
      }
      push(tokens,tok);
    }
    return;
  }
  if(tokens->data[tokens->size-1]==tmsize &&
     e-b==1){
    switch(*b){
    case 'g': push(tokens,(int)tmglobal);return;
    case 't': push(tokens,(int)tmtile);return;
    case 'p': push(tokens,(int)tmpiece);return;
    case 'l': push(tokens,(int)tmlocal);return;
    }
  }

  *e=0;
  fail("palabra no reconocida: %s",b);
}

void tokenCentinel(vector<int>* tokens,char** sp){
  int tok;
  switch(**sp){
  case ';': tok=tmovEnd;break;
  case '#': do{(*sp)++;}while(**sp!='\n');return;
  }
  push(tokens,tok);
}
void debugPrintTokens(parseData* pd,vector<int>* tokens){
#if debugMode
  printf("\n\n");
  for(int i=0;i<tokens->size;i++){
    printf("%s ",tokenToWord(pd,at(tokens,i)));
  }
  printf("\n\n");
#endif
}

void generateTokens(parseData* pd,vector<int>* tokens,char* s){
  char* b=nullptr;
  while(true){
    if(*s==0||*s==':'){
      failIf(b!=nullptr||tokens->data[tokens->size-1]!=tmovEnd,"missing ;");
      break;
    }
    if(whiteSpace(*s)||centinel(*s)||*s=='>'){
      if(b!=nullptr){
        tokenWord(pd,tokens,b,s);
        b=nullptr;
      }
      if(centinel(*s)){
        tokenCentinel(tokens,&s);
      }
      else if(*s=='>'&& (tokens->size==0||tokens->data[tokens->size-1]==tmovEnd)){
        loadMacro<false>(pd,&s);
        continue;
      }
    }else{
      if(b==nullptr){
        b=s;
      }
    }
    s++;
  }
  //debugPrintTokens(pd,tokens);
}

/*
  definir un macro adentro de un macro (ej. >x = >y,>z) esta prohibido.
  Es una funcionalidad rara, e implementarla sería un poco complicado,
  porque necesitaria volver a la etapa de creacion de macros durante la etapa
  de expansion.
  No veo que valga la pena. Ahora esto se prohibe a traves de que el procesado
  de tokens normal no conoce la idea de macros, y no reconoce > y falla.
  Más adelante, cuando implemente operadores de memoria infijos va a fallar porque
  interpreta > como mayor, y falla ahi. Es una ventaja de usar el mismo simbolo
  para las 2 cosas
*/


void generateTokensForMacros(parseData* pd,vector<macro>* macros,char** sp){
  char* s=*sp;
  //copy paste de generateTokens
  char* b=nullptr;
  int i=0;
  bool moreThanOneExpansion=false;
  while(true){
    failIf(*s==':',"macro missing ;");
    if(whiteSpace(*s)||centinel(*s)||*s=='|'||*s=='&'){
      if(b!=nullptr){
        tokenWord(pd,&at(macros,i).expansion,b,s);
        b=nullptr;
      }
      if(*s==';'){
        failIf(i+1!=macros->size,"mismatching quantity of tangled expansions and tangled macros");
        s++;
        break;
      }else if(*s=='|'){
        moreThanOneExpansion=true;
        failIf(i+1!=macros->size,"mismatching quantity of tangled expansions and tangled macros");
        for(int j=0;j<=i;j++){
          push(&at(macros,j).expansion,(int)tmacroSeparator);
        }
        i=0;
      }else if(*s=='&'){
        i++;
      }else if(centinel(*s)){
        tokenCentinel(&at(macros,i).expansion,&s);
      }
    }else{
      if(b==nullptr){
        b=s;
      }
    }
    s++;
  }
  if(moreThanOneExpansion){
    for(macro& m:*macros){
      m.moreThanOneExpansion=true;
    }
  }
  //debugPrintTokens(pd,&macros->data[0].expansion);
  *sp=s;
}

void init(macro* m){
  init(&m->expansion);
  m->tangledGroup=0;
  m->moreThanOneExpansion=false;
}

template<bool global>
void loadMacro(parseData* pd,char** sp){
  char* s=*sp;
  bool tangled=false;
  vector<stringForHash> names;init(&names);defer(&names);
  vector<macro> macros;init(&macros);defer(&macros);

 loop:
  s++;
  while(*s==' ') s++;
  char* b=s;
  while(*s!=' '&&*s!='&'&&*s!='=') s++;

  //char newWord[255]={};
  //memcpy(newWord,b,s-b);
  //push(&names,stringForHash(newWord));
  push(&names,stringForHash(b,s));

  while(*s==' ') s++;
  if(*s=='&'){
    if(!tangled){//puede haber un ampersand suelto si es la primera variable
      tangled=true;
      char* k=s+1;
      while(*k==' ') k++;
      if(*k!='='){
        goto loop;
      }
      s=k;
    }else{
      goto loop;
    }
  }
  failIf(*s!='=',"macro definition must be > name = expansion; with maybe multiple names linked with &");
  s++;

  for(int i=0;i<names.size;i++){
    if(pd->wordToToken.find(names[i])!=pd->wordToToken.end()){
      if(global){
        fail("global macro %s shadows reserved word or previous global macro",names[i].word);
      }else{
        int tok=pd->wordToToken[names[i]];
        failIf(tok<pd->lastGlobalMacro,"local macro %s shadows reserved word or previous global macro",names[i].word);
        //por ahi estaria bueno shadowear globales TODO
      }
    }
    char* b=names[i].word;
    char* e;
    for(e=names[i].word;*e!=0;e++);//medio garca pero bueno
    if(wordIsMov(b,e)||wordIsNum(b,e)||wordIsGetter(b,e)){
      fail("invalid macro name %s",b);
    }

    macro m;
    init(&m);
    push(&macros,m);
  }
  generateTokensForMacros(pd,&macros,&s);


  int group=tangled?++pd->lastTangledGroup:0;
  for(int i=0;i<macros.size;i++){
    int tok=global?pd->lastGlobalMacro++:pd->lastLocalMacro++;
    macros[i].tangledGroup=group;
    push(&pd->macros,macros[i]);
    pd->wordToToken[names[i]]=tok;//lo agrego ahora para que no se lo reconozca durante la carga de tokens
    //porque no hay macros recursivos

    if(pd->tokenToWord.cap<=tok){//por ahi debería hacer un vector no push para estos casos
      reserve(&pd->tokenToWord,tok*2);
    }
    pd->tokenToWord.size=tok+1;
    pd->tokenToWord[tok]=names[i];//puede que tenga que hacer un memcpy no se
  }

  /*
    en caso de tener
    >X=a;
    >Y=X;
    Y mov;
    >X=b;
    Y mov;
    Y va a usar el X viejo, no el nuevo. Para hacer que use el nuevo debería tener un flag que se active si se esta pisando un local,
    y que despues recorra y actualice macros locales. Se puede hacer y no sería caro. No sé si me convence porque la unica utilidad
    que tiene es poder meter hacer macros parametricos, que arranquen con un X que no signifique nada y despues se acomode a una X especifica.
    No me convence mucho porque no funcionaria con globales (por ahora), y nomas funciona entre macros, y es medio raro por necesitar una X dummy.
    Tenia pensado agregar macros con parametros bien, y si lo hago no tendría sentido hacer esto asi que no lo voy a hacer
  */


  *sp=s;
}

void loadGlobalMacros(parseData* pd,char* s){
  while(true){
    if(*s==':') break;
    if(*s=='#'){
      do{s++;}while(*s!='\n');
      s++;
    }else if(*s=='>'){
      loadMacro<true>(pd,&s);
    }else if(whiteSpace(*s))
      s++;
    else
      fail("piece code outside piece"); //por ahi permitir msize g y t,aunque estos podrían estar en el codigo every turn tambien
  }
  pd->lastLocalMacro=pd->lastGlobalMacro;
}

/*podría probar expandir los macros que contienen macros antes, pero como algunos tienen varias expansiones
tendría que ir subiendo la multiplicidad hasta el primer macro y es medio raro. Al final sería mas rapido igual,
podría probar. TODO?

Ligamiento:
Por ejemplo, tengo
> a&b = 1&2 | 3&4
> c = abbababb
mov c a a b

si expando por macro terminaria con
> a&b = 1&2 | 3&4
> c = 12212122 | 34434344
mov c a a b
que no es lo mismo
Igual creo que andaría si se propaga el ligamiento, quedando c ligado con a y b. 



> a&b = 1&2 | 3&4
> c = hjkf | abba
 a b c b a
Si hay macros no ligados se tienen que expandir primero


se pueden ligar variables despues? tipo
> a = 1 | 3
> b = 2 | 4
> c = a&b | b&a
> d = a b b #no ligados
teoricamente se podría pero es re raro, necesitaria tener que mirar por el simbolo & cuando parseo y solo
tiene sentido si la multiplicidad de las 2 variables es igual.
Ademas necesitaria algun mecanismo para ligar variables temporalmente, porque esas variables solo estarían ligadas
durante esa expresion supongo, y otras b y a's incluso en el mismo movimiento se manejarian normal? Es raro

Supongo que si agrego algo asi va a ser con una funcion especial, algo como
> a = 1 | 3
> b = 2 | 4
> x&y = zip(a,b)
> c = x y | y x
> d = a b b
igual no creo que lo valga, es algo bastante raro. Y no es que no se puede hacer, si se puede copiando y pegando a y b


*/


bool isMacro(int tok){
  return tok>=tlast&&tok<1024;
}

//se hace una pasada donde se expanden los macros simples
//los que sean multiples o ligados se separa el movimiento y despues se agrega al final cada version
void expandMacros(parseData* pd,vector<int>* into,vector<int>* from,int* movStart,char* movType,int* expansionTypeData){
  for(int i=0;i<from->size;i++){
    int tok=from->data[i];
    if(isMacro(tok)){
      macro m=getMacro(pd,tok);
      if(m.moreThanOneExpansion){
        char macroType=m.tangledGroup==0?1:2;
        if(*movType==0){
          *movType=macroType;
          *expansionTypeData=macroType==1?into->size:m.tangledGroup;
        }else if(*movType==2&&macroType==1){//expandir no ligados toma prioridad
          *movType=1;
          *expansionTypeData=into->size;
        }
        push(into, tok);
      }else{
        expandMacros(pd,into,&m.expansion,movStart,movType,expansionTypeData);
      }
    }else{
      assert(tok!=tmacroSeparator);
      push(into,tok);
      if(tok==tmovEnd){ //solo relevante en movimiento, no en macro
        //printf("expand  f: ");
        //debugPrintTokens(pd,from);
        //printf("expand  i: ");
        //debugPrintTokens(pd,into);
        if(*movType==1){
          expandVersions(pd,from,into,*movStart,into->size,*expansionTypeData);
          *movType=0;
          continue;
        }else if(*movType==2){
          expandTangledVersions(pd,from,into,*movStart,into->size,*expansionTypeData);
          *movType=0;
          continue;
        }
        *movStart=into->size;
      }
    }
  }
}

/*se expanden las versiones del ultimo movimiento y se ponen en el final del from, cosa que
se agarren devuelta y se pasen al into, expandiendo mas si se necesita
la version original del movimiento que queda en into se pisa
Tecnicamente no se necesitaria regurgitar si lo que queda no tiene macros o tiene macros simples,
pero no puedo copiar de into a into porque estaria pisando el mismo espacio, por lo que tendría que
usar un buffer intermedio, y from cumple esa funcion
Para macros ligados, que solo pueden aparecer aca porque son multiples tambien,
se expande el primero y si se encuentra otro que este ligado tambien. Se necesita mantener un array de punteros
de a donde se freno en los otros
*/
void expandVersions(parseData* pd,vector<int>* from,vector<int>* into,int movStart,int movEnd,int firstMultiMacro){
  int j=0;
  bool lastLap=false;
  do{
    for(int i=movStart;i<movEnd;i++){ //osea hasta ; inclusive
      int tok=into->data[i];

      if(i==firstMultiMacro){//TODO deberia haber un for antes y despues en vez de un for cortado por un if
        assert(isMacro(tok));
        macro m=getMacro(pd,tok);
        assert(m.moreThanOneExpansion);
        assert(m.tangledGroup==0);
        //printf("macro ");
        //debugPrintTokens(pd,&m.expansion);
        for(;;j++){
          if(j==m.expansion.size){
            lastLap=true;
            break;
          }
          tok=m.expansion[j];
          if(tok==tmacroSeparator){
            j++;
            break;
          }
          push(from,tok);
        }
      }else
        push(from,tok);
    }
    //printf("expandVersions  f: ");
    //debugPrintTokens(pd,from);
    //printf("expandVersions  i: ");
    //debugPrintTokens(pd,into);
  }while(!lastLap);
  into->size=movStart;
}
/*
  macros ligados son distintos que los multiples comunes, incluso si es una sola variable ligada
  > x = a | b
  x x x
  genera 8 versiones, mientras que
  > x& = a | b
  x x x
  genera 2
*/
struct tangledMacroIteration{
  int tok;
  int beg;
  int end;
};
//TODO segun valgrind hay un leak aca pero ni idea
void expandTangledVersions(parseData* pd,vector<int>* from,vector<int>* into,int movStart,int movEnd,int tangledGroup){
  bool lastLap=false;
  vector<tangledMacroIteration> iterations;init(&iterations);defer(&iterations);
  do{
    for(int i=movStart;i<movEnd;i++){
      int tok=into->data[i];

      if(isMacro(tok)){
        macro m=getMacro(pd,tok);
        assert(m.moreThanOneExpansion);
        assert(m.tangledGroup!=0);
        if(m.tangledGroup==tangledGroup){
          tangledMacroIteration* it;
          for(int k=0;k<iterations.size;k++){
            if(iterations[k].tok==tok){
              it=&iterations[k];
              goto found;
            }
          }
          push(&iterations,tangledMacroIteration{tok,0,0});
          it=&iterations[iterations.size-1];
        found:
          for(int j=it->beg;;j++){
            if(j==m.expansion.size){
              lastLap=true;
              break;
            }
            tok=m.expansion[j];
            if(tok==tmacroSeparator){
              it->end=j+1;
              break;
            }
            push(from,tok);
          }
        }else
          push(from,tok);
      }else
        push(from,tok);
    }
    if(lastLap)
      break;
    for(int k=0;k<iterations.size;k++)
      iterations[k].beg=iterations[k].end;
  }while(true);
  into->size=movStart;
}
/*from puede crecer considerablemente si hay muchos macros multiples. Como no es necesario volver a mirar los tokens que ya
 se leyeron, se podrían ir borrando para ahorrar memoria. El ind se debería mantener, tendría que hacer una estructura parecida
al vector que haga estas cosas al momento de expandir*/

void growMemory(parseData* pd,int gType,int val){
  val++;//0 counted
  switch(gType){
  case tmlocal: pd->memLocal[pd->movQ-1].size = std::max(pd->memLocal[pd->movQ-1].size,val);
                pd->memLocalSizeMax=std::max(pd->memLocalSizeMax,val); break;
  case tmpiece: pd->memPieceSize = std::max(pd->memPieceSize,val);break;
  case tmtile:  pd->memTileSlots = std::max(pd->memTileSlots,val);break;
  case tmglobal:pd->memGlobalSize = std::max(pd->memGlobalSize,val);break;
  default: fail("bad getter");
  }
}

void processTokens(parseData* pd,vector<int>* tokens){
  vector<int> tokensExpanded;init(&tokensExpanded,tokens->size*2);
  defer(&tokensExpanded);

  int movStart=0;
  char movType=0;
  int expansionTypeData=0;
  expandMacros(pd,&tokensExpanded,tokens,&movStart,&movType,&expansionTypeData);

  tokens->size=0;
  vector<int>* finalTokens=tokens;

  auto matchNum=[&](int tok,char const* op,bool doPush=true){
                  if(tok<1024)
                    fail("%s requires number",op);
                  if(doPush)
                    push(finalTokens,tok);
                };
  /*auto matchGetter=[&](int tok,int* gettersSeen,int* lastG){
                    switch(tok){
                    case tmlocal:*lastG=tmlocal;break;
                    case tmpiece:*lastG=tmpiece;break;
                    case tmtile:*lastG=tmtile;break;
                    case tmglobal:*lastG=tmglobal;break;
                    case tposX:
                    case tposY:
                      *lastG=0;
                      gettersSeen++;break;
                    default:
                      if(tok<1024)
                        fail("bad getter");
                      *lastG=0;
                      gettersSeen++;
                      if(*lastG!=0)
                        growMemory(pd,*lastG,tok-2048);
                    }
                    push(finalTokens,tok);
                  };*/
  auto boundCheck=[&](int ind,char const* op){
                    if(ind>=tokensExpanded.size)
                      fail("% with no parameters at end of input",op);
                  };

  push(&pd->memLocal,memLocalt{0,-1});
  pd->movQ=1;
  //esto se podría hacer en el scanner, pero hacerlo aca es casi gratis y es mas comodo
  //si se hace en el scanner no se podrían mezclar ciertas cosas con macros, turn y llaves, por ejemplo
  //color ROJO o mcmp MEMORIA 4

  //tambien se podría hacer desde la formacion del operador, pero complicaria
  //el codigo y de todas formas tengo que recorrer una vez para contar la
  //cantidad de movimientos y tamaños de memoria local.
  for(int i=0;i<tokensExpanded.size;i++){
    int tok=tokensExpanded[i];
    switch(tok){//TODO limpiar tambien click repetidos
    case tmovEnd:
      if(finalTokens->size==0||
         finalTokens->data[finalTokens->size-1]==tmovEnd)
        continue;
      else{
        push(&pd->memLocal,memLocalt{0,-1});
        pd->movQ++;
      }break;
    case tmsize:
      boundCheck(i+2,"mSize");
      matchNum(tokensExpanded[i+2],"mSize",false);
      growMemory(pd,tokensExpanded[i+1],tokensExpanded[i+2]-1-2048);//-1 para contrarrestar el +1 de la funcion xd
      i+=2;continue;
    case tmreset:
      boundCheck(i+1,"mReset");
      matchNum(tokensExpanded[i+1],"mReset",false);
      pd->memLocal[pd->movQ-1].resetUntil=tokensExpanded[i+1]-2048;
      i+=1;
      continue;
      /*case spawn:
      boundCheck(i+1,"spawn");
      matchNum(tv[i+1],"spawn");
      i+1;
    case color:
      boundCheck(i+3,"color");
      matchNum(tv[i+1],"color");
      matchNum(tv[i+2],"color");
      matchNum(tv[i+3],"color");
      i+=3;
    case mcmp:
    case mset:
    case madd:
    case mless:
    case mmore:
    case mdist:
      {
        int gettersSeen=0;
        while(gettersSeen<2){
          i++;
          boundCheck(i,"memOp");
          matchGetter(tv[i],&gettersSeen);
        }
      }
      case L:
    case P:
    case T:
    case G:
    case X:
    case Y:
      fail("random getter in input");
    default:
      if(tok>1024)
      fail("random number in input");*/
    case tmlocal:
    case tmpiece:
    case tmtile:
    case tmglobal:
      boundCheck(i+1,"memOp");
      if(tokensExpanded[i+1]>=2048){
        growMemory(pd,tokensExpanded[i],tokensExpanded[i+1]-2048);
      }
    }
    push(finalTokens,tok);
  }

  pd->movQ--;
}

