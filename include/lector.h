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

    Pieza* crearPieza(int);
    enum token{def,igual,llaveizq,llaveder,coma,
    W,A,S,D,
    mov,capt,
    vacio,enemigo,
    desliz,opt,multi,
    esp,eol,sep,lim,last};//last se usa para los sig token, generados por los def
    bool hayAtras;
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
