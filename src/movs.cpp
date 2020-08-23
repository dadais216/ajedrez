


//TODO mirar esto
//reutilizar rs
void color::draw(){
  rs->setPosition(actualHolder.nh->pos.x*32*escala,actualHolder.nh->pos.y*32*escala);
  window.draw(*rs);
}

vector<color> coloresImp;
//TODO temporal, despues voy a hacer lo de la union y eso
//necesito guardar aparte la implementacion porque la aloco cada vez por algun motivo
//(es una indireccion totalmente al pedo, es algo que quedo de antes)

//TODO retornar int en vez de puntero
vector<RectangleShape*> colores;
int crearColor(int r,int g,int b){
    ///se crea una instancia del sprite y cada colort la guarda en un puntero, se diferencia por tipo
    ///en un parametro de esta funcion, por ahora solo manejo colores

    //el motivo de manejarme con rectangleshapes es que es lo que comparten todos, total la pos se va a tener
    //que setear en cada dibujo sea compartida o no
    for(RectangleShape* c:colores)
      if(c->getFillColor().r==r&&c->getFillColor().g==g&&c->getFillColor().b==b){
        color* col=newElem(&coloresImp);
        new(col)color();
        col->init(c);
        return coloresImp.size-1;
      }
    RectangleShape* rs=new RectangleShape();
    rs->setFillColor(sf::Color(r,g,b,40));
    push(&colores,rs);
    color* col=newElem(&coloresImp);
    new(col)color();
    col->init(rs);
    return coloresImp.size-1;
}

/*
sprt::sprt(){
    int sn=tokens.front()-1000;tokens.pop_front();
    _sprt.setTexture(imagen->get("sprites.png"));
    _sprt.setTextureRect(IntRect(64+sn*64,0,32,32));
    _sprt.setScale(escala,escala);
    _sprt.setColor(Color(255,255,255,120));
    tipo=colort;
}
void sprt::func(){
    bufferColores.push_back(pair<drawable,v>(drawable(1,&_sprt),pos));
}
void sprt::debug(){
    cout<<"sprt ";
}
numShow::numShow(){
    txt.setFont(j->font);
    index=tokens.front()-1000;tokens.pop_front();
    txt.setFillColor(Color::Black);
    tipo=colort;
}
void numShow::func(){
    std::ostringstream stm;
    stm<<numeros[index];
    txt.setString(stm.str());
    bufferColores.push_back(pair<drawable,v>(drawable(2,&txt),pos));
}
void numShow::debug(){
    cout<<"numShow "<<index<<" ";
}
*/





/*
///version condt, la version acct necesita un clone() que le guarde el holder.
template<void(*t2)(int*)> struct pieceac{
    int ind;
    piecea(int ind_):ind(ind_){}
    int* val(){
        t2(&ind);//version con y sin triggers
        return &hAct->memPiece[ind];
    }
};
*/

/*
struct spwn:public acm{
    int n;
    spwn(){
        tipo=acct;
        n=tokens.front()-1000; tokens.pop_front();
        //cuando anden los negativos se pueden invocar píezas del bando opuesto
    }
    virtual void func(){
        (*tabl)(pos.show(),lect.crearPiece(n*act->bando));
        cout<<"spwn "<<n<<" ";
    }
    virtual void debug(){
        cout<<"spwn "<<n<<" ";
    }
};
*/

void mov(){
    actualHolder.h->tile->step++;
    actualHolder.h->tile->holder=nullptr;
    actualHolder.h->tile=actualHolder.tile;
    actualHolder.h->tile->holder=actualHolder.h;
}

void pausa(){
    drawScreen(properDraw);
    sleep(milliseconds(40));
}
vector<Holder*> reciclaje;
void capt(){
    actualHolder.tile->holder->inPlay=false;
    //@optim se podria eliminar triggers estaticos en global aca para que no se iteren ni activen en falso
    //for(memTriggers& mt:memGlobalTriggers[ind])
    //    remove_if(mt.perma.begin(),mt.perma.end(),[&captT](normalHolder* nh)->bool{
    //            return nh->h==captT->holder;
    //          });
    //el problema esta en recrearlos en spawn. Se tendria que agregar otra rama de polimorfismo para acceder a cada
    //normalHolder y setear las memorias devuelta y no creo que lo valga
    push(&reciclaje,actualHolder.tile->holder);
    actualHolder.tile->holder=nullptr;
    actualHolder.tile->step++;
    push(&pisados,actualHolder.tile);
}

//retorna void(*)(void) normalmente, pero puede tener otras cosas metidas
void* getNextInBuffer(){
  //buffer es el buffer de punteros de funcion actual (sean acct o conds)
  //bufferPos es un puntero al iterador. Puede que necesite marcar el iterador como volatil?
  (*actualHolder.bufferPos)++;
  return (void*)actualHolder.buffer[*actualHolder.bufferPos];
}


