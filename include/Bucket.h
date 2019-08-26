#ifndef BUCKET_H
#define BUCKET_H



struct Buckett{
  char* data;
  char* head;
  int size;
  Buckett* next;
}

Buckett* allocNewBucket(Buckett* b,int size_=bucketSize){
  b->size=size_;
  b->data=b->head=new char[size];//si no compila malloc(size)
  b->next=nullptr;
}

//podria ser un define pero perderia la capacidad de retornar
template<typename T> T* alloc(Buckett* b){
  if(b->head+sizeof(T)>b->data+b->size){
    b->next=allocNewBucket(b->next,b->size);
    b=b->next;
  }
  T* ret=new(b->head) T;//uso new para constructores 
  b->head+=sizeof(T);
  return ret;
}

#define getStruct(type,name,from)               \
  type* name=(type*)from.data;







struct Bucket;
extern int bucketSize;
extern Bucket* actualBucket,** lastBucket;
extern Bucket* bucketPiezas;
//el objetivo del bucket es solo mantener la memoria junta. No se puede iterar elementos
struct Bucket{
    Bucket(){
        head=data=new char[bucketSize];
        assert(data&&"no hay memoria");
        next=nullptr;
        actualBucket=this;
        *lastBucket=this;
    };
    char* data;
    char* head;
    Bucket* next;
    void enoughSize(size_t size){
        if(head+size>data+bucketSize)
            next=new Bucket();
    }
};

template<typename T,typename... Args> T* bucketAdd(Args... a){
    //asume que hay espacio disponible
    std::cout<<"!"<<(intptr_t)(actualBucket->head-actualBucket->data)<<"b/"<<(intptr_t)(bucketSize)<<"b"<<"  "<<(lastBucket==&bucketPiezas?"pieza":"holder")<<std::endl;
    if(actualBucket->head+sizeof(T)>actualBucket->data+bucketSize)
        actualBucket->next=new Bucket();
    T* pointer=(T*)actualBucket->head;
    actualBucket->head+=sizeof(T);
    new(pointer) T(a...);
    return pointer;
}

template<typename T>  struct barraySizeManager{
    size_t size(){return sizeof(T);}
    void setSize(size_t size__){}
};
template<> struct barraySizeManager<void>{//esto no se puede hacer si esta anidado en barray por algun motivo
    size_t size_;
    size_t size(){return size_;}
    void setSize(size_t size__){size_=size__;}
};
template<typename T> struct barray{
    T* begptr;
    T* endptr;
    barraySizeManager<T> elem;

    void reserve(int elems){
        if(actualBucket->head+elem.size()*elems>actualBucket->data+bucketSize)
            actualBucket->next=new Bucket();
        begptr=(T*)actualBucket->head;
        actualBucket->head+=elem.size()*elems;
        endptr=(T*)actualBucket->head;
    }
    void copy(void* data){
        memcpy(begptr,data,size());
    }
    void init(int elems,void* data){
        reserve(elems);
        memcpy(begptr,data,elem.size()*elems);
    }
    int count(){
        return ((char*)endptr-(char*)begptr)/elem.size();
    }
    size_t size(){
        return count()*elem.size();
    }

    void beginConstruct(){
        begptr=(T*)actualBucket->head;
        endptr=(T*)actualBucket;
    }
    void endConstruct(){
        if(actualBucket==(Bucket*)endptr){
            endptr=(T*)actualBucket->head;
        }else{
            std::cout<<"!memory shenanigans";
            size_t sizeOfMemToMove=((Bucket*)endptr)->data+bucketSize-(char*)begptr;
            memcpy(actualBucket->data+sizeOfMemToMove,actualBucket->data,sizeOfMemToMove);
            memcpy(actualBucket->data,begptr,sizeOfMemToMove);
            begptr=(T*)actualBucket->data;
            actualBucket->head+=sizeOfMemToMove;
            endptr=(T*)actualBucket->head;
        }
    }
    T* operator[](int i){
        return (T*)((char*)begptr+elem.size()*i);
    }

    T* begin(){return begptr;}
    T* end(){return endptr;}
};

#endif // BUCKET_H
