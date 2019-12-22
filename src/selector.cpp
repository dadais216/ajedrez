

struct boton{
  string name;
  int n,x,y;
  boton* next;
  //string es una indireccion por lo que ahora es leak. Cuando me mueva a sdl pongo un char[20] y listo
  //es una linked list porque me permite tener infinitos botones (habria que agregar scrollin), y no tiene sentido hacer algo mas eficiente aca
}

boton* buttonAllocInit(string& s,int n,int x,int y){
  boton* b=alloc<boton>(&stateBucket);
  b->name=s;
  b->n=n;
  b->x=x;
  b->y=y;
  b->next=stateBucket->head;//se crean en secuencia, el ultimo se pone en null desde afuera
  return b;
}

void drawButton(boton* b){
  selectorState->sprite.setPosition(b->x,b->y);
  selectorState->text.setPosition(b->x+5,b->y+10);
  selectorState->text.setString(b->name);
  window.draw(selectorState->sprite);
  window.draw(selectorState->text);
}

int buttonClicked(boton* b){
  v ve=input->pixel();
  if(ve.x>=b->x&&ve.x<=b->x+128&&ve.y>=b->y&&ve.y<=b->y+64) //puede que este mal
    return b->n;
  return 0;
}

struct PlayerSelector{
  int bando;
  boton* firstButton;//linked porque reuso botones nomas
  int selected;
};

void genPlayerSelector(PlayerSelector* playerSelector,int bando_){
    playerSelector->bando=bando_;
    playerSelector->firstButton=stateBucket.head;
    if(bando_==1){
      buttonAllocInit("nadie",0,540,320);
    }
    buttonAllocInit("humano",1,500+40*bando,360);
    buttonAllocInit("aleatorio",2,500+40*bando,400);
    buttonAllocInit("IA",3,500+40*bando,440)->next=nullptr;

    playerSelector->selected=1;
}
void updatePlayerSelector(playerSelector* ps){
  for(boton* b=ps->firstButton;
      b;
      b=b->next){
    if(buttonClicked(b)){
      ps->selected=b->n;//n-1?
      drawScreen(selectorDraw);
      break;
    }
  }
}

struct selectorState{
  Sprite sprite;
  Text text;

  RectangleShape bordeSeleccion;
  PlayerSelector player1,player2;

  boton* firstButton;
}

void selectorUpdate();
void selectorInit(){
  resetBucket(&stateBucket);
  selectorState* spt=alloc<selectorState>(&stateBucket);

  selectorState->sprite.setTexture(image->get("tiles.png"));
  selectorState->sprite.setTextureRect(IntRect(0,32,64,32));
  selectorState->sprite.setScale(2,2);
  selectorState->text.setFont(font);
  selectorState->text.setColor(Color::Black);
  selectorState->text.setScale(1,1);
  
  selectorState->bordeSeleccion->setFillColor(Color(0,0,0,0));
  selectorState->bordeSeleccion->setSize(sf::Vector2f(64, 32));
  selectorState->bordeSeleccion->setOutlineColor(Color::White);
  selectorState->bordeSeleccion->setOutlineThickness(1);

  genPlayerSelector(1);
  genPlayerSelector(-1);

  firstButton=stateBucket.head;

  fstream tableros;
  tableros.open("tableros.txt");
  string linea; 
  int j=0;
  boton* lastButton;
  while(getline(tableros,linea)){
    if(!linea.empty()&&linea[0]=='"'){
      int i=1;
      for(; linea[i]!='"'; i++);

      lastButton=buttonAllocInit(linea.substr(1,i-1),
                 j,//j+1?
                 32+(70*j/420)*140,
                 40+(j*70)%420);

      j++;
    }
  }
  lastButton->next=nullptr;
  drawScreen(selectorDraw);
  actualStateUpdate=selectorUpdate;
} 
void selectorUpdate(){
  getStruct(selectorState,bg,stateBucket);
  if(input->click()){
    for(boton* b=bg->firstButton();
        b;
        b=b->next){
      if(buttonClicked(b)){
        properInit(b->n,bg->player1.selected,bg->player2.selected);
        return;
      }
    }
    updatePlayerSelector(bg->player1);
    updatePlayerSelector(bg->player2);
  }
}
void selectorDraw(){
  getStruct(selectorState,bg,stateBucket);

  selectorState->sprite.setScale(2f,2f);
  selectorState->text.setScale(2f,2f);

  for(boton* b=bg->firstButton();
      b;
      b=b->next){
    drawButton(b);
  }

  selectorState->sprite.setScale(1f,1f);
  selectorState->text.setScale(1f,1f);

  PlayerSelector* ps=bg->player1;
 drawSelected:
  for(boton* b=ps->firstButton;
      b;
      b=b->next){
    drawButton(b);
  }
  bordeSeleccion.setPosition(500+40*bando,320+40*selected);
  window.draw(cuadrado);
  if(ps==bg->player1){
    ps=bg->player2;
    goto drawSelected;
  }
}
