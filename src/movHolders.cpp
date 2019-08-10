

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
            turnoTrigs[base->h->bando].push_back({base->h,this}); ///@check
        }
    memAct.begptr=(int*)*head;
    memAct.endptr=((int*)*head)+base_->memLocalSize;
    *head=(char*)memAct.endptr;

    relPos=op->relPos;
}

inline void normalHolder::generarProper(){
    actualHolder.nh=this;//lo usan algunas cosas de memoria y debug
    for(condt* c:op->conds)
        if(!c->check()){
            bools&=~(valorFinal|valorCadena|valor);
            return;
        }
    offset=pos;

    bools|=valor;
    generarSig();
}
void normalHolder::generar(){
    memcpy(memAct.begptr,memMov.data(),base->memLocalSize*sizeof(int));
    pos=getActualPos(relPos,offset);//pos se calcula siempre porque se usa para actualizar offset
    if(bools&doEsp){
        if(pos.x<0||pos.x>=tablptr->tam.x||pos.y<0||pos.y>=tablptr->tam.y){
            bools&=~(valorFinal|valorCadena|valor);
            return;
        }
        actualTile=tablptr->tile(pos);
        //los triggers se ponen aca y se reponen en la reaccion. Las normales esp ahora son
        //las normales que se originan despues de un movimiento. (o hacen miran memoria de tile)
        //antes eran las normales que contenian un chequeo posicional, como vacio o memoria de tile,
        //lo que tiene mas sentido. Decidi ir por este otro camino porque la gran mayoria de normales
        //que siguen un movimiento hacen un chequeo posicional (creo que las que no se podrian reescribir siempre),
        //y hacerlo por posicion tiene el problema de que en cosas como w exc mover or captura end, que son comunes,
        //se esta poniendo un trigger en las dos ramas de exc en vez de antes de este, lo que no aprovecha la normal
        //de w que va a estar ahi de todas formas y hace que se tengan multiples triggers, lo que es mas caro.
        //Puede que se pueda hacer un sistema mejor en la version compilada, aunque no sé si lo vale.
        actualTile->triggers.push_back(Trigger{base->h->tile,this,base->h->tile->step});
    }
    generarProper();
}
void normalHolder::reaccionar(normalHolder* nh){
    if(nh==this){
        memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
        switchToGen=true;
        actualTile=tablptr->tile(pos);
        actualTile->triggers.push_back(Trigger{base->h->tile,this,base->h->tile->step});
        generarProper();
        if(!(bools&valorFinal)){
            offset=getOffset(relPos,pos);
            memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
            //esta restauracion esta para que el operador que contenga reciba el offset y mem local correcta
            //la alternativa a hacer esto es que todos los operadores contenedores guarden offset y memoria local
            //para cada iteración/rama, lo que tambien tiene sus desventajas.
        }
    }else if(valor){
        reaccionarSig(nh);
        if(!(bools&valorFinal)){
            offset=getOffset(relPos,pos);
            memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
        }
    }
}
void normalHolder::reaccionar(vector<normalHolder*>* nhs){
    for(int i=0;i<nhs->size();++i){
        normalHolder* nh=(*nhs)[i];
        if(nh==this){
            memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
            switchToGen=true;
            actualTile=tablptr->tile(pos);
            actualTile->triggers.push_back(Trigger{base->h->tile,this,base->h->tile->step});
            generarProper();
            if(!(bools&valorFinal)){
                offset=getOffset(relPos,pos);
                memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
            }
            nhs->erase(nhs->begin()+i);//puede que no lo valga
            return;
        }
    }
    if(valor){
        reaccionarSig(nhs);
        if(!(bools&valorFinal)){
            offset=getOffset(relPos,pos);
            memcpy(memMov.data(),memAct.begptr,base->memLocalSize*sizeof(int));
        }
    }
}

