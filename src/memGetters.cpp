#include "global.h"
#include "memGetters.h"
#include "movHolders.h"


vector<int> memMov;
vector<vector<pair<normalHolder*,getterCondTrig*>>> memGlobalTriggers;
vector<int> memGlobal;
int maxMemMovSize=0;
RectangleShape backGroundMemDebug;

struct locala:public getterCond{
    int ind;
    locala(int ind_):ind(ind_){
        if(ind>=memLocalSize)
            memLocalSize=ind+1;
    }
    virtual int* val(){
        return &memMov[ind];
    }
    virtual int* valFast(){
        return &memMov[ind];
    }
    virtual void drawDebugMem(){
        int memSize=actualHolder.nh->base.movSize;
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),405+45*(ind/4-memSize/4)));
        window->draw(backGroundMemDebug);
    }
};
struct localaAcc:public getter{
    int ind;
    localaAcc(int ind_):ind(ind_){
        if(ind>=memLocalSize)
            memLocalSize=ind+1;///@innecesario?
    }
    virtual int* val(){
        return &actualHolder.nh->memAct[ind];
        ///@optim creo que es el unico lugar donde se usa actualHolder.nh (en acciones)
        ///como ahora las acciones no guardan informacion es la unica forma
        ///Se podria hacer algo con el stack ya que la actualHolder.nh es la normal que llama a esta funcion
        ///no creo que afecte la version compilada
    }
};
struct localai:public getterCond{
    getterCond* g;
    localai(getterCond* g_):g(g_){}
    int before;
    virtual int* val(){
        before=*g->val();//para cosas de debug. Es necesario guardar la informacion porque puede variar antes de llamar a drawDebugMem (ej mset ll0 1), de paso ahorra una llamada
        return &memMov[before];
    }
    virtual int* valFast(){
        return &memMov[*g->valFast()];
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        int memSize=actualHolder.nh->base.movSize;
        backGroundMemDebug.setPosition(Vector2f(530+25*(before%4),405+45*(before/4-memSize/4)));
        window->draw(backGroundMemDebug);
    }
};
struct localaiAcc:public getter{//aparece en cadenas de 3 o mas
    normalHolder* nh;
    getter* g;
    localaiAcc(getter* g_):g(g_){}
    virtual int* val(){
        return &actualHolder.nh->memAct[*g->val()];
    }
};
//se podrían meter templates para evitar copiar y pegar, pero no vale la pena
struct globalaWrite:public getter{
    int ind;
    globalaWrite(int ind_):ind(ind_){}
    virtual int* val(){
        for(pair<normalHolder*,getterCondTrig*> p:memGlobalTriggers[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &memGlobal[ind];
    }
};
struct globalaRead:public getterCondTrig{
    int ind;
    int* val_;
    int before;
    globalaRead(int ind_):ind(ind_),val_(&memGlobal[ind_]){}
    virtual int* val(){
        before=*val_;
        return val_;
    }
    virtual int* valFast(){
        return val_;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),305+45*(ind/4-memGlobalSize/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        return before!=*val_;
    }
};
struct globalaReadNT:public getter{
    int ind;
    globalaReadNT(int ind_):ind(ind_){}
    virtual int* val(){
        return &memGlobal[ind];
    }
};
struct globalaiWrite:public getter{
    getter* g;
    globalaiWrite(getter* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();
        for(pair<normalHolder*,getterCondTrig*> p:memGlobalTriggers[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &memGlobal[ind];
    }
};
struct globalaiRead:public getterCondTrig{
    getterCond* g;
    int before;
    int* beforeInd;
    globalaiRead(getterCond* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();

        bool notSet=true;
        for(pair<normalHolder*,getterCondTrig*>& p:memGlobalTriggers[ind])
            if(p.second==this){
                notSet=false;
                break;
            }
        if(notSet)
            memGlobalTriggers[ind].push_back(make_pair(actualHolder.nh,this));
        ///@optim lo malo de este sistema es que pone triggers dinamicos que nunca se van, y pueden estar
        //soltando triggers falso positivo a cada rato si se lee una memoria en alguna rama que no se ejecuta seguido.
        //Podría manejarse con un mecanismo de aging, pero como es algo raro en una mecanica que ya es rara, puede
        //que lo mas eficiente sea no hacer nada
        beforeInd=&memGlobal[ind];
        before=*beforeInd;
        return beforeInd;
    }
    virtual int* valFast(){
        return &memGlobal[*g->val()];
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setPosition(Vector2f(530+25*(before%4),305+45*(before/4-memGlobalSize/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        return before!=*beforeInd; //un cambio en g se maneja desde su trigger
    }
};
struct globalaiReadNT:public getter{
    getter* g;
    globalaiReadNT(getter* g_):g(g_){}
    virtual int* val(){
        return &memGlobal[*g->val()];
    }
};

struct piezaaWrite:public getter{
    int ind;
    piezaaWrite(int ind_):ind(ind_){}
    virtual int* val(){
        for(pair<normalHolder*,getterCondTrig*> p:actualHolder.h->memPiezaTrigs[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &actualHolder.h->memPieza[ind].actual;
    }
};
struct piezaaRead:public getterCondTrig{
    int ind;
    piezaaRead(int ind_):ind(ind_){}
    virtual int* val(){
        Holder::int2* ret=&actualHolder.h->memPieza[ind];
        ret->before=ret->actual;
        return &ret->actual;
    }
    virtual int* valFast(){
        return &actualHolder.h->memPieza[ind].actual;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),205+45*(ind/4-actualHolder.h->memPieza.size()/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        Holder::int2* ret=&actualHolder.nh->h->memPieza[ind];
        return ret->before!=ret->actual;
    }
};
struct piezaaReadNT:public getter{
    int ind;
    piezaaReadNT(int ind_):ind(ind_){}
    virtual int* val(){
        return &actualHolder.h->memPieza[ind].actual;
    }
};
struct piezaaiWrite:public getter{
    getter* g;
    piezaaiWrite(getter* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();
        for(pair<normalHolder*,getterCondTrig*> p:actualHolder.h->memPiezaTrigs[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &actualHolder.h->memPieza[ind].actual;
    }
};
struct piezaaiRead:public getterCondTrig{
    getterCond* g;
    piezaaiRead(getterCond* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();

        bool notSet=true;
        for(pair<normalHolder*,getterCondTrig*>& p:actualHolder.h->memPiezaTrigs[ind])
            if(p.second==this){
                notSet=false;
                break;
            }
        if(notSet)
            actualHolder.h->memPiezaTrigs[ind].push_back(make_pair(actualHolder.nh,this));
        Holder::int2* ret=&actualHolder.h->memPieza[ind];
        ret->before=ret->actual;
        return &ret->actual;
    }
    virtual int* valFast(){
        return &actualHolder.h->memPieza[*g->val()].actual;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        int ind=*g->valFast();
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),205+45*(ind/4-actualHolder.h->memPieza.size()/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        Holder::int2 vals=actualHolder.nh->h->memPieza[*g->valFast()];
        return vals.actual!=vals.before;
    }
};
struct piezaaiReadNT:public getter{
    getter* g;
    piezaaiReadNT(getter* g_):g(g_){}
    virtual int* val(){
        return &actualHolder.h->memPieza[*g->val()].actual;
    }
};

struct tileaWrite:public getter{
    int ind;
    v offset;
    tileaWrite(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        for(pair<normalHolder*,getterCondTrig*>& p:t->memTileTrigs[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &t->memTile[ind].actual;
    }
};
v posDebugTile(0,0);
struct tileaRead:public getterCondTrig{
    int ind;
    v offset;
    tileaRead(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        for(pair<normalHolder*,getterCondTrig*>& p:t->memTileTrigs[ind])
            if(p.second==this)
                goto afterSetup;
        t->memTileTrigs[ind].push_back(make_pair(actualHolder.nh,this));
        afterSetup:
        auto* ret=&t->memTile[ind];
        ret->before=ret->actual;
        return &ret->actual;
    }
    virtual int* valFast(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[ind].actual;
    }
    virtual void drawDebugMem(){
        posDebugTile=offset+actualHolder.nh->offsetAct;
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),105+45*(ind/4-memTileSize/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        auto vals=tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[ind];
        return vals.actual!=vals.before;
    }
};
struct tileaReadNT:public getter{
    int ind;
    v offset;
    tileaReadNT(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[ind].actual;
    }
};
struct tileaiWrite:public getter{
    getter* g;
    v offset;
    tileaiWrite(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        int ind=*g->val();
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        for(pair<normalHolder*,getterCondTrig*> p:t->memTileTrigs[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &t->memTile[ind].actual;
    }
};
struct tileaiRead:public getterCondTrig{
    getterCond* g;
    v offset;
    int beforeInd;
    tileaiRead(getterCond* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        beforeInd=*g->val();
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        for(pair<normalHolder*,getterCondTrig*>& p:t->memTileTrigs[beforeInd])
            if(p.second==this)
                goto afterSetup;
        t->memTileTrigs[beforeInd].push_back(make_pair(actualHolder.nh,this));
        afterSetup:
        auto* ret=&t->memTile[beforeInd];
        ret->before=ret->actual;
        return &ret->actual;
    }
    virtual int* valFast(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[*g->val()].actual;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setPosition(Vector2f(530+25*(beforeInd%4),105+45*(beforeInd/4-memTileSize/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        auto vals=tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[*g->valFast()];
        return vals.actual!=vals.before;
    }
};
struct tileaiReadNT:public getter{
    getter* g;
    v offset;
    tileaiReadNT(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[*g->val()].actual;
    }
};

///todos los other asumen que hay una pieza en la posicion actual con una memoria que contenga su indice
struct otheraWrite:public getter{
    int ind;
    v offset;
    otheraWrite(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        for(pair<normalHolder*,getterCondTrig*>& p:h->memPiezaTrigs[ind]){
            cout<<h->memPieza.size()<<"  "<<p.first->h->memPieza.size()<<"!!\n";
            if(p.first->h!=actualHolder.h) ///podria asumir que other nunca se usa sobre uno y sacar este if
                trigsMemToCheck.push_back(p);
        }
        return &h->memPieza[ind].actual;
    }
};
int memOtherSize=0;
struct otheraRead:public getterCondTrig{
    int ind;
    v offset;
    otheraRead(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        for(pair<normalHolder*,getterCondTrig*>& p:h->memPiezaTrigs[ind])
            if(p.second==this)
                goto afterSetup;
        h->memPiezaTrigs[ind].push_back(make_pair(actualHolder.nh,this));
        afterSetup:
        auto* ret=&h->memPieza[ind];
        ret->before=ret->actual;
        return &ret->actual;
    }
    virtual int* valFast(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[ind].actual;
    }
    virtual void drawDebugMem(){
        posDebugTile=offset+actualHolder.nh->offsetAct;
        memOtherSize=actualHolder.nh->h->memPieza.size();
        backGroundMemDebug.setPosition(Vector2f(630+25*(ind%4),105+45*(ind/4-memOtherSize/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        if(h){
            auto vals=h->memPieza[ind];
            return vals.actual!=vals.before;
        }
        return true;///@sospechoso
    }
};
struct otheraReadNT:public getter{
    int ind;
    v offset;
    otheraReadNT(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[ind].actual;
    }
};
struct otheraiWrite:public getter{
    getter* g;
    v offset;
    otheraiWrite(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        int ind=*g->val();
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        for(pair<normalHolder*,getterCondTrig*> p:h->memPiezaTrigs[ind])
            if(p.first->h!=actualHolder.h)
                trigsMemToCheck.push_back(p);
        return &h->memPieza[ind].actual;
    }
};
struct otheraiRead:public getterCondTrig{
    getterCond* g;
    v offset;
    int beforeInd;
    otheraiRead(getterCond* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        beforeInd=*g->val();
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        for(pair<normalHolder*,getterCondTrig*>& p:h->memPiezaTrigs[beforeInd])
            if(p.second==this)
                goto afterSetup;
        h->memPiezaTrigs[beforeInd].push_back(make_pair(actualHolder.nh,this));
        afterSetup:
        auto* ret=&h->memPieza[beforeInd];
        ret->before=ret->actual;
        return &ret->actual;
    }
    virtual int* valFast(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[*g->val()].actual;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setPosition(Vector2f(530+25*(beforeInd%4),105+45*(beforeInd/4-memTileSize/4)));
        window->draw(backGroundMemDebug);
    }
    virtual bool change(){
        auto vals=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[*g->valFast()];
        return vals.actual!=vals.before;
    }
};
struct otheraiReadNT:public getter{
    getter* g;
    v offset;
    otheraiReadNT(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[*g->val()].actual;
    }
};

struct ctea:public getterCond{
    int v;
    ctea(int v_):v(v_){}
    virtual int* val(){
        return &v;
    }
    virtual int* valFast(){}
    virtual void drawDebugMem(){
            textValMem.setPosition(610,470);
            textValMem.setString(to_string(v));
            window->draw(textValMem);
    }
};
