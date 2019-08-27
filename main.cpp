


RenderWindow window(VideoMode(640,512),"ajedres");
Manager<Texture> image;
Font font;
Input input();
void (*actualStateUpdate)(void);

int main()
{
  image.adddir("sprites/");
  font.loadFromFile("sprites/VL-PGothic-Regular.ttf");
  arranqueInit();


  float fpsLock=1f/60f; //maximos fps @todo en test de velocidad que sea 0
  Clock clock;//@check investigar bien esto, por ahi deberia usar el mismo
  //que uso para los test de velocidad?

  float dt=0;
  while(true){
    dt+=clock.restart().asSeconds();
    //@todo faltaria el sleep, ahora hay busy wait
    while(dt>fpsLock){
      dt-=fpsLock;

      input.check();
      actualStateUpdate();

      Event event;
      while(window.pollEvent(event)){
        if(event.type == Event::Closed){
          window.close();
          return;
        }
      }//@todo mover a otro thread, manejar demas eventos
    }
  }
}

void drawScreen(void (*drawState)(void)){
  window.clear(Color(209,177,158));
  drawState();
  window.display();
}

