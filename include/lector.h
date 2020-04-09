#ifndef LECTOR_H
#define LECTOR_H

//hago este objeto porque c++ no me deja asignarle un char[256] a map directamente
//osea me deja crear la estructura pero despues no tengo forma de asignarle nada
struct stringForHash{//resulta que map usa un arbol en vez de un hash asi que este nombre no es muy acertado
  char word[256];
  stringForHash(char const* lit){
    int i=0;
    for(;i<256;i++){
      word[i]=lit[i];
      if(lit[i]==0)
        break;
    }
    for(;i<256;i++){
      word[i]=0;
    }
    //memset(word+i,0,256-i);
  }
  stringForHash(char* b,char* e){
    int i=0;
    for(;i<256;i++){
      word[i]=*(b++);
      if(b==e+1)
        break;
    }
    for(;i<256;i++){
      word[i]=0;
    }
    //memcpy(word,b,e-b);
    //memset(word+i,0,256-i);
  }
  stringForHash(){}
  friend bool operator<(const stringForHash& a,const stringForHash& b){//lo necesita map
    int i;
    for(i=0;a.word[i]==b.word[i]&&a.word[i]!=0;i++);
    return a.word[i]<b.word[i];
  }
};
/*
  cada macro se puede expandir en varias versiones, y puede estar ligado a otros macros que se expanden coordinadamente.
  Estas 2 funcionalidades son utiles en macros locales, pero en globales es medio niche, normalmente con usar un macro que
  se expanda una vez basta. Pero si se lo dejo reutilizo el mismo codigo en todos lados y dejo esa funcionalidad disponible,
  de todas formas no es costoso
*/
struct macro{
  vector<int> expansion;//si tiene mas de una expansion esta concatenada, separada por un ;
  int tangledGroup;//0 si no esta ligado. Antes tenia un buffer circular pero los grupos son mejor
  bool moreThanOneExpansion;//si es 1 se expande ahi nomas
};


struct parseData{
  vector<int> ids;
  v dims;
  vector<int> boardInit;
  std::map<stringForHash,int> wordToToken;
  vector<stringForHash> tokenToWord;
  vector<macro> macros;
  int lastGlobalMacro;//arranca en el mayor token + 1
  int lastLocalMacro;//arranca en lastGlobalMacro + 1
  int lastTangledGroup;

  int movQ;

  vector<int> memLocalSize;
  int memPieceSize;
  int memTileSlots;
  int memGlobalSize;
};

struct parseMovData{
  bucket* b;
  parseData* pd;
  vector<int> tokens;
  int ind;
  int movSize;
  int memLocalSize;
  bool clickExplicit;
  bool kamikase;
  bool spawner;
};


///el motivo de separar los movimientos normales en el operador normal y el normalholder es que toda la informacion
///normal (estructura de movimientos, posiciones relativas) se comparte entre todas las piezas del mismo tipo, como tambien
///el arbol de operadores.

struct operador{
  int tipo;
  operador* sig;
  int32_t bools;//makeClick, hasClick, doEsp en normal
};
struct normal:public operador{
  barray<void(*)(void)> accs;
  barray<bool(*)(void)> conds;
  barray<colort*> colors;
  //struct setupTrigInfo{
  //    char type; //0 global 1 pieza 2 turno
  //    int ind;
  //};
  //barray<setupTrigInfo> setUpMemTriggersPerNormalHolder;//para que se pongan triggers permanentes de memoria que apunten a cada normalholder correspondiente
  v relPos;
};
struct desliz:public operador{
  operador* inside;
  size_t insideSize;
  size_t iterSize;
};
struct exc:public operador{
  barray<operador*> ops;
  int insideSize;//tamaño de movHolders + lo que ocupe ops
};
struct isol:public operador{
  int size;
  operador* inside;
};
struct desopt:public operador{
  barray<operador*> ops;
  barray<int> movSizes;//tamaño de cada movimiento, incluyendo puntero al proximo cluster
  int desoptInsideSize;
  int clusterSize;
  int dinamClusterBaseOffset;
};


/*
Por ahora los operadores se guardan en buckets grandes porque no sé cuanto miden antes de crearlos.
Mas adelante podría probar meterlos en una memoria sola que crezca tipo vector, lo que tiene la ventaja
de que puedo usar ints en vez de punteros y no se tiene que mirar en 2 memorias si el operador esta justo
en el borde del bucket. No sé si va a ser un cambio importante, pero probar TODO
Tecnicamente podría saber cuanto mide antes de crearlos usando el parser, una vez que tengo los tokens finales
puedo sumar sizeof puntero por cada token, y sizeof movholder por cada token de movholder. Puede que sobreestime
el tamaño un poco, pero no importa. El tema es que tengo que tener los tamaños de todas las piezas, por lo que
tengo que hacer todas las tokenizaciones primero y las construcciones despues. Durante el tiempo de tokenizacion
deberia mirar los spawn para traer otras piezas. Se puede hacer, no sé si valdra la pena. Al final del dia tiene
las mismas ventajas que usar un vector, pero sin la necesidad de agrandar durante la generacion, por lo que
podría ser un poco más rapido durante el tiempo de creacion de piezas. No es muy importante, tener en cuenta que
esto solo pasa cuando se crear las piezas la primera vez.
*/

#define actionBuffer void(*)(void)
#define conditionBuffer bool(*)(void)

int getCodedPieceIndexById(vector<int>*,int);
void generateTokens(parseData*,vector<int>*,char*);
void loadGlobalMacros(parseData*,char*);
void processTokens(parseData*,vector<int>*);
void makePiece(parseData*,int,int,vector<int>*,vector<Piece*>*,bucket*);
template<bool global> void loadMacro(parseData*,char**);
void expandVersions(parseData*,vector<int>*,vector<int>*,int,int,int);
void expandTangledVersions(parseData*,vector<int>*,vector<int>*,int,int,int);
operador* parseOp(parseMovData*,bool fromNormal=false);
normal* parseNormal(parseMovData*);
desliz* parseDesliz(parseMovData*);
exc* parseExc(parseMovData*);
isol* parseIsol(parseMovData*);
desopt* parseDesopt(parseMovData*);

#endif // LECTOR_H
