

struct boton{
  string name;
  int n,x,y;
  boton* next;
  //string es una indireccion por lo que ahora es leak. Cuando me mueva a sdl pongo un char[20] y listo
  //es una linked list porque me permite tener infinitos botones (habria que agregar scrollin), y no tiene sentido hacer algo mas eficiente aca
};

struct playerSelector{
  int bando;
  boton* firstButton;//linked porque reuso botones nomas
  int selected;
};

struct selectorState{
  Sprite sprite;
  Text text;

  RectangleShape bordeSeleccion;
  playerSelector player1,player2;

  boton* firstButton;
};

void drawButton(boton* b){
  getStruct(selectorState,spt,stateBucket);
  spt->sprite.setPosition(b->x,b->y);
  spt->text.setPosition(b->x+5,b->y+10);
  spt->text.setString(b->name);
  window.draw(spt->sprite);
  window.draw(spt->text);
}

int buttonClicked(boton* b){
  v ve=input.pixel();
  if(ve.x>=b->x&&ve.x<=b->x+128&&ve.y>=b->y&&ve.y<=b->y+64) //puede que este mal
    return b->n;
  return 0;
}


void genPlayerSelector(playerSelector* playerSelector,int bando_){
    playerSelector->bando=bando_;
    playerSelector->firstButton=(boton*)stateBucket.head;
    if(bando_==1){
      buttonAllocInit("nadie",0,540,320);
    }
    buttonAllocInit("humano",1,500+40*bando_,360);
    buttonAllocInit("aleatorio",2,500+40*bando_,400);
    buttonAllocInit("IA",3,500+40*bando_,440)->next=nullptr;

    playerSelector->selected=1;
}
void selectorDraw();
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

void selectorUpdate();
void selectorInit(){
  resetBucket(&stateBucket);
  selectorState* spt=alloc<selectorState>(&stateBucket);

  spt->sprite.setTexture(image.get("tiles.png"));
  spt->sprite.setTextureRect(IntRect(0,32,64,32));
  spt->sprite.setScale(2,2);
  spt->text.setFont(font);
  spt->text.setColor(Color::Black);
  spt->text.setScale(1,1);
  
  spt->bordeSeleccion.setFillColor(Color(0,0,0,0));
  spt->bordeSeleccion.setSize(sf::Vector2f(64, 32));
  spt->bordeSeleccion.setOutlineColor(Color::White);
  spt->bordeSeleccion.setOutlineThickness(1);

  genPlayerSelector(&spt->player1,1);
  genPlayerSelector(&spt->player2,-1);

  spt->firstButton=(boton*)stateBucket.head;


  fstream tableros;
  tableros.open("tableros.txt");
  cString line;
  int i=0;

  boton* button;
  while(readFileUntil(tableros,line,'\n')){
    if(line.len!=0&&linea[0]=='"'){
      int j=1;
      for(; linea[j]!='"'; j++);

      button=alloc<boton>(&stateBucket);
      string str=string(line.c[1],line.c[j-1]);
      button->name = str;
      button->n=i++;
      button->x=32+(70*i/420)*140;
      button->y=40+(i*70)%420;
      button->next=(boton*)stateBucket.head;
    }
  }
  button->next=nullptr;


  drawScreen(selectorDraw);
  actualStateUpdate=selectorUpdate;
}
void properInit(int,int,int);
void selectorUpdate(){
  getStruct(selectorState,bg,stateBucket);
  if(input.click()){
    for(boton* b=bg->firstButton;
        b;
        b=b->next){
      if(buttonClicked(b)){
        properInit(b->n,bg->player1.selected,bg->player2.selected);
        return;
      }
    }
    updatePlayerSelector(&bg->player1);
    updatePlayerSelector(&bg->player2);
  }
}
void selectorDraw(){
  getStruct(selectorState,bg,stateBucket);

  bg->sprite.setScale(2.,2.);
  bg->text.setScale(2.,2.);

  for(boton* b=bg->firstButton;
      b;
      b=b->next){
    drawButton(b);
  }

  bg->sprite.setScale(1.,1.);
  bg->text.setScale(1.,1.);

  playerSelector* ps=&bg->player1;
 drawSelected:
  for(boton* b=ps->firstButton;
      b;
      b=b->next){
    drawButton(b);
  }
  bg->bordeSeleccion.setPosition(500+40*ps->bando,320+40*ps->selected);
  window.draw(bg->bordeSeleccion);
  if(ps==&bg->player1){
    ps=&bg->player2;
    goto drawSelected;
  }
}
