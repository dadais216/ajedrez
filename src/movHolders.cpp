#include "movHolders.h"
#include "Clicker.h"
#include "Pieza.h"

const int32_t valorCadena=1;
const int32_t valorFinal=1<<1;
const int32_t valor=1<<2;
const int32_t lastNotFalse=1<<2;
const int32_t makeClick=1<<3;
const int32_t hasClick=1<<4;
const int32_t doEsp=1<<5;


movHolder::movHolder(operador* op,Base* base_){
    if(!base_->beg)
        base_->beg=this;
    base=base_;
    bools|=op->bools;//setea makeClick, hasClick y doEsp en normalh
}
void movHolder::generarSig(){
    //se llama cuando valor == true
    if(sig){
        sig->generar();
        if(bools&hasClick||sig->bools&valorCadena)//puede que haya una forma de no usar el if?
            bools|=valorCadena;
        else
            bools&=~valorCadena;
        bools|=sig->bools&valorFinal;
    }else
        bools|=valorFinal|valorCadena;
}
normalHolder::normalHolder(normal* org,Base* base_,char** head)
:movHolder(org,base_){
    op=org;
    for(auto trigInfo:op->setUpMemTriggersPerNormalHolder)
        switch(trigInfo.type){
        case 0:
            memGlobalTriggers[trigInfo.ind].perma.push_back(this);
        break;case 1:
            base->h->memPiezaTrigs[trigInfo.ind]->perma.push_back(this);
        break;case 2:
            turnoTrigs[base->h->bando==-1].push_back({base->h,this});
        }
    memAct.begptr=(int*)*head;
    memAct.endptr=((int*)*head)+base_->memLocalSize;
    *head=(char*)memAct.endptr;

    relPos=op->relPos;
}
v offset;
Tile* actualTile;
AH actualHolder;
void normalHolder::generar(){
    actualHolder.h=base->h;///@optim mover esto mas atras
    actualHolder.nh=this;
    offsetAct=offset;///se setea el offset con el que arrancó la normal para tenerlo cuando se recalcula. Cuando se recalcula se setea devuelta al pedo, pero bueno. No justifica hacer una funcion aparte para el recalculo
    memcpy(memAct.begptr,memMov.data(),base->memLocalSize*sizeof(int));

    if(bools&doEsp){
        v actualPos=offset+relPos;
        if(actualPos.x<0||actualPos.x>=tablptr->tam.x||actualPos.y<0||actualPos.y>=tablptr->tam.y){
            bools&=~(valorFinal|valorCadena|valor);
            return;
        }
        actualTile=tablptr->tile(relPos+offset);
        actualTile->triggers.push_back(Trigger{base->h->tile,this,base->h->tile->step});
    }

    for(condt* c:op->conds)
        if(!c->check()){
            bools&=~(valorFinal|valorCadena|valor);
            return;
        }

    offset=relPos+offset;

    bools|=valor;
    generarSig();
}
void normalHolder::reaccionar(normalHolder* nh){
    if(nh==this){
        offset=offsetAct;
        memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
        switchToGen=true;
        actualTile=tablptr->tile(relPos+offset);
        actualTile->triggers.push_back(Trigger{base->h->tile,this,base->h->tile->step});
        int32_t doEspTemp=bools&doEsp;
        bools&=~doEsp;
        generar();
        bools|=doEspTemp;
    }else if(valor){
        reaccionarSig(nh);
    }
}
void normalHolder::reaccionar(vector<normalHolder*> nhs){
    for(normalHolder* nh:nhs){
        if(nh==this){
            offset=offsetAct;
            memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
            switchToGen=true;
            actualTile=tablptr->tile(relPos+offset);
            actualTile->triggers.push_back(Trigger{base->h->tile,this,base->h->tile->step});
            int32_t doEspTemp=bools&doEsp;
            bools&=~doEsp;
            generar();
            bools|=doEspTemp;
            ///@optim sacar nh del vector
            return;
        }
    }
    if(valor){
        reaccionarSig(nhs);
    }
}

