#include "movHolders.h"
#include "Clicker.h"
#include "Pieza.h"

movHolder::movHolder(Holder* h_,operador* op,Base* base_){
    if(!base_->beg)
        base_->beg=this;
    base=*base_;
    h=h_;
    makeClick=op->makeClick;
    hasClick=op->hasClick;
}
void movHolder::generarSig(){
    //se llama cuando valor == true
    if(sig){
        sig->generar();
        valorCadena=hasClick||sig->valorCadena;
        valorFinal=sig->valorFinal;//sig->valorCadena&&
    }else
        valorFinal=valorCadena=true;
}
normalHolder::normalHolder(Holder* h_,normal* org,Base* base_)
:movHolder(h_,org,base_){
    op=org;
    for(auto trigInfo:op->setUpMemTriggersPerNormalHolder)
        switch(trigInfo.type){
        case 0:
            memGlobalTriggers[trigInfo.ind].perma.push_back({this,trigInfo.c});
        break;case 1:
            h->memPiezaTrigs[trigInfo.ind].perma.push_back({this,trigInfo.c});
        break;case 2:
            turnoTrigs[h->bando==-1].push_back({h,this,trigInfo.c});
        }
    memAct.resize(base.movSize);
}
v offset;
AH actualHolder;
void normalHolder::generar(){
    offsetAct=offset;///se setea el offset con el que arrancó la normal para tenerlo cuando se recalcula. Cuando se recalcula se setea devuelta al pedo, pero bueno. No justifica hacer una funcion aparte para el recalculo
    memcpy(memAct.data(),memMov.data(),base.movSize*sizeof(int));

    actualHolder.h=h;
    actualHolder.nh=this;
    actualHolder.tile=h->tile;
    actualHolder.offset=offset;

    for(condt* c:op->conds)
        if(!c->check()){
            valorFinal=valorCadena=valor=false;
            return;
        }
    ///@optim aca podría haber un switch para tirar los triggers de pos y mem, no sé si es mejor que tenerlos spameados en los cond

    offset=op->lastPos+offset;

    valor=true;
    generarSig();
}
void normalHolder::generarFromCond(int condIndex){
    //puede que sea medio exagerado un int. Igual creo que el tamaño no importa

    //el objetivo de esta funcion es ser un generar en regenerar mas rapido. Va directo a la condicion
    //que puso el trigger, en el caso usual saltandose el esp que esta al principio. Si es un movimiento
    //que puso varios triggers en varias posiciones, se salta las posiciones anteriores al activado

    //se podria optimizar mas para que multiples condiciones que ponen triggers de una normal en una misma posicion
    //no pongan mas de un trigger, porque no aportaria nada, pero en el lenguaje eso no pasa practicamente nunca.
    offsetAct=offset;
    memcpy(memAct.data(),memMov.data(),base.movSize*sizeof(int));

    actualHolder.h=h;
    actualHolder.nh=this;
    actualHolder.tile=h->tile;
    actualHolder.offset=offset;
    int size=op->conds.size();
    do{
        if(!op->conds[condIndex]->check()){
            valorFinal=valorCadena=valor=false;
            return;
        }
        condIndex++;
    }while(condIndex!=size);
    offset=op->lastPos+offset;

    valor=true;
    generarSig();
}
void normalHolder::reaccionar(activeTrig at){
    if(at.nh==this){
        offset=at.nh->offsetAct;
        memcpy(memMov.data(),memAct.data(),base.movSize*sizeof(int));
        switchToGen=true;
        generarFromCond(at.condIndex);
        ///@optim mecanismo para cortar todo si la validez no vario, un lngjmp
        ///@optim si es verdadero hacerlo falso directamente? Creo que los unicos casos donde puede mantenerse verdadero es si
        ///una pieza va y vuelve, y por ahi eso se puede manejar haciendo que active el trigger dos veces o alguna otra cosa.
        ///en estos casos sería mas ineficiente porque se hace el recorrido dos veces, pero son casos muy raros. El general seria
        ///mas eficiente porque se salta un recalculo de una normal entera
    }else if(valor){
        reaccionarSig(at);
    }
}
void normalHolder::reaccionar(vector<activeTrig> ats){
    for(activeTrig& at:ats){
        if(at.nh==this){
            offset=at.nh->offsetAct;
            memcpy(memMov.data(),memAct.data(),base.movSize*sizeof(int));
            switchToGen=true;
            generarFromCond(at.condIndex);
            ///@optim sacar nh del vector?
            return;
        }
    }
    if(valor){
        reaccionarSig(ats);
    }
}

