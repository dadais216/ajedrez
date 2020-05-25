
bool Clicker::drawClickers;

//hacer un test de esto, cambiarlo por buckets en stateBucket. Tiene que ser buckets o una estructura asi porque nada asegura que la cantidad de clickers sea menor a los tiles del tablero, aunque sería razonable asumir que lo es en el caso comun
vector<Clicker> clickers;
v getActualPos(v,v);

#if debugMode
bool debugInCondition=true;
#endif

Clicker makeClicker(vector<normalHolder*>* normales,Holder* h){
  Clicker clicker;
  clicker.h=h;

  //TODO No alocar devuelta
  initCopy(&clicker.normales,normales->data,normales->size);//con un array bastaria, pero usar un vector tiene el costo de un int mas nomas asi que ya fue

  normalHolder* lastN=normales->data[normales->size-1];

  actualHolder.h=h;
  clicker.clickPos=lastN->pos;

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
  return clicker;
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


  for(Tile* tile:pisados){
    chargeTriggers(&tile->triggersUsed,&tile->firstTriggerBox);
  }

  actualHolder.ps->turno++;

  activateTriggers();
  pisados.size=0;

  try{
    generar(c->h);
  }catch(...){}//lngjmp para kamikases

  ///una pieza nunca activa sus propios triggers porque al moverse los invalida
  ///necesita generar todos sus movimientos devuelta de forma explicita
  ///piezas que no se mueven no pisan

  //si no se quiere tener este if se podría generar en spwn, pero eso genera recalculos y algunos bugs oscuros
  //(creo que el bug era que una pieza se capturaba a si misma, ponia triggers, se spawneaba a si misma y los
  //activaba. Esto por algun motivo rompia a veces)

    /*
    cout<<endl;
    for(int i=0; i<tablptr->tam.y; i++){
        for(int j=0; j<tablptr->tam.x; j++){
            cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
        }
        cout<<endl;
    }
    */

  clickers.size=0;
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

