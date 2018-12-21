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
    void generarIdsTablero(int);
    vector<vector<int>> matriz;

    Holder* crearPieza(int,v);
    enum token {def,llaveizq,llaveder,coma,lineJoin,
                W,A,S,D,N,
                mov,capt,spwn,pausa,pass,
                vacio,pieza,enemigo,esp,outbounds,
                mcmp,mset,madd,mless,
                mlocal,mpieza,mcte,
                desliz,exc,isol,desopt,
                click,
                color,sprt,numShow,
                eol,sep,end,last
               };
    bool hayAtras,doEsp;
    int i,j;
    map<int,list<int>> defs;
    list<int>* lista; ///@optim vector?
    map<string,int> tabla;
    map<string,int> tablaMem;
    int extra;

    int memPiezaSize;

    void centinela(string,char);
    void tokenizarPalabra(string);
    void tokenizarCaracter(char);
    void tokenizarLinea(string);
    void cargarDefs();
    void procesarTokens(list<int>&);
};

int stringToInt(string&);

#endif // LECTOR_H
