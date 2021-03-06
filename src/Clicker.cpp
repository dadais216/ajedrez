
bool Clicker::drawClickers;

//hacer un test de esto, cambiarlo por buckets en stateBucket. Tiene que ser buckets o una estructura asi porque nada asegura que la cantidad de clickers sea menor a los tiles del tablero, aunque sería razonable asumir que lo es en el caso comun
vector<Clicker> clickers;
v getActualPos(v,v);


void makeClicker(vector<normalHolder*>* normales,int bInd){
  Holder* h=gameVector<Holder>(gameVector<Base>(bInd)->holder);

  Clicker* clicker=newElem(&clickers);
  clicker->h=indGameVector(h);

  //TODO No alocar devuelta
  //TODO creo? que tengo que alocar algo aparte si o si, igual estaria bueno tenerlo en un espacio continuo, total se libera todo junto y
  //si tengo ganas podría tener superposiciones
  initCopy(&clicker->normales,normales->data,normales->size);//con un array bastaria, pero usar un vector tiene el costo de un int mas nomas asi que ya fue

  actualHolder.h=h;//?

  normalHolder* lastN=normales->data[normales->size-1];
  clicker->clickPos=lastN->pos;

  //clickPos.show();
  ///solapamientos
  /*
    val=0;
    mod=1;
    int conflictos=0;
    for(Clicker* c:clickers)
    if(c->clickPos==clickPos){
    conflictos++;
    c->mod++;
    }
    if(conflictos!=0){
    val=conflictos;
    mod=conflictos+1;
    }
  */
}

void clearClickers(){
  for(Clicker& cli:clickers){
    free(&cli.normales);
  }
  clickers.size=0;
}

void drawClicker(Clicker* c){
  for(normalHolder* n:c->normales){
    drawNormalH(n);
  }
    /*
    if(!activo) return;
    for(pair<drawable,v> c:colores){
        if(get<0>(c).tipo==0){
            RectangleShape* rs=(RectangleShape*)get<0>(c).obj;
            rs->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*rs);
        }else if(get<0>(c).tipo==1){
            Sprite* s=(Sprite*)get<0>(c).obj;
            s->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*s);
        }else{
            Text* t=(Text*)get<0>(c).obj;
            t->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*t);
        }
        //seguro que se puede simplificar con cast y esas boludeces

        //cout<<c->cuadrado.getPosition().x/32/escala<<" "<<c->cuadrado.getPosition().y/32/escala<<endl;
    }
    */
}

vector<int> pisados;
void executeClicker(Clicker* c){
  /*
  //esto es para confirmar el toque
  if(mod>1){
  clickers.clear();
  clickers.push_back(this);
  confirm=true;
  val=0;
  mod=1;
  return true;
  }
  */
  Holder* h=gameVector<Holder>(c->h);
  int tileBef=h->tile;

  Clicker::drawClickers=false;

#if debugMode
  debugInCondition=false;
#endif
  for(normalHolder* n:c->normales){
    accionarNormalH(n);
  }
#if debugMode
  debugInCondition=true;
#endif

  h->step++;

  push(&pisados,tileBef);
  push(&pisados,h->tile);

  //TODO cuando tenga movimientos que no mueven la pieza (o que termina en el mismo lugar)
  //podría probar poner un if que no agregue nada a pisados. Agregar la misma posicion 2 veces
  //no es un problema porque la segunda vez no va a haber triggers que levantar, pero se podría
  //hacer 0 veces considerando que las reacciones van a generar lo mismo porque la pieza sigue
  //estando en el mismo lugar. El step se actualiza igual porque sino se activarian triggers viejos,
  //lo que llevaria a calculos redundantes, se reaccionaria varias veces a lo mismo. Y en caso de
  //que haya desopt puede que haya errores, por activar normales en el espacio dinamico que ahora
  //son otra cosa.

  for(int tileInd:pisados){
    Tile* tile=gameVector<Tile>(tileInd);
    chargeTriggers(&tile->triggersUsed,&tile->firstTriggerBox);
  }

  actualHolder.ps->turno++;
  activateTriggers();
  pisados.size=0;

  //estos chequeos antes estaban adentro de movholders especiales, que hacian la generacion de spawners y
  //un longjmp en caso de que fuera un kamikase que se suicido. Testeando vi que mover ese codigo ahi no
  //hace un cambio en la eficiencia, asi que lo saque porque tener mas movholders, y en especial esos que
  //eran hacks, agregaba complejidad y no aportaba nada a cambio. Ademas determinar si una pieza es kamikase
  //es complejo, goto hace que haya muchos falsos positivos
  for(int sh:justSpawned){
    if(c->h!=sh)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
      generar(gameVector<Holder>(sh));
  }
  if(h->inPlay){
    generar(h);
  }
  justSpawned.size=0;

  ///una pieza nunca activa sus propios triggers porque al moverse los invalida
  ///necesita generar todos sus movimientos devuelta de forma explicita
  ///piezas que no se mueven no pisan

  //si no se quiere tener este if se podría generar en spwn, pero eso genera recalculos y algunos bugs oscuros
  //(creo que el bug era que una pieza se capturaba a si misma, ponia triggers, se spawneaba a si misma y los
  //activaba. Esto por algun motivo rompia a veces)

}

void debugPrintClickers(){
  vector<int> clicks;init(&clicks,brd->dims.x*brd->dims.y);defer(&clicks);
  clicks.size=clicks.cap;
  for(int i=0;i<clicks.size;i++){
    clicks[i]=0;
  }
  for(Clicker& cli:clickers){
    clicks[cli.clickPos.x+cli.clickPos.y*brd->dims.x]++;
  }

  printf("c=%d\n",clickers.size);
  for(int i=0;i<clicks.size;i++){
    printf("%d ",clicks[i]);
    if((i+1)%brd->dims.x==0){
      printf("\n");
    }
  }
  printf("\n\n");
}
/*void Clicker::activacion(int clickI)
{
    activo=val==clickI%mod;
}

*/

