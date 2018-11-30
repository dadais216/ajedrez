#ifndef LECTOR_H
#define LECTOR_H
#include <fstream>
#include<vector>
#include<list>
#include<map>
#include "vec.h"
using namespace std;
struct Holder;

struct lector
{
    lector();
    ifstream archPiezas;
    ifstream archTablero;
    void leer(int);
    vector<vector<int>> matriz;
    void mostrar();
    int stringToIntR(string&);

    Holder* crearPieza(int,v);
    enum token {def,llaveizq,llaveder,coma,lineJoin,
                W,A,S,D,N,numSet,numAdd,numSeti,numAddi,
                mov,capt,spwn,pausa,pass,
                vacio,pieza,enemigo,esp,outbounds,numCmp,numDst,numCmpi,numDsti,numLess,numLessi,
                desliz,exc,isol,desopt,
                click,
                color,sprt,numShow,
                eol,sep,end,last
               };
    bool hayAtras,doEsp;
    int i,j;
    map<int,list<int>> defs;
    list<int>* lista;
    map<string,int> tabla;
    int extra;

    int piezaSize,movSize;

    bool token(string,char);
    void token(string);
    void token(char);
    void tokenizarLinea(string);
    void cargarDefs();
};


#endif // LECTOR_H
