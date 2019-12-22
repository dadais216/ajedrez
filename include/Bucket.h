#ifndef BUCKET_H
#define BUCKET_H



//estructura de control. Hay una sola al inicio de la cadena de bloques, tiene la información del ultimo bloque para hacer las alocaciones. De necesitar mas de un bloque hay un puntero al siguiente al inicio de cada bloque. 
struct bucket{
  char* data;
  char* head;
  int size;
  char* firstBlock;
}

void bucketInit(bucket* b,int size=bucketSize){
  b->size=size;
  allocNewBucket(b);
  b->firstBlock=b->data;
}

void allocNewBucket(bucket* b){
  b->data=new char[size];//si no compila malloc(size)
  ((char*)*b->data)=nullptr;//se podria hacer un nuevo struct con next y despues espacio pero terminaba siendo mas feo el codigo
  b->head=b->data+=sizeof(char*);
}
//podria ser un define pero perderia la capacidad de retornar
template<typename T> T* alloc(bucket* b){
  if(b->head+sizeof(T)>b->data+b->size){
    char* nextBlock=(char*)*b->data;
    if(nextBlock==nullptr){
      char* before=b->data;
      allocNewBucket(b,b->size);
      ((char*)*before->data)=b->data;
    }else{
      b->head=b->data=nextBlock;
    }
  }
  T* ret=new(b->head) T;//uso new para constructores de sfml y std
  b->head+=sizeof(T);
  return ret;
}

void resetBucket(bucket* b){
  b->head=b->data=b->firstBlock;
}
void clearBucket(bucket* b){
  for(char* data=b->firstBlock;
      data;
     ){
    char* before;
    data=(char*)*data;
    delete before;//no tengo claro si esto esta borrando 1 char o todo el bloque
  }
}

#define getStruct(type,name,from)               \
  type* name=(type*)from.data;

struct barray{
  char* beg;
  char* after;
}







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
