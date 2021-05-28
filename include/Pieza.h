#ifndef PIEZA_H
#define PIEZA_H
struct operador;
struct Piece;
struct pBase{
  int root;
  memLocalt memLocal;
  int size;
};
struct Piece{
  Sprite spriteb,spriten;
  int ind;//se usa en el reciclaje en spawn
  int sn;

  varray<pBase> movs;

  int memPieceSize;
  int hsSize;
  //bool kamikase;
  //bool spawner;
};

extern vector<int> pisados;


struct Holder{
  int piece;
  int tile;
  
  varray<int> memPiece;//podria ser 1 puntero en vez de 2 TODO
  
  varray<int> movs;
  bool bando;
  bool inPlay;//false cuando la pieza esta generada y capturada. Solo se usa para evitar activar triggers dinamicos a capturados
  int step;//valor que varia para indicar si la pieza se movio, para comprobar validez de triggers
};


void crearMovHolder(int,int,bigVector*);
int initHolder(Piece*,int,int,bigVector*);
void generar(Holder*);
void makeCli(Holder*);



#endif // PIEZA_H
