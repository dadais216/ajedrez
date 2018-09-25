#include "../include/operador.h"
#include "../include/lector.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Pieza.h"
#include "movs.cpp"


bool separator;
bool clickExplicit;///cuando se usa click explicitamente no se pone un click implicitamente
///al final del movimiento si este termina en una no normal
///@detail una condicion mejor ser�a no poner click implicito si el ultimo operador no normal contiene algun click explicito

normal::normal(bool make){
    tipo=NORMAL;
    makeClick=false;
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
                cond(outbounds);
                cond(pieza);
                cond(enemigo);
                case lector::esp: conds.push_back(new esp(pos));break; //podria agregarse un debug que nomas muestre cuando falle

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

    #undef acc //(TOKEN)
    #undef cond //(TOKEN)
    #undef colorr //(TOKEN)
    #undef caseT //(TIPO,TOKEN)

            case lector::sep:
                //cout<<"sep"<<endl;
                separator=true;
                return;
            case lector::eol:
                makeClick=true;
                return;
            case lector::end:
                //cout<<"lim"<<endl;
                return;
            case lector::click:
                makeClick=true;
                clickExplicit=true;
                sig=tomar();
                ///@todo mirar casos raros como dos clicks seguidos, cosas con separador, etc
                return;
            default:
                tokens.push_front(tok);
                sig=tomar();
                return;
            }
        }

    }
}

void normal::debug(){
    cout<<"normal: conds:\n";
    for(condt* c:conds)
        c->debug();
    cout<<"accs:\n";
    for(acct* a:accs)
        a->debug();
    cout<<"colors:\n";
    for(colort* c:colors)
        c->debug();
    cout<<endl;
}

desliz::desliz(){
    tipo=DESLIZ;
    makeClick=false;
    inside=new normal(true); ///@todo deber�a ser un tomar() aca no?
    sig=keepOn(&makeClick);
}
exc::exc(){
    tipo=EXC;
    makeClick=false;
    do{
        separator=false;
        operador* op=tomar();
        ops.push_back(op);
    }while(separator);
    sig=keepOn(&makeClick);
}

//mira si hay algun token adelante que genere un operador
operador* keepOn(bool* makeClick){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front())
    {
    case lector::sep:
        separator=true;
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
#define caseTomar(TOKEN) case lector::TOKEN: cout<<#TOKEN<<endl;return new TOKEN
    switch(tok)
    {
    caseTomar(desliz);
    caseTomar(exc);
//    caseTomar(opt);
//    caseTomar(bloque);
    case lector::eol:
    case lector::end:
        return nullptr;
    default:
        tokens.push_front(tok);
        return new normal(true);
    }
}
