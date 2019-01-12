#include "global.h"
#include "memGetters.h"
#include "movHolders.h"


vector<int> memMov;
vector<vector<trigMemGlobal>> memGlobalTriggers;
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

vector<trigMemGlobal>* trigsMaybeActivate;

//se podrían meter templates para evitar copiar y pegar, pero no vale la pena
struct globalaWrite:public getter{
    int ind;
    globalaWrite(int ind_):ind(ind_){}
    virtual int* val(){
        trigsMaybeActivate=&memGlobalTriggers[ind];
        return &memGlobal[ind];
    }
};
struct globalaRead:public getterCond{
    int ind;
    int* val_;
    globalaRead(int ind_):ind(ind_),val_(&memGlobal[ind_]){}
    virtual int* val(){
        return val_;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),305+45*(ind/4-memGlobalSize/4)));
        window->draw(backGroundMemDebug);
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
        trigsMaybeActivate=&memGlobalTriggers[ind];
        return &memGlobal[ind];
    }
};
struct globalaiRead:public getterCond{
    getterCond* g;
    int indDebug;
    globalaiRead(getterCond* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();

        bool notSet=true;
        for(trigMemGlobal& tmg:memGlobalTriggers[ind])
            if(tmg.gc==this){
                goto afterSetup;
            }
        memGlobalTriggers[ind].push_back({actualHolder.nh,this});
        afterSetup:
        ///@optim lo malo de este sistema es que pone triggers dinamicos que nunca se van, y pueden estar
        //soltando triggers falso positivo a cada rato si se lee una memoria en alguna rama que no se ejecuta seguido.
        //Podría manejarse con un mecanismo de aging, pero como es algo raro en una mecanica que ya es rara, puede
        //que lo mas eficiente sea no hacer nada
        int* ret=&memGlobal[ind];
        indDebug=*ret;
        return ret;
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setPosition(Vector2f(530+25*(indDebug%4),305+45*(indDebug/4-memGlobalSize/4)));
        window->draw(backGroundMemDebug);
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
        trigsMaybeActivate=&actualHolder.h->memPiezaTrigs[ind];
        return &actualHolder.h->memPieza[ind];
    }
};
struct piezaaRead:public getterCond{
    int ind;
    piezaaRead(int ind_):ind(ind_){}
    virtual int* val(){
        return &actualHolder.h->memPieza[ind];
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),205+45*(ind/4-actualHolder.h->memPieza.size()/4)));
        window->draw(backGroundMemDebug);
    }
};
struct piezaaReadNT:public getter{
    int ind;
    piezaaReadNT(int ind_):ind(ind_){}
    virtual int* val(){
        return &actualHolder.h->memPieza[ind];
    }
};
struct piezaaiWrite:public getter{
    getter* g;
    piezaaiWrite(getter* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();
        trigsMaybeActivate=&actualHolder.h->memPiezaTrigs[ind];
        return &actualHolder.h->memPieza[ind];
    }
};
struct piezaaiRead:public getterCond{
    getterCond* g;
    piezaaiRead(getterCond* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();
        for(trigMemGlobal& tmg:actualHolder.h->memPiezaTrigs[ind])
            if(tmg.gc==this)
                goto afterSetup;
        actualHolder.h->memPiezaTrigs[ind].push_back({actualHolder.nh,this});
        afterSetup:
        return &actualHolder.h->memPieza[ind];
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        int ind=*g->val();
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),205+45*(ind/4-actualHolder.h->memPieza.size()/4)));
        window->draw(backGroundMemDebug);
    }
};
struct piezaaiReadNT:public getter{
    getter* g;
    piezaaiReadNT(getter* g_):g(g_){}
    virtual int* val(){
        return &actualHolder.h->memPieza[*g->val()];
    }
};

