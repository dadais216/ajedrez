
struct botonGraphics{
  Sprite sprite;
  Text text;

  RectangleShape bordeSeleccion;
  PlayerSelector player1,player2;
  boton* firstButton;
}

struct boton{
  string name;//aca hay una indireccion(y leak) que eventualmente podria sacar
  int n,x,y;//no es que lo necesite pero ya que estamos
  boton* next;//next es para eso, si lo dejo como ahora no sirve para nada
}

boton* buttonAllocInit(string& s,int n,int x,int y){
  boton* b=alloc<boton>(&stateBucket);
  b->name=s;
  b->n=n;
  b->x=x;
  b->y=y;
  b->next=stateBucket->head;
  return b;
}

void drawButton(boton* b){
  botonGraphics->sprite.setPosition(b->x,b->y);
  botonGraphics->text.setPosition(b->x+5,b->y+10);
  botonGraphics->text.setString(b->name);
  window.draw(botonGraphics->sprite);
  window.draw(botonGraphics->text);
}

int buttonClicked(boton* b){
  v ve=input->pixel();
  if(ve.x>=b->x&&ve.x<=b->x+128&&ve.y>=b->y&&ve.y<=b->y+64) //puede que este mal
    return b->n;
  return 0;
}

struct PlayerSelector{
  int bando;
  boton* firstButton;//medio ambiguo que necesite ser linked? ver como termine implementando lo de string en boton, si es un char[20] ya fue
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

    selected=1;
}
void updatePlayerSelector(playerSelector* ps){
  for(boton* b=ps->firstButton;
      b;
      b=b->next){
    if(buttonClicked(b)){
      ps->selected=b->n;//n-1?
      drawScreen();
    }
  }
}