void normalHolder::accionar(){
    actualHolder.h=base->h;
    actualHolder.nh=this;
    actualTile=tablptr->tile(relPos+offsetAct);
    for(acct* ac:op->accs)
        ac->func();
}
void normalHolder::cargar(vector<normalHolder*>* norms){
    if(!(bools&valorCadena)) return;
    norms->push_back(this);
    if(bools&makeClick)
        clickers.emplace_back(norms,base->h);
    if(sig)
        sig->cargar(norms);
}
void normalHolder::draw(){
    actualPosColor=offsetAct+relPos;
    for(colort* c:op->colors)
        c->draw();
}
deslizHolder::deslizHolder(desliz* org,Base* base_,char** head)
:movHolder(org,base_){
    op=org;
    movs.begptr=*head;
    movs.endptr=(char*)movs.begptr+org->insideSize;
    movs.elem.setSize(org->iterSize);
    crearMovHolder(head,org->inside,base);//crear primera iteracion
    *head=(char*)movs.endptr;
    cantElems=1;
}
void deslizHolder::maybeAddIteration(int i){
    if(cantElems==i){
        char* place=(char*)movs.begptr+movs.elem.size()*i;//crearMovHolder necesita **
        crearMovHolder(&place,op->inside,base);
        cantElems++;
        assert(cantElems<=movs.count());
    }
}
void deslizHolder::generar(){
    movHolder* act;
    int i=0;
    for(;;){
        act=(movHolder*)movs[i];
        act->generar();
        if(!(act->bools&valorFinal))
            break;
        i++;
        maybeAddIteration(i);
    }
    if(act->bools&valorCadena)
        bools|=lastNotFalse;
    else
        bools&=~lastNotFalse;
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
            for(;act->bools&valorFinal;){
                i++;
                $->maybeAddIteration(i);
                act=(movHolder*)$->movs[i];
                act->generar();
            }
            if(act->bools&valorCadena)
                $->bools|=lastNotFalse;
            else
                $->bools&=~lastNotFalse;
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
    if(!(bools&valorCadena)) return;
    for(int i=0;i<(bools&lastNotFalse?f+1:f);i++){
        ((movHolder*)movs[i])->cargar(norms);
    }
    if(bools&makeClick&&!norms->empty()) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
        clickers.emplace_back(norms,base->h);
    if(sig)
        sig->cargar(norms);
}
excHolder::excHolder(exc* org,Base* base_,char** head)
:movHolder(org,base_){
    ops.begptr=(movHolder**)*head;
    ops.endptr=(movHolder**)(*head+org->ops.size());
    *head=(char*)ops.endptr;
    int i=0;
    for(operador* opos:org->ops){
        *(ops.begptr+i++)=(movHolder*)*head;
        crearMovHolder(head,opos,base);
    }
}
void excHolder::generar(){
    int i;
    for(i=0;i<ops.count();i++){
        movHolder* branch=*ops[i];
        branch->generar();
        if(branch->bools&valorCadena){
            bools|=valor;
            actualBranch=i; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que valorCadena==true
            generarSig();
            return;
        }
    }
    bools&=~(valorFinal|valorCadena|valor);
    actualBranch=i-1;
}
void excReaccionar(auto nh,excHolder* $){
    for(int i=0;i<=$->actualBranch;i++){
        movHolder* branch=*$->ops[i];
        branch->reaccionar(nh);
        if(switchToGen){
            if(!(branch->bools&valorCadena)){ ///si el ab al recalcularse se invalida generar todo devuelta, saltandolo
                int j;
                for(j=0;j<$->ops.count();j++){
                    if(i!=j){
                        movHolder* brancj=*$->ops[j];
                        brancj->generar();
                        if(brancj->bools&valorCadena){
                            $->bools|=valor;
                            $->actualBranch=j;
                            $->generarSig();
                            return;
                        }
                    }
                }
                $->bools&=~(valorCadena|valor);
                $->actualBranch=j-1;
                return;
            }else{ ///se valido una rama que era invalida
                $->actualBranch=i;
                $->bools|=valor;
                $->generarSig();
                return;
            }
        }
    }
    if($->bools&valor)
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
        clickers.emplace_back(norms,base->h);
    if(sig)
        sig->cargar(norms);
}
isolHolder::isolHolder(isol* org,Base* base_,char** head)
:movHolder(org,base_){
    inside=(movHolder*)*head;
    crearMovHolder(head,org->inside,base_);
    bools|=valorFinal|valorCadena;
}
void isolHolder::generar(){
    v tempPos=offset;
    inside->generar();
    offset=tempPos;
    ///@todo reestablecer memoria
    if(sig){
        sig->generar();
        bools|=sig->bools&valorFinal;///necesario?
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
    if(bools&makeClick&&!normExt.empty())//evitar generar clickers sin normales
        clickers.emplace_back(norms,base->h);
    if(sig)
        sig->cargar(norms);
}

vector<int> memGenStack;//copia de memoria actual para reestablecer en el cambio de rama en generaciones. Es un stack de arrays aplanado
desoptHolder::desoptHolder(desopt* org,Base* base_,char** head)
:movHolder(org,base_){
    op=org;
    char* nextIteration=*head;
    char* headFirst=*head;
    for(operador* opos:org->ops){//armar base
        assert(sizeof(movHolder*)==sizeof(char*));//debe haber una forma menos sospechosa de hacer esto
        **((movHolder***)head)=(movHolder*)(nextIteration+=op->clusterSize);//head apunta a puntero al proximo cluester de movimientos,
        *head+=sizeof(node*);//que corresponde a la proxima iteracion de este movHolder
        crearMovHolder(head,opos,base);
    }
    for(int i=0;i<org->ops.count();i++){//armar primer iteracion
        for(operador* opos:org->ops){
            **(movHolder***)head=nullptr;//cuando se use apunta a un espacio dinamico
            *head+=sizeof(node*);
            crearMovHolder(head,opos,base);
        }
    }
    *head=headFirst+op->desoptInsideSize;
    bools|=valorCadena|valorFinal;
}
void desoptHolder::generar(){
    //se tiene la base y primera iteracion construidos, rondas mas alla de estas usan el resto de memoria
    //de forma dinamica. Es un punto intermedio entre tener todo construido y reservar mucha memoria que probablemente
    //no se use, y hacer todo dinamico y estar reconstruyendo operadores en cada generacion
    //Seria lo mas optimizado para la dama

    //con limpiar la caja de movHolders de la primera iteracion antes de la generacion total
    //y sobreescribir el espacio dinamico no deberia haber ningun problema de construcciones kakeadas y cosas asi
    dinamClusterHead=movs()+op->dinamClusterBaseOffset;


    int clusterOffset=0;
    char* correspondingCluster=movs()+op->clusterSize;

    v offsetAct=offset;

    memGenStack.insert(memGenStack.end(),memMov.begin(),memMov.end());

    for(int tam:op->movSizes){
        node* firstIter=(node*)(movs()+clusterOffset);
        //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
        movHolder* actualMov=(movHolder*)(firstIter+1);

        actualMov->generar();
        if(actualMov->bools&valorFinal){
            firstIter->iter=(node*)correspondingCluster;
            int clusterOffset2=0;
            v offsetAct2=offset;
            memGenStack.insert(memGenStack.end(),memMov.begin(),memMov.end());

            for(int tam:op->movSizes){
                node* secondIter=(node*)((char*)firstIter->iter+clusterOffset2);
                movHolder* actualMov2=(movHolder*)(secondIter+1);

                actualMov2->generar();
                if(actualMov2->bools&valorFinal){
                    secondIter->iter=(node*)dinamClusterHead;
                    construirYGenerarNodo();
                }else
                    secondIter->iter=nullptr;
                clusterOffset2+=tam;
                offset=offsetAct2;
                memMov.assign(memGenStack.end()-memMov.size(),memGenStack.end());
            }
            memGenStack.erase(memGenStack.end()-memMov.size(),memGenStack.end());
        }else
            firstIter->iter=nullptr;
        clusterOffset+=tam;
        correspondingCluster+=op->clusterSize;
        offset=offsetAct;
        memMov.assign(memGenStack.end()-memMov.size(),memGenStack.end());
    }
    memGenStack.erase(memGenStack.end()-memMov.size(),memGenStack.end());
}
void desoptHolder::construirYGenerarNodo(){
    char* clusterBeg=dinamClusterHead;
    assert(dinamClusterHead<movs()+op->desoptInsideSize);
    for(operador* opos:op->ops){
        *(movHolder**)dinamClusterHead=nullptr;
        dinamClusterHead+=sizeof(node);
        crearMovHolder(&dinamClusterHead,opos,base);
    }
    int clusterOffset=0;
    v offsetAct=offset;
    memGenStack.insert(memGenStack.end(),memMov.begin(),memMov.end());

    for(int tam:op->movSizes){
        node* nextIter=(node*)(clusterBeg+clusterOffset);
        movHolder* actualMov=(movHolder*)(nextIter+1);

        actualMov->generar();
        if(actualMov->bools&valorFinal){
            nextIter->iter=(node*)dinamClusterHead;
            construirYGenerarNodo();
        }
        clusterOffset+=tam;
        offset=offsetAct;
        memMov.assign(memGenStack.end()-memMov.size(),memGenStack.end());
    }
    memGenStack.erase(memGenStack.end()-memMov.size(),memGenStack.end());
}
void desoptHolder::generarNodo(node* iter){//iter valido
    int clusterOffset=0;
    v offsetAct=offset;
    memGenStack.insert(memGenStack.end(),memMov.begin(),memMov.end());
    for(int tam:op->movSizes){
        node* nextIter=(node*)((char*)iter+clusterOffset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        actualMov->generar();
        if(actualMov->bools&valorFinal){
            if(nextIter->iter){
                generarNodo(nextIter->iter);
            }else{
                nextIter->iter=(node*)dinamClusterHead;
                construirYGenerarNodo();
            }
        }
        clusterOffset+=tam;
        offset=offsetAct;
        memMov.assign(memGenStack.end()-memMov.size(),memGenStack.end());
    }
    memGenStack.erase(memGenStack.end()-memMov.size(),memGenStack.end());
}
void desoptReaccionar(auto nh,desoptHolder* $,desoptHolder::node* iter){
    int offset=0;
    for(int tam:$->op->movSizes){
        desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+offset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        offset+=tam;

        actualMov->reaccionar(nh);
        if(actualMov->bools&valorFinal){
            if(switchToGen){
                if(nextIter->iter)
                    $->generarNodo(nextIter->iter);
                else{
                    nextIter->iter=(desoptHolder::node*)$->dinamClusterHead;
                    $->construirYGenerarNodo();
                }
            }else
                desoptReaccionar(nh,$,nextIter->iter);
        }
        switchToGen=false;
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
        if(actualMov->bools&valorFinal){
            actualMov->cargar(norms);
            assert(nextIter->iter);
            cargarNodos(nextIter->iter,norms);
        }else if(makeClick&&!norms->empty()){
            clickers.emplace_back(norms,base->h);
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


spawnerGen::spawnerGen(Base* base_){
    base=base_;
}
void spawnerGen::generar(){
    if(justSpawned.empty())
        return;

    vector<Holder*> justSpawnedL(justSpawned);//@optim estaria bueno usar el stack para estas cosas
    justSpawned.clear();//evitar bucles infinitos

    for(Holder* s:justSpawnedL)
        if(base->h!=s)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
            s->generar();
}
/*
void spawnerGen::reaccionar(normalHolder* nh){
    assert(false);
}
void spawnerGen::reaccionar(vector<normalHolder*> nhs){
    assert(false);
}
void spawnerGen::cargar(vector<normalHolder*>* norms){
}
*/
kamikaseCntrl::kamikaseCntrl(Base* base_){
    base=base_;
}
void kamikaseCntrl::generar(){
    if(!base->h->inPlay)
        throw nullptr;
}
/*
void kamikaseCntrl::reaccionar(normalHolder* nh){
    assert(false);
}
void kamikaseCntrl::reaccionar(vector<normalHolder*> nhs){
    assert(false);
}
void kamikaseCntrl::cargar(vector<normalHolder*>* norms){
}*/

//si spawnea y muere en el mismo turno no pasa nada porque spawn corre antes de kamikase
//si se suicida y se spawnea a si mismo
