#ifndef BUCKET_H
#define BUCKET_H
#include <assert.h>
#include <iostream>
#include <stdint.h>

extern int bucketSize;
//el objetivo del bucket es solo mantener la memoria junta. No se puede iterar elementos
struct Bucket{
    Bucket();
    char* data;
    char* head;
    Bucket* next;
};
extern Bucket* actualBucket,** lastBucket;

template<typename T,typename... Args> T* bucketAdd(Args... a){
    //asume que hay espacio disponible
    std::cout<<"!"<<(intptr_t)(actualBucket->head-actualBucket->data)<<"b/"<<(intptr_t)(bucketSize)<<"b"<<std::endl;
    if(actualBucket->head+sizeof(T)>actualBucket->data+bucketSize)
        actualBucket->next=new Bucket();
    T* pointer=(T*)actualBucket->head;
    actualBucket->head+=sizeof(T);
    new(pointer) T(a...);
    return pointer;
}

template<typename T> struct barray{
    T* begptr;
    T* endptr;
    void init(int elems){
        if(actualBucket->head+sizeof(T)*elems>actualBucket->data+bucketSize)
            actualBucket->next=new Bucket();
        begptr=(T*)actualBucket->head;
        actualBucket->head+=sizeof(T)*elems;
        endptr=(T*)actualBucket->head;
    }
    void init(int elems,void* data){
        init(elems);
        memcpy(begptr,data,sizeof(T)*elems);
    }
    T* begin(){return begptr;}
    T* end(){return endptr;}
};

#endif // BUCKET_H
