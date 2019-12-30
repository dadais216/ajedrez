

color::color(RectangleShape* rs_){
    rs=rs_;///@optim es necesario tener un rectangleShape para cada uno? por ahi es mas rapido reutilizar. No usaria memoria dinamica ahi
}
void color::draw(){
    rs->setPosition(actualPosColor.x*32*escala,actualPosColor.y*32*escala);
    window->draw(*rs);
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
    drawScreen();
    sleep(milliseconds(40));
}
vector<Holder*> reciclaje;
void capt(){
    actualTile->holder->inPlay=false;
    //@optim se podria eliminar triggers estaticos en global aca para que no se iteren ni activen en falso
    //for(memTriggers& mt:memGlobalTriggers)
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

void* getNextInBuffer(){
  //buffer es el buffer de punteros de funcion actual (sean acct o conds)
  //bufferPos es un puntero al iterador. Puede que necesite marcar el iterador como volatil?

  debug(
        if(actualHolder->skipDebugName){
          *actualHolder.bufferPos++;//mirar bien lo de los tamaños
        }
        //ver donde meto el codigo de mostrar el debug, si es un macro dentro del check como pienso hacerlo podria mover este codigo ahi
)
  
  return actualHolder.buffer[*(actualHolder.bufferPos)++];
}

void spawn(){
  //antes cada acct era un objeto polimorfico en vez de una funcion, por lo que algunos podrian tener datos propios. Como ahora tengo un nivel de indireccion menos no puedo hacer eso, osea lo podría hacer pero tendría algo igual que lo anterior y podría probar otra cosa.
  //lo que voy a hacer es poner la informacion que necesiten los acc/cond en el mismo buffer en el que estan, despues de si, indicando al que recorre el buffer que los ignore. Lo malo de esto es que por ahi entorpece la iteracion, aunque seguro es mejor que tener un nivel de indireccion mal. Lo otro malo es que cada dato tiene que caber en el tamaño de un puntero de funcion

  int id=(int)getNextInBuffer();

  for(int i=0;i<reciclaje.size();i++){
    Holder* h=reciclaje[i];
    if(h->id==abso(id)){//reciclo piezas enemigas tambien
      h->inPlay=true;
      memset(h->memPieza.begptr,0,sizeof(int)*h->memPieza.size());
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

inline bool mcmpCond(getter* a1,getter* a2){
    return *a1->val()==*a2->val();
}
inline bool msetCond(getter* a1,getter* a2){
    *a1->val()=*a2->val();
    return true;
}
inline bool msetAcc(getter* a1,getter* a2){///version para memorias globales, para activar triggers
    int* val=a1->val();
    int before=*val;
    *val=*a2->val();
    if(before!=*val){
        for(normalHolder* nh:trigsMaybeActivate->perma)
            if(nh->base->h!=actualHolder.h&&nh->base->h->inPlay)
                trigsActivados.push_back(nh);
        for(normalHolder* nh:trigsMaybeActivate->dinam)
            if(nh->base->h!=actualHolder.h&&nh->base->h->inPlay)
                trigsActivados.push_back(nh);
        trigsMaybeActivate->dinam.clear();
        ///@optim creo que no habria problema en dejar recalcular triggers dinamicos a piezas capturadas. Seria
        ///un calculo de mas pero no romperia nada y por ahi es mas rapido que poner un if aca?
    }
    return true;
}
inline bool msetAccTile(getter* a1,getter* a2){///version para tiles que necesitan que la pieza que puso el trigger no se haya movido a demas de que la memoria varie
    int* val=a1->val();
    int before=*val;
    *val=*a2->val();
    if(before!=*val){
        vector<Tile::tileTrigInfo>* memTile=reinterpret_cast<vector<Tile::tileTrigInfo>*>(trigsMaybeActivate);
        for(Tile::tileTrigInfo& tti:*memTile)
            if(tti.nh->base->h!=actualHolder.h&&tti.step==*tti.stepCheck)
                trigsActivados.push_back(tti.nh);
        memTile->clear();
    }
    //el chequeo de step es necesario porque evita que un trigger viejo se active y haga recalcular a una pieza,
    //ahora en otra tile, que va a repetir su generacion innecesariamente.
    //other podria tener un chequeo de step parecido, usando el step de la tile donde esta el trigger y la tile
    //de la pieza que lo puso. Pero no es tan util porque una activacion de trigger viejo solo va a causar un
    //recalculo que no va a quedar en nada, esta asegurado que no va a generar (por estar despues de una condicion
    //pieza falsa, o esta dentro de una rama innaccesible o cuando intente generar va a dar falso por esa condicion)
    //a demas la implementacion en other es mas dificil porque a diferencia de tile other no tiene memoria propia,
    //la comparte con pieza, y necesitaria guardar la informacion de los steps y relacionarlos.
    //Y necesitaria una version mas de cada accion. No creo que lo valga, y puede que la implementacion termine siendo
    //mas lenta
}
inline bool maddCond(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    return true;
}
inline bool maddAcc(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    ///@todo cargar triggers
    return true;
}
inline bool maddAccTile(getter* a1,getter* a2){
    *a1->val()+=*a2->val();
    ///@todo cargar triggers
    return true;
}
inline bool mlessCond(getter* a1,getter* a2){
    return *a1->val()<*a2->val();
}
inline bool mmoreCond(getter* a1,getter* a2){
    return *a1->val()>*a2->val();
}
inline bool mdistCond(getter* a1,getter* a2){
    return *a1->val()!=*a2->val();
}
//me imagino que n indirecciones se van a implementar con un bucle for que castee a puntero y deferencie n veces
//el getter tendria un int que sea n

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

      void debugShowAndWait(int type){
        bool ret=cond->check();
        textDebug.setString(*cond->nomb);

        if(type==DEBUGDRAWTILE){
          v posAct=actualHolder.nh->pos;
          if(ret){
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
          drawScreen();
          drawDebugTiles=false;
        }else{

          //esto lo habia hecho porque como aca solo llegan memConds estoy seguro de que tienen estos campos. Podría haber agregado un nivel de herencia y hacer que todos los memConds hereden de un memCond, pero molestaba y nomas lo necesito aca
          struct mock:public condt{
            getterCond* i1;
            getterCond* i2;
          };
          if(ret)
            textDebug.setColor(sf::Color(78,84,68,100));
          else
            textDebug.setColor(sf::Color(240,70,40,240));
          posPieza.setPosition(actualHolder.h->tile->pos.x*32*escala,actualHolder.h->tile->pos.y*32*escala);

          getterMemDebug1=static_cast<mock*>(cond)->i1;
          getterMemDebug2=static_cast<mock*>(cond)->i2;
          drawMemDebug=true;
          drawScreen();
          drawMemDebug=false;
          getterMemDebug1=nullptr;
        }



        ///@cleanup como esta todo tirado aca en vez de en input no se puede cerrar la ventana, pero bueno
        while(true){
          sleep(milliseconds(mil));
          if(!window->hasFocus()) continue;
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
        return ret;
      }
      );



