


RenderWindow window(VideoMode(640,512),"ajedres");
Manager<Texture> image;
Font font;
Input input(&window);
void (*actualStateUpdate)(char*);

constexpr int max(int a,int b){
  return a>b?a:b;
}
char stateMem[max(max(sizeof(sf::Sprite),sizeof(selectorState)),sizeof(properState))];

void arranqueInit(char*);

int fpsLock;

int main()
{
  setbuf(stdout,NULL);//eso esto porque printf se lleva mal con cout. Debería dejar uno de los 2

  image.adddir("sprites/");
  font.loadFromFile("sprites/VL-PGothic-Regular.ttf");

  //properInit(stateMem,0,1,1);
  arranqueInit(stateMem);

  float fpsLock=1./60.; //maximos fps TODO en test de velocidad que sea 0
  Clock clock;//@check investigar bien esto, por ahi deberia usar el mismo
  //que uso para los test de velocidad?


  //TODO me gustaría dibujar solo cuando sea necesario, correr a 60fps solo para el input
  float dt=0;
  while(true){
    dt+=clock.restart().asSeconds();
    //TODO faltaria el sleep, ahora hay busy wait
    while(dt>fpsLock){
      dt-=fpsLock;

      input.check();
      actualStateUpdate(stateMem);

      Event event;
      while(window.pollEvent(event)){
        if(event.type == Event::Closed){
          window.close();
          return 0;
        }
      }//TODO mover a otro thread, manejar demas eventos
    }
  }
}

#define drawScreen(drawFunc) \
  window.clear(Color(209,177,158));\
  drawFunc(stateMem);\
  window.display();

