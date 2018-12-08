#include "movHolders.h"
#include "Clicker.h"
#include "Pieza.h"

movHolder::movHolder(Holder* h_,operador* org,Base* base_){
    if(!base_->beg)
        base_->beg=this;
    base=*base_;
    h=h_;
    op=org;
    makeClick=op->makeClick;
    hasClick=op->hasClick;
}
void movHolder::generarSig(){
    if(sig){
        sig->generar();
        continuar=hasClick||sig->continuar;
        allTheWay=sig->continuar&&sig->allTheWay;
    }else{
        continuar=true;
        allTheWay=true;
    }
}
normalHolder::normalHolder(Holder* h_,normal* org,Base* base_)
:movHolder(h_,org,base_){
    accs.reserve(org->accs.size()*sizeof(acct*));
    ///no es la mejor forma pero bueno
    for(acct* a:org->accs)
        accs.push_back(a->clone(h));
    colors.reserve(org->colors.size()*sizeof(colort*));
    for(colort* c:org->colors)
        colors.push_back(c->clone());

    //memAct.resize(base.movSize);
}
v offset;
vector<int> memMov;
void normalHolder::generar(){
    //cout<<"GENERANDO"<<endl;
    const normal* n=static_cast<normal*>(op);
    offsetAct=offset;///se setea el offset con el que arrancó la normal para tenerlo cuando se recalcula. Cuando se recalcula se setea devuelta al pedo, pero bueno. No justifica hacer una funcion aparte para el recalculo
    //memcpy(memAct.data(),memMov.data(),base.movSize*sizeof(int));

    for(mcondt* cm:n->condsM){///triggers de memoria (lecturas)
        if(!cm->check()){
            allTheWay=continuar=valido=false;
            return;
        }
    }
    for(condt* cnp:n->condsNP){///sin triggers
        if(!cnp->check(h)){
            allTheWay=continuar=valido=false;
            return;
        }
    }
    for(condt* cp:n->condsP){///triggers posicionales
        tablptr->tile(cp->pos+offset)->triggers.push_back({h->tile,this,h->tile->step});
        if(!cp->check(h)){
            allTheWay=continuar=valido=false;
            return;
        }
    }
    for(mcondt* cnm:n->condsNM){/// (escrituras)
        if(!cnm->check()){
            allTheWay=continuar=valido=false;
            return;
        }
    }
    //solo se actualiza la pos porque la accion (y sus parametros si tiene) no varian
    for(int i=0; i<accs.size(); i++)
        accs[i]->pos=n->accs[i]->pos+offset;///podria mandar el tile en vez de la pos, pero como no todas las acciones lo usan mientras menos procesado se haga antes mejor
    ///@optim ver si vale la pena separar accs no pos
    for(int i=0; i<colors.size(); i++)
        colors[i]->pos=n->colors[i]->pos+offset;

    offset=n->lastPos+offset;

    valido=true;
    generarSig();
}
void normalHolder::reaccionar(normalHolder* nh){
    if(nh==this){
        offset=nh->offsetAct;
        //memcpy(memMov.data(),memAct.data(),base.movSize*sizeof(int));
        switchToGen=true;
        generar();
        ///@optim mecanismo para cortar todo si la validez no vario, un lngjmp
        ///@optim si es verdadero hacerlo falso directamente? Creo que los unicos casos donde puede mantenerse verdadero es si
        ///una pieza va y vuelve, y por ahi eso se puede manejar haciendo que active el trigger dos veces o alguna otra cosa.
        ///en estos casos sería mas ineficiente porque se hace el recorrido dos veces, pero son casos muy raros. El general seria
        ///mas eficiente porque se salta un recalculo de una normal entera
    }else if(valido&&sig){
        sig->reaccionar(nh);
        if(switchToGen){
            continuar=hasClick||sig->continuar;
            allTheWay=sig->continuar&&sig->allTheWay;
        }
    }
}
void normalHolder::reaccionar(vector<normalHolder*> nhs){
    for(normalHolder* nh:nhs){
        if(nh==this){
            offset=nh->offsetAct;
            memcpy(memMov.data(),memAct.data(),base.movSize*sizeof(int));
            switchToGen=true;
            generar();
            ///@optim sacar nh del vector?
            return;
        }
    }
    if(valido&&sig){
        sig->reaccionar(nhs);
        if(switchToGen){
            continuar=hasClick||sig->continuar;
            allTheWay=sig->continuar&&sig->allTheWay;
        }
    }
}

