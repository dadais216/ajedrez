#ifndef BUCKET_H
#define BUCKET_H


//estructura de control. Hay una sola al inicio de la cadena de bloques, tiene la información del ultimo bloque para hacer las alocaciones. De necesitar mas de un bloque hay un puntero al siguiente al inicio de cada bloque.
struct bucket{
  struct block{
    block* next;
    char data[0];
  };
  char* data;
  char* head;
  int size;
  block* firstBlock;
  block* actualBlock(){
    return (block*)(data-sizeof(block));
  }
};
int bucketSize=1<<16;//20

void allocNewBucketBlock(bucket* b){
  bucket::block* nextBlock=(bucket::block*)::operator new(b->size+sizeof(bucket::block));
  nextBlock->next=nullptr;
  b->head=b->data=nextBlock->data;
#if debugMode || testing  //para darse cuenta mas facil de memoria sin setear, y para tocar todos los espacios y tenerlos reservados cuando se hacen test
  memset(b->data,-1,b->size);
#endif
}

void initBucket(bucket* b,int size=bucketSize){
  b->size=size;
  allocNewBucketBlock(b);
  b->firstBlock=b->actualBlock();
}

void reserve(bucket* b,int size){
  assert(size<b->size);
  if(b->head+size>b->data+b->size){
    //printf("ajoi\n");
    bucket::block* actualBlock=b->actualBlock();
#if testing
    if(actualBlock->next){
      b->head=b->data=actualBlock->next->data;
      return;
    }
#endif
    assert(actualBlock->next==nullptr);
    allocNewBucketBlock(b);
    actualBlock->next=b->actualBlock();
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
  reserve(BUCKET,sizeof(TIPO));                    \
  allocInitNC(BUCKET,TIPO,VAR,__VA_ARGS__);

template<typename T> T* alloc(bucket* b){
  reserve(b,sizeof(T));
  return allocNC<T>(b);
}

/* si no anda se puede hacer un macro tipo
   new(b->head)TIPO(LISTA);
   TIPO* base=(TIPO*)b->head;
   b->head+=sizeof(TIPO);
*/

void clearBucketNoFree(bucket* b){//para tests
  b->head=b->data=b->firstBlock->data;
}

void clearBucket(bucket* b){
  bucket::block* block=b->firstBlock;
  b->head=b->data=b->firstBlock->data;

  if(block->next==nullptr) return;
  block=block->next;
  do{
    bucket::block* temp=block;
    block=block->next;
    delete temp;
  }while(block);
}

struct bucketSizeData{
  int usedSize;
  int usedBuckets;
};
bucketSizeData getBucketSizeData(bucket* b){
  bucket::block* block=b->firstBlock;
  for(int i=1;;i++){
    if(!block->next){
      bucketSizeData sd;
      sd.usedBuckets=i;
      sd.usedSize=(i-1)*b->size + (int)(b->head - b->data);
      return sd;
    }
    block=block->next;
  }
}

#define getStruct(type,name,from)               \
  type* name=(type*)from.data;

template<typename T> struct barray{
  T* beg;
  T* after;
  T& operator[](int i){
    return *(beg+i);
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
template<typename T,typename B> void allocNC(B* b,barray<T>* ba,int elems){
  ba->beg=(T*)head(b);
  b->size+=sizeof(T)*elems;
  ba->after=(T*)head(b);
}
template<typename T,typename B> void alloc(B* b,barray<T>* ba,int elems){
  reserve(b,elems*sizeof(T));
  allocNC(b,ba,elems);
}
template<typename T,typename Y> void copy(barray<T> ba,Y* data){
  memcpy(ba.beg,data,size(ba));
}

template<typename T,typename B> void allocCopy(B* b,barray<T>* ba,int size,void* data){
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
  reserve(b,elems*elemSize);
  ba->beg=b->head;
  b->head+=elems*elemSize;
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
  reserve(b,sizeof(bbucket<size,T>));
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
  assert(vec->size<=vec->cap);
  assert(vec->cap>0);
  if(vec->size==vec->cap){
    privateGrow(vec,vec->cap*2);//TODO probar 1,5
  }
  vec->data[vec->size]=obj;
  vec->size++;
}

template<typename T>
int elems(vector<T> vec){
  return vec.size;
}

template<typename T>
T* newElem(vector<T>* vec){
  assert(vec->size<=vec->cap);
  assert(vec->cap>0);
  if(vec->size==vec->cap){
    privateGrow(vec,vec->cap*2);//TODO probar 1,5
  }
  vec->size++;
  return &vec->data[vec->size-1];
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
T& at(vector<T>* vec,int ind){//retorna referencia para que se pueda tomar direccion
  assert(ind>=0&&ind<vec->size);
  return vec->data[ind];
}

template<typename T>
void free(vector<T>* vec){
  delete[] vec->data;
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


//los motivos para usar un vector sobre un bucket para manejo de memoria principal es que es mas simple de usar,
//porque no tengo que lidiar con los casos del borde del bucket, al costo de que hacer reservas enormes de
//memoria contigua podria ser malo por algun motivo? por el tema de memoria virtual no deberia pasar nada,
//tambien esta el tema de que ahora es un acceso indirecto
//Una ventaja del bucket es que podía usar punteros directamente, porque la memoria no cambia de posicion.
//pero como ahora voy a tener que usar indices para poder manejar instancias duplicadas de la memoria en
//referencias internas, ya no tengo esa ventaja. Usar indices con buckets es mas feo porque necesito 2 numeros, bucket y offset

//podria hacer que sea una especializacion de vector<char> pero me gustaria separar bien las funciones
struct bigVector{
  int cap;
  int size;
  char* data;
};

void init(bigVector* v,int size=1<<20){
  v->size=0;
  v->cap=size;
  v->data=new char[v->cap];
}

template<typename T>
T* allocNC(bigVector* vec){
  assert((int)sizeof(T)<vec->cap);
  assert(vec->size+(int)sizeof(T)<=vec->cap);
  char* ret=vec->data+vec->size;
  new(ret)T;//llamo a constructor para manejar cosas de la libreria
  vec->size+=sizeof(T);
  return (T*)ret;
}

void free(bigVector* vec){
  delete[] vec->data;
  vec->size=vec->cap=0;
}

void reserve(bigVector* vec, int size){
  if(vec->size+size>vec->cap){
    vec->cap=vec->cap*1.5;//TODO probar con otros numeros
    char* oldData=vec->data;
    vec->data=new char[vec->cap];
    memcpy(vec->data,oldData,vec->size);
    delete[] oldData;
    #if debugMode
    printf("realocando uwu\n");
    #endif
  }
}

template<typename T>
T* alloc(bigVector* vec){
  reserve(vec,sizeof(T));
  return allocNC<T>(vec);
}


char* head(bigVector vec){
  return &vec.data[vec.size];
}

char* head(bigVector* vec){
  return head(*vec);
}

//TODO creo que siempre el inicio del vector esta inmediatamente despues de su struct, por lo que podría ahorrarme beg
template<typename T> struct varray{
  int beg;
  int size;//en bytes. Por ahi es mejor tenerlo en elementos, no sé. Creo que en el codigo termino usando muchas mas
  //veces elems que size, igual es un shift nomas
};

template<typename T> int elems(varray<T> ba){
  return ba.size/sizeof(T);
}
template<typename T> void allocNC(bigVector* b,varray<T>* ba,int elems){
  ba->beg=b->size;
  ba->size=sizeof(T)*elems;
  b->size+=ba->size;
}
template<typename T> void copy(bigVector* bv,varray<T>* ba,void* data){
  memcpy(&bv->data[ba->beg],data,ba->size);
}

template<typename T> void allocCopy(bigVector* b,varray<T>* ba,int size,void* data){
  allocNC(b,ba,size);
  copy(b,ba,data);
}

template<typename T> void alloc(bigVector* b,varray<T>* ba,int elems){
  reserve(b,elems*sizeof(T));
  allocNC(b,ba,elems);
}


//TODO no seria mejor tener un beg y size en vez de beg y after? after estaba para manejar el bucle for feo ese, con size me ahorro tener que llamar
//a elems

/*pense en tener un forEach pero la indentacion queda media fea y como usa macros no puedo usar comas, medio choto

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


//TODO estaria bueno tener una estructura tipo vector pero con capacidad nomas, que se acceda por index.
//entonces haces at(vec,8) y hay un if que te aloca si 8 esta fuera de rango. Tendría el mismo costo que
//vector, un if, un int menos y es util en algunos lugares

















#endif // BUCKET_H
