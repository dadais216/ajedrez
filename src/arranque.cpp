

bucket stateBucket;
//podria no hacerla global y pasarla por parametro, pero no veo que me aporte algo y tendria que meter el parametro en todas las funciones de estados, el puntero de funcion de update y draw

void arranqueDraw(){
  Sprite* portada=(Sprite*)stateBucket.data;
  window.draw(*portada);
}
void arranqueUpdate();
void arranqueInit(){
  allocNewBucket(&stateBucket);
  Sprite* portada=alloc<Sprite>(&stateBucket);
  portada->setTexture(image.get("portada.png"));
  drawScreen(arranqueDraw);
  actualStateUpdate=arranqueUpdate;
}
void selectorInit();
void arranqueUpdate(){
  if(input.click()&&input.inRange()){
    selectorInit();
  }
}
