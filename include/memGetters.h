#ifndef MEMGETTERS_H
#define MEMGETTERS_H

struct getter{ //getters en acciones
    getter(){}
    virtual int* val()=0;
};
struct getterCond:public getter{ //getters condiciones locales
    getterCond(){}
    virtual int* val()=0;
    virtual int* valFast()=0;
    virtual void drawDebugMem()=0;
};
struct getterCondTrig:public getterCond{ //getters condiciones globales
    getterCondTrig(){}
    virtual int* val()=0;
    virtual bool change()=0;
    virtual int* valFast()=0;//se usa en el change de pieza para evitar cambiar el before cuando se quiere obtener val, de paso ahorra unas instrucciones
    virtual void drawDebugMem()=0;
};

#endif // MEMGETTERS_H