void spwn(){
  //antes cada acct era un objeto polimorfico en vez de una funcion, por lo que algunos podrian tener datos propios. Como ahora tengo un nivel de indireccion menos no puedo hacer eso, osea lo podría hacer pero tendría algo igual que lo anterior y podría probar otra cosa.
  //lo que voy a hacer es poner la informacion que necesiten los acc/cond en el mismo buffer en el que estan, despues de si, indicando al que recorre el buffer que los ignore. Lo malo de esto es que por ahi entorpece la iteracion, aunque seguro es mejor que tener un nivel de indireccion mal. Lo otro malo es que cada dato tiene que caber en el tamaño de un puntero de funcion

  int codedId=(intptr)getNextInBuffer();
  int ind=abs(codedId)-1;
  bool bando=codedId>0?actualHolder.h->bando:!actualHolder.h->bando;

  Holder* h;
  //TODO puede que sea buena idea mover todo este procesado a generarNewlySpawned
  for(int i=0;i<reciclaje.size;i++){
    Holder* r=reciclaje[i];
    if(r->piece->ind==ind){//reciclo piezas enemigas tambien
      r->inPlay=true;
      r->bando=bando;
      memset(r->memPiece.beg,0,sizeof(int)*size(r->memPiece));
      unorderedErase(&reciclaje,i);
      h=r;
      goto end;
    }
  }
  h=initHolder(actualHolder.ps->pieces[ind],bando,actualHolder.tile,&actualHolder.ps->gameState);
 end:
  actualHolder.tile->holder=h;
  h->tile=actualHolder.tile;

  push(&justSpawned,actualHolder.tile->holder);
  push(&pisados,actualHolder.tile);
}


void debugShowAndWait(const char*,bool);
//tambien podria haber hecho un define tome el nombre y el codigo y construya la funcion, retornando al final como aca. Es lo mismo
#if debugMode
#define CONDRET(VAL) debugShowAndWait(__func__,VAL); return VAL
#else
#define CONDRET(VAL) return VAL
#endif


bool vacio(){
  CONDRET(actualHolder.tile->holder==nullptr);
}
bool piece(){
  CONDRET(actualHolder.tile->holder);
}
bool enemigo(){
  Holder* other=actualHolder.tile->holder;
  if(other){
    CONDRET(other->bando!=actualHolder.h->bando);
  }
  CONDRET(false);
}
bool aliado(){
  Holder* other=actualHolder.tile->holder;
  if(other){
    CONDRET(other->bando==actualHolder.h->bando);
  }
  CONDRET(false);
}
bool self(){
  CONDRET(actualHolder.tile->holder==actualHolder.h);
}

bool pass(){
  CONDRET(true);
}//se usa al final de exc para retornar verdadero aunque las otras ramas hayan fallado



bool ZPressed=false;
int mil=25;
void stall(){
  ///@cleanup como esta todo tirado aca en vez de en input no se puede cerrar la ventana, pero bueno
  while(true){
    sleep(milliseconds(mil));
    if(!window.hasFocus()) continue;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
      if(!ZPressed){
        ZPressed=true;
        break;
      }
    }else
      ZPressed=false;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
      mil=75;
      break;
    }else
      mil=25;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
      mil=0;
      break;
    }
  }
}
bool langAssert(){
  printf("something wrong!!\n");
  mil=2000;
  stall();
  return true;
}

#if debugMode
RectangleShape backgroundMem;
RectangleShape backgroundMemDebug;
RectangleShape localMemorySeparator;
Text textValMem;

RectangleShape posPiece;
RectangleShape posActGood;
RectangleShape posActBad;
RectangleShape posMem;
RectangleShape* tileActDebug;

Text textDebug;
bool drawDebug;





void debugShowAndWait(char const* name,bool val){
  textDebug.setString(name);
  
  v posAct=actualHolder.nh->pos;
  if(val){
    posActGood.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActGood;
    textDebug.setColor(sf::Color(78,84,68,100));
  }else{
    posActBad.setPosition(posAct.x*32*escala,posAct.y*32*escala);
    tileActDebug=&posActBad;
    textDebug.setColor(sf::Color(240,70,40,240));
  }
  posPiece.setPosition(actualHolder.h->tile->pos.x*32*escala,actualHolder.h->tile->pos.y*32*escala);
  drawDebug=true;
  drawScreen(properDraw);
  drawDebug=false;
        
  stall();
}
void debugShowAndWaitMem(char const* name,bool val){
  textDebug.setString(name);

  tileActDebug=&posPiece;
  posPiece.setPosition(-32*escala,-32*escala);

  if(val)
    textDebug.setColor(sf::Color(78,84,68,100));
  else
    textDebug.setColor(sf::Color(240,70,40,240));
  
  drawDebug=true;
  drawScreen(properDraw);
  drawDebug=false;
  
  stall();
}
#endif

