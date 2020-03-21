

/*
  en un momento considere hacer todo el parseo en una pasada, osea ir de texto a tener los operadores hechos. Creo que es posible difiriendo algunas cosas como el tamaño de memoria local, y usando buffer temporales por todos lados. Pero sería un asco y probablemente no sea un incremento significativo de velocidad, y esta parte no necesita ser rapida.
  Tambien se podrían hacer cosas raras como mezclar tokens con no tokens, pero lo mismo, agrega complejidad y no aporta nada.

  Una idea tambien es que en la primera pasada en lugar de terminar con la lista de tokens termine con un arbol de operadores sobre tokens, con algunos datos como cantidad de acciones y condiciones. Pero hacer algo asi sería bastante complejo, porque los cortes entre normales no son obvios (se cortan despues de cada movimiento, y en una accion que use una memoria local que se escribio previamente), y detectar que son acciones y que condiciones en operaciones de memoria requiere hacer el analisis, lo que basicamente lleva a hacer todo lo de la segunda pasada a la primera y sería una version distinta de lo mismo.
 */
enum token {def,llaveizq,llaveder,coma,lineJoin,
            W,A,S,D,N,
            mov,capt,spwn,pausa,
            vacio,pieza,enemigo,pass,esp,
            mcmp,mset,madd,mless,mmore,mdist,msize,
            mlocal,mglobal,mpieza,mtile,mother,turno,posX,posY,
            desliz,exc,isol,desopt,
            click,
            color,sprt,numShow,
            eol,sep,end,last
};
//el token de un def se guarda de last en adelante
//los numeros se guardan en el mismo espacio, se les suma 2048 para diferenciarlos. Como hay numeros negativos, para verificar si un token en un numero se mira que sea mas grande que 1024. Suponiendo que el codigo sea correcto no sería necesario hacer esto, pero en caso de codigo incorrecto si no se hace un numero podría interpretarse como una instruccion valida.


int memGlobalSize;
int memTileSize;
list<int> tokens;
void initParser(){
    extra=0;
    #define rel(T) tabla[#T]=T
    rel(def);
    rel(sprt);
    rel(numShow);
    rel(end);

    rel(mov);
    rel(capt);
    rel(pausa);
    rel(spwn);
    rel(capt);
    rel(pass);

    rel(esp);
    rel(vacio);
    rel(enemigo);
    rel(pieza);
    rel(pass);

    rel(desliz);
    rel(exc);
    rel(isol);
    rel(desopt);

    tabla["c"]=click;
    tabla["or"]=sep;

    #undef rel
    #define rel(T) tablaMem[#T]=T
    rel(mcmp);
    rel(mset);
    rel(madd);
    rel(mless);
    rel(mmore);
    rel(mdist);
    #undef rel
    tablaMem["=="]=mcmp;
    tablaMem["="]=mset;
    tablaMem["+="]=madd;
    tablaMem["<"]=mless;
    tablaMem[">"]=mmore;
    tablaMem["!="]=mdist;
    lista=nullptr;
}

char* tokenToWord(int tok){
  
}

int stringToInt(char** s){
  int a=0;sign=1;
  if(**s=='-'){
    sign=-1;
    (*s)++;
  }
  char* check=*s;
  for(;**s>='0'&&**s<='9';(*s)++){
    a=a*10+**s-'0';
  }
  if(check==*s)
    fail("bad number");
  return a*sign;
}

void fail(char* err){
  printf(err);
  exit(0);
}

template<typename T>
void failIfNull(T* s,char* err="null error"){
  if(s==nullptr){
    fail(err);
  }
}


//creo que es mas inteligente cargar todo el archivo y listo
//al principio quería manejarme con funciones de archivo porque tenia la idea de
//que iban a ser mas rapidas, pero supongo que cargar todo el archivo de una y
//hacer el procesado yo va a ser mejor, y mas comodo
char* loadFile(char* fileName){
  FILE* file=fopen(fileName,"r");
  failIfNull(file,"bad file");
  fseek(file,0,2);
  int size=ftell(file);
  char* content=(char*)malloc(size);
  rewind(file);
  content=fgets(content,size,file);
  failIfNull(content,"file empty");
  //TODO mirar ferror y limpiarlo
  fclose(file);
  return content;
  //TODO creo que le tengo que agregar un null manualmente al final
}




