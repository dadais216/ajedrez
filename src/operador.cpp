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
            case lector::mcmp:
            case lector::mset:
            case lector::madd:
            case lector::mless:
                {
                    int a1=tokens.front();tokens.pop_front();
                    int i1=tokens.front()-1000;tokens.pop_front();
                    int a2=tokens.front();tokens.pop_front();
                    int i2=tokens.front()-1000;tokens.pop_front();

                    ///un poco mas ineficiente que separar por cada uno, pero mas legible
                    ///@todo agregarle una m al principio a cada accesor para poder meter todo en macros, porque al final \
                    van a ser como 36 ifs
                    ///y el debug lo va a duplicar
                    #define CMP(T,A1,A2) lector::T==tok&&a1==lector::A1&&a2==lector::A2
                    if(CMP(mcmp,mlocal,mlocal))
                        conds.push_back(new mcond<locala,locala,mcmp<locala,locala>,&str_cmp>(locala(i1),locala(i2)));
                    else if(CMP(mcmp,mlocal,mcte))
                        conds.push_back(new mcond<locala,ctea,mcmp<locala,ctea>,&str_cmp>(locala(i1),ctea(i2)));
                    else if(CMP(mcmp,mcte,mlocal))
                        conds.push_back(new mcond<ctea,locala,mcmp<ctea,locala>,&str_cmp>(ctea(i1),locala(i2)));
                    else if(CMP(mset,mlocal,mlocal))
                        conds.push_back(new mcond<locala,locala,mset<locala,locala>,&str_set>(locala(i1),locala(i2)));
                    else if(CMP(mset,mlocal,mcte))
                        conds.push_back(new mcond<locala,ctea,mset<locala,ctea>,&str_set>(locala(i1),ctea(i2)));
                    else if(CMP(madd,mlocal,mlocal))
                        conds.push_back(new mcond<locala,locala,madd<locala,locala>,&str_add>(locala(i1),locala(i2)));
                    else if(CMP(madd,mlocal,mcte))
                        conds.push_back(new mcond<locala,ctea,madd<locala,ctea>,&str_add>(locala(i1),ctea(i2)));
                    else if(CMP(mless,mlocal,mlocal))
                        conds.push_back(new mcond<locala,locala,mless<locala,locala>,&str_less>(locala(i1),locala(i2)));
                    else if(CMP(mless,mlocal,mcte))
                        conds.push_back(new mcond<locala,ctea,mless<locala,ctea>,&str_less>(locala(i1),ctea(i2)));
                    else{
                        cout<<"operacion de memoria invalida\n";
                        exit(EXIT_FAILURE);
                    }
                    #undef CMP
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
