#ifndef MEMGETTERS_H
#define MEMGETTERS_H

struct getter{ //getters en acciones
    getter(){}
    virtual int* val()=0;
};
struct getterCond:public getter{ //getters condiciones locales
    getterCond(){}
    virtual int* val()=0;
    virtual void drawDebugMem()=0;
};

struct memTriggers{
    vector<normalHolder*> perma,dinam;
};

vector<int> memMov;
vector<memTriggers> memGlobalTriggers;
vector<int> memGlobal;
int maxMemMovSize=0;


Text textValMem;

#endif // MEMGETTERS_H
