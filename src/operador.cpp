#include "../include/operador.h"
#include "../include/lector.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Pieza.h"
#include "movs.cpp"
#include "memGetters.cpp"

bool separator;
bool clickExplicit;///cuando se usa click explicitamente no se pone un click implicitamente
///al final del movimiento si este termina en una no normal
///@detail una condicion mejor sería no poner click implicito si el ultimo operador no normal contiene algun click explicito

string str_cmp="cmp";
string str_set="set";
string str_add="add";
string str_less="less";
string str_more="more";

normal::normal(bool make){
    tipo=NORMAL;
    hasClick=makeClick=false;
    sig=nullptr;
    if(make){
        v pos(0,0);
        bool changeInLocalMem=false,isLocalAcc=false;
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
                cond(vacio);
                cond(pieza);
                cond(enemigo);
            case lector::esp: conds.push_back(new esp(pos));break; //podria agregarse un debug que nomas muestre cuando falle
    #define acc(TOKEN) case lector::TOKEN: accs.push_back(new TOKEN(pos));break

                acc(mov);
                acc(capt);
                acc(pausa);
            case lector::spwn:
                accs.push_back(new spwn(pos,tokens.front()-1000));tokens.pop_front();break;

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
            case lector::mmore:
                {
                    ///hay 3 tipos de operaciones en memoria no local
                    ///condiciones que leen
                    ///acciones que escriben la variable izquierda y leen la derecha
                    ///@todo condiciones que escriben la variable izquierda y leen la derecha
                    getter* g[2];
                    bool write=tok==lector::mset||tok==lector::madd;
                    bool action;
                    for(int i=0;i<2;i++){
                        bool left=i==0;
                        int tg[5],j;
                        for(j=0;;j++){
                            assert(j<5);
                            tg[j]=tokens.front();tokens.pop_front();
                            if(tg[j]>=900)///@sospechoso no 1000 para tomar algunos numeros negativos
                                break;
                        }
                        if(left)
                            action=write&&tg[0]!=lector::mlocal;
                        tg[j]-=1000;
                        if(j==0){
                            assert(!(write&&left)&&"escritura en constante");
                            g[i]=new ctea(tg[0]);
                        }else{
                            bool hasIndirection=j>1;
                            if(tg[j-1]==lector::mlocal)
                                if(left){
                                    g[i]=new locala(tg[j]);
                                    changeInLocalMem=write;
                                }else
                                    if(action){
                                        g[i]=new localaAcc(tg[j]);
                                        isLocalAcc=true;
                                    }else
                                        g[i]=new locala(tg[j]);
                            else
                                if(action)
                                    if(hasIndirection)
                                        switch(tg[j-1]){
                                        case lector::mglobal: g[i]=new globalaReadNT(tg[j]); break;
                                        case lector::mpieza: g[i]=new piezaaReadNT(tg[j]);break;
                                        case lector::mtile: g[i]=new tileaReadNT(tg[j],pos);break;
                                        case lector::mother: g[i]=new otheraReadNT(tg[j],pos);break;
                                        }
                                    else
                                        if(left)
                                            switch(tg[j-1]){
                                            case lector::mglobal: g[i]=new globalaWrite(tg[j]); break;
                                            case lector::mpieza: g[i]=new piezaaWrite(tg[j]);break;
                                            case lector::mtile: g[i]=new tileaWrite(tg[j],pos);break;
                                            case lector::mother: g[i]=new otheraWrite(tg[j],pos);break;
                                            }
                                        else
                                            switch(tg[j-1]){
                                            case lector::mglobal: g[i]=new globalaReadNT(tg[j]); break;
                                            case lector::mpieza: g[i]=new piezaaReadNT(tg[j]);break;
                                            case lector::mtile: g[i]=new tileaReadNT(tg[j],pos);break;
                                            case lector::mother: g[i]=new otheraReadNT(tg[j],pos);break;
                                            }
                                else
                                    switch(tg[j-1]){
                                    case lector::mglobal:
                                        g[i]=new globalaRead(tg[j]);
                                        setUpMemTriggersPerNormalHolder.push_back(normal::setupTrigInfo{true,tg[j],static_cast<getterCondTrig*>(g[i])});
                                    break;case lector::mpieza:
                                        g[i]=new piezaaRead(tg[j]);
                                        setUpMemTriggersPerNormalHolder.push_back(normal::setupTrigInfo{false,tg[j],static_cast<getterCondTrig*>(g[i])});
                                    break;
                                    case lector::mtile: g[i]=new tileaRead(tg[j],pos);break;
                                    case lector::mother: g[i]=new otheraRead(tg[j],pos);break;
                                    }

                            for(int k=j-2;k>=0;k--){//getters indirectos
                                hasIndirection=k>0;
                                if(tg[k]==lector::mlocal)
                                    if(left){
                                        g[i]=new localai(static_cast<getterCond*>(g[i]));
                                        changeInLocalMem=write;
                                    }else
                                        if(action){
                                            g[i]=new localaiAcc(g[i]);
                                            isLocalAcc=true;
                                        }else
                                            g[i]=new localai(static_cast<getterCond*>(g[i]));
                                else
                                    if(action)
                                        if(hasIndirection)
                                            switch(tg[k]){
                                            case lector::mglobal: g[i]=new globalaiReadNT(g[i]); break;
                                            case lector::mpieza: g[i]=new piezaaiReadNT(g[i]);break;
                                            case lector::mtile: g[i]=new tileaiReadNT(g[i],pos);break;
                                            case lector::mother: g[i]=new otheraiReadNT(g[i],pos);break;
                                            }
                                        else
                                            if(left)
                                                switch(tg[k]){
                                                case lector::mglobal: g[i]=new globalaiWrite(g[i]); break;
                                                case lector::mpieza: g[i]=new piezaaiWrite(g[i]);break;
                                                case lector::mtile: g[i]=new tileaiWrite(g[i],pos);break;
                                                case lector::mother: g[i]=new otheraiWrite(g[i],pos);break;
                                                }
                                            else
                                                switch(tg[k]){
                                                case lector::mglobal: g[i]=new globalaiReadNT(g[i]); break;
                                                case lector::mpieza: g[i]=new piezaaiReadNT(g[i]);break;
                                                case lector::mtile: g[i]=new tileaiReadNT(g[i],pos);break;
                                                case lector::mother: g[i]=new otheraiReadNT(g[i],pos);break;
                                                }
                                    else
                                        switch(tg[k]){
                                        case lector::mglobal:
                                            g[i]=new globalaiRead(static_cast<getterCond*>(g[i]));
                                            setUpMemTriggersPerNormalHolder.push_back(normal::setupTrigInfo{true,tg[j],static_cast<getterCondTrig*>(g[i])});
                                        break;case lector::mpieza:
                                            g[i]=new piezaaiRead(static_cast<getterCond*>(g[i]));
                                            setUpMemTriggersPerNormalHolder.push_back(normal::setupTrigInfo{false,tg[j],static_cast<getterCondTrig*>(g[i])});
                                        break;
                                        case lector::mtile: g[i]=new tileaiRead(static_cast<getterCond*>(g[i]),pos);break;
                                        case lector::mother: g[i]=new otheraiRead(static_cast<getterCond*>(g[i]),pos);break;
                                        }
                            }
                        }
                    }
                    if(action){
                        #define memCase(F) case lector::m##F: accs.push_back(new macc<m##F,&str_##F>(g[0],g[1]));break;
                            switch(tok){
                            memCase(set);
                            memCase(add);
                        }
                        #undef memCase
                        ///manejo de locales en acciones que usen memoria local que haya cambiado
                        if(isLocalAcc&&changeInLocalMem){
                            sig=new normal(true);
                            auto& accsSig=static_cast<normal*>(sig)->accs;
                            accsSig.insert(accsSig.begin(),accs.back());
                            accs.pop_back();
                            goto then;
                        }
                    }
                    else{
                        #define memCase(F) case lector::m##F: if(debugMode) \
                        conds.push_back(new debugMem(new mcond<m##F,&str_##F>(g[0],g[1]))); \
                        else conds.push_back(new mcond<m##F,&str_##F>(g[0],g[1])); break;
                        switch(tok){
                            memCase(cmp);
                            memCase(set);
                            memCase(add);
                            memCase(less);
                            memCase(more);
                        }
                        #undef memCase
                    }
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
