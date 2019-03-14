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
normalHolder::normalHolder(Holder* h_,normal* org,Base* base_,char** head)
:movHolder(h_,org,base_){
    op=org;
    for(auto trigInfo:op->setUpMemTriggersPerNormalHolder)
        switch(trigInfo.type){
        case 0:
            memGlobalTriggers[trigInfo.ind].perma.push_back(this);
        break;case 1:
            h->memPiezaTrigs[trigInfo.ind]->perma.push_back(this);
        break;case 2:
            turnoTrigs[h->bando==-1].push_back({h,this});
        }
    memAct.begptr=(int*)*head;
    memAct.endptr=((int*)*head)+base_->memLocalSize;
    *head=(char*)memAct.endptr;

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
    memcpy(memAct.begptr,memMov.data(),base.memLocalSize*sizeof(int));

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
        memcpy(memMov.data(),memAct.begptr,base.memLocalSize*sizeof(int));
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
            memcpy(memMov.data(),memAct.begptr,base.memLocalSize*sizeof(int));
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
deslizHolder::deslizHolder(Holder* h_,desliz* org,Base* base_,char** head)
:movHolder(h_,org,base_){
    op=org;
    movs.begptr=*head;
    movs.endptr=(char*)movs.begptr+org->insideSize;
    movs.elem.setSize(org->iterSize);
    crearMovHolder(head,h,org->inside,&base);//crear primera iteracion
    *head=(char*)movs.endptr;
    cantElems=1;
}
void deslizHolder::maybeAddIteration(int i){
    if(cantElems==i){
        char* place=(char*)movs.begptr+movs.elem.size()*i;//crearMovHolder necesita **
        crearMovHolder(&place,h,op->inside,&base);
        cantElems++;
        assert(cantElems<=movs.count());
    }
}
void deslizHolder::generar(){
    lastNotFalse=false;
    movHolder* act;
    int i=0;
    for(;;){
        act=(movHolder*)movs[i];
        act->generar();
        if(!act->valorFinal)
            break;
        i++;
        maybeAddIteration(i);
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
        movHolder* act=(movHolder*)$->movs[i];
        act->reaccionar(nh);
        if(switchToGen){
            $->lastNotFalse=false;
            for(;act->valorFinal;){
                i++;
                $->maybeAddIteration(i);
                act=(movHolder*)$->movs[i];
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
        ((movHolder*)movs[i])->cargar(norms);
    }
    if(makeClick&&!norms->empty()) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
excHolder::excHolder(Holder* h_,exc* org,Base* base_,char** head)
:movHolder(h_,org,base_){
    ops.begptr=(movHolder**)*head;
    ops.endptr=(movHolder**)(*head+org->ops.size());
    *head=(char*)ops.endptr;
    int i=0;
    for(operador* opos:org->ops){
        *(ops.begptr+i++)=(movHolder*)*head;
        crearMovHolder(head,h,opos,&base);
    }
}
void excHolder::generar(){
    int i;
    for(i=0;i<ops.count();i++){
        movHolder* branch=*ops[i];
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
        movHolder* branch=*$->ops[i];
        branch->reaccionar(nh);
        if(switchToGen){
            if(!branch->valorCadena){ ///si el ab al recalcularse se invalida generar todo devuelta, saltandolo
                int j;
                for(j=0;j<$->ops.count();j++){
                    if(i!=j){
                        movHolder* brancj=*$->ops[j];
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
    (*(ops[actualBranch]))->cargar(norms);
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
isolHolder::isolHolder(Holder* h_,isol* org,Base* base_,char** head)
:movHolder(h_,org,base_){
    inside=(movHolder*)*head;
    crearMovHolder(head,h_,org->inside,base_);
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
desoptHolder::desoptHolder(Holder* h_,desopt* org,Base* base_,char** head)
:movHolder(h_,org,base_){
    op=org;
    char* nextIteration=*head;
    char* headFirst=*head;
    for(operador* opos:org->ops){//armar base
        assert(sizeof(movHolder*)==sizeof(char*));//debe haber una forma menos sospechosa de hacer esto
        **((movHolder***)head)=(movHolder*)(nextIteration+=op->clusterSize);//head apunta a puntero al proximo cluester de movimientos,
        *head+=sizeof(node*);//que corresponde a la proxima iteracion de este movHolder
        crearMovHolder(head,h,opos,&base);
    }
    for(int i=0;i<org->ops.count();i++){//armar primer iteracion
        for(operador* opos:org->ops){
            **(movHolder***)head=nullptr;//cuando se use apunta a un espacio dinamico
            *head+=sizeof(node*);
            crearMovHolder(head,h,opos,&base);
        }
    }
    *head=headFirst+op->desoptInsideSize;
    //memAct.resize(base.memLocalSize);
    valorCadena=valorFinal=true;
}


void desoptHolder::generar(){
    /*
    ///la iteracion inicial no necesita indireccion y no tiene un movimiento raiz
    dataPass dp{&static_cast<desopt*>(op)->ops,h,&base};
    v offsetAct=offset;
    memcpy(memAct.begptr,memMov.data(),base.memLocalSize*sizeof(int));
    for(node& n:nodes){
        n.mh->generar();
        if(n.mh->valorFinal)
            generarNodos(n.nodes,dp);
        offset=offsetAct;
        memcpy(memMov.data(),memAct.begptr,base.memLocalSize*sizeof(int));
    }
    generarSig();
    */
    ///se tiene la base y primera iteracion construidos, rondas mas alla de estas usan el resto de memoria
    ///de forma dinamica. Es un punto intermedio entre tener todo construido y reservar mucha memoria que probablemente
    ///no se use, y hacer todo dinamico y estar reconstruyendo operadores en cada generacion
    ///Seria lo mas optimizado para la dama

    //creo que con limpiar la caja de movHolders de la primera iteracion antes de la generacion total
    //y sobreescribir el espacio dinamico
    //no deberia haber ningun problema de construcciones kakeadas y cosas asi
    dinamClusterHead=movs()+op->dinamClusterBaseOffset;


    int clusterOffset=0;
    char* correspondingCluster=movs()+op->clusterSize;

    v offsetAct=offset;
    for(int tam:op->movSizes){
        node* firstIter=(node*)(movs()+clusterOffset);
        //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
        movHolder* actualMov=(movHolder*)(firstIter+1);

        actualMov->generar();
        if(actualMov->valorFinal){
            firstIter->iter=(node*)correspondingCluster;
            int clusterOffset2=0;
            v offsetAct2=offset;
            for(int tam:op->movSizes){
                node* secondIter=(node*)((char*)firstIter->iter+clusterOffset2);
                movHolder* actualMov2=(movHolder*)(secondIter+1);

                actualMov2->generar();
                if(actualMov2->valorFinal){
                    secondIter->iter=(node*)dinamClusterHead;
                    construirYGenerarNodo();
                }else
                    secondIter->iter=nullptr;
                clusterOffset2+=tam;
                offset=offsetAct2;
            }
        }else
            firstIter->iter=nullptr;
        clusterOffset+=tam;
        correspondingCluster+=op->clusterSize;
        offset=offsetAct;
    }
}
void desoptHolder::construirYGenerarNodo(){
    char* clusterBeg=dinamClusterHead;
    assert(dinamClusterHead<movs()+op->desoptInsideSize);
    for(operador* opos:op->ops){
        *(movHolder**)dinamClusterHead=nullptr;
        dinamClusterHead+=sizeof(node);
        crearMovHolder(&dinamClusterHead,h,opos,&base);
    }
    int clusterOffset=0;
    v offsetAct=offset;
    for(int tam:op->movSizes){
        node* nextIter=(node*)(clusterBeg+clusterOffset);
        movHolder* actualMov=(movHolder*)(nextIter+1);

        actualMov->generar();
        if(actualMov->valorFinal){
            nextIter->iter=(node*)dinamClusterHead;
            construirYGenerarNodo();
        }
        clusterOffset+=tam;
        offset=offsetAct;
    }
}
void desoptHolder::generarNodo(node* iter){//iter valido
    int clusterOffset=0;
    v offsetAct=offset;
    for(int tam:op->movSizes){
        node* nextIter=(node*)((char*)iter+clusterOffset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        actualMov->generar();
        if(actualMov->valorFinal){
            if(nextIter->iter){
                generarNodo(nextIter->iter);
            }else{
                nextIter->iter=(node*)dinamClusterHead;
                construirYGenerarNodo();
            }
        }
        clusterOffset+=tam;
        offset=offsetAct;
    }
}
void desoptReaccionar(auto nh,desoptHolder* $,desoptHolder::node* iter){
    int offset=0;
    for(int tam:$->op->movSizes){
        desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+offset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        offset+=tam;

        actualMov->reaccionar(nh);
        if(switchToGen){
            if(actualMov->valorFinal){
                if(nextIter->iter)
                    $->generarNodo(nextIter->iter);
                else{
                    nextIter->iter=(desoptHolder::node*)$->dinamClusterHead;
                    $->construirYGenerarNodo();
                }
            }
            switchToGen=false;
            continue;
        }
        desoptReaccionar(nh,$,nextIter->iter);
    }
}
void desoptHolder::reaccionar(normalHolder* nh){
    desoptReaccionar(nh,this,(node*)movs());
}
void desoptHolder::reaccionar(vector<normalHolder*> nhs){
    desoptReaccionar(nhs,this,(node*)movs());
}
void desoptHolder::cargarNodos(node* iter,vector<normalHolder*>* norms){
    int res=norms->size();
    int offset=0;
    for(int tam:op->movSizes){
        node* nextIter=(node*)((char*)iter+offset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        if(actualMov->valorFinal){
            actualMov->cargar(norms);
            assert(nextIter->iter);
            cargarNodos(nextIter->iter,norms);
        }else if(makeClick&&!norms->empty()){
            clickers.push_back(new Clicker(norms,h));
        }
        offset+=tam;
        norms->resize(res);
    }
}
void desoptHolder::cargar(vector<normalHolder*>* norms){
    cargarNodos((node*)movs(),norms);
    if(sig)//puede que decida prohibir poner cosas despues de un desopt, obligar a envolver en un isol si se quiere
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
