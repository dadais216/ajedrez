
#if debugMode
void debugShowAndWait(bool);


RectangleShape posPiece;
RectangleShape posActGood;
RectangleShape posActBad;
RectangleShape posMem;
RectangleShape* tileActDebug;

std::map<void(*)(),char*> funcToWord;

//necesito diferir los dibujados de debug para que se hagan despues de que se dibujen las demas cosas.
//tengo un vector de ints donde estan las instrucciones y los datos de este frame. Esta bueno porque tengo todo
//el codigo en un lugar solo
//tecnicamente podría hacer que los getters dibujen directamente arrancando el dibujado antes de
//correr la condicion, pero como estan las cosas ahora tiene mas sentido esto. (ademas a veces requiero dibujar varias
//veces, como cuando se mueve una ventana, y no querría correr las condiciones varias veces porque tienen efecto)
//en un momento pense poner un switch de getters en dibujado, que es recursivo para indirecciones. Tiene la ventaja
//de que no necesita meter hooks en los getters, es mas comodo y simple. Pero no funcionaría porque el switch estaría
//mirando el estado DESPUES de aplicar la condicion, y algunas condiciones tienen efecto. mset l0 3 no mostraría el
//estado de l0 antes de meterle el 3, y esto haría dibujados especialmente erroneos si tengo indireccion ej mset ll0 3
vector<int> debugDrawChannel;
enum{
     tdebugSetIndirectColor=-99999,tdebugUnsetIndirectColor,
     tdebugDrawPos, 
};
int debugMultiParameterBegin;

bool debugInCondition=true;//para bloquear los getters que se comparten en acciones y condiciones de tocar cosas debug. Tambien podría dejar que las toquen y limpiar

//hay un presupuesto de alturas para cada ventana, por default:
//las memorias como se sabe cuanto van a ocupar se le intenta dar suficiente para que entre todas
//si no entran se les da una cantidad equitativa a todas las que no entran
//el texto tiene un tamaño minimo y toma mas si queda espacio
struct visualWindow{
  union{
    struct{
      int x,y;//para texto
      bool madeIt;
      bool jumpToWord;
    };
    struct{
      int cells;//para memorias
      int rows;
      bool fixed;
    };
  };
  int height;
  int size;

  int beg=0;
};

struct{
  bigVector* bv;
  int end;//para manejar poner la cola de exc, medio hack pero prefiero hacer eso a arrastrar calculos de tamaño
  int windowBeg;
  int maxScrollingHeight;
}bvectorDraw;

int debugState=2;//0 movHolder 1 opBucket 2 moveText
void handleModeSelectors();
void initDebugSystem();
void debugChangeMoveResetWindows();
void computeWindowHeights();

void debugUpdateAndDrawBvectors();
void debugUpdateAndDrawBvectorsInit(bool);

void debugDrawMemories();
void debugShowMove(bool);
void drawMoveText(operador*);
void debugShowClickerActions();

void drawNormalText(operador*);
void drawBvectorDesoptHNodes(desoptHolder*,int,sf::Color);

/*
  en un momento quise imprimir un buffer de acciones actual, e ir mostrando como se consolidan al hacer clickers.
  por esto es que las funciones de draw usan un puntero a una ventana de texto, porque se suponia que iba a haber 2

  decidi no hacerlo porque para que se actualice con cada normal tengo que recorrer el arbol de alguna forma, no basta
  con mantener un estado por los operadores que van para atras. Y recorrer el arbol no sirve porque me traería todos los
  clickers que pueda acceder, no solo el que se esta generando actualmente

  pense agregar un hook en makeClicker pero eso se llama en tiempo de carga, no en generacion. Y en generacion no se mantiene
  algo como una lista de normales, nomas se recorre y se setean valores (que despues la carga usa para generar las listas)
  Creo que la unica forma de conseguir lo que quiero es llenar de hooks movHolder para que mantenga una lista de normales validas actuales
  o algo asi, pero es un quilombo y los beneficios que da son pocos. Asi que no lo voy a hacer.

  Por ahí podría agregar una herramienta debug que, una vez generados los clickers, diga que acciones tiene cada uno. No sé que tan util sería, ver.
  Hasta ahora el unico problema que tuve con clickers es que se generaban inesperados, pero eso es porque el lenguaje era medio raro, podría mejorar el
  lenguaje para que eso no pase y listo. Cuando maneje solapamiento clickers malos no se van a esconder

  void debugShowClickerActions(){
  actionW.size=4;
  actionW.height=5;

  movHolder* root=actualHolder.nh->base->root;

  //if(root->bools&valorCadena){
  //  root->table->cargar(root,&normales);
  //}
  drawScreen([=](){
  properDraw(stateMem);
  debugDrawMemories();
  debugShowMoveAndWait(moveW.activated);
  for(int i=0;i<normales.size;i++){
  drawNormalText((*(normales.data+i))->op,true,&actionW);
  }
  });
  stall();
  }
*/
#endif

