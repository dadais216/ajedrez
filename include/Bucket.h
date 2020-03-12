#ifndef BUCKET_H
#define BUCKET_H



//estructura de control. Hay una sola al inicio de la cadena de bloques, tiene la información del ultimo bloque para hacer las alocaciones. De necesitar mas de un bloque hay un puntero al siguiente al inicio de cada bloque. 
struct bucket{
  char* data;
  char* head;
  int size;
  char* firstBlock;
} *actualBucket,*bucketPiezas,*bucketHolders,**lastBucket;
int bucketSize=100000000;


void allocNewBucket(bucket* b=actualBucket){
  b->data=new char[b->size];

  char** insertNull=(char**)b->data;
  *insertNull=nullptr;//se podria hacer un nuevo struct con next y despues espacio pero terminaba siendo mas feo el codigo

  b->head=b->data+=sizeof(char*);
}

void bucketInit(bucket* b=actualBucket,int size=bucketSize){
  b->size=size;
  allocNewBucket(b);
  b->firstBlock=b->data;
}

void ensureSpace(size_t size,bucket*b=actualBucket){
  if(b->head+size>b->data+b->size){
    char* nextBlock=(char*)*b->data;
    if(nextBlock==nullptr){
      char** before=(char**)b->data;
      allocNewBucket(b);
      *before=b->data;
    }else{
      b->head=b->data=nextBlock;
    }
  }
}

template<typename T> T* allocNC(bucket* b=actualBucket){
  T* ret=new(b->head) T; //uso new para constructores de sfml y std
  b->head+=sizeof(T);
  return ret;
}

//intente usar esto
//template<typename T,typename... Args> T* allocInitNC(initializer_list<Args...> list,bucket* b=actualBucket){
//pero no termina de funcionar porque intializer_list no se banca tipos distintos
//tampoco puedo usar Args... solo porque necesito inicializar el struct

//despues se me ocurrio que se podría hacer usando Args... y la extension de gcc que llama al constructor
//explicitamente, igual si lo hago con eso y anda estaria haciendome dependiente de gcc


#define allocInitNC(TIPO,VAR,...)                            \
  TIPO* VAR=new(actualBucket->head)TIPO __VA_ARGS__ ;        \
  actualBucket->head+=sizeof(TIPO);  

#define allocInit(TIPO,VAR,...)                 \
  ensureSpace(sizeof(TIPO));                    \
  allocInitNC(TIPO,VAR,__VA_ARGS__);

template<typename T> T* alloc(bucket* b=actualBucket){
  ensureSpace(sizeof(T));
  return allocNC<T>(b);
}

/* si no anda se puede hacer un macro tipo
   new(actualBucket->head)TIPO(LISTA);
   TIPO* base=(TIPO*)actualBucket->head;
   actualBucket->head+=sizeof(TIPO);
*/








void resetBucket(bucket* b){
  b->head=b->data=b->firstBlock;
}
void clearBucket(bucket* b){
  for(char* data=b->firstBlock;
      data;
     ){
    char* before;
    data=(char*)*data;
    delete[] before;
  }
}

#define getStruct(type,name,from)               \
  type* name=(type*)from.data;

template<typename T> struct barray{
  T* beg;
  T* after;
  T* operator[](int i){
    return beg+i;
  }
  T* begin(){return beg;}
  T* end(){return after;}
};


inline template<typename T> int count(barray<T> b){
  return b.after-b.beg;
}
inline template<typename T> int size(barray<T> b){
  return (char*)b.after-(char*)b.beg;
}
template<typename T> void alloc(barray<T>* b,int elems){
  ensureSpace(elems*sizeof(T));
  b->beg=(T*)actualBucket->head;
  actualBucket->head+=sizeof(T)*elems;
  b->after=(T*)actualBucket->head;
}
inline template<typename T> void copy(barray<T> b,void* data){
  memcpy(b.beg,data,size(b));
}

template<typename T> void init(barray<T>* b,int size,void* data){
  alloc(b,elems);
  copy(*b,data);
}


//#define forbarray(type,var,barray) for(type var=barray.beg;var!=barray.after,var++)

//en la version anterior tenia esto mezclado con barrays, como una especializacion de barray<void>. Ahora lo manejo aparte porque me parece mas simple, el otro codigo tenia que hacer cosas feas para generalizar y tampoco que me este aportado algo
struct barrayE{
  char* beg;
  char* after;
  size_t elemSize;
  char* operator[](int i){
    return beg+i*elemSize;
  }

};

int count(barrayE b){
  return (b.after-b.beg)/b.elemSize;
}
int size(barrayE b){
  return b.after-b.beg;
}
void alloc(barrayE* b,int elemSize,int elems){
  b->elemSize=elemSize;
  ensureSpace(elems*elemSize);
  b->beg=actualBucket->head;
  actualBucket->head+=elems;
  b->after=actualBucket->head;
}
void copy(barrayE b,void* data){
  memcpy(b.beg,data,size(b));
}

  /*
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
  */


//bbucket de uso general para cosas temporales
//los bbuckets de tile son distintos, en un momento intente generalizar pero quedaba un asco
template<int size,typename T>
struct bbucket{
  int used;
  T[size] elems;
  bbucket<nextSize,T>* next;
};

template<int size,typename T>
return bbucket<size,T>* allocInitBbucket(){
  ensureSpace(sizeof(*b));
  bbucket<size,T>* ret=(bbucket<size,T>*)actualBucket->head;
  actualBucket->head+=sizeof(*b);
  ret->used=0;
  ret->next=nullptr;
  return ret;
}

template<int size,typename T>
void add(bbucket<size,T>* b,T elem){
  if(b->used==b->size){
    if(b->next==nullptr){
      b->next=allocInitBbucket<size,T>();
    }
    add(b->next,elem);
  }
  b->elems[b->used]=elem;
  b->used++;
}

template<int size,typename T>
void clear(bbucket<size,T>* b){
  b->used=0;
  if(b->next!=nullptr){
    clear(b->next);
  }
}


#endif // BUCKET_H
