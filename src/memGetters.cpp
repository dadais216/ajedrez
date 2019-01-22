#include "global.h"
#include "memGetters.h"
#include "movHolders.h"


vector<int> memMov;
vector<memTriggers> memGlobalTriggers;
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

memTriggers* trigsMaybeActivate;
///hay 2 tipos de triggers de memoria, estaticos y dinamicos. Los estaticos son los de accesos a la memoria global
///y de pieza directos, que se conocen cuando se arma el holder y quedan fijos. Los dinamicos son el resto (indirectos,
///tile y other). Funcionan mas o menos como los trigger posicionales, se crean durante las condiciones (lectura) y
///se activan durante accion (en escritura) y se eliminan

//se podrían meter templates para evitar copiar y pegar, pero no vale la pena
struct globalaWrite:public getter{
    int ind;
    globalaWrite(int ind_):ind(ind_){}
    virtual int* val(){
        trigsMaybeActivate=&memGlobalTriggers[ind];
        return &memGlobal[ind];
    }
};
///en una escritura que cambie el valor se activan todos los triggers asociados que no sean del mismo holder.
///tile tiene una condicion mas (que no haya variado el step), y other tambien (que no hayan variado dos steps)
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
///puede que se activen falsos positivos en indirectos, lo que causa que se recorra un recalculo que no encuentra su
///normalHolder. Solo pasa una vez porque los indirectos son dinamicos, se eliminan despues de usarse
struct globalaiRead:public getterCond{
    getterCond* g;
    int indDebug;
    globalaiRead(getterCond* g_):g(g_){}
    virtual int* val(){
        int ind=*g->val();
        memGlobalTriggers[ind].dinam.push_back(actualHolder.nh);
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
        actualHolder.h->memPiezaTrigs[ind].dinam.push_back(actualHolder.nh);
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
        trigsMaybeActivate=reinterpret_cast<memTriggers*>(&t->memTileTrigs[ind]);
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
        t->memTileTrigs[ind].push_back({actualHolder.nh,step,stepCheck});
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
        trigsMaybeActivate=reinterpret_cast<memTriggers*>(&t->memTileTrigs[ind]);
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
        t->memTileTrigs[ind].push_back({actualHolder.nh,step,stepCheck});
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
        h->memPiezaTrigs[ind].dinam.push_back(actualHolder.nh);
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
        h->memPiezaTrigs[ind].dinam.push_back(actualHolder.nh);
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
    virtual void drawDebugMem(){
            textValMem.setPosition(610,470);
            textValMem.setString(to_string(v));
            window->draw(textValMem);
    }
};
//turno es una memoria global que solo puede ser leida
vector<turnTrigInfo> turnoTrigs[2];
struct turnoaObj:public getterCond{
    turnoaObj(){}
    virtual int* val(){
        return &turno;
    }
    virtual void drawDebugMem(){}
};
turnoaObj turnoa;

//pos se trata como una cte porque no puede variar desde la generacion hasta la accion inclusive
struct posX:public getterCond{
    v offset;
    int val_;
    posX(v pos):offset(pos){}
    virtual int* val(){
        val_=offset.x+actualHolder.nh->offsetAct.x;
        return &val_;
    }
    virtual void drawDebugMem(){}
};
struct posY:public getterCond{
    v offset;
    int val_;
    posY(v pos):offset(pos){}
    virtual int* val(){
        val_=offset.y+actualHolder.nh->offsetAct.y;
        if(actualHolder.nh->h->bando==-1)
            val_=tablptr->tam.y-1-val_;
        return &val_;
    }
    virtual void drawDebugMem(){}
};
