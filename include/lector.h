#ifndef LECTOR_H
#define LECTOR_H
#include <fstream>
#include <global.h>
using namespace std;
struct Pieza;


struct lector
{
    lector();
    ifstream archPiezas;
    ifstream archTablero;
    void leer(int);
    vector<vector<int>> matriz;
    void mostrar();
    int stringToIntR(string&);

    Holder* crearPieza(int);
    enum token{def,llaveizq,llaveder,coma,lineJoin,
    W,A,S,D,N,posRemember,posRestore,numSet,numAdd,numSeti,numAddi,
    mov,capt,
    vacio,pieza,enemigo,esp,outbounds,prob,numCmp,numDst,numCmpi,numDsti,numLess,numLessi,
    desliz,opt,bloque,
    nc,t,exc,
    click,contr,
    color,pausa,
    eol,sep,lim,last};
    bool hayAtras,doEsp;
    int i,j;
    map<int,list<int>> defs;
    list<int>* lista;
    map<string,int> tabla;
    int extra;
    bool token(string,char);
    void token(string);
    void token(char);
    void tokenizarLinea(string);
    void cargarDefs();
};


#endif // LECTOR_H
