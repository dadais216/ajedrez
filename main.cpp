


Manager<Texture> image;
Font font;
void (*actualStateUpdate)(char*);

constexpr int max(int a,int b){
  return a>b?a:b;
}
char stateMem[max(max(sizeof(sf::Sprite),sizeof(selectorState)),sizeof(properState))];

void arranqueInit(char*);
void doTests(char*);

int fpsLock;

bool saveBenchmark;

int main(int argc,char** argv){
  setbuf(stdout,NULL);//eso esto porque printf se lleva mal con cout. Debería dejar uno de los 2 TODO podria hacer un printf sin tipos para boludear

  image.adddir("sprites/");
  font.loadFromFile("sprites/VL-PGothic-Regular.ttf");

  if(argc==2){
    saveBenchmark=argv[1][1]!='n';
    doTests(stateMem);
    return 0;
  }

  //properInit(stateMem,0,1,4);//23
  arranqueInit(stateMem);

  float fpsLock=1./60.; //maximos fps TODO en test de velocidad que sea 0
  Clock clock;//@check investigar bien esto, por ahi deberia usar el mismo
  //que uso para los test de velocidad?


  float dt=0;
  while(true){
    dt+=clock.restart().asSeconds();//TODO esto se usa para algo?
    //TODO faltaria el sleep, ahora hay busy wait
    while(dt>fpsLock){
      dt-=fpsLock;

      handleSystemEvents();
      actualStateUpdate(stateMem);
    }
  }
}

void drawScreen(auto drawFunc){
  window.clear(sf::Color(209,177,158));
  drawFunc();
  window.display();
}