struct tileaWrite:public getter{
    int ind;
    v offset;
    tileaWrite(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        trigsMaybeActivate=reinterpret_cast<vector<trigMemGlobal>*>(&t->memTileTrigs[ind]);
        return &t->memTile[ind];
    }
};
v posDebugTile(0,0);
struct tileaRead:public getterCond{
    int ind;
    v offset;
    tileaRead(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        int* stepCheck=&actualHolder.tile->step;
        int step=*stepCheck;
        for(Tile::tileTrigInfo& tti:t->memTileTrigs[ind])
            if(tti.gc==this){
                tti.step=step;
                tti.stepCheck=stepCheck;//creo que no es necesario actualizar este
                goto afterSetup;
            }
        t->memTileTrigs[ind].push_back({actualHolder.nh,this,step,stepCheck});
        afterSetup:
        return &t->memTile[ind];
    }
    virtual void drawDebugMem(){
        posDebugTile=offset+actualHolder.nh->offsetAct;
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),105+45*(ind/4-memTileSize/4)));
        window->draw(backGroundMemDebug);
    }
};
struct tileaReadNT:public getter{
    int ind;
    v offset;
    tileaReadNT(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[ind];
    }
};
struct tileaiWrite:public getter{
    getter* g;
    v offset;
    tileaiWrite(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        int ind=*g->val();
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        trigsMaybeActivate=reinterpret_cast<vector<trigMemGlobal>*>(&t->memTileTrigs[ind]);
        return &t->memTile[ind];
    }
};
struct tileaiRead:public getterCond{
    getterCond* g;
    v offset;
    int ind;
    tileaiRead(getterCond* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        ind=*g->val();
        Tile* t=tablptr->tile(offset+actualHolder.nh->offsetAct);
        int* stepCheck=&actualHolder.tile->step;
        int step=*stepCheck;
        for(Tile::tileTrigInfo& tti:t->memTileTrigs[ind])
            if(tti.gc==this){
                tti.step=step;
                tti.stepCheck=stepCheck;//creo que no es necesario actualizar este
                goto afterSetup;
            }
        t->memTileTrigs[ind].push_back({actualHolder.nh,this,step,stepCheck});
        afterSetup:
        return &t->memTile[ind];
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),105+45*(ind/4-memTileSize/4)));
        window->draw(backGroundMemDebug);
    }
};
struct tileaiReadNT:public getter{
    getter* g;
    v offset;
    tileaiReadNT(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->memTile[*g->val()];
    }
};

///todos los other asumen que hay una pieza en la posicion actual con una memoria que contenga su indice
///@optim other deja triggers colgados en las otras piezas, que lo van a activar cada vez que cambien esa memoria
///lo que no es grave porque siempre se van a ignorar por estar en ramas falsas (cond pieza antes de mother), aun asi
///es lento. Se podría guardar mas informacion como en tile, solo para tener una condicion con la que eliminar triggers
///la condicion es que el step de la pieza propia y la que lee no varie. Se puede guardar la suma en vez de cada numero
///para ahorrar espacio. Haría necesario una version mas de cada accion que escribe
struct otheraWrite:public getter{
    int ind;
    v offset;
    otheraWrite(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        trigsMaybeActivate=&h->memPiezaTrigs[ind];//podria asumir que other nunca se usa sobre uno y sacar el if holder==
        return &h->memPieza[ind];
    }
};
int memOtherSize=0;
struct otheraRead:public getterCond{
    int ind;
    v offset;
    otheraRead(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        for(trigMemGlobal& tmg:h->memPiezaTrigs[ind])
            if(tmg.gc==this)
                goto afterSetup;
        h->memPiezaTrigs[ind].push_back({actualHolder.nh,this});
        afterSetup:
        return &h->memPieza[ind];
    }
    virtual void drawDebugMem(){
        posDebugTile=offset+actualHolder.nh->offsetAct;
        memOtherSize=actualHolder.nh->h->memPieza.size();
        backGroundMemDebug.setPosition(Vector2f(630+25*(ind%4),105+45*(ind/4-memOtherSize/4)));
        window->draw(backGroundMemDebug);
    }
};
struct otheraReadNT:public getter{
    int ind;
    v offset;
    otheraReadNT(int ind_,v offset_):ind(ind_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[ind];
    }
};
struct otheraiWrite:public getter{
    getter* g;
    v offset;
    otheraiWrite(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        int ind=*g->val();
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        trigsMaybeActivate=&h->memPiezaTrigs[ind];
        return &h->memPieza[ind];
    }
};
struct otheraiRead:public getterCond{
    getterCond* g;
    v offset;
    int ind;
    otheraiRead(getterCond* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        ind=*g->val();
        Holder* h=tablptr->tile(offset+actualHolder.nh->offsetAct)->holder;
        for(trigMemGlobal& tmg:h->memPiezaTrigs[ind])
            if(tmg.gc==this)
                goto afterSetup;
        h->memPiezaTrigs[ind].push_back({actualHolder.nh,this});
        afterSetup:
        return &h->memPieza[ind];
    }
    virtual void drawDebugMem(){
        backGroundMemDebug.setFillColor(sf::Color(178,235,221));
        g->drawDebugMem();
        backGroundMemDebug.setFillColor(sf::Color(163,230,128,150));
        backGroundMemDebug.setPosition(Vector2f(530+25*(ind%4),105+45*(ind/4-memTileSize/4)));
        window->draw(backGroundMemDebug);
    }
};
struct otheraiReadNT:public getter{
    getter* g;
    v offset;
    otheraiReadNT(getter* g_,v offset_):g(g_),offset(offset_){}
    virtual int* val(){
        return &tablptr->tile(offset+actualHolder.nh->offsetAct)->holder->memPieza[*g->val()];
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
