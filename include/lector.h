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
  //en realidad no son el ultimo, son el proximo. Deberia cambiar el nombre
  int lastTangledGroup;

  int movQ;
  bool spawner;

  vector<memLocalt> memLocal;
  int memLocalSizeMax;
  int memPieceSize;
  int memTileSlots;
  int memGlobalSize;
};

int getCodedPieceIndexById(vector<int>*,int);
void generateTokens(parseData*,vector<int>*,char*);
void loadGlobalMacros(parseData*,char*);
void processTokens(parseData*,vector<int>*);
void makePiece(parseData*,int,int,vector<int>*,vector<Piece*>*,bucket*);
template<bool global> void loadMacro(parseData*,char**);
void expandVersions(parseData*,vector<int>*,vector<int>*,int,int,int);
void expandTangledVersions(parseData*,vector<int>*,vector<int>*,int,int,int);

#endif // LECTOR_H
