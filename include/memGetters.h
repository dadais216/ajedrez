#ifndef MEMGETTERS_H
#define MEMGETTERS_H

typedef int*(*getter)(void*);


struct memTriggers{
    vector<normalHolder*> perma,dinam;
};

vector<int> memMov;
vector<memTriggers> memGlobalTriggers;
vector<int> memGlobal;
int maxMemMovSize=0;


Text textValMem;

#endif // MEMGETTERS_H
