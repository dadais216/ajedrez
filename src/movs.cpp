


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
  actualHolder.h->tile->holder=nullptr;
  actualHolder.h->tile=actualHolder.tile;
  actualHolder.h->tile->holder=actualHolder.h;
}

void pausa(){
  drawScreen([&](){properDraw(stateMem);});
  sleep(milliseconds(40));
}
vector<Holder*> reciclaje;
void capt(){
  actualHolder.tile->holder->step++;
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
  push(&pisados,actualHolder.tile);
}

//retorna void(*)(void) normalmente, pero puede tener otras cosas metidas
void* getNextInBuffer(){
#if debugMode
  if(debugInCondition&&debugMultiParameterBegin==-1){//para pintar toda la condicion
    debugMultiParameterBegin=*actualHolder.bufferPos;
  }
#endif

  //buffer es el buffer de punteros de funcion actual (sean acct o conds)
  //bufferPos es un puntero al iterador. Puede que necesite marcar el iterador como volatil?
  (*actualHolder.bufferPos)++;
  return (void*)actualHolder.buffer[*actualHolder.bufferPos];
}


void spwn(){
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

  //tecnicamente podría generar la pieza nueva inmediatamente, el problema es que como las acciones de la actual no terminaron
  //va a generarse con el tablero en un estado erroneo. Cuando la actual termine se van a activar triggers y se va a reaccionar
  //y regenerar. Si no hay problemas con el step en principio esto funcionaria, pero basicamente esta generando 2 veces y no me cierra.
}


//cuando mostraba de una condicion a la vez aca hacía #define CONDRET(VAL) debugShowAndWait(__func__,VAL); return VAL

bool vacio(){
  return actualHolder.tile->holder==nullptr;
}
bool piece(){
  return actualHolder.tile->holder;
}
bool enemigo(){
  Holder* other=actualHolder.tile->holder;
  if(other){
    return other->bando!=actualHolder.h->bando;
  }
  return false;
}
bool aliado(){
  Holder* other=actualHolder.tile->holder;
  if(other){
    return other->bando==actualHolder.h->bando;
  }
  return false;
}
bool self(){
  return actualHolder.tile->holder==actualHolder.h;
}

bool pass(){
  return true;
}//se usa al final de exc para retornar verdadero aunque las otras ramas hayan fallado


/*
bool ZPressed=false;
int mil=25;
void stall(){
  while(true){
    handleSystemEvents();
    sleep(milliseconds(mil));
    if(!window.hasFocus()) continue;
    if(Input.z){
      if(!ZPressed){
        ZPressed=true;
        break;
      }
    }else
      ZPressed=false;
    if(Input.x){
      mil=75;
      break;
    }else
      mil=25;
    if(Input.c){
      mil=0;
      break;
    }
  }
}
*/
bool langAssert(){
  printf("something wrong!!\n");
  //mil=2000;
  sleep(milliseconds(2000));
  //stall();
  return true;
}


