


RenderWindow window(VideoMode(640,512),"ajedres");
Manager<Texture> image;
Font font;
Input input(&window);
void (*actualStateUpdate)();

void arranqueInit();

int main()
{
  image.adddir("sprites/");
  font.loadFromFile("sprites/VL-PGothic-Regular.ttf");

  arranqueInit();

  float fpsLock=1./60.; //maximos fps @todo en test de velocidad que sea 0
  Clock clock;//@check investigar bien esto, por ahi deberia usar el mismo
  //que uso para los test de velocidad?


  //@todo me gustaría dibujar solo cuando sea necesario, correr a 60fps solo para el input
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
          return 0;
        }
      }//@todo mover a otro thread, manejar demas eventos
    }
  }
}

#define drawScreen(drawFunc) \
  window.clear(Color(209,177,158));\
  drawFunc();\
  window.display();