void normalHolder::accionar(){
    actualHolder.h=h;
    actualHolder.nh=this;
    actualHolder.offset=offsetAct;
    actualHolder.tile=h->tile;
    for(acct* ac:op->accs)
        ac->func();
}
void normalHolder::cargar(vector<normalHolder*>* norms){
    if(!valorCadena) return;///@optim poner if antes de llamada
    norms->push_back(this);
    //cout<<"#"<<makeClick<<"  ";
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
void normalHolder::draw(){
    actualHolder.offset=offsetAct;
    for(colort* c:op->colors)
        c->draw();
}
deslizHolder::deslizHolder(Holder* h_,desliz* org,Base* base_)
:movHolder(h_,org,base_){
    op=org;
    movs.reserve(10*sizeof(movHolder));///@todo @optim temporal, eventualmente voy a usar buckets
    movs.push_back(crearMovHolder(h,org->inside,&base));
}
void deslizHolder::generar(){
    lastNotFalse=false;
    movHolder* act;
    int i=0;
    for(;;){
        act=movs[i];
        act->generar();
        if(!act->valorFinal)
            break;
        i++;
        if(movs.size()==i)
            movs.push_back(crearMovHolder(h,op->inside,&base));
    }
    if(act->valorCadena)
        lastNotFalse=true;
    f=i;
    generarSig();
}
int x=0;
bool switchToGen;
void deslizReaccionar(auto at,deslizHolder* $){
    for(int i=0;i<=$->f;i++){
        movHolder* act=$->movs[i];
        act->reaccionar(at);
        if(switchToGen){
            $->lastNotFalse=false;
            for(;act->valorFinal;){
                i++;
                if($->movs.size()==i)
                    $->movs.push_back(crearMovHolder($->h,$->op->inside,&$->base));
                act=$->movs[i];
                act->generar();
            }
            if(act->valorCadena)
                $->lastNotFalse=true;
            $->f=i;
            $->generarSig();
            return;
        }
    }
    $->reaccionarSig(at);
}
void deslizHolder::reaccionar(activeTrig at){
    deslizReaccionar(at,this);
}
void deslizHolder::reaccionar(vector<activeTrig> ats){
    deslizReaccionar(ats,this);
}
void deslizHolder::cargar(vector<normalHolder*>* norms){
    if(!valorCadena) return;
    for(int i=0;i<(lastNotFalse?f+1:f);i++){
        movs[i]->cargar(norms);
    }
    if(makeClick&&!norms->empty()) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
excHolder::excHolder(Holder* h_,exc* org,Base* base_)
:movHolder(h_,org,base_){
    ops.reserve(10*sizeof(movHolder));///@todo @optim temporal, eventualmente voy a usar buckets
    for(operador* opos:org->ops)
        ops.push_back(crearMovHolder(h,opos,&base));
}
void excHolder::generar(){
    int i;
    for(i=0;i<ops.size();i++){
        movHolder* branch=ops[i];
        branch->generar();
        if(branch->valorCadena){
            valor=true;
            actualBranch=i; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que valorCadena==true
            generarSig();
            return;
        }
    }
    valorFinal=valorCadena=valor=false;
    actualBranch=i-1;
}
void excReaccionar(auto at,excHolder* $){
    for(int i=0;i<=$->actualBranch;i++){
        movHolder* branch=$->ops[i];
        branch->reaccionar(at);
        if(switchToGen){
            if(!branch->valorCadena){ ///si el ab al recalcularse se invalida generar todo devuelta, saltandolo
                int j;
                for(j=0;j<$->ops.size();j++){
                    if(i!=j){
                        movHolder* brancj=$->ops[j];
                        brancj->generar();
                        if(brancj->valorCadena){
                            $->valor=true;
                            $->actualBranch=j;
                            $->generarSig();
                            return;
                        }
                    }
                }
                $->valorCadena=$->valor=false;
                $->actualBranch=j-1;
                return;
            }else{ ///se valido una rama que era invalida
                $->actualBranch=i;
                $->valor=true;
                $->generarSig();
                return;
            }
        }
    }
    if($->valor)
        $->reaccionarSig(at);
}
void excHolder::reaccionar(activeTrig at){
    excReaccionar(at,this);
}
void excHolder::reaccionar(vector<activeTrig> ats){
    excReaccionar(ats,this);
}
void excHolder::cargar(vector<normalHolder*>* norms){
    if(!valorCadena) return;
    ops[actualBranch]->cargar(norms);
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
isolHolder::isolHolder(Holder* h_,isol* org,Base* base_)
:movHolder(h_,org,base_){
    inside=crearMovHolder(h_,org->inside,base_);
    valorFinal=valorCadena=true;
}
void isolHolder::generar(){
    v tempPos=offset;
    inside->generar();
    offset=tempPos;
    if(sig){
        sig->generar();
        valorFinal=sig->valorFinal;
    }
}
void isolReaccionar(auto at,isolHolder* $){
    $->inside->reaccionar(at);
    if(switchToGen){
        ///@optim podria hacerse un lngjmp
        switchToGen=false;
    }
    else if($->sig)
        $->sig->reaccionar(at);
}
void isolHolder::reaccionar(activeTrig at){
    isolReaccionar(at,this);
}
void isolHolder::reaccionar(vector<activeTrig> ats){
    isolReaccionar(ats,this);
}
void isolHolder::cargar(vector<normalHolder*>* norms){
    vector<normalHolder*> normExt=*norms; ///@optim agregar y cortar en lugar de copiar. O copiar aca y no en clicker devuelta
    inside->cargar(&normExt);
    if(makeClick&&!normExt.empty())//evitar generar clickers sin normales
        clickers.push_back(new Clicker(&normExt,h));
    if(sig)
        sig->cargar(norms);
}
node::node(movHolder* m):mh(m){}
desoptHolder::desoptHolder(Holder* h_,desopt* org,Base* base_)
:movHolder(h_,org,base_){
    op=org;
    nodes.reserve(org->ops.size()*sizeof(node));///@todo @optim temporal, eventualmente voy a usar buckets
    for(operador* opos:org->ops)
        nodes.emplace_back(crearMovHolder(h,opos,&base));
    memAct.resize(base.movSize);
}
struct dataPass{
    vector<operador*>* ops;
    Holder* h;
    Base* b;
};
///@optim stack dp global?
void generarNodos(vector<node*>& nodes,dataPass& dp){
    if(nodes.empty())
        for(operador* opos:*(dp.ops))
            nodes.push_back(new node(crearMovHolder(dp.h,opos,dp.b)));
    v offsetAct=offset;
    for(node* n:nodes){
        n->mh->generar();
        if(n->mh->valorFinal)
            generarNodos(n->nodes,dp);
        offset=offsetAct;
    }
}
void desoptHolder::generar(){
    ///la iteracion inicial no necesita indireccion y no tiene un movimiento raiz
    dataPass dp{&static_cast<desopt*>(op)->ops,h,&base};
    v offsetAct=offset;
    memcpy(memAct.data(),memMov.data(),base.movSize*sizeof(int));
    for(node& n:nodes){
        n.mh->generar();
        if(n.mh->valorFinal)
            generarNodos(n.nodes,dp);
        offset=offsetAct;
        memcpy(memMov.data(),memAct.data(),base.movSize*sizeof(int));
    }
    generarSig();
}
void reaccionarNodos(auto at,vector<node*>& nodes,dataPass& dp){
    for(node* n:nodes){
        n->mh->reaccionar(at);
        if(switchToGen){
            if(n->mh->valorFinal)
                generarNodos(n->nodes,dp);
            switchToGen=false;
            continue;
        }
        reaccionarNodos(at,n->nodes,dp);
    }
}
void desoptReaccionar(auto at,desoptHolder* $){
    dataPass dp{&static_cast<desopt*>($->op)->ops,$->h,&$->base};
    for(node n:$->nodes){
        n.mh->reaccionar(at);
        if(switchToGen){
            if(n.mh->valorFinal)
                generarNodos(n.nodes,dp);
            switchToGen=false;
            continue;
        }
        reaccionarNodos(at,n.nodes,dp);
    }
}
void desoptHolder::reaccionar(activeTrig at){
    desoptReaccionar(at,this);
}
void desoptHolder::reaccionar(vector<activeTrig> ats){
    desoptReaccionar(ats,this);
}
void cargarNodos(vector<node*>& nodes,vector<normalHolder*>* norms){
    int res=norms->size();
    for(node* n:nodes){
        n->mh->cargar(norms);
        if(n->mh->valorFinal)
            cargarNodos(n->nodes,norms);
        norms->resize(res);
    }
}
void desoptHolder::cargar(vector<normalHolder*>* norms){
    int res=norms->size();
    norms->reserve(100);///@todo buckets
    for(node& n:nodes){
        n.mh->cargar(norms);
        if(n.mh->valorFinal)
            cargarNodos(n.nodes,norms);
        norms->resize(res);
    }
    if(sig)
        sig->cargar(norms);
}
/*
desopt actua como un isol respecto a lo que esta antes y despues.
De no hacerlo todo lo que venga despues tendria que agregarse al final de cada rama, lo que no
es obvio por la forma en que se escribe


Si se quiere hacer algo como desliz A optar B , C end D c end se tiene que escribir
A desopt BD c A , CD c A end
Esto refleja lo que se hace y no oscurece la cantidad de calculos distintos que se hacen
(de la otra forma se podría pensar que D y A se calculan una vez en lugar de por cada rama)
@testarVelocidad con A a la izquierda, sin partir con c

*/
