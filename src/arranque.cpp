

void arranqueUpdate(char*);


void arranqueDraw(char* mem){
  Sprite* portada=(Sprite*)mem;
  window.draw(*portada);
}
void arranqueInit(char* mem){
  Sprite* portada=new(mem)Sprite();
  portada->setTexture(image.get("portada.png"));
  drawScreen([&](){arranqueDraw(stateMem);});
  actualStateUpdate=arranqueUpdate;
}
void arranqueUpdate(char* mem){
  if(Input.leftClick&&clickInRange()){
    selectorInit(mem);
  }
}