//en un momento pense en guardar punteros al archivo durante la carga de nombres para evitar tener que volver a recorrer el archivo
//decidi no hacerlo porque no se bancaria hotloading. De todas formas cuando use el editor voy a tener todo en varios archivos supongo.



void addIdIfMissing(parseData* ps,int id){
  for(int j=0;j<ps->ids.length();j++){
    if(ps->ids[j]==id)
      return;
  }
  push(ps->ids,id);
}


void getBoardIds(parseData* pd,int n){
  char* filePtr=loadFile("tableros.txt");
  defer(free(filePtr));
  char* s=filePtr;

  char c;
  while(c=*(s++)){
    if(c=='"'){
      i++;
      if(i==n){
        while((*s++)!='\n');
        goto foundBoard;
      }
    }
  }
  fail("board not found");//no deberia pasar nunca igual
 foundBoard:
  bool firstLine;
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
      if(*(s+1)=='n'||*(s+1)=='"')
        return;
    }
    int pieceId=stringToInt(&s);
    if(pieceId!=0)
      addIdIfMissing(ps,abs(pieceId));
    pd->boardInit.push_back(pieceId);
    x++;
  }
}

//consigo todas las piezas juntas para saber los tamaños de las memorias. Esto incluye piezas que no esten en el tablero, aparezcan por spawn
void makePieces(parseData* ps, int id){
  char* filePtr=loadFile("piezas.txt");
  defer(free(filePtr));

  loadDefs(filePtr);

  for(int i=0;i<ps->ids;i++){
    char* s=filePtr;
    char c;
    while((c=*(s++))){
      if(c==':'){
        int id_=stringToInt(&s);
        if(id==id_){
          while((*s++)==' ');
          int sn=stringToInt(&s);
          while((*s++)!='\n');
          goto pieceFound;
        }
      }
    }
    fail("piece not found");
  pieceFound:
    vector<int> tokens;
    generateTokensWhile<inPiece>(&scannerTokens,s);//tokenizar pieza
    processTokens(&scannerTokens,&finalTokens);//verificar errores, contar memoria, aplicar macros y expandir llaves
    makePiece(ps,id,sn,&tokens);
    //guardar pieza en algun lado
  }
}

bool whiteSpace(char c){
  return c==' '||c=='\n';
}

bool centinel(char c){
  return c==';'||c=='{'||c=='}'||c==',';
}

