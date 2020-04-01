#ifndef BUCKET_H
#define BUCKET_H



//estructura de control. Hay una sola al inicio de la cadena de bloques, tiene la información del ultimo bloque para hacer las alocaciones. De necesitar mas de un bloque hay un puntero al siguiente al inicio de cada bloque. 
struct bucket{
  char* data;
  char* head;
  int size;
  char* firstBlock;
};
int bucketSize=100000000;


void allocNewBucket(bucket* b){
  b->data=new char[b->size];

  char** insertNull=(char**)b->data;
  *insertNull=nullptr;//se podria hacer un nuevo struct con next y despues espacio pero terminaba siendo mas feo el codigo

  b->head=b->data+=sizeof(char*);
}

void initBucket(bucket* b,int size=bucketSize){
  b->size=size;
  allocNewBucket(b);
  b->firstBlock=b->data;
}

void ensureSpace(bucket* b,size_t size){
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

template<typename T> T* allocNC(bucket* b){
  T* ret=new(b->head) T; //uso new para constructores de sfml y std
  b->head+=sizeof(T);
  return ret;
}

//intente usar esto
//template<typename T,typename... Args> T* allocInitNC(initializer_list<Args...> list,bucket* b){
//pero no termina de funcionar porque intializer_list no se banca tipos distintos
//tampoco puedo usar Args... solo porque necesito inicializar el struct

//despues se me ocurrio que se podría hacer usando Args... y la extension de gcc que llama al constructor
//explicitamente, igual si lo hago con eso y anda estaria haciendome dependiente de gcc


#define allocInitNC(BUCKET,TIPO,VAR,...)          \
  TIPO* VAR=new(BUCKET->head)TIPO __VA_ARGS__ ;        \
  BUCKET->head+=sizeof(TIPO);  

#define allocInit(BUCKET,TIPO,VAR,...)            \
  ensureSpace(BUCKET,sizeof(TIPO));                    \
  allocInitNC(BUCKET,TIPO,VAR,__VA_ARGS__);

template<typename T> T* alloc(bucket* b){
  ensureSpace(b,sizeof(T));
  return allocNC<T>(b);
}

/* si no anda se puede hacer un macro tipo
   new(b->head)TIPO(LISTA);
   TIPO* base=(TIPO*)b->head;
   b->head+=sizeof(TIPO);
*/








void resetBucket(bucket* b){
  b->head=b->data=b->firstBlock;
}
void clearBucket(bucket* b){
  for(char* data=b->firstBlock;
      data;
     ){
    char* before=data;
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


template<typename T> int count(barray<T> ba){
  return ba.after-ba.beg;
}
template<typename T> int size(barray<T> ba){
  return (char*)ba.after-(char*)ba.beg;
}
template<typename T> void alloc(bucket* b,barray<T>* ba,int elems){
  ensureSpace(b,elems*sizeof(T));
  ba->beg=(T*)b->head;
  b->head+=sizeof(T)*elems;
  ba->after=(T*)b->head;
}
template<typename T,typename Y> void copy(barray<T> ba,Y* data){
  memcpy(ba.beg,data,size(ba));
}

template<typename T> void allocCopy(bucket* b,barray<T>* ba,int size,void* data){
  alloc(b,ba,size);
  copy(*ba,data);
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

int count(barrayE ba){
  return (ba.after-ba.beg)/ba.elemSize;
}
int size(barrayE ba){
  return ba.after-ba.beg;
}
void alloc(bucket* b,barrayE* ba,int elemSize,int elems){
  ba->elemSize=elemSize;
  ensureSpace(b,elems*elemSize);
  ba->beg=b->head;
  b->head+=elems;
  ba->after=b->head;
}
void copy(barrayE ba,void* data){
  memcpy(ba.beg,data,size(ba));
}

  /*
    void beginConstruct(){
        begptr=(T*)b->head;
        endptr=(T*)b;
    }
    void endConstruct(){
        if(b==(Bucket*)endptr){
            endptr=(T*)b->head;
        }else{
            std::cout<<"!memory shenanigans";
            size_t sizeOfMemToMove=((Bucket*)endptr)->data+bucketSize-(char*)begptr;
            memcpy(b->data+sizeOfMemToMove,b->data,sizeOfMemToMove);
            memcpy(b->data,begptr,sizeOfMemToMove);
            begptr=(T*)b->data;
            b->head+=sizeOfMemToMove;
            endptr=(T*)b->head;
        }
    }
  */


//bbucket de uso general para cosas temporales
//los bbuckets de tile son distintos, en un momento intente generalizar pero quedaba un asco
template<int size,typename T>
struct bbucket{
  int used;
  T elems[size];
  bbucket<size,T>* next;
};

template<int size,typename T>
bbucket<size,T>* allocInitBbucket(bucket* b){
  ensureSpace(b,sizeof(bbucket<size,T>));
  bbucket<size,T>* ret=(bbucket<size,T>*)b->head;
  b->head+=sizeof(bbucket<size,T>);
  ret->used=0;
  ret->next=nullptr;
  return ret;
}

template<int size,typename T>
void add(bucket* b,bbucket<size,T>* ba,T elem){
  if(ba->used==ba->size){
    if(ba->next==nullptr){
      ba->next=allocInitBbucket<size,T>(b);
    }
    add(b,ba->next,elem);
  }
  ba->elems[ba->used]=elem;
  ba->used++;
}

template<int size,typename T>
void clear(bbucket<size,T>* ba){
  ba->used=0;
  if(ba->next!=nullptr){
    clear(ba->next);
  }
}


template<typename T>
struct vector{
  int size;
  int cap;
  T* data;
  T& operator[](int i){//retorno ref para que no haga la copia, lo que es importante si tomo la direccion
    assert(i>=0&&i<size);
    return data[i];
  }
  T* begin(){return data;}
  T* end(){return &data[size];}
};

template<typename T>
void init(vector<T>* v,int size=16){
  v->size=0;
  v->cap=size;
  v->data=new T[v->cap];
}

template<typename T>
void initCopy(vector<T>* v,T* from,int size){
  v->cap=v->size=size;
  v->data=new T[size];
  memcpy(v->data,from,size*sizeof(T));
}

template<typename T>
void privateGrow(vector<T>* vec,int cap){
  vec->cap=cap;
  T* oldData=vec->data;
  vec->data=new T[cap];
  memcpy(vec->data,oldData,vec->size*sizeof(T));
  delete[] oldData;
}

template<typename T>
void push(vector<T>* vec,T obj){
  if(vec->size==vec->cap){
    privateGrow(vec,vec->cap*2);//TODO probar 1,5
  }
  vec->data[vec->size]=obj;
  vec->size++;
}

template<typename T>
T pop(vector<T>* vec){
  vec->size--;
  return vec->data[vec->size+1];
}

template<typename T>
void reserve(vector<T>* vec,int res){
  if(res>vec->cap){
    privateGrow(vec,res);
  }
}

template<typename T>
void free(vector<T>* vec){
  free(vec->data);
}

template<typename T>
void copy(void* copyTo,vector<T>* copyFrom){
  memcpy(copyTo,copyFrom->data,copyFrom->size*sizeof(T));
}

template<typename T>
void unorderedErase(vector<T>* vec,int ind){
  vec->data[ind]=vec->data[vec->size-1];
  vec->size--;
}


/*
pense en tener un forEach pero la indentacion queda media fea y como usa macros no puedo usar comas, medio choto

void forEachFunc(vector<auto>* vec,auto lambda){
  for(int i=0;i<vec.size;i++){
    lambda(vec->data[i]);
  }
}

void forEachFunc(vector<auto> vec,auto lambda){
  forEachFunc(&vec,lambda);
}

#define forEach(v,f) forEachFunc(v,[&](){f;})
*/


















#endif // BUCKET_H
