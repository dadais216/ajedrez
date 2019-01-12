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

#endif // MEMGETTERS_H
