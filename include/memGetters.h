#ifndef MEMGETTERS_H
#define MEMGETTERS_H

typedef int*(*getter)(void);

struct normalHolder;

vector<int> memMov;
vector<int> memGlobal;


int* tileReadNT();
int* localAccg();
int* pieceAccg();
int* globalReadNTi();
int* tileReadNTi();
int* localAccgi();
int* pieceAccgi();

int* globalRead();
int* tileRead();
int* localg();
int* pieceg();
int* globalReadi();
int* tileReadi();
int* localgi();
int* piecegi();
int* posXRead();
int* posYRead();


Text textValMem;

#endif // MEMGETTERS_H
