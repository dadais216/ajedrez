#include "../include/operador.h"
#include "../include/lector.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Pieza.h"
#include "movs.cpp"


bool separator;
bool clickExplicit;///cuando se usa click explicitamente no se pone un click implicitamente
///al final del movimiento si este termina en una no normal
///@detail una condicion mejor sería no poner click implicito si el ultimo operador no normal contiene algun click explicito

string str_cmp="cmp";
string str_set="set";
string str_add="add";
string str_less="less";

normal::normal(bool make){
    tipo=NORMAL;
    hasClick=makeClick=false;
    sig=nullptr;
    if(make){
        v pos(0,0);
        while(true){
            if(tokens.empty()) return;
            int tok=tokens.front();
            tokens.pop_front();
            switch(tok){
            case lector::W:
                pos.y--; //el espejado se va a tener que hacer cuando se construyan las absolutas
                break;
            case lector::S:
                pos.y++;
                break;
            case lector::D:
                pos.x++;
                break;
            case lector::A:
                pos.x--;
                break;
                //cout<<#TOKEN<<endl;
    #define cond(TOKEN)  case lector::TOKEN: if(debugMode) conds.push_back(new debugMov(new TOKEN(pos))); else conds.push_back(new TOKEN(pos));break
    //        cond(posRemember);
    //        cond(numSet);
    //        cond(numAdd);
    //        cond(numSeti);
    //        cond(numAddi);
    //        cond(numCmp);
    //        cond(numDst);
    //        cond(numCmpi);
    //        cond(numDsti);
    //        cond(numLess);
    //        cond(numLessi);

                cond(vacio);
                cond(pieza);
                cond(enemigo);
                case lector::esp: conds.push_back(new esp(pos));break; //podria agregarse un debug que nomas muestre cuando falle
                case lector::outbounds: conds.push_back(new outbounds(pos));break;
    #define acc(TOKEN) case lector::TOKEN: accs.push_back(new TOKEN(pos));break

                acc(mov);
                acc(capt);
                acc(pausa);
    //        acc(spwn);

            case lector::color:
                colors.push_back(crearColor(pos));
                break;
    //       colorr(sprt);
    //       colorr(numShow);
            case lector::msize:
                {
                    int size=tokens.front()-1000;tokens.pop_front();
                    if(size>memLocalSize)
                        memLocalSize=size;
                }
                break;
            case lector::mcmp:
            case lector::mset:
            case lector::madd:
            case lector::mless:
                {
                    getter* g[2];
                    for(int i=0;i<2;i++){
                        int tg[5],j;
                        for(j=0;;j++){
                            assert(j<5);
                            tg[j]=tokens.front();tokens.pop_front();
                            if(tg[j]>=1000)
                                break;
                        }
                        tg[j]-=1000;
                        if(j==0)
                            g[i]=new ctea(tg[0]);
                        else{
                            switch(tg[j-1]){
                            case lector::mlocal:
                                g[i]=new locala(tg[j]);
                            }
                            for(int k=j-2;k>=0;k--)//getters indirectos
                                switch(tg[k]){
                                case lector::mlocal:
                                    g[i]=new localai(g[i]);continue;
                                }
                        }
                    }
                    #define memCase(F) case lector::m##F: if(debugMode) \
                        conds.push_back(new debugMem(new mcond<m##F,&str_##F>(g[0],g[1]))); \
                        else conds.push_back(new mcond<m##F,&str_##F>(g[0],g[1])); break;
                    switch(tok){
                        memCase(cmp);
                        memCase(set);
                        memCase(add);
                        memCase(less);
                    }
                    #undef memCase
                }
                break;

    #undef acc //(TOKEN)
    #undef cond //(TOKEN)
    #undef colorr //(TOKEN)
    #undef caseT //(TIPO,TOKEN)

            case lector::sep:
                //cout<<"sep"<<endl;
                separator=true;
                goto then;
            case lector::eol:
                hasClick=makeClick=true;
                goto then;
            case lector::end:
                //cout<<"lim"<<endl;
                goto then;
            case lector::click:
                hasClick=makeClick=true;
                clickExplicit=true;
                sig=tomar();
                ///@todo mirar casos raros como dos clicks seguidos
                goto then;
            default:
                tokens.push_front(tok);
                sig=tomar();
                //hasClick=sig->hasClick;
                goto then;
            }
        }
        then:
        lastPos=pos;
    }
}

void normal::debug(){

}

desliz::desliz(){
    tipo=DESLIZ;
    makeClick=false;
    inside=tomar();
    sig=keepOn(&makeClick);

    if(makeClick)
        hasClick=true;
    else
        for(operador* op=inside;op!=nullptr;op=op->sig)
            if(op->hasClick){
                hasClick=true;
                break;
            }
}
exc::exc(){
    tipo=EXC;
    do{
        separator=false;
        operador* op=tomar();
        ops.push_back(op);
    }while(separator);
    makeClick=false;
    sig=keepOn(&makeClick);
    if(makeClick)
        hasClick=true;
    else{
        hasClick=false;
        for(operador* op:ops)
            if(op->hasClick){
                hasClick=true;
                break;
            }
    }

}
isol::isol(){
    tipo=ISOL;
    hasClick=true;
    makeClick=false;
    bool clickExplicitBack=clickExplicit;
    inside=tomar();
    if(!clickExplicit)
        makeClick=true;
    clickExplicit=clickExplicitBack;
    sig=keepOn(&makeClick);
}
desopt::desopt(){
    tipo=DESOPT;
    do{
        separator=false;
        operador* op=tomar();
        ops.push_back(op);
    }while(separator);
    makeClick=false;
    sig=keepOn(&makeClick);
    if(makeClick)
        hasClick=makeClick;
    else{
        hasClick=false;
        for(operador* op:ops)
            if(op->hasClick){
                hasClick=true;
                break;
            }
    }
}

//mira si hay algun token adelante que genere un operador
operador* keepOn(bool* makeClick){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front())
    {
    case lector::click:
        *makeClick=true;
        tokens.pop_front();
        return keepOn(makeClick);
    case lector::sep:
        separator=true;
        tokens.pop_front();
        return nullptr;
    case lector::eol:
        if(!clickExplicit)
            *makeClick=true;
    case lector::end:
        tokens.pop_front();
        return nullptr;
    }
    return tomar();
}

operador* tomar(){
    if(tokens.empty()) return nullptr;
    int tok=tokens.front();
    tokens.pop_front();
#define caseTomar(TOKEN) case lector::TOKEN: return new TOKEN
    switch(tok)
    {
    caseTomar(desliz);
    caseTomar(exc);
    caseTomar(isol);
    caseTomar(desopt);
    case lector::sep:
        separator=true;
    case lector::eol:
    case lector::end:
        return nullptr;
    default:
        tokens.push_front(tok);
        return new normal(true);
    }
}