void normalHolder::accionar(){
    actualHolder.h=base->h;
    actualHolder.nh=this;
    actualTile=tablptr->tile(pos);
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
    actualPosColor=pos;
    for(colort* c:op->colors)
        c->draw();
}
//esta version del desliz retrocede a la posicion de inicio de una iteracion si la iteracion falla
//esto la hace mas consistente en casos donde se usen multiples normales y cosas asi. Pero para
//casos comunes como la torre no se usa, agrega codigo y almacenamiento, y lo mas importante es
//que genera multiples triggers. No vale la pena agregar otra version ahora para no hacer bloat,
//pero en la version compilada podria estar. Tambien se podria agregar instrucciones bizarras
//solo de desliz sin mucho problema, como cambiar la pos de retorno o breaks.
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
        v offsetOrg=offset;
        act=(movHolder*)movs[i];
        act->generar();
        if(!(act->bools&valorFinal)){
            offset=offsetOrg;
            break;
        }
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
void deslizHolder::reaccionar(normalHolder* nh){
    if((char*)nh<(char*)this+op->insideSize)
        reaccionarNh(nh);
    else
        reaccionarSig(nh);
}
void deslizHolder::reaccionar(vector<normalHolder*>* nhs){
    if((char*)(*nhs)[0]<(char*)this+op->insideSize)
        reaccionarNh((*nhs)[0]);
    else
        reaccionarSig(nhs);
}
inline void deslizHolder::reaccionarNh(normalHolder* nh){
    assert((char*)nh>(char*)this);
    intptr_t diff=(char*)nh-(char*)this;
    int iter=diff/op->iterSize;
    for(int i=0;i<iter;++i){
        if(!(((movHolder*)movs[i])->bools&valorFinal)){
            return;
        }
    }
    movHolder* act=(movHolder*)movs[iter];
    act->reaccionar(nh);
    assert(switchToGen);
    for(;act->bools&valorFinal;){
        iter++;
        maybeAddIteration(iter);
        act=(movHolder*)movs[iter];
        act->generar();
    }
    if(act->bools&valorCadena)
        bools|=lastNotFalse;
    else
        bools&=~lastNotFalse;
    f=iter;
    generarSig();
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
    size=org->insideSize+sizeof(excHolder);
    int i=0;
    for(operador* opos:org->ops){
        *(ops.begptr+i++)=(movHolder*)*head;
        crearMovHolder(head,opos,base);
    }
}
void excHolder::generar(){
    int i;
    v offsetOrg=offset;
    for(i=0;i<ops.count();i++){
        movHolder* branch=*ops[i];
        branch->generar();
        if(branch->bools&valorCadena){
            //si una rama con un clicker puso el clicker y despues fallo se toma como rama buena,
            //pero la pos vuelve al origen. Medio raro. Por ahora lo dejo asi porque asi esta el sistema,
            //si se quiere hacer algo distinto no sería problema agregarlo en la version compilada
            bools|=valor;
            actualBranch=i; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que valorCadena==true
            generarSig();
            return;
        }
        offset=offsetOrg;
    }
    bools&=~(valorFinal|valorCadena|valor);
    actualBranch=i-1;
}
inline void excHolder::reaccionarNh(normalHolder* nh){
    movHolder* branch;
    int i;
    for(i=1;i<=actualBranch;++i){
        movHolder* nextBranch=*ops[i];
        if(nextBranch>nh){
            branch=*ops[i-1];
            goto branchFound;
        }
    }
    branch=*ops[actualBranch];
branchFound:
    branch->reaccionar(nh);
    if(switchToGen){//solo falso si la nh es innaccesible, por ejemplo esta en la parte invalida de un desliz
        if(!(branch->bools&valorCadena)){ //si el ab al recalcularse se invalida generar todo devuelta, saltandolo
            int j;
            for(j=0;j<ops.count();j++){
                if(i-1!=j){
                    movHolder* brancj=*ops[j];
                    brancj->generar();
                    if(brancj->bools&valorCadena){
                        bools|=valor;
                        actualBranch=j;
                        generarSig();
                        return;
                    }
                }
            }
            bools&=~(valorCadena|valor);
            actualBranch=j-1;
        }else{ //se valido una rama que era invalida
            actualBranch=i-1;
            bools|=valor;
            generarSig();
        }
    }
}
void excHolder::reaccionar(normalHolder* nh){
    if((char*)nh-(char*)this<size){
        reaccionarNh(nh);
    }else if(bools&valor)
        reaccionarSig(nh);
}
void excHolder::reaccionar(vector<normalHolder*>* nhs){
    //no puedo asumir que la primera normal es la que va a causar el cambio a generacion
    //porque puede que esta este en un lugar innaccesible
    for(normalHolder* nh:*nhs){
        if((char*)nh-(char*)this<size){
            reaccionarNh(nh);
            if(switchToGen){
                break;
            }
        }else{
            reaccionarSig(nh);
            return;
        }
    }
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
    size=org->size;
}
void isolHolder::generar(){
    v tempPos=offset;
    void* memTemp=alloca(base->memLocalSize*sizeof(int));
    memcpy(memTemp,memMov.data(),base->memLocalSize*sizeof(int));
    inside->generar();
    offset=tempPos;
    memcpy(memMov.data(),memTemp,base->memLocalSize*sizeof(int));
    if(sig){
        sig->generar();
        bools&=~valorFinal;
        bools|=sig->bools&valorFinal;
    }
}
void isolHolder::reaccionar(normalHolder* nh){
    if((char*)nh-(char*)this<size){
        inside->reaccionar(nh);
        if(switchToGen){
            ///@optim podria hacerse un lngjmp
            switchToGen=false;
        }
    }
    else if(sig)
        sig->reaccionar(nh);
}
void isolHolder::reaccionar(vector<normalHolder*>* nhs){
    if((char*)(*nhs)[0]-(char*)this<size){
        inside->reaccionar((*nhs)[0]);
        if(switchToGen){
            ///@optim podria hacerse un lngjmp
            switchToGen=false;
        }
    }
    if(sig)
        sig->reaccionar(nhs);
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

    v offsetOrg=offset;

    int localMemSize=base->memLocalSize*sizeof(int);
    void* memTemp=alloca(localMemSize);
    memcpy(memTemp,memMov.data(),localMemSize);

    void* memTemp2=alloca(localMemSize);

    for(int tam:op->movSizes){
        node* firstIter=(node*)(movs()+clusterOffset);
        //puede que sea mas rapido no usar un puntero para la primera iteracion, no sé
        movHolder* actualMov=(movHolder*)(firstIter+1);

        actualMov->generar();
        if(actualMov->bools&valorFinal){
            firstIter->iter=(node*)correspondingCluster;
            int clusterOffset2=0;
            v offsetOrg2=offset;
            memcpy(memTemp2,memMov.data(),localMemSize);

            for(int tam:op->movSizes){
                node* secondIter=(node*)((char*)firstIter->iter+clusterOffset2);
                movHolder* actualMov2=(movHolder*)(secondIter+1);

                actualMov2->generar();
                if(actualMov2->bools&valorFinal){
                    secondIter->iter=(node*)dinamClusterHead;
                    construirYGenerarNodo(localMemSize);
                }else
                    secondIter->iter=nullptr;
                clusterOffset2+=tam;
                offset=offsetOrg2;
                memcpy(memMov.data(),memTemp2,localMemSize);
            }
        }else
            firstIter->iter=nullptr;
        clusterOffset+=tam;
        correspondingCluster+=op->clusterSize;
        offset=offsetOrg;
        memcpy(memMov.data(),memTemp,localMemSize);
    }
}
void desoptHolder::construirYGenerarNodo(int localMemSize){
    char* clusterBeg=dinamClusterHead;
    assert(dinamClusterHead<movs()+op->desoptInsideSize);
    for(operador* opos:op->ops){
        *(movHolder**)dinamClusterHead=nullptr;
        dinamClusterHead+=sizeof(node);
        crearMovHolder(&dinamClusterHead,opos,base);
    }
    int clusterOffset=0;
    v offsetOrg=offset;
    void* memTemp=alloca(localMemSize);
    memcpy(memTemp,memMov.data(),localMemSize);

    for(int tam:op->movSizes){
        node* nextIter=(node*)(clusterBeg+clusterOffset);
        movHolder* actualMov=(movHolder*)(nextIter+1);

        actualMov->generar();
        if(actualMov->bools&valorFinal){
            nextIter->iter=(node*)dinamClusterHead;
            construirYGenerarNodo(localMemSize);
        }
        clusterOffset+=tam;
        offset=offsetOrg;
        memcpy(memMov.data(),memTemp,localMemSize);
    }
}
void desoptHolder::generarNodo(node* iter,int localMemSize){//iter valido
    int clusterOffset=0;
    v offsetOrg=offset;

    void* memTemp=alloca(localMemSize);
    memcpy(memTemp,memMov.data(),localMemSize);
    for(int tam:op->movSizes){
        node* nextIter=(node*)((char*)iter+clusterOffset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        actualMov->generar();
        if(actualMov->bools&valorFinal){
            if(nextIter->iter){
                generarNodo(nextIter->iter,localMemSize);
            }else{
                nextIter->iter=(node*)dinamClusterHead;
                construirYGenerarNodo(localMemSize);
            }
        }
        clusterOffset+=tam;
        offset=offsetOrg;
        memcpy(memMov.data(),memTemp,localMemSize);
    }
}
void reactIfNh(normalHolder* nh,movHolder* actualMov,int tam){
    if((char*)nh>=(char*)actualMov&&(char*)nh<(char*)actualMov+tam){
        actualMov->reaccionar(nh);
    }
}
void reactIfNh(vector<normalHolder*>* nhs,movHolder* actualMov,int tam){
    for(normalHolder* nh:*nhs){
        reactIfNh(nh,actualMov,tam);
    }
}
void desoptReaccionar(auto nh,desoptHolder* $,desoptHolder::node* iter){
    int offset=0;
    for(int tam:$->op->movSizes){
        desoptHolder::node* nextIter=(desoptHolder::node*)((char*)iter+offset);
        movHolder* actualMov=(movHolder*)(nextIter+1);
        offset+=tam;

        reactIfNh(nh,actualMov,tam);

        if(actualMov->bools&valorFinal){
            if(switchToGen){
                if(nextIter->iter)
                    $->generarNodo(nextIter->iter,$->base->memLocalSize*sizeof(int));
                else{
                    nextIter->iter=(desoptHolder::node*)$->dinamClusterHead;
                    $->construirYGenerarNodo($->base->memLocalSize*sizeof(int));
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
void desoptHolder::reaccionar(vector<normalHolder*>* nhs){
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

    vector<Holder*> justSpawnedL(justSpawned);//@optim estaria bueno usar el stack para estas cosas, o un move
    justSpawned.clear();//evitar bucles infinitos

    for(Holder* s:justSpawnedL)
        if(base->h!=s)//esto es un seguro contra un kamikase que se spawnea a si mismo inmediatamente
            s->generar();
}
kamikaseCntrl::kamikaseCntrl(Base* base_){
    base=base_;
}
void kamikaseCntrl::generar(){
    if(!base->h->inPlay)
        throw nullptr;
}
//si spawnea y muere en el mismo turno no pasa nada porque spawn corre antes de kamikase


