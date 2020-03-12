

color::color(RectangleShape* rs_){
    rs=rs_;///@optim es necesario tener un rectangleShape para cada uno? por ahi es mas rapido reutilizar. No usaria memoria dinamica ahi
}
void color::draw(){
    rs->setPosition(actualPosColor.x*32*escala,actualPosColor.y*32*escala);
    window.draw(*rs);
}
list<RectangleShape*> colores;
colort* crearColor(){
    ///se crea una instancia del sprite y cada colort la guarda en un puntero, se diferencia por tipo
    ///en un parametro de esta funcion, por ahora solo manejo colores

    //el motivo de manejarme con rectangleshapes es que es lo que comparten todos, total la pos se va a tener
    //que setear en cada dibujo sea compartida o no
    int r=tokens.front()-1000;
    tokens.pop_front();
    int g=tokens.front()-1000;
    tokens.pop_front();
    int b=tokens.front()-1000;
    tokens.pop_front();
    for(RectangleShape* c:colores)
        if(c->getFillColor().r==r&&c->getFillColor().g==g&&c->getFillColor().b==b)
            return new color(c);
    RectangleShape* rs=new RectangleShape(Vector2f(32*escala,32*escala));
    rs->setFillColor(sf::Color(r,g,b,40));
    colores.push_back(rs);
    return new color(rs);
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
template<void(*t2)(int*)> struct piezaac{
    int ind;
    piezaa(int ind_):ind(ind_){}
    int* val(){
        t2(&ind);//version con y sin triggers
        return &hAct->memPieza[ind];
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
        (*tabl)(pos.show(),lect.crearPieza(n*act->bando));
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
    actualHolder.h->tile=actualTile;
    actualHolder.h->tile->holder=actualHolder.h;
}

void pausa(){
    drawScreen(properDraw);
    sleep(milliseconds(40));
}
vector<Holder*> reciclaje;
void capt(){
    actualTile->holder->inPlay=false;
    //@optim se podria eliminar triggers estaticos en global aca para que no se iteren ni activen en falso
    //for(memTriggers& mt:memGlobalTriggers[ind])
    //    remove_if(mt.perma.begin(),mt.perma.end(),[&captT](normalHolder* nh)->bool{
    //            return nh->h==captT->holder;
    //          });
    //el problema esta en recrearlos en spawn. Se tendria que agregar otra rama de polimorfismo para acceder a cada
    //normalHolder y setear las memorias devuelta y no creo que lo valga
    reciclaje.push_back(actualTile->holder);
    actualTile->holder=nullptr;
    actualTile->step++;
    pisados.push_back(actualTile);
}

//retorna void(*)(void) normalmente, pero puede tener otras cosas metidas
void* getNextInBuffer(){
  //buffer es el buffer de punteros de funcion actual (sean acct o conds)
  //bufferPos es un puntero al iterador. Puede que necesite marcar el iterador como volatil?
    return (void*)actualHolder.buffer[(*actualHolder.bufferPos)++];
}


void spawn(){
  //antes cada acct era un objeto polimorfico en vez de una funcion, por lo que algunos podrian tener datos propios. Como ahora tengo un nivel de indireccion menos no puedo hacer eso, osea lo podría hacer pero tendría algo igual que lo anterior y podría probar otra cosa.
  //lo que voy a hacer es poner la informacion que necesiten los acc/cond en el mismo buffer en el que estan, despues de si, indicando al que recorre el buffer que los ignore. Lo malo de esto es que por ahi entorpece la iteracion, aunque seguro es mejor que tener un nivel de indireccion mal. Lo otro malo es que cada dato tiene que caber en el tamaño de un puntero de funcion

  int id=*(int*)getNextInBuffer();

  for(int i=0;i<reciclaje.size();i++){
    Holder* h=reciclaje[i];
    if(h->id==abso(id)){//reciclo piezas enemigas tambien
      h->inPlay=true;
      memset(h->memPieza.beg,0,sizeof(int)*size(h->memPieza));
      actualTile->holder=h;
      h->tile=actualTile;
      reciclaje.erase(reciclaje.begin()+i);
      goto gen;
    }
  }
  actualTile->holder=lect.crearPieza(id,actualTile->pos);
 gen:
  actualTile->holder->bando=id>0?actualHolder.h->bando:!actualHolder.h->bando;//el signo de id indica si se quiere el mismo bando u opuesto
  justSpawned.push_back(actualTile->holder);
  pisados.push_back(actualTile);

}


void debugShowAndWait(const char*,bool);
//tambien podria haber hecho un define tome el nombre y el codigo y construya la funcion, retornando al final como aca. Es lo mismo
#if debugMode
#define CONDRET(VAL) debugShowAndWait(__func__,VAL); return VAL
#else
#define CONDRET(VAL) return VAL
#endif


bool vacio(){
  CONDRET(actualTile->holder==nullptr);
}
bool pieza(){
  CONDRET(actualTile->holder);
}
bool enemigo(){
    Holder* other=actualTile->holder;
    if(other)
      CONDRET(other->bando!=actualHolder.h->bando);
    CONDRET(false);
}

bool pass(){
  CONDRET(true);
}//se usa al final de exc para retornar verdadero aunque las otras ramas hayan fallado

debug(
      RectangleShape backGroundMem;

      RectangleShape posPieza;
      RectangleShape posActGood;
      RectangleShape posActBad;
      RectangleShape* tileActDebug;

      Text textDebug;
      bool drawDebugTiles;
      bool ZPressed=false;
      int mil=25;

      bool drawMemDebug;
      getterCond* getterMemDebug1;
      getterCond* getterMemDebug2;

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
            if(mil>10) mil-=1;
            break;
          }else
            mil=25;
          if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
            mil=0;
            break;
          }
        }
      }
      void debugShowAndWait(char* name,bool val){
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
        posPieza.setPosition(actualHolder.h->tile->pos.x*32*escala,actualHolder.h->tile->pos.y*32*escala);
        drawDebugTiles=true;
        drawScreen(properDraw);
        drawDebugTiles=false;

        stall();
      }
      void debugShowAndWaitMem(char* name,bool val){
        textDebug.setString(name);

        if(val)
          textDebug.setColor(sf::Color(78,84,68,100));
        else
          textDebug.setColor(sf::Color(240,70,40,240));

        drawMemDebug=true;
        drawScreen(properDraw);
        drawMemDebug=false;
        getterMemDebug1=nullptr;

        stall();
      }
      );

