

void arranqueDraw(char* mem){
  Sprite* portada=(Sprite*)mem;
  window.draw(*portada);
}
void arranqueUpdate(char*);
void arranqueInit(char* mem){
  Sprite* portada=new(mem)Sprite();
  portada->setTexture(image.get("portada.png"));
  drawScreen(arranqueDraw);
  actualStateUpdate=arranqueUpdate;
}
void selectorInit(char*);
void arranqueUpdate(char* mem){
  if(input.click()&&input.inRange()){
    selectorInit(mem);
  }
}
