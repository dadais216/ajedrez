#include "operador.h"
#include "lector.h"

normal::normal(){
    sig=nullptr;
    while(true){
        if(tokens.empty()) return;
        int tok=tokens.front();
        tokens.pop_front();
        switch(tok){
        case W:

        case A:

        case S:

        case D:

        case sep:
            separator=true;
            return;
        case eol:
        case lim:
            return;
        case mov:
        case capt:
            accs.push_back(tokToObj(tok));break;
        case vacio:
        case enemigo:
            conds.push_back(tokToObj(tok));break;
        case desliz:
            sig=new desliz;break;
        case multi:
            sig=new multi;break;
        case or:
            sig=new or;break;
        }
    }
}

bool normal::operar(vec v){
    for(auto cond:conds){
        if(!cond(v))
            return false;
    }
    for(auto ac:accs){
        ac(v);
    }
    return true;
}

desliz::desliz(){
    //tomar movs
    inside=new normal;
    sig=new normal;
}

bool desliz::operar(vec v){
    //mover org
    int i=-1;
    while(i++,inside->operar(v));//v tendría que ser un puntero o regurjitarse
    return i;
}

or::or(){
    separator=true;
    while(separator){
        separator=false;
        ops.push_back(new normal);
    }
}

bool or::operar(vec v){
    for(auto op:ops)
        if(op->operar(v))
            return true;
    return false;
}


