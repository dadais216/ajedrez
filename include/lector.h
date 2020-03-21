#ifndef LECTOR_H
#define LECTOR_H

struct macro{
  vector<int> expand;
  int next;//en caso de ser macros ligados forman un buffer circular
};

struct parseData{
  vector<int> ids;
  vector<int> boardInitIds;
  v dims;
  map<string,int> wordsToToken;
  vector<vector<int>> defs;
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





int stringToInt(string&);

#endif // LECTOR_H
