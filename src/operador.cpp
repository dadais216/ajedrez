#include "../include/operador.h"
#include "../include/lector.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Pieza.h"
#include "movs.cpp"


bool separator;

normal::normal(bool make){
    tipo=NORMAL;
    contGenCl=false;
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
    //        cond(prob);
                cond(inicial);
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
                //cout<<"eol"<<endl;
                return;
            case lector::end:
                //cout<<"lim"<<endl;
                return;
            default:
                tokens.push_front(tok);
                //sig=tomar();
                return;
            }
        }

    }
}

void normal::operar(normalHolder* mh,Holder* h){
    normalHolder* nh=static_cast<normalHolder*>(mh);

    ///se le avisa a la base del mov para que cuando este termine se actualice su estado de validez y haga sus cosas
    basesAActualizar.insert(nh->base);


    ///habria que distinguir a los cond que no son posicionales, creo que son los de memoria nomas
    for(condt* c:conds){
        v posAct=c->pos+h->tile->pos;

        //todo podría hacerse que cuando el mov es falso se tenga en cuenta solo
        //el trigger final, el que hizo falso al mov. Creo que funcionaria para todos
        //los casos. Habria que meter el concepto de normalholders verdaderas y falsas, no se si lo valga

        ///no definitivo. lo de addTrigger esta para evitar que esp tire triggers, no sé si esp es algo final o se va
        ///a sacar. Podría volver a ponerse la idea de que todos los conds tiren triggers, depende como implemente memoria
        addTrigger=false;
        if(!c->check(h,posAct)){
            nh->valido=false;

            if(h->outbounds) return;
            if(addTrigger) tablptr->tile(posAct)->triggers.push_back({h->tile,nh,h->tile->step});
            return;
        }
        if(addTrigger) tablptr->tile(posAct)->triggers.push_back({h->tile,nh,h->tile->step});
    }
    //accs en holder ya esta generado
    //solo se actualiza la pos porque la accion (y sus parametros si tiene) no varian
    for(int i=0; i<accs.size(); i++)
        nh->accs[i]->pos=accs[i]->pos+h->tile->pos;///podria mandar el tile en vez de la pos, pero como no todas las acciones lo usan mientras menos procesado se haga antes mejor
    for(int i=0; i<colors.size(); i++)
        nh->colors[i]->pos=colors[i]->pos+h->tile->pos;

    nh->valido=true;

    if(sig)
        nh->sig->generar();
        ///no estoy seguro de si generar es necesario. La llamada podría ser sig->operar(mh->sig,h)
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
    contGenCl=true;
    inside=new normal(true);
    sig=keepOn();
}



/*
void desliz::operar(deslizHolder* mh,Holder* h){
    deslizHolder* dh=static_cast<deslizHolder*>(mh);
    Tile* posAct=h->tile;
    int i=dh->i;
    while(true){

        inside->operar(dh->movs[i],h);
        if(!dh->movs[i]->valido)
            break;
        i++;
        if(i==dh->movs.size()){
            dh->movs.push_back();
            dh->op->generarMovHolder();
            ///akward af
        }
    }
    dh->lim=dh->movs[i];
}
*/

//mira si hay algun token adelante que genere un operador
operador* keepOn(){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front())
    {
    case lector::sep:
        separator=true;
    case lector::eol:
    case lector::end:
        tokens.pop_front(); //por ahi rompe todo
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
//    caseTomar(opt);
//    caseTomar(bloque);
//    caseTomar(click);
//    caseTomar(contr);
    default:
        tokens.push_front(tok);
        return new normal(true);
    }
}

/*
bool operador::then()
{
    if(!sig)
        return true;
    return false;
    ///no se si la seguidilla se va a hacer desde aca, no creo porque no tengo acceso a el movHolder, aunque podría conseguirlo
    //return sig->operar();
}

void crearClicker()
{
//    if(cambios) new Clicker(true);
//    cambios=false;
}
*/
