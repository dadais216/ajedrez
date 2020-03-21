#ifndef LECTOR_H
#define LECTOR_H

/*
  cada macro se puede expandir en varias versiones, y puede estar ligado a otros macros que se expanden coordinadamente.
  Estas 2 funcionalidades son utiles en macros locales, pero en globales es medio niche, normalmente con usar un macro que
  se expanda una vez basta. Pero si se lo dejo reutilizo el mismo codigo en todos lados y dejo esa funcionalidad disponible,
  de todas formas no es costoso
*/
struct macro{
  vector<int> expansion;//si tiene mas de una expansion esta concatenada, separada por un ;
  int next;//en caso de ser macros ligados forman un buffer circular
  bool moreThanOneExpansion;//si es 1 se expande ahi nomas
};

struct parseData{
  vector<int> ids;
  vector<int> boardInitIds;
  v dims;
  map<string,int> wordsToToken;
  vector<macro> globalMacro;
  vector<macro> localMacro;
  int lastGlobalMacro;//arranca en el mayor token + 1
  int lastLocalMacro;//arranca en lastGlobalMacro + 1
};

struct parseMovData{
  parseData* ps;
  vector<int> tokens;
  int ind;
  int size;
  int memLocalSize;
  bool clickExplicit;
  bool kamikase;
  bool spawner;
};







#endif // LECTOR_H
