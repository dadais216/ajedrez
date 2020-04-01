

struct button{
  char name[256];
  int n,x,y;
};

button initButton(char const* name,int n,int x,int y){
  button b;
  char const* c=name;
  int i=0;
  do{
    b.name[i]=*c;
    i++;
  }while(*(c++));//hay una manera mejor?
  b.n=n;
  b.x=x;
  b.y=y;
  return b;
}

struct playerSelector{
  int bando;
  int buttonQ;
  button buttons[4];//hay un poco de desperdicio pero eh
  int selected;
};

struct selectorState{
  Sprite sprite;
  Text text;

  RectangleShape bordeSeleccion;
  playerSelector player1,player2;

  vector<button> buttons;
};

void drawButton(selectorState* st,button* b){
  st->sprite.setPosition(b->x,b->y);
  st->text.setPosition(b->x+5,b->y+10);
  st->text.setString(std::string(b->name));
  window.draw(st->sprite);
  window.draw(st->text);
}

int buttonClicked(button* b,int scale){
  v ve=input.pixel();
  if(ve.x>=b->x&&ve.x<=b->x+64*scale&&ve.y>=b->y&&ve.y<=b->y+32*scale) //puede que este mal
    return b->n;
  return 0;
}


void genPlayerSelector(playerSelector* ps,int bando_){
    ps->bando=bando_;
    ps->buttons[0]=initButton("humano",1,500+40*bando_,360);
    ps->buttons[1]=initButton("aleatorio",2,500+40*bando_,400);
    ps->buttons[2]=initButton("IA",3,500+40*bando_,440);
    if(bando_==1){
      ps->buttons[3]=initButton("nadie",4,540,480);
      ps->buttonQ=4;
    }else
      ps->buttonQ=3;
    ps->selected=1;
}
void selectorDraw(char*);
void updatePlayerSelector(playerSelector* ps){
  for(int i=0;i<ps->buttonQ;i++){
    button b=ps->buttons[i];
    if(buttonClicked(&b,1)){
      ps->selected=b.n;//n-1?
      drawScreen(selectorDraw);
      break;
    }
  }
}

void selectorUpdate(char*);
void selectorInit(char* mem){
  selectorState* st=new(mem)selectorState();//llamar a constructores de smfl
  init(&st->buttons);


  st->sprite.setTexture(image.get("tiles.png"));
  st->sprite.setTextureRect(IntRect(0,32,64,32));
  st->text.setFont(font);
  st->text.setColor(Color::Black);
  
  st->bordeSeleccion.setFillColor(Color(0,0,0,0));
  st->bordeSeleccion.setSize(sf::Vector2f(64, 32));
  st->bordeSeleccion.setOutlineColor(Color::White);
  st->bordeSeleccion.setOutlineThickness(1);

  genPlayerSelector(&st->player1,1);
  genPlayerSelector(&st->player2,-1);

  char* boardFile=loadFile("tableros.txt");//podría guardarla en algun lado para no volver a cargarla en el parser
  char* s=boardFile;

  int i=0;
  while(*s!=0){
    if(*s=='"'){
      s++;
      int j=1;
      for(; *(s+j)!='"'; j++);
      failIf(j>255,"board name too long");

      button b;
      memcpy(b.name,s,j);
      b.name[j]=0;
      puts(b.name);
      b.n=i;
      b.x=32+(70*i/420)*140;
      b.y=40+(i*70)%420;
      i++;
      push(&st->buttons,b);
      s+=j;
    }
    s++;
  }

  drawScreen(selectorDraw);
  actualStateUpdate=selectorUpdate;
}
void properInit(char*,int,int,int);
void selectorUpdate(char* mem){
  selectorState* st=(selectorState*)mem;
  if(input.click()){
    for(int i=0;
        i<st->buttons.size;
        i++){
      button* b=&st->buttons[i];
      if(buttonClicked(b,2)){
        properInit(mem,b->n,st->player1.selected,st->player2.selected);
        return;
      }
    }
    updatePlayerSelector(&st->player1);
    updatePlayerSelector(&st->player2);
  }
}
void playerSelectorDraw(selectorState* st,playerSelector* ps){
  for(int i=0;
      i<ps->buttonQ;
      i++){
    drawButton(st,&ps->buttons[i]);
  }
  st->bordeSeleccion.setPosition(500+40*ps->bando,320+40*ps->selected);
  window.draw(st->bordeSeleccion);
}
void selectorDraw(char* mem){
  selectorState* st=(selectorState*)mem;

  st->sprite.setScale(2.,2.);
  st->text.setScale(1.,1.);

  for(int i=0;
      i<st->buttons.size;
      i++){
    drawButton(st,&st->buttons[i]);
  }

  st->sprite.setScale(1.,1.);
  st->text.setScale(.5,.5);

  playerSelectorDraw(st,&st->player1);
  playerSelectorDraw(st,&st->player2);
}
