#ifndef MOVHOLDERS_H
#define MOVHOLDERS_H

enum{NORMAL,DESLIZ,EXC,ISOL,DESOPT};

const int32_t valorCadena=1;//la cadena de movholders es valida. Una cadena va desde la base hasta un clicker o el final
const int32_t valorFinal=1<<1;//se llegó al final. Esto sirve para saber si seguir iterando en un desliz
const int32_t valor=1<<2;//lo usa normal para saber si sus condiciones son verdaderas,y exc si tiene una rama valida
const int32_t lastNotFalse=1<<2;//lo usa desliz
const int32_t makeClick=1<<3;
const int32_t hasClick=1<<4;
const int32_t doEsp=1<<5;//se usa en normalh

struct normalHolder;
struct AH{
    Holder* h;
    normalHolder* nh;
};
v offset;
Tile* actualTile;
AH actualHolder;

bool switchToGen;

struct Base{ ///datos compartidos de un movimiento entero
    Holder* h;
    movHolder* beg;
    int memLocalSize;//este es cte entre todos los holders por lo que podria estar en algun lugar del lado de operador
};
//@optim? se podria hacer que esta base sea global durante la generacion para no tenerla apuntada desde cada
//movholder. el problema esta que durante la reaccion necesito recuperar esa informacion, y la unica forma de
//hacerlo es cargandolo en los triggers, lo que puede que termine haciendo todo mas lento?
//@optim lo que se podria hacer para solucionar esto es guardar en operador el offset de la base, y usar eso
//en lugar de informacion en trigger para recuperar el estado en reaccion (osea, cambiaria un puntero en cada
//movHolder por un offset en normal y el uso de una global)
inline v getActualPos(v relPos,v offset){
    bool negate=actualHolder.h->bando;
    relPos.y=(relPos.y^-negate)+negate;
    return relPos+offset;
}
inline v getOffset(v relPos,v pos){
    bool negate=actualHolder.h->bando;
    relPos.y=(relPos.y^-negate)+negate;
    return pos-relPos;
}

struct movHolder{
    movHolder(operador*,Base*);
    virtual void generar()=0;
    virtual void reaccionar(normalHolder*)=0;
    virtual void reaccionar(vector<normalHolder*>*)=0;
    virtual void cargar(vector<normalHolder*>*)=0;
    void generarSig();
    void reaccionarSig(auto nhs){
        if(sig){
            sig->reaccionar(nhs);
            if(switchToGen){
                if(bools&hasClick||sig->bools&valorCadena)
                    bools|=valorCadena;
                else
                    bools&=~valorCadena;
                bools|=sig->bools&valorFinal;
            }
        }
    }
    Base* base;
    movHolder* sig;
    int32_t bools;
};
struct normalHolder:public movHolder{
    normalHolder(normal*,Base*,char**);//supongo que ni bien se crea el op le copias las accs
    normal* op;
    virtual void generar();
    void generarProper();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>*);
    virtual void cargar(vector<normalHolder*>*);
    void draw();
    void accionar();///desencadena los acct
    barray<int> memAct;
    //no separo entre piezas con y sin memoria porque duplicaria mucho codigo.
    //Cuando haga la version compilada puedo hacer esa optimizacion y cosas mas especificas
    v relPos; //pos actual = relPös + offset. Todas las acciones y condiciones la comparten
    v pos; //pos actual
};
struct deslizHolder:public movHolder{
    deslizHolder(desliz*,Base*,char**);
    desliz* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>*);
    void reaccionarNh(normalHolder*);
    virtual void cargar(vector<normalHolder*>*);
    barray<void> movs;
    int cantElems;//cantidad de iteraciones armadas
    void maybeAddIteration(int);
    int f;
};
struct excHolder:public movHolder{
    excHolder(exc*,Base*,char**);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>*);
    void reaccionarNh(normalHolder* nh);
    virtual void cargar(vector<normalHolder*>*);
    barray<movHolder*> ops;
    ///@optim podria probar usar barray<int> tamaños en vez de los punteros. Ocupa menos espacio.
    ///no estoy seguro de si seria mas rapido
    int size;
    int actualBranch;
};
struct isolHolder:public movHolder{
    isolHolder(isol*,Base*,char**);
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>*);
    virtual void cargar(vector<normalHolder*>*);
    int size;
    movHolder* inside;
};

struct desoptHolder:public movHolder{
    desoptHolder(desopt*,Base*,char**);
    desopt* op;
    virtual void generar();
    virtual void reaccionar(normalHolder*);
    virtual void reaccionar(vector<normalHolder*>*);
    virtual void cargar(vector<normalHolder*>*);
    struct node{
        node* iter;
        //movimiento
    };
    char* dinamClusterHead;
    void generarNodo(node*,int);
    void construirYGenerarNodo(int);
    void cargarNodos(node*,vector<normalHolder*>*);


    char* movs(){
        return (char*)this+sizeof(desoptHolder);
    }//esto se puede implementar en c++17 con [[no_unique_address]]
};


struct movHolderMock{//para que spawner y kamikase no hereden datos que no usan
    virtual void generar()=0;
    virtual void reaccionar(normalHolder*){assert(false);};
    virtual void reaccionar(vector<normalHolder*>*){assert(false);};
    virtual void cargar(vector<normalHolder*>*){};
    Base* base;
};
struct spawnerGen:public movHolderMock{
    spawnerGen(Base*);
    virtual void generar();
};
struct kamikaseCntrl:public movHolderMock{
    kamikaseCntrl(Base*);
    virtual void generar();
};

#endif // MOVHOLDERS_H
