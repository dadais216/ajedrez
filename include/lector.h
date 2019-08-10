#ifndef LECTOR_H
#define LECTOR_H

struct lector{
    lector();
    ifstream archPiezas;
    ifstream archTablero;
    void generarIdsTablero(int);
    vector<vector<int>> matriz;

    Holder* crearPieza(int,v);
    enum token {def,llaveizq,llaveder,coma,lineJoin,
                W,A,S,D,N,
                mov,capt,spwn,pausa,
                vacio,pieza,enemigo,pass,esp,
                mcmp,mset,madd,mless,mmore,mdist,msize,
                mlocal,mglobal,mpieza,mtile,mother,turno,posX,posY,
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
    vector<int> memLocalSizes;

    void centinela(string&,char);
    void tokenizarPalabra(string&);
    void tokenizarCaracter(char);
    void tokenizarLinea(string&);
    void cerrarLinea();
    int getNum(string&);
    void cargarDefs();
    void procesarTokens(list<int>&);
};
lector lect;

int stringToInt(string&);

#endif // LECTOR_H
