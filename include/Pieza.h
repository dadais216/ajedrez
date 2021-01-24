#ifndef PIEZA_H
#define PIEZA_H
struct operador;
struct Piece;
struct pBase{
  operador* root;
  memLocalt memLocal;
  int size;
};
struct Piece{
  Sprite spriteb,spriten;
  int ind;//se usa en el reciclaje en spawn
  int sn;

  barray<pBase> movs;

  int memPieceSize;
  int hsSize;
  //bool kamikase;
  //bool spawner;
};

extern vector<Tile*> pisados;


struct Holder{
  Piece* piece;
  Tile* tile;
  
  barray<int> memPiece;//podria ser 1 puntero en vez de 2 TODO
  
  barray<movHolder*> movs;
  bool bando;
  bool inPlay;//false cuando la pieza esta generada y capturada. Solo se usa para evitar activar triggers dinamicos a capturados
  int step;//valor que varia para indicar si la pieza se movio, para comprobar validez de triggers
};


void crearMovHolder(operador*,Base*,char**);
Holder* initHolder(Piece*,int,Tile*,bucket*);
void generar(Holder*);
void makeCli(Holder*);



#endif // PIEZA_H
