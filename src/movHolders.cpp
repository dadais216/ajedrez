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
            memGlobalTriggers[trigInfo.ind].perma.push_back(this);
        break;case 1:
            h->memPiezaTrigs[trigInfo.ind].perma.push_back(this);
        break;case 2:
            turnoTrigs[h->bando==-1].push_back({h,this});
        }
    memAct.resize(base.memLocalSize);
    doEsp=op->doEsp;
    relPos=op->relPos;
}
v offset;
Tile* actualTile;
AH actualHolder;
void normalHolder::generar(){
    actualHolder.h=h;
    actualHolder.nh=this;
    offsetAct=offset;///se setea el offset con el que arrancó la normal para tenerlo cuando se recalcula. Cuando se recalcula se setea devuelta al pedo, pero bueno. No justifica hacer una funcion aparte para el recalculo
    memcpy(memAct.data(),memMov.data(),base.memLocalSize*sizeof(int));

    if(doEsp){
        v actualPos=offset+relPos;
        if(actualPos.x<0||actualPos.x>=tablptr->tam.x||actualPos.y<0||actualPos.y>=tablptr->tam.y){
            valorFinal=valorCadena=valor=false;
            return;
        }
        actualTile=tablptr->tile(relPos+offset);
        actualTile->triggers.push_back(Trigger{h->tile,this,h->tile->step});
    }

    for(condt* c:op->conds)
        if(!c->check()){
            valorFinal=valorCadena=valor=false;
            return;
        }

    offset=relPos+offset;

    valor=true;
    generarSig();
}
void normalHolder::reaccionar(normalHolder* nh){
    if(nh==this){
        offset=offsetAct;
        memcpy(memMov.data(),memAct.data(),base.memLocalSize*sizeof(int));
        switchToGen=true;
        actualTile=tablptr->tile(relPos+offset);
        actualTile->triggers.push_back(Trigger{h->tile,this,h->tile->step});
        bool doEspTemp=doEsp;
        doEsp=false;
        generar();
        doEsp=doEspTemp;
    }else if(valor){
        reaccionarSig(nh);
    }
}
void normalHolder::reaccionar(vector<normalHolder*> nhs){
    for(normalHolder* nh:nhs){
        if(nh==this){
            offset=offsetAct;
            memcpy(memMov.data(),memAct.data(),base.memLocalSize*sizeof(int));
            switchToGen=true;
            actualTile=tablptr->tile(relPos+offset);
            actualTile->triggers.push_back(Trigger{h->tile,this,h->tile->step});
            bool doEspTemp=doEsp;
            doEsp=false;
            generar();
            doEsp=doEspTemp;
            ///@optim sacar nh del vector
            return;
        }
    }
    if(valor){
        reaccionarSig(nhs);
    }
}

void normalHolder::accionar(){
    actualHolder.h=h;
    actualHolder.nh=this;
    actualTile=tablptr->tile(relPos+offsetAct);
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
    actualPosColor=offsetAct+relPos;
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
void deslizReaccionar(auto nh,deslizHolder* $){
    for(int i=0;i<=$->f;i++){
        movHolder* act=$->movs[i];
        act->reaccionar(nh);
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
    $->reaccionarSig(nh);
}
void deslizHolder::reaccionar(normalHolder* nh){
    deslizReaccionar(nh,this);
}
void deslizHolder::reaccionar(vector<normalHolder*> nhs){
    deslizReaccionar(nhs,this);
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
void excReaccionar(auto nh,excHolder* $){
    for(int i=0;i<=$->actualBranch;i++){
        movHolder* branch=$->ops[i];
        branch->reaccionar(nh);
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
        $->reaccionarSig(nh);
}
void excHolder::reaccionar(normalHolder* nh){
    excReaccionar(nh,this);
}
void excHolder::reaccionar(vector<normalHolder*> nhs){
    excReaccionar(nhs,this);
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
void isolReaccionar(auto nh,isolHolder* $){
    $->inside->reaccionar(nh);
    if(switchToGen){
        ///@optim podria hacerse un lngjmp
        switchToGen=false;
    }
    else if($->sig)
        $->sig->reaccionar(nh);
}
void isolHolder::reaccionar(normalHolder* nh){
    isolReaccionar(nh,this);
}
void isolHolder::reaccionar(vector<normalHolder*> nhs){
    isolReaccionar(nhs,this);
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
    memAct.resize(base.memLocalSize);
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
    memcpy(memAct.data(),memMov.data(),base.memLocalSize*sizeof(int));
    for(node& n:nodes){
        n.mh->generar();
        if(n.mh->valorFinal)
            generarNodos(n.nodes,dp);
        offset=offsetAct;
        memcpy(memMov.data(),memAct.data(),base.memLocalSize*sizeof(int));
    }
    generarSig();
}
void reaccionarNodos(auto nh,vector<node*>& nodes,dataPass& dp){
    for(node* n:nodes){
        n->mh->reaccionar(nh);
        if(switchToGen){
            if(n->mh->valorFinal)
                generarNodos(n->nodes,dp);
            switchToGen=false;
            continue;
        }
        reaccionarNodos(nh,n->nodes,dp);
    }
}
void desoptReaccionar(auto nh,desoptHolder* $){
    dataPass dp{&static_cast<desopt*>($->op)->ops,$->h,&$->base};
    for(node n:$->nodes){
        n.mh->reaccionar(nh);
        if(switchToGen){
            if(n.mh->valorFinal)
                generarNodos(n.nodes,dp);
            switchToGen=false;
            continue;
        }
        reaccionarNodos(nh,n.nodes,dp);
    }
}
void desoptHolder::reaccionar(normalHolder* nh){
    desoptReaccionar(nh,this);
}
void desoptHolder::reaccionar(vector<normalHolder*> nhs){
    desoptReaccionar(nhs,this);
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