void normalHolder::accionar(){
    for(acct* ac:accs)
        ac->func();
}
void normalHolder::cargar(vector<normalHolder*>* norms){
    if(!continuar) return;
    norms->push_back(this);
    //cout<<"#"<<makeClick<<"  ";
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
void normalHolder::draw(){
    for(colort* c:colors)
        c->draw();
}
void normalHolder::debug(){
}
deslizHolder::deslizHolder(Holder* h_,desliz* org,Base* base_)
:movHolder(h_,org,base_){
    movs.reserve(10*sizeof(movHolder));///@todo @optim temporal, eventualmente voy a usar buckets
    movs.push_back(crearMovHolder(h,org->inside,&base));
    valido=true;///desliz es siempre valido
    hasClick=org->inside->hasClick;
}
void deslizHolder::generarSig(){
    if(sig){
        sig->generar();
        continuar=hasClick||sig->continuar;
        allTheWay=sig->continuar&&sig->allTheWay;
    }else{
        continuar=lastNotFalse||f!=0;
        allTheWay=continuar; //salva algunos casos de bucles infinitos como desopt desliz A end or B end
    }
}
void deslizHolder::generar(){
    lastNotFalse=false;
    for(int i=0;;){
        movHolder* act=movs[i];
        act->generar();
        if(!act->continuar){
            f=i;
            break;
        }else if(!act->allTheWay){
            lastNotFalse=true;
            f=i;
            break;
        }
        i++;
        if(movs.size()==i)
            movs.push_back(crearMovHolder(h,static_cast<desliz*>(op)->inside,&base));
    }
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
            if(act->allTheWay){
                i++;
                if($->movs.size()==i)
                    $->movs.push_back(crearMovHolder($->h,static_cast<desliz*>($->op)->inside,&$->base));
                for(;;){
                    act=$->movs[i];
                    act->generar();
                    if(!act->continuar){
                        $->f=i;
                        break;
                    }else if(!act->allTheWay){
                        $->lastNotFalse=true;
                        $->f=i;
                        break;
                    }
                    i++;
                    if($->movs.size()==i)
                        $->movs.push_back(crearMovHolder($->h,static_cast<desliz*>($->op)->inside,&$->base));
                }
            }else if(act->continuar){
                $->lastNotFalse=true;
                $->f=i;
            }else
                $->f=i;
            $->generarSig();
            return;
        }
    }
    if($->sig)
        $->sig->reaccionar(nh);
}
void deslizHolder::reaccionar(normalHolder* nh){
    deslizReaccionar(nh,this);
}
void deslizHolder::reaccionar(vector<normalHolder*> nhs){
    deslizReaccionar(nhs,this);
}
void deslizHolder::cargar(vector<normalHolder*>* norms){
    if(!continuar) return;
    int limit=lastNotFalse?f+1:f;
    for(int i=0;i<limit;i++){
        movs[i]->cargar(norms);
    }
    if(makeClick&&!norms->empty()) ///un desliz con makeClick genera clickers incluso cuando f=0. Tiene sentido cuando hay algo antes del desliz
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
void deslizHolder::debug(){
    for(movHolder* m:movs)
        if(m->valido)
            m->debug();
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
        if(branch->continuar){
            valido=true;
            actualBranch=i; ///para ahorrar tener que buscarla en draw, reaccionar y cargar. Asegura que continuar==true
            generarSig();
            return;
        }
    }
    continuar=valido=false;
    actualBranch=i-1;
}
void excReaccionar(auto nh,excHolder* $){
    for(int i=0;i<=$->actualBranch;i++){
        movHolder* branch=$->ops[i];
        branch->reaccionar(nh);
        if(switchToGen){
            if(!branch->continuar){ ///si el ab al recalcularse se invalida generar todo devuelta, saltandolo
                int j;
                for(j=0;j<$->ops.size();j++){
                    if(i!=j){
                        movHolder* brancj=$->ops[j];
                        brancj->generar();
                        if(brancj->continuar){
                            $->valido=true;
                            $->actualBranch=j;
                            $->generarSig();
                            return;
                        }
                    }
                }
                $->continuar=$->valido=false;
                $->actualBranch=j-1;
                return;
            }else{ ///se valido una rama que era invalida
                $->actualBranch=i;
                $->valido=true;
                $->generarSig();
                return;
            }
        }
    }
    if($->valido&&$->sig)
        $->sig->reaccionar(nh);
}
void excHolder::reaccionar(normalHolder* nh){
    excReaccionar(nh,this);
}
void excHolder::reaccionar(vector<normalHolder*> nhs){
    excReaccionar(nhs,this);
}
void excHolder::cargar(vector<normalHolder*>* norms){
    if(!continuar) return;
    ops[actualBranch]->cargar(norms);
    if(makeClick)
        clickers.push_back(new Clicker(norms,h));
    if(sig)
        sig->cargar(norms);
}
void excHolder::debug(){}
isolHolder::isolHolder(Holder* h_,isol* org,Base* base_)
:movHolder(h_,org,base_){
    inside=crearMovHolder(h_,org->inside,base_);
    selfCount=0;
    allTheWay=continuar=valido=true;
}
void isolHolder::generar(){
    tempPos=offset;
    inside->generar();
    offset=tempPos;
    if(sig){
        sig->generar();
        allTheWay=sig->allTheWay;
    }
}
void isolReaccionar(auto nh,isolHolder* $){
    $->inside->reaccionar(nh);
    if(switchToGen){
        ///@optim podria hacerse un lngjmp
        //offset=$->tempPos;
        //if($->sig)
        //    $->sig->generar();
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
    if(selfCount<isolCount){
        selfCount=isolCount;

        vector<normalHolder*> normExt=*norms; ///@optim agregar y cortar en lugar de copiar. O copiar aca y no en clicker devuelta
        inside->cargar(&normExt);
        if(makeClick)
            clickers.push_back(new Clicker(&normExt,h));
    }
    if(sig)
        sig->cargar(norms);
}
void isolHolder::debug(){}

node::node(movHolder* m):mh(m){}
desoptHolder::desoptHolder(Holder* h_,desopt* org,Base* base_)
:movHolder(h_,org,base_){
    nodes.reserve(org->ops.size()*sizeof(node));///@todo @optim temporal, eventualmente voy a usar buckets
    for(operador* opos:org->ops)
        nodes.emplace_back(crearMovHolder(h,opos,&base));
    valido=true;
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
        if(n->mh->allTheWay)
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
        if(n.mh->allTheWay)
            generarNodos(n.nodes,dp);
        offset=offsetAct;
        memcpy(memMov.data(),memAct.data(),base.movSize*sizeof(int));
    }
    generarSig();
}
void reaccionarNodos(auto nh,vector<node*>& nodes,dataPass& dp){
    for(node* n:nodes){
        n->mh->reaccionar(nh);
        if(switchToGen){
            if(n->mh->allTheWay)
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
            if(n.mh->allTheWay)
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
        if(n->mh->allTheWay)
            cargarNodos(n->nodes,norms);
        norms->resize(res);
    }
}
void desoptHolder::cargar(vector<normalHolder*>* norms){
    int res=norms->size();
    norms->reserve(100);///@todo buckets
    for(node& n:nodes){
        n.mh->cargar(norms);
        if(n.mh->allTheWay)
            cargarNodos(n.nodes,norms);
        norms->resize(res);
    }
    if(sig)
        sig->cargar(norms);
}
void desoptHolder::debug(){}
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