bool inDef(char c){
  return c!=';';
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


void generateTokens(vector<int>* tokens,char* s){
  char* b=nullptr;
  while(true){
    if(c==0||c==':'){
      if(b!=nullptr)
        fail("missing ;");
      break;
    }
    if(whiteSpace(*s)||centinel(*s)){
      if(b!=nullptr){
        tokenWord(tokens,b,s);
        b=nullptr;
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
}
void generateTokensForMacros(vector<int>* tokens,char** sp){
  char* s=*sp;
  //copy paste de generateTokens
  char* b=nullptr;
  while(true){
    if(*s==':'){
      fail("macro missing ;");
    }
    if(whiteSpace(*s)||centinel(*s)){
      if(b!=nullptr){
        tokenWord(&tokens,b,s);
        b=nullptr;
      }
      if(*s==';'){
        break;
      }else if(centinel(*s)){
        tokenCentinel(&tokens,*s);
      }
    }else{
      if(b==nullptr){
        b=s;
      }
    }
    if(*s=='#') do{s++;}while(*s!='\n');
    s++;
  }
  *sp=s;
}

void loadGlobalMacros(parseData* ps,char* s){
  while(true){
    if(*s==':') return;
    if(*s=='#'){
      do{s++;}while(*s!='\n');
      s++;
      continue;
    }
    if(*s=='>'){
      while(*s==' ') s++;
      char* b=s;
      while(*s!=' ') s++;
      string newWord(b,s);

      if(ps->wordsToToken.hasKey(newWord)){
        *s=0;
        fail("global macro %s shadows reserved word or previous global macro",b);
      }
      if(isMov(b,s)||isNum(b,s)||isGetter(b,s)){
        *s=0;
        fail("invalid macro name %s",b);
      }
      vector<int> tokens;
      generateTokensForMacros(&tokens,&s);
      push(&ps->gMacro,tokens);

      ps->wordsToToken[newWord]=ps->defIndex++;//lo agrego ahora para que no se lo reconozca durante la carga de tokens
      //porque no hay macros recursivos
    }
    s++;
  }
}

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
    if(*s!='g'&&s!='t'&&s!='p'&&s!='l'){
      if(s+1==e){
        if(*s=='x'||*s=='y') return true;
      }
      return isNum(s,e);
    }
  }
  return false;
}


void tokenWord(parseData* ps,vector<int>* tokens,char* b,char* e){
  string word(b,e);//despues podría hacer mi propio hash para probar
  if(ps->wordsToToken.hasKey(word)){
    push(tokens,ps->wordsToToken[word]);
    return;
  }

  if(wordIsMov(b,e)){
    for(char* s=b;s!=e;s++){
      int tok;
      switch(*s){
      case 'w': tok=W;break;
      case 'a': tok=A;break;
      case 's': tok=S;break;
      case 'd': tok=D;break;
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
      case 'g': tok=G;break;
      case 't': tok=T;break;
      case 'p': tok=P;break;
      case 'l': tok=L;break;
      case 'x': tok=X;break;
      case 'y': tok=Y;break;
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

void tokenCentinel(vector<int>* tokens,char* c){
  int tok;
  switch(c){
  case ',': tok=sep;break;
  case ';': tok=movEnd;break;
  case '{': tok=llaveizq;break;
  case '}': tok=llaveder;break;
  case '#': return;
  }
  push(tokens,tok);
}

//los defs se podrían aplanar de antes y hacer esto menos recursivo
void expandDef(parseData* ps,vector<int>* into,vector<int>* from){
  for(int i=0;i<from->size;i++){
    int tok=from->data[i];
    if(tok>=last&&tok<1024){ //no 2048 porque puede ser un numero negativo
      expandDef(ps,into,&ps->defs[tok-last]);
    }else{
      push(into,tok);
    }
  }
}


bool growMemory(parseData* ps,int gType,int val){
  switch(gType){
  case L: ps->localSize[ps->movQ-1] = max(ps->localSize[ps->movQ-1],val);break;
  case P: ps->pieceSize = max(ps->pieceSize,val);break;
  case T: ps->tileSize = max(ps->tileSize,val);break;
  case G: ps->globalSize = max(ps->globalSize,val);break;
  default: fail("bad getter");
  }
}

void processTokens(parseData* ps,vector<int>* tokens){
  vector<int> tokensWDefs(tokens->size);
  expandDef(ps,&tokensWDefs,tokens);

  //manejar turn, llaves

  tokens->size=0;
  vector<int>* finalTokens=tokens;

  auto matchNum=[&](int tok,char* op,bool doPush=true){
                  if(tok<1024)
                    fail("% requires number",op);
                  if(doPush)
                    push(finalTokens,tok);
                };
  auto matchGetter[&](int tok,int* gettersSeen,int* lastG){
                    switch(tok){
                    case L:*lastG=L;break;
                    case P:*lastG=P;break;
                    case T:*lastG=T;break;
                    case G:*lastG=G;break;
                    case X:
                    case Y:
                      *lastG=0;
                      gettersSeen++;break;
                    default:
                      if(tok<1024)
                        fail("bad getter");
                      *lastG=0;
                      gettersSeen++;
                      if(*lastG!=0)
                        growMemory(ps,*lastG,tok-2048);
                    }
                    push(finalTokens,tok);
                  };
  auto boundCheck=[&](int ind,char* op){
                    if(ind>=tokensWDefs.size)
                      fail("% with no parameters at end of input",op);
                  };

  push(ps->localSize,0);
  ps->movQ=1;
  auto& tv=tokensWDefs;
  //esto se podría hacer en el scanner, pero hacerlo aca es casi gratis y es mas comodo
  //si se hace en el scanner no se podrían mezclar ciertas cosas con macros, turn y llaves, por ejemplo
  //color ROJO o mcmp MEMORIA 4

  //tambien se podría hacer desde la formacion del operador, pero complicaria
  //el codigo y de todas formas tengo que recorrer una vez para contar la
  //cantidad de movimientos y tamaños de memoria local.
  for(int i=0;i<tv.size;i++){
    int tok=tv[i];
    switch(tok){//TODO limpiar tambien comas y click repetidos
    case movEnd:
      if(finalTokens.size==0||
         finalTokens[size-1]==movEnd)
        continue;
      else{
        push(ps->localSize,0);
        movQ++;
      }break;
    case mSize:
      boundCheck(i+2,"mSize");
      matchNum(tv[i+2],"mSize",false);
      growMemory(ps,tv[i+1],tv[i+2]-2048);
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
    case L:
    case P:
    case T:
    case G:
      boundCheck(i+1,"memOp");
      if(tv[i+1]>=2048){
        growMemory(ps,tv[i],tv[i+1]-2048);
      }
    }
    push(finalTokens,tok);
  }

  movQ--;
}



/*
    for(int s:tokens){
        switch(s){
            #define CASE(A) case A: cout<<#A" "; break;
            CASE(W);
            CASE(A);
            CASE(S);
            CASE(D);
            CASE(N);
            CASE(mov);
            CASE(capt);
            CASE(spwn);
            CASE(pausa);
            CASE(vacio);
            CASE(pieza);
            CASE(enemigo);
            CASE(pass);
            CASE(esp);
            CASE(msize);
            CASE(mcmp);
            CASE(mset);
            CASE(madd);
            CASE(mless);
            CASE(mmore);
            CASE(mdist);
            CASE(mlocal);
            CASE(mglobal);
            CASE(mpieza);
            CASE(mtile);
            CASE(mother);
            CASE(turno);
            CASE(posX);
            CASE(posY);
            CASE(desliz);
            CASE(exc);
            CASE(isol);
            CASE(desopt);
            CASE(click);
            CASE(color);
            CASE(sprt);
            CASE(numShow);
            case eol: cout<<"eol"<<endl;break;
            CASE(sep);
            CASE(end);
            default:
                cout<<s-1000<<" ";
        }
    }
    cout<<endl;
*/

void lector::procesarTokens(list<int>& tokens){
    //aplicar def
    for(auto it=tokens.begin(); it!=tokens.end(); it++)///@optim tendria mas sentido cargarlo directamente en vez de en 2 pasos
        for(auto ent:defs)
            if(ent.first==*it){
                it=tokens.erase(it);
                auto aux=it;
                aux--;
                tokens.insert(it,ent.second.begin(),ent.second.end());
                it=aux;
                break;
            }

    //aplicar llaves
    bool loop=true;
    int numeroLinea;//para manejar incersion de memLocalSize en movimientos con llaves
    while(loop){
        loop=false;
        numeroLinea=0;
        list<int>::iterator inicioLinea=tokens.begin();
        for(auto izq=tokens.begin();izq!=tokens.end();++izq){
            if(*izq==eol){
                inicioLinea=izq;
                ++inicioLinea;
                numeroLinea++;
            }
            else if(*izq==llaveizq){
                loop=true;
                int anid=0;
                ++izq;
                for(auto der=izq; der!=tokens.end();++der){
                    if(*der==llaveizq)
                        ++anid;
                    else if(*der==llaveder){
                        if(anid)
                            --anid;
                        else{
                            auto pos=inicioLinea;
                            --pos;
                            ++der;
                            auto finLinea=der;
                            for(;*finLinea!=eol&&finLinea!=tokens.end();++finLinea);
                            if(*finLinea==eol) ++finLinea;
                            list<int> act,pre,post;
                            act.splice(act.begin(),tokens,inicioLinea,finLinea);
                            --izq;
                            pre.splice(pre.begin(),act,act.begin(),izq);
                            post.splice(post.begin(),act,der,act.end());
                            act.pop_back();
                            act.pop_front();
                            auto jt=act.begin(),it=act.begin();
                            ++pos;
                            --it;
                            do{
                                ++it;
                                if(*it==llaveizq)
                                    ++anid;
                                else if(*it==llaveder)
                                    --anid;
                                else if(*it==coma&&anid==0||it==act.end()){
                                    list<int> exp;
                                    exp.assign(pre.begin(),pre.end());
                                    exp.insert(exp.end(),jt,it);
                                    exp.insert(exp.end(),post.begin(),post.end());
                                    jt=it;
                                    ++jt;
                                    tokens.splice(pos,exp,exp.begin(),exp.end());

                                    memLocalSizes.insert(memLocalSizes.begin()+numeroLinea,memLocalSizes[numeroLinea]);
                                }
                            }
                            while(it!=act.end());
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    //aplicar reglas especiales, como limpiar los eol con lineJoins
    for(list<int>::iterator it=tokens.begin();it!=tokens.end();++it){
        //list<int>::iterator jt=it++;
        list<int>::iterator jt=it;
        ++jt;
        if(*it==N&&*jt==esp){
            tokens.erase(jt);
            it=tokens.erase(it);
            ----it;
            continue;
        }
        if(*it==sep&&*jt==end)
            it=tokens.erase(it);
    }
}


void makePiece(parseData* ps,int id,int sn,vector<int>* tokens){
  Pieza piece;
  piece.id=id;
  piece.sn=sn;

  piece.spriteb.setTexture(image.get("piezas.png"));
  piece.spriteb.setTextureRect(IntRect(sn*64%384,(sn*64/384)*32,32,32));
  piece.spriteb.setScale(escala,escala);
  piece.spriten.setTexture(image . get("piezas.png"));
  piece.spriten.setTextureRect(IntRect(sn*64%384+32,(sn*64/384)*32,32,32));
  piece.spriten.setScale(escala,escala);

  actualBucket=bucketPiezas;
  lastBucket=&bucketPiezas;

  piece.memPieceSize=ps->memPieceSize;
  alloc(&piece.movs,ps->movsQ+1);

  parseMovData p(ps,*tokens,tokensConsumed,0,ps->memLocalSize[i],false,false,false);
  //se deja un hueco al principio para spawner y kamikase. Como no tengo forma de saber de antes si es o no es hago esto, dentro de todo no debería tener un costo dejar ese espacio sin usar y parece mejor que alocar las bases despues de los movimientos, que es lo que hacia antes
  for(int i=1;i<ps->movsQ+1;i++){
    p.movSize=0;
    p.memLocalSize=ps->memLocalSize[i];
    p.clickExplicit=false;

    piece.movs[i].memLocalSize=ps->memLocalSize[i];
    piece.movs[i].raiz=parseOp(&p);
    piece.movs[i].size=p.movSize;
    failIf(pop(p)!=movEnd,"missing ;");
  }
  assert(tokensConsumed==tokens->size);

  if(p.spawner||p.kamikase){
    base* b=&piece.movs[0];
    if(p.spawner){
      b->raiz=alloc<spawnerGen>();
      b->raiz->kamikaseNext=p.kamikase;
      b->size=sizeof(spawnerGen);
    }else{
      b->raiz=alloc<kamikaseCntrl>();
      b->size=sizeof(kamikaseCntrl);
    }
  }else{
    piece.movs.beg++;
  }


  push(&piezas,pieza);
}

operador* parseOp(parseMovData* p,bool fromNormal=false){
  operador* op;
  switch(p->tokens[p->ind]){
  case DESLIZ: p->ind++;op=parseDesliz(p);break;
  case EXC:    p->ind++;op=parseExc(p);break;
  case ISOL:   p->ind++;op=parseISol(p);break;
  case DESOPT: p->ind++;op=parseDesopt(p);break;
  case END:
  case ENDMOV:
  case SEPARATOR:       op=nullptr;break;
  default:
    if(fromNormal){
      fail("%s out of place",tokenToWord(p->tokens[p->ind]));
    }else{
      op=parseNormal(p);
    }
  }
  return op;
}

int pop(parseMovData* p){
  return p->tokens[p->ind++];
}

int peek(parseMovData* p){
  return p->tokens[p->ind];
}

operador* parseNormal(parseMovData* p){
  normal* n=alloc<normal>();

  vector<void(*)(void)> accsTemp;
  vector<bool(*)(void)> condsTemp;
  vector<colort*> colorsTemp;

  auto getNum=[&]()->int{
                int tok=pop(p);
                failIf(tok<1024,"%s requires number");
                return tok-2048;
              };

  auto setupBarrays=[&](){
/*cargo listas de punteros despues del resto de memoria de la normal.
Lo pongo al final en vez de al principio para evitar tener que hacer un analisis previo a la carga para saber hasta donde llega la normal y que cosas tiene. Supongo que que este antes o despues tiene el mismo efecto en la cache
TODO probar haciendo un memcpy al final de todo. Si resulta ser mas rapido deberia replantearme el parseo, en la etapa anterior debería anotar cuantas operaciones de que tipo tiene cada normal, lo que implica hacer el procesado de memoria antes, y de paso se podría ya tener el arbol de ops armado. Igual seria un re quilombo, ver. Si resulta que no importa podría probar mover las bases al final tambien, para no tener el hueco de spawner y kamikase.*/
                      init(n->accs,accsTemp.size(),accsTemp.data());
                      init(n->conds,condsTemp.size(),condsTemp.data());
                      init(n->colors,colorsTemp.size(),colorsTemp.data());
                    };

  p->movSize+=sizeof(normalHolder)+p->memLocalSizeAct*sizeof(int);
  n->tipo=NORMAL;
  n->bools&=~(hasClick|makeClick|doEsp);
  n->relPos=v(0,0);
  bool writeInLocalMem=false;
  while(true){
    int tok=pop(p);
    switch(tok){
    case lector::W:
    case lector::S:
    case lector::D:
    case lector::A:

      //antes nomas cortaba en cuando hubo una cond posicional antes. Ahora lo hago por cualquiera
      //porque sino cosas como mcmp p0 1 w mover ponen un trigger en w aun cuando mcmp es falso
      //por ahi no vale la pena cortar por eso igual.
      if(!condsTemp.empty()||!accsTemp.empty()){
        p->ind--;
        setupBarrays();
        n->sig=parseNormal(p);
        return n;
      }else{
        switch(tok){
        case lector::W: n->relPos.y--;break;
        case lector::S: n->relPos.y++;break;
        case lector::D: n->relPos.x++;break;
        case lector::A: n->relPos.x--;break;
        }
      }
      break;
#define cond(TOKEN) case lector::TOKEN:         \
      push(condsTemp,TOKEN) ;break
      cond(vacio);
      cond(pieza);
      cond(enemigo);
      cond(pass);
    case lector::esp:
      bools|=doEsp;
      break;
#define acc(TOKEN) case lector::TOKEN: push(accsTemp,TOKEN);break
      acc(mov);
      acc(pausa);
    case lector::capt:
      push(accsTemp,capt);
      if(n->relPos==v(0,0))
        p->kamikase=true;
      break;
    case lector::spwn:
      {
        int id=getNum();
        push(accsTemp,spwn);
        push(accsTemp,void(*)(void)(id));
        p->spawner=true;
        addIdIfMissing(p->ps,id);
      }
      break;
      //spwn n con n positivo quiere decir mismo bando, negativo bando enemigo
    case lector::color:
      push(colorsTemp,crearColor(getNum(),getNum(),getNum()));
      break;
      //       colorr(sprt);
      //       colorr(numShow);
    case lector::mcmp:
    case lector::mset:
    case lector::madd:
    case lector::mless:
    case lector::mmore:
    case lector::mdist:
      {
        //mset l0 4 mset g0 l0 <-en este caso necesito cortar en 2 normales, para que el segundo set tenga registrado el primero
        //esto no se maneja ahora porque prefiero delegar eso al parser cuando lo haga bien. Igual se puede manejar aca tambien, antes lo hacia

        auto isCte=[](int tok)->bool{
                     return isNum(tok)||
                       tok==lector::posX||
                       tok==lector::posY||
                       tok==lector::turno;
                   };


        int op=tok;
        bool write=op==lector::mset||op==lector::madd;
        tok=pop(p);

        if(write&&isCte(tok)){
          fail("write on constant");
        }
        bool actionOnLocal=false;
        bool action=write&&(tok==lector::global||lector::tile);
        if(action){
          int i=0;
          int nextTok=peek(p);
          if(isCte(nextTok)){
            switch(tok){//manejo sets nomas
            case lector::global: push(accsTemp,msetG);break;
            case lector::tile:   push(accsTemp,msetT);break;
            }
            tok=pop(p);
            gatherCte(accsTemp,tok);
            i++;
          }else{
            switch(tok){
            case lector::global: push(accsTemp,msetGi);break;
            case lector::tile:   push(accsTemp,msetTi);break;
            }
          }
          for(;i<2;i++){
            tok=pop(p);
            if(i==1&&isCte(tok)){
              gatherCte(accsTemp,tok);
              break;
            }
            while(true){
              nextTok=peek(p);
              if(isCte(nextTok)){
                switch(tok){
                case lector::global: push(accsTemp,globalRead);break;
                case lector::tile:   push(accsTemp,tileReadNT);break;
                case lector::local:  push(accsTemp,localAccg);
                  if(writeInLocalMem) goto splitNormal;break;
                case lector::pieza:  push(accsTemp,piezaAccg);break;
                }
                tok=pop(p);
                gatherCte(accsTemp,tok);
                return;
              }else{
                switch(tok){
                case lector::global: push(accsTemp,globalReadNTi);break;
                case lector::tile:   push(accsTemp,tileReadNTi);break;
                case lector::local:  push(accsTemp,localAccgi);
                  if(writeInLocalMem) goto splitNormal;break;
                case lector::pieza:  push(accsTemp,piezaAccgi);break;
                }
              }
              tok=pop(p);
            }
          }
        }else{
          push(condsTemp,op);
          for(int i=0;i<2;i++){
            if(isCte(tok)){
              gatherCte(condsTemp,tok);
              continue;
            }
            while(true){
              int nextTok=tokens.front();
              if(isCte(nextTok)){
                switch(tok){
                case lector::global: push(condsTemp,globalRead);/*setUpMemTriggersPerNormalHolderTemp.push_back({0,tg[j]});*/break;
                case lector::tile:   push(condsTemp,tileRead);break;
                case lector::local:  push(condsTemp,localg);
                  if(i==0&&write){writeInLocalMem=true;}break;
                case lector::pieza:  push(condsTemp,piezag);break;
                }
                tok=pop(p);
                gatherCte(condsTemp,tok);
                break;
              }else{
                switch(tok){
                case lector::global: push(condsTemp,globalReadi);break;
                case lector::tile:   push(condsTemp,tileReadi);break;
                case lector::local:  push(condsTemp,localgi);break;
                case lector::pieza:  push(condsTemp,piezagi);break;
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
          }while(tok!=lector::mset&&tok!=lector::madd);
          void(*cmd)(void);
          do{
            accsTemp.size--;
            cmd=accsTemp[size-1];
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
    case lector::click:
      p->clickExplicit=true;
      n->bools|=hasClick|makeClick;
      setupBarrays();
      n->sig=parseOp(p);
      ///TODO prohibir dos clicks seguidos en el preprocesado
      return n;
    case lector::endMov:
      if(!p->clickExplicit)
        n->bools|=hasClick|makeClick;
    default:
      p->ind--;
      setupBarrays();
      n->sig=parseOp(p);
      return n;
      }
    }
}

void peekClick(operador* op,parseMovData* p){
  if(peek(p)==lector::click){
    pop(p);
    op->bools|=makeClick;
  }
  if(!p->clickExplicit&&peek(p)==lector::endMov){
    op->bools|=makeClick;
  }
}

desliz* parseDesliz(parseMovData* p){
  desliz* d=alloc<desliz>();

  d->tipo=DESLIZ;
  d->bools&=~makeClick;

  int iters=0;
  if(peek(p)>2048){
    iters=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;

  d->inside=parseOp(p);

  failIf(pop(p)!=lector::end,"desliz with no end");

  d->iterSize=p->movSize-movSizeBefore;
  if(iters==0){
    v& tam=tablptr->tam;
    d->insideSize=d->iterSize*((tam.x>tam.y?tam.x:tam.y))*2;
  //por default iteraciones necesarias para recorrer el tablero en linea recta
  }else{
    d->insideSize=d->iterSize*iters;
  }
  p->movSize+=sizeof(deslizHolder)-d->iterSize+d->insideSize;
  
  peekClick((operador*)d,p);
  sig=parseOp(p);

  if(bools&makeClick)
    bools|=hasClick;
  else
    for(operador* op=d->inside;op!=nullptr;op=op->sig)
      if(op->bools&hasClick){
        bools|=hasClick;
        break;
      }
  return d;
}


exc* parseExc(parseMovData* p){
  exc* e=alloc<exc>();

  e->tipo=EXC;
  int movSizeBefore=p->movSize;

  int finalTok;
  vector<operador*> opsTemp;
  do{
    operador* op=parseOp(p);
    push(opsTemp,op);
    finalTok=pop(p);
  }while(finalTok!=lector::separator);
  failIf(finalTok!=lector::end,"exc with no end");

  e->ops.init(opsTemp.size,opsTemp.data);
  p->movSize+=e->ops.size();

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
  isol* i=alloc<isol>();

  bool clickExplicitBefore=p->clickExplicit;
  p->clickExplicit=false;

  i->tipo=ISOL;
  i->bools|=hasClick;
  i->bools&=~makeClick;

  int movSizeBefore=p->movSize;
  i->inside=parseOp(p);
  p->movSize+=sizeof(isolHolder);
  i->size=p->movSize-movSizeBefore;

  if(!p->clickExplicit)
    i->bools|=makeClick;
  p->clickExplicit=clickExplicitBefore;
  sig=keepOn(&i->bools);

  return i;
}

desopt* parseDesopt(parseMovData* p){
  desopt* d=alloc<desopt>();
  d->tipo=DESOPT;

  int slots=0;
  if(peek(p)>2048){
    slots=pop(p)-2048;
  }

  int movSizeBefore=p->movSize;
  vector<operador*> opsTemp;defer(free(opsTemp));
  vector<int> sizesTemp;defer(free(sizesTemp));
  int branches=0;
  do{
    int movSizeBefore=p->movSize;
    operador* op=parseOp(p);
    push(opsTemp,op);
    push(sizesTemp,p->movSize-movSizeBefore);
    branches++;
  }while(pop(p)==lector::separator);
  failIf(p->tokens[p->ind-1]!=lector::end,"desopt with no end");

  ops.init(opsTemp.size(),opsTemp.data());

  for(int i=0;i<sizeTemp.size;i++)
    sizeTemp[i]+=sizeof(desoptHolder::node*);

  p->movSize+=branches*sizeof(desoptHolder::node*);

  d->movSizes.init(branches,sizesTemp.data());

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
    for(Pieza::base& b:movs){
        showOp(b.raiz);
        cout<<endl;
        }*/
