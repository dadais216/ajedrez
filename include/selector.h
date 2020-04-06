
struct button{
  char name[256];
  int n,x,y;
};
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
void selectorDraw(char*);
void selectorUpdate(char*);
void selectorInit(char*);
