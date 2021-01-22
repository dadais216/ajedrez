
bool Clicker::drawClickers;

//hacer un test de esto, cambiarlo por buckets en stateBucket. Tiene que ser buckets o una estructura asi porque nada asegura que la cantidad de clickers sea menor a los tiles del tablero, aunque sería razonable asumir que lo es en el caso comun
vector<Clicker> clickers;
v getActualPos(v,v);


void makeClicker(vector<normalHolder*>* normales,Holder* h){
  Clicker* clicker=newElem(&clickers);
  clicker->h=h;

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

vector<Tile*> pisados;
void executeClicker(Clicker* c,board* brd){
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
  Tile* tileBef=c->h->tile;
  int stepBef=tileBef->step;

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

  //TODO con step en holder esto no es necesario
  ///@optim esto esta para movimientos que no mueven la pieza, que son una minoria
  if(tileBef->step!=stepBef){
    push(&pisados,tileBef);
    push(&pisados,c->h->tile);
    ///@optim piezas que no se mueven no deberian generar todo
  }
  else{
    tileBef->step++;//evitar que se activen triggers viejos.
                    //En principio evitaria redundancia nomas, pero como desopt reutiliza nhs puede triggerear un nh que ahora es distinta,
                    //y eso es malo porque ahora se deja de buscar despues de encontrar la nh, y como esta es falsa ni siquiera se va a encontrar la verdadera
  }

  for(Tile* tile:pisados){
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
  for(Holder* h:justSpawned){
    if(c->h!=h)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
      generar(h);
  }
  if(c->h->inPlay){
    generar(c->h);
  }
  justSpawned.size=0;

  ///una pieza nunca activa sus propios triggers porque al moverse los invalida
  ///necesita generar todos sus movimientos devuelta de forma explicita
  ///piezas que no se mueven no pisan

  //si no se quiere tener este if se podría generar en spwn, pero eso genera recalculos y algunos bugs oscuros
  //(creo que el bug era que una pieza se capturaba a si misma, ponia triggers, se spawneaba a si misma y los
  //activaba. Esto por algun motivo rompia a veces)

}

void debugPrintClickers(board* brd){
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

