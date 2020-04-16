

/*
  en un momento considere hacer todo el parseo en una pasada, osea ir de texto a tener los operadores hechos. Creo que es posible difiriendo algunas cosas como el tamaño de memoria local, y usando buffer temporales por todos lados. Pero sería un asco y probablemente no sea un incremento significativo de velocidad, y esta parte no necesita ser rapida.
  Tambien se podrían hacer cosas raras como mezclar tokens con no tokens, pero lo mismo, agrega complejidad y no aporta nada.

  Una idea tambien es que en la primera pasada en lugar de terminar con la lista de tokens termine con un arbol de operadores sobre tokens, con algunos datos como cantidad de acciones y condiciones. Pero hacer algo asi sería bastante complejo, porque los cortes entre normales no son obvios (se cortan despues de cada movimiento, y en una accion que use una memoria local que se escribio previamente), y detectar que son acciones y que condiciones en operaciones de memoria requiere hacer el analisis, lo que basicamente lleva a hacer todo lo de la segunda pasada a la primera y sería una version distinta de lo mismo.
 */
enum {
      tW,tA,tS,tD,tN,
      tmov,tcapt,tspwn,tpausa,
      tvacio,tpiece,tenemigo,tpass,tesp,
      tmcmp,tmset,tmadd,tmless,tmmore,tmdist,tmsize,
      tmlocal,tmglobal,tmpiece,tmtile,tposX,tposY,
      tdesliz,texc,tisol,tdesopt,
      tclick,
      tcolor,tsprt,tnumShow,
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
    pd->wordToToken[stringForHash(#T)]=t##T;    \
    pd->tokenToWord[t##T]=stringForHash(#T);    

  rel(sprt);
  rel(numShow);
  rel(color);

  rel(mov);
  rel(pausa);
  rel(spwn);
  rel(capt);
  rel(pass);

  rel(esp);
  rel(vacio);
  rel(enemigo);
  rel(piece);
  rel(pass);

  rel(desliz);
  rel(exc);
  rel(isol);
  rel(desopt);
  rel(end);

  pd->wordToToken["c"]=tclick;
  pd->wordToToken["or"]=tseparator;
  
  rel(mcmp);
  rel(mset);
  rel(madd);
  rel(mless);
  rel(mmore);
  rel(mdist);
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
#endif
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
  fail("board not found");//no deberia pasar nunca igual
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
        assert(pd->dims.x==x);
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
//consigo todas las pieces juntas para saber los tamaños de las memorias. Esto incluye piezas que no esten en el tablero, aparezcan por spawn
void makePieces(parseData* pd,vector<Piece*>* pieces,bucket* b){
  char* filePtr=loadFile("piezas.txt");
  defer(filePtr);

  loadGlobalMacros(pd,filePtr);

  vector<int> tokens;init(&tokens);
  for(int i=0;i<pd->ids.size;i++){
    char* s=filePtr;
    char c;
    int id,sn;
    while((c=*(s++))){
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
    //guardar piece en algun lado
    for(int i=pd->lastGlobalMacro;i<pd->lastLocalMacro;i++){
      pd->wordToToken.erase(pd->tokenToWord[i]);
    }
    pd->lastLocalMacro=pd->lastGlobalMacro;
    pd->macros.size=pd->lastGlobalMacro-tlast;
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
        if(*s=='x'||*s=='y') return true;
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
      default:
        push(tokens,stringToInt(&s)+2048);
        return;
      }
      push(tokens,tok);
    }
    return;
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
  debugPrintTokens(pd,tokens);
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
  debugPrintTokens(pd,&macros->data[0].expansion);
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
  vector<stringForHash> names;init(&names);
  vector<macro> macros;init(&macros);defer(&macros);

 loop:
  s++;
  while(*s==' ') s++;
  char* b=s;
  while(*s!=' '&&*s!='&'&&*s!='=') s++;

  char newWord[255]={};
  memcpy(newWord,b,s-b);
  push(&names,stringForHash(newWord));

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
  failIf(*s!='=',"macro definition must be > name = , with maybe multiple names linked with &");
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

macro getMacro(parseData* pd,int token){
  int ind=token-tlast;
  return pd->macros[ind];
}

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
      assert(tok!=tmacroSeparator);
      push(into,tok);
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
    for(int i=movStart;i<movEnd;i++){
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
    push(from,(int)tmovEnd);
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
    push(from,(int)tmovEnd);
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
  case tmlocal: pd->memLocalSize[pd->movQ-1] = std::max(pd->memLocalSize[pd->movQ-1],val);
                pd->memLocalSizeMax=std::max(pd->memLocalSizeMax,val); break;
  case tmpiece: pd->memPieceSize = std::max(pd->memPieceSize,val);break;
  case tmtile:  pd->memTileSlots = std::max(pd->memTileSlots,val);break;
  case tmglobal:pd->memGlobalSize = std::max(pd->memGlobalSize,val);break;
  default: fail("bad getter");
  }
}

void processTokens(parseData* pd,vector<int>* tokens){
  vector<int> tokensExpanded;init(&tokensExpanded,tokens->size*2);

  int movStart=0;
  char movType=0;
  int expansionTypeData=0;
  expandMacros(pd,&tokensExpanded,tokens,&movStart,&movType,&expansionTypeData);

  tokens->size=0;
  vector<int>* finalTokens=tokens;

  auto matchNum=[&](int tok,char const* op,bool doPush=true){
                  if(tok<1024)
                    fail("% requires number",op);
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

  push(&pd->memLocalSize,0);
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
        push(&pd->memLocalSize,0);
        pd->movQ++;
      }break;
    case tmsize:
      boundCheck(i+2,"mSize");
      matchNum(tokensExpanded[i+2],"mSize",false);
      growMemory(pd,tokensExpanded[i+1],tokensExpanded[i+2]-2048);
      i+=2;continue;
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

int pop(parseMovData* p){
  return p->tokens[p->ind++];
}

int peek(parseMovData* p){
  return p->tokens[p->ind];
}

void makePiece(parseData* pd,int id,int sn,vector<int>* tokens,
               vector<Piece*>* pieces,bucket* operatorBucket){
  Piece* piece=alloc<Piece>(operatorBucket);
  piece->sn=sn;

  piece->spriteb.setTexture(image.get("piezas.png"));
  piece->spriteb.setTextureRect(IntRect(sn*64%384,(sn*64/384)*32,32,32));
  //piece->spriteb.setScale(escala,escala);
  piece->spriten.setTexture(image . get("piezas.png"));
  piece->spriten.setTextureRect(IntRect(sn*64%384+32,(sn*64/384)*32,32,32));
  //piece->spriten.setScale(escala,escala);

  piece->memPieceSize=pd->memPieceSize;

  alloc(operatorBucket,&piece->movs,pd->movQ);

  piece->hsSize=0;
  parseMovData p{operatorBucket,pd,*tokens,0,0,0,false};
  for(int i=0;i<pd->movQ;i++){
    p.movSize=0;
    p.memLocalSize=pd->memLocalSize[i];
    p.clickExplicit=false;

    piece->movs[i]->memLocalSize=pd->memLocalSize[i];
    piece->movs[i]->raiz=parseOp(&p);
    piece->movs[i]->size=p.movSize;
    piece->hsSize+=p.movSize;
    failIf(pop(&p)!=tmovEnd,"missing ;");
  }
  assert(p.ind==tokens->size);
  piece->spawner=pd->spawner;
  //piece->kamikase=p.kamikase;
  piece->hsSize+=sizeof(Holder)
    +pd->memPieceSize*sizeof(int)
    +pd->movQ*(sizeof(movHolder*)+sizeof(Base))
    +(pd->spawner?(sizeof(movHolder*)+sizeof(Base)+sizeof(spawnerGen)):0);
  piece->ind=pieces->size;
  push(pieces,piece);
}

operador* parseOp(parseMovData* p,bool fromNormal){//=false
  operador* op;
  switch(p->tokens[p->ind]){
  case tdesliz: p->ind++;op=parseDesliz(p);break;
  case texc:    p->ind++;op=parseExc(p);break;
  case tisol:   p->ind++;op=parseIsol(p);break;
  case tdesopt: p->ind++;op=parseDesopt(p);break;
  case tend:
  case tmovEnd:
  case tseparator:       op=nullptr;break;
  default:
    if(fromNormal){
      fail("%s out of place",tokenToWord(p->pd,p->tokens[p->ind]));
    }else{
      op=parseNormal(p);
    }
  }
  return op;
}

//esto esta aca porque no hay lambdas con templates
template<typename T>
void gatherCte(vector<T>* vec,int tok){
  switch(tok){
  case tposX: push(vec,(T)posXRead);break;
  case tposY: push(vec,(T)posYRead);break;
  default:    push(vec,(T)(tok-2048));break;
  }
}

normal* parseNormal(parseMovData* p){
  normal* n=alloc<normal>(p->b);

  vector<void(*)(void)> accsTemp;init(&accsTemp);defer(&accsTemp);
  vector<bool(*)(void)> condsTemp;init(&condsTemp);defer2(&condsTemp);
  vector<colort*> colorsTemp;init(&colorsTemp);defer3(&colorsTemp);

  auto getNum=[&]()->int{
                int tok=pop(p);
                failIf(tok<1024,"%s requires number");
                return tok-2048;
              };

  auto setupBarrays=[&](){
/*cargo listas de punteros despues del resto de memoria de la normal.
Lo pongo al final en vez de al principio para evitar tener que hacer un analisis previo a la carga para saber hasta donde llega la normal y que cosas tiene. Supongo que que este antes o despues tiene el mismo efecto en la cache
TODO probar haciendo un memcpy al final de todo. Si resulta ser mas rapido deberia replantearme el parseo, en la etapa anterior debería anotar cuantas operaciones de que tipo tiene cada normal, lo que implica hacer el procesado de memoria antes, y de paso se podría ya tener el arbol de ops armado. Igual seria un re quilombo, ver. Si resulta que no importa podría probar mover las bases al final tambien, para no tener el hueco de spawner y kamikase.*/
                      allocCopy(p->b,&n->accs,accsTemp.size,accsTemp.data);
                      allocCopy(p->b,&n->conds,condsTemp.size,condsTemp.data);
                      allocCopy(p->b,&n->colors,colorsTemp.size,colorsTemp.data);
                    };

  p->movSize+=sizeof(normalHolder)+p->memLocalSize*sizeof(int);//+ mov de spawner
  n->tipo=NORMAL;
  n->bools&=~(hasClick|makeClick|doEsp);
  n->relPos=v(0,0);
  bool writeInLocalMem=false;
  while(true){
    int tok=pop(p);
    switch(tok){
    case tW:
    case tS:
    case tD:
    case tA:

      //antes nomas cortaba en cuando hubo una cond posicional antes. Ahora lo hago por cualquiera
      //porque sino cosas como mcmp p0 1 w mover ponen un trigger en w aun cuando mcmp es falso
      //por ahi no vale la pena cortar por eso igual.
      if(!condsTemp.size==0||!accsTemp.size==0){
        p->ind--;
        setupBarrays();
        n->sig=parseNormal(p);
        return n;
      }else{
        switch(tok){
        case tW: n->relPos.y--;break;
        case tS: n->relPos.y++;break;
        case tD: n->relPos.x++;break;
        case tA: n->relPos.x--;break;
        }
        n->bools|=doEsp;
        //TODO mirar el tema de esp. Por ahi hacer que se explicite cuando se usa y
        //tener una normal aparte, sacar el if? probar.
        //esa normal prohibiria todos los movimientos posicionales. La construccion podría hacerse
        //como un filtro sobre esta normal para no reescribir
      }
      break;
#define cond(TOKEN) case t##TOKEN:  push(&condsTemp,TOKEN) ;break
      cond(vacio);
      cond(piece);
      cond(enemigo);
      cond(pass);
    case tesp:
      n->bools|=doEsp;
      break;
#define acc(TOKEN) case t##TOKEN: push(&accsTemp,TOKEN);break
      acc(mov);
      acc(pausa);
    case tcapt:
      push(&accsTemp,capt);
      //if(n->relPos==v(0,0))
      //  p->kamikase=true;
      break;
    case tspwn:
      {
        intptr id=getNum();
        push(&accsTemp,spwn);
        push(&accsTemp,(actionBuffer)(id));
        p->pd->spawner=true;
        addIdIfMissing(p->pd,id);
      }
      break;
      //spwn n con n positivo quiere decir mismo bando, negativo bando enemigo
    case tcolor:
      {
        int r=getNum();
        int g=getNum();
        push(&colorsTemp,crearColor(r,g,getNum()));
      }
      break;
      //       colorr(sprt);
      //       colorr(numShow);
    case tmcmp:
    case tmset:
    case tmadd:
    case tmless:
    case tmmore:
    case tmdist:
      {
        //mset l0 4 mset g0 l0 <-en este caso necesito cortar en 2 normales, para que el segundo set tenga registrado el primero
        //esto no se maneja ahora porque prefiero delegar eso al parser cuando lo haga bien. Igual se puede manejar aca tambien, antes lo hacia

        auto isCte=[](int tok)->bool{
                     return tok>1024||
                       tok==tposX||
                       tok==tposY;
                   };


        int op=tok;
        bool write=op==tmset||op==tmadd;
        tok=pop(p);

        if(write&&isCte(tok)){
          fail("write on constant");
        }
        bool action=write&&(tok==tmglobal||tok==tmtile);
        if(action){
          int i=0;
          int nextTok=peek(p);
          if(isCte(nextTok)){
            switch(tok){//manejo sets nomas
            case tmglobal: push(&accsTemp,msetG);break;
            case tmtile:   push(&accsTemp,msetT);break;
            }
            tok=pop(p);
            gatherCte(&accsTemp,tok);
            i++;
          }else{
            switch(tok){
            case tmglobal: push(&accsTemp,msetGi);break;
            case tmtile:   push(&accsTemp,msetTi);break;
            }
          }
          for(;i<2;i++){
            tok=pop(p);
            if(i==1&&isCte(tok)){
              push(&accsTemp,(actionBuffer)cteRead);
              gatherCte(&accsTemp,tok);
              break;
            }
            while(true){
              nextTok=peek(p);
              if(isCte(nextTok)){
                switch(tok){
                case tmglobal: push(&accsTemp,(actionBuffer)globalRead);break;
                case tmtile:   push(&accsTemp,(actionBuffer)tileReadNT);break;
                case tmlocal:  push(&accsTemp,(actionBuffer)localAccg);
                  if(writeInLocalMem) goto splitNormal;break;
                case tmpiece:  push(&accsTemp,(actionBuffer)pieceAccg);break;
                }
                tok=pop(p);
                gatherCte(&accsTemp,tok);
                break;
              }else{
                switch(tok){
                case tmglobal: push(&accsTemp,(actionBuffer)globalReadNTi);break;
                case tmtile:   push(&accsTemp,(actionBuffer)tileReadNTi);break;
                case tmlocal:  push(&accsTemp,(actionBuffer)localAccgi);
                  if(writeInLocalMem) goto splitNormal;break;
                case tmpiece:  push(&accsTemp,(actionBuffer)pieceAccgi);break;
                }
              }
              tok=pop(p);
            }
          }
        }else{
          conditionBuffer cond;
          switch(op){
          case tmset: cond=msetC;break;
          case tmadd: cond=maddC;break;
          case tmdist:cond=mdist;break;
          case tmcmp: cond=mcmp;break;
          default: fail("bad condition");
          }
          push(&condsTemp,cond);
          for(int i=0;i<2;i++){
            if(isCte(tok)){
              push(&condsTemp,(conditionBuffer)cteRead);
              gatherCte(&condsTemp,tok);
              continue;
            }
            while(true){
              int nextTok=peek(p);
              if(isCte(nextTok)){
                switch(tok){
                case tmglobal: push(&condsTemp,(conditionBuffer)globalRead);/*setUpMemTriggersPerNormalHolderTemp.push_back({0,tg[j]});*/break;
                case tmtile:   push(&condsTemp,(conditionBuffer)tileRead);break;
                case tmlocal:  push(&condsTemp,(conditionBuffer)localg);
                  if(i==0&&write){writeInLocalMem=true;}break;
                case tmpiece:  push(&condsTemp,(conditionBuffer)pieceg);break;
                }
                tok=pop(p);
                gatherCte(&condsTemp,tok);
                tok=pop(p);
                break;
              }else{
                switch(tok){
                case tmglobal: push(&condsTemp,(conditionBuffer)globalReadi);break;
                case tmtile:   push(&condsTemp,(conditionBuffer)tileReadi);break;
                case tmlocal:  push(&condsTemp,(conditionBuffer)localgi);break;
                case tmpiece:  push(&condsTemp,(conditionBuffer)piecegi);break;
                }
              }
              tok=pop(p);
            }
          }
        }
        break;
        {
        splitNormal:
          int tok;
          do{
            p->ind--;
            tok=p->tokens[p->ind];
          }while(tok!=tmset&&tok!=tmadd);
          void(*cmd)(void);
          do{
            accsTemp.size--;
            cmd=accsTemp[accsTemp.size-1];
          }while(cmd!=msetG  && cmd!=msetT  &&
                 cmd!=msetGi && cmd!=msetTi);
          setupBarrays();
          n->sig=parseNormal(p);
          return n;
          //medio choto esto porque tengo que reconstruir el comando,
          //por ahi vale la pena dejarlo terminar y copiarlo terminado.
          //el tema es que no hay forma segura de copiarlo, debería
          //parametrizarlo y esta funcion ya es un quilombo y este es un caso
          //oscuro
        }
        break;
      }
    case tclick:
      p->clickExplicit=true;
      n->bools|=hasClick|makeClick;
      setupBarrays();
      n->sig=parseOp(p);
      ///TODO prohibir dos clicks seguidos en el preprocesado
      return n;
    case tmovEnd:
      if(!p->clickExplicit)
        n->bools|=hasClick|makeClick;
    default:
      p->ind--;
      setupBarrays();
      n->sig=parseOp(p,true);
      return n;
      }
    }
}

void peekClick(operador* op,parseMovData* p){
  if(peek(p)==tclick){
    pop(p);
    op->bools|=makeClick;
  }
  if(!p->clickExplicit&&peek(p)==tmovEnd){
    op->bools|=makeClick;
  }
}

desliz* parseDesliz(parseMovData* p){
  desliz* d=alloc<desliz>(p->b);

  d->tipo=DESLIZ;
  d->bools&=~makeClick;

  int iters=0;
  if(peek(p)>2048){
    iters=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;

  d->inside=parseOp(p);

  failIf(pop(p)!=tend,"desliz with no end");

  d->iterSize=p->movSize-movSizeBefore;
  if(iters==0){
    v dims=p->pd->dims;
    d->insideSize=d->iterSize*((dims.x>dims.y?dims.x:dims.y));
  //por default iteraciones necesarias para recorrer el tablero en linea recta
  }else{
    d->insideSize=d->iterSize*iters;
  }
  p->movSize+=sizeof(deslizHolder)-d->iterSize+d->insideSize;
  
  peekClick((operador*)d,p);
  d->sig=parseOp(p);

  if(d->bools&makeClick)
    d->bools|=hasClick;
  else
    for(operador* op=d->inside;op!=nullptr;op=op->sig)
      if(op->bools&hasClick){
        d->bools|=hasClick;
        break;
      }
  return d;
}


exc* parseExc(parseMovData* p){
  exc* e=alloc<exc>(p->b);

  e->tipo=EXC;
  int movSizeBefore=p->movSize;

  int finalTok;
  vector<operador*> opsTemp;init(&opsTemp);defer(&opsTemp);
  do{
    operador* op=parseOp(p);
    push(&opsTemp,op);
    finalTok=pop(p);
  }while(finalTok==tseparator);
  failIf(finalTok!=tend,"exc with no end");

  allocCopy(p->b,&e->ops,opsTemp.size,opsTemp.data);
  p->movSize+=size(e->ops);

  e->insideSize=p->movSize-movSizeBefore;
  p->movSize+=sizeof(excHolder);

  peekClick((operador*)e,p);
  e->sig=parseOp(p);
  if(e->bools&makeClick)
    e->bools|=hasClick;
  else{
    e->bools&=~hasClick;
    for(operador* op:e->ops)
      if(op->bools&hasClick){
        e->bools|=hasClick;
        break;
      }
  }
  return e;
}

isol* parseIsol(parseMovData* p){
  isol* i=alloc<isol>(p->b);

  bool clickExplicitBefore=p->clickExplicit;
  p->clickExplicit=false;

  i->tipo=ISOL;
  i->bools|=hasClick;
  i->bools&=~makeClick;

  int movSizeBefore=p->movSize;

  i->inside=parseOp(p);
  failIf(pop(p)!=tend,"isol with no end");

  p->movSize+=sizeof(isolHolder);
  i->size=p->movSize-movSizeBefore;


  if(!p->clickExplicit)
    i->bools|=makeClick;
  p->clickExplicit=clickExplicitBefore;

  i->sig=parseOp(p);

  return i;
}

desopt* parseDesopt(parseMovData* p){
  desopt* d=alloc<desopt>(p->b);
  d->tipo=DESOPT;

  int slots=0;
  if(peek(p)>2048){
    slots=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;
  vector<operador*> opsTemp;init(&opsTemp);defer(&opsTemp);
  vector<int> sizesTemp;init(&sizesTemp);defer2(&sizesTemp);
  int branches=0;
  do{
    int movSizeBefore=p->movSize;
    operador* op=parseOp(p);
    push(&opsTemp,op);
    push(&sizesTemp,p->movSize-movSizeBefore);
    branches++;
  }while(pop(p)==tseparator);
  failIf(p->tokens[p->ind-1]!=tend,"desopt with no end");

  allocCopy(p->b,&d->ops,opsTemp.size,opsTemp.data);

  for(int i=0;i<sizesTemp.size;i++)
    sizesTemp[i]+=sizeof(desoptHolder::node*);


  allocCopy(p->b,&d->movSizes,branches,sizesTemp.data);

  p->movSize+=branches*sizeof(desoptHolder::node*);
  d->clusterSize=p->movSize-movSizeBefore;
  d->dinamClusterBaseOffset=d->clusterSize+d->clusterSize*branches;

  if(slots==0) slots=branches*6;
  d->desoptInsideSize=d->clusterSize+d->clusterSize*branches+d->clusterSize*slots;

  p->movSize+=sizeof(desoptHolder)+d->desoptInsideSize-d->clusterSize;

  d->bools&=~makeClick;

  peekClick((operador*)d,p);//que significa que desopt sea click?
  d->sig=parseOp(p);
  if(d->bools&makeClick)
    d->bools|=hasClick;
  else{
    d->bools&=~hasClick;
    for(operador* op:d->ops)
      if(op->bools&hasClick){
        d->bools|=hasClick;
        break;
      }
  }
  return d;
}
  /*function<void(operador*)> showOp=[&showOp](operador* op)->void{
        switch(op->tipo){
        case NORMAL:
            cout<<"NORMAL";
            if(((normal*)op)->bools&doEsp)
                cout<<"e";
            cout<<" ";
            break;
        case DESLIZ:
            cout<<"DESLIZ (";
            showOp(static_cast<desliz*>(op)->inside);
            cout<<") ";
            break;
        case EXC:
            cout<<"EXC (";
            for(operador* opi:static_cast<exc*>(op)->ops){
                showOp(opi);
                cout<<" |";
            }
            cout<<"x)";
            break;
        case ISOL:
            cout<<"ISOL (";
            showOp(static_cast<isol*>(op)->inside);
            cout<<") ";
            break;
        case DESOPT:
            cout<<"DESOPT (";
            for(operador* opi:static_cast<desopt*>(op)->ops){
                showOp(opi);
                cout<<" |";
            }
            cout<<"x) ";
        }
        if(op->sig)
            showOp(op->sig);
    };
    cout<<endl;
    for(Piece::base& b:movs){
        showOp(b.raiz);
        cout<<endl;
        }*/
