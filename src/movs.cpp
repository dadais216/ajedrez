#include "movs.h"
#include "Pieza.h"

///este archivo se compila a traves de operador.cpp


color::color(RectangleShape* rs_,v pos_){
    rs=rs_;
    pos=pos_;
}
void color::draw(){
    rs->setPosition((pos.x+actualHolder.offset.x)*32*escala,(pos.y+actualHolder.offset.y)*32*escala);
    window->draw(*rs);
}
list<RectangleShape*> colores;
colort* normal::crearColor(v pos){
    ///se crea una instancia del sprite y cada colort la guarda en un puntero, se diferencia por tipo
    ///en un parametro de esta funcion, por ahora solo manejo colores

    //el motivo de manejarme con rectangleshapes es que es lo que comparten todos, total la pos se va a tener
    //que setear en cada dibujo sea compartida o no
    int r=tokens.front()-1000;
    tokens.pop_front();
    int g=tokens.front()-1000;
    tokens.pop_front();
    int b=tokens.front()-1000;
    tokens.pop_front();
    for(RectangleShape* c:colores)
        if(c->getFillColor().r==r&&c->getFillColor().g==g&&c->getFillColor().b==b)
            return new color(c,pos);
    RectangleShape* rs=new RectangleShape(Vector2f(32*escala,32*escala));
    rs->setFillColor(sf::Color(r,g,b,40));
    colores.push_back(rs);
    return new color(rs,pos);
}

/*
sprt::sprt(){
    int sn=tokens.front()-1000;tokens.pop_front();
    _sprt.setTexture(imagen->get("sprites.png"));
    _sprt.setTextureRect(IntRect(64+sn*64,0,32,32));
    _sprt.setScale(escala,escala);
    _sprt.setColor(Color(255,255,255,120));
    tipo=colort;
}
void sprt::func(){
    bufferColores.push_back(pair<drawable,v>(drawable(1,&_sprt),pos));
}
void sprt::debug(){
    cout<<"sprt ";
}
numShow::numShow(){
    txt.setFont(j->font);
    index=tokens.front()-1000;tokens.pop_front();
    txt.setFillColor(Color::Black);
    tipo=colort;
}
void numShow::func(){
    std::ostringstream stm;
    stm<<numeros[index];
    txt.setString(stm.str());
    bufferColores.push_back(pair<drawable,v>(drawable(2,&txt),pos));
}
void numShow::debug(){
    cout<<"numShow "<<index<<" ";
}
*/





/*
///version condt, la version acct necesita un clone() que le guarde el holder.
template<void(*t2)(int*)> struct piezaac{
    int ind;
    piezaa(int ind_):ind(ind_){}
    int* val(){
        t2(&ind);//version con y sin triggers
        return &hAct->memPieza[ind];
    }
};
*/

/*
struct spwn:public acm{
    int n;
    spwn(){
        tipo=acct;
        n=tokens.front()-1000; tokens.pop_front();
        //cuando anden los negativos se pueden invocar píezas del bando opuesto
    }
    virtual void func(){
        (*tabl)(pos.show(),lect.crearPieza(n*act->bando));
        cout<<"spwn "<<n<<" ";
    }
    virtual void debug(){
        cout<<"spwn "<<n<<" ";
    }
};
*/

template<void(*funct)(v),string* n> struct acc:public acct{
    v relPos;
    acc(v pos_):acct(n),relPos(pos_){}
    virtual void func(){
        funct(relPos);
    }
};

#define fabAcc(NAME,FUNC) \
inline void NAME##func(v relPos){ \
    FUNC \
}\
string str##NAME=#NAME;\
typedef acc<NAME##func,&str##NAME> NAME;

fabAcc(mov,
    actualHolder.tile->step++;
    actualHolder.tile->holder=nullptr;
    actualHolder.h->tile=tablptr->tile(relPos+actualHolder.offset);
    actualHolder.h->tile->holder=actualHolder.h;
)

fabAcc(pausa,
    ///@optim sacar nh
    drawScreen();
    sleep(milliseconds(40));
)
vector<Holder*> reciclaje;
fabAcc(capt,
    Tile* captT=tablptr->tile(relPos+actualHolder.offset);
    reciclaje.push_back(captT->holder);
    captT->holder=nullptr;
    captT->step++;
    pisados.push_back(captT);
);
string spwn_str="spwn";
struct spwn:public acct{
    v relPos;
    int id;
    spwn(v pos_,int id_):acct(&spwn_str),relPos(pos_),id(id_){}
    virtual void func(){
        v pos=relPos+actualHolder.offset;
        Tile* spwnT=tablptr->tile(pos);
        for(int i=0;i<reciclaje.size();i++){
            Holder* h=reciclaje[i];
            if(h->id==id){//no tomo bandos distintos porque los movimientos estan espejados
                spwnT->holder=h;
                h->tile=spwnT;
                reciclaje.erase(reciclaje.begin()+i);
                goto gen;
            }
        }
        spwnT->holder=lect.crearPieza(id*actualHolder.h->bando,pos);
        gen:
        spwnT->holder->generar();
        ///@optim se podria generar despues agregando un if al curso principal. Pense meterlo como un trigger
        ///pero eso llama a recalcular pasandole el primer normalHolder como el que tira el error. Pero encontrar
        ///ese normalHolder es un problema y los otros holders no terminan de generar sus cosas hasta la primera generacion
        pisados.push_back(spwnT);
    }
};


template<bool(*chck)(v),string* n> struct cond:public condt{
    v pos;
    cond(v pos_):condt(n),pos(pos_){}
    virtual bool check(){
        return chck(pos);
    }
};
#define fabCond(NAME,FUNC)\
inline bool NAME##check(v pos){ \
    FUNC \
}\
extern string str##NAME=#NAME;\
typedef cond<NAME##check,&str##NAME> NAME;

inline /* o no? */ void setTrigger(v pos){
    tablptr->tile(pos+offset)->triggers.push_back(Trigger{actualHolder.tile,actualHolder.nh,actualHolder.tile->step});
}

fabCond(vacio,
    setTrigger(pos);
    return tablptr->tile(pos+offset)->holder==nullptr;
)
fabCond(pieza,
    setTrigger(pos);
    return tablptr->tile(pos+offset)->holder;
)
fabCond(enemigo,
    setTrigger(pos);
    if(tablptr->tile(pos+offset)->holder)
        return tablptr->tile(pos+offset)->holder->bando!=actualHolder.h->bando;
    return false;
)
fabCond(esp,
    v posAct=pos+offset;
    return posAct.x>=0&&posAct.x<tablptr->tam.x&&posAct.y>=0&&posAct.y<tablptr->tam.y;
)

RectangleShape backGroundMem;
Text textValMem;

RectangleShape posPieza;
RectangleShape posActGood;
RectangleShape posActBad;
RectangleShape* tileActDebug;
Text textDebug;
bool drawDebugTiles;
bool ZPressed=false;
int mil=25;
extern string strNil="-";
template<void(*drawBlocks)(condt*,bool)> struct debugWrapper:public condt{
    condt* cond;
    debugWrapper(condt* cond_):condt(&strNil),cond(cond_){}
    virtual bool check(){
        bool ret=cond->check();
        textDebug.setString(*cond->nomb);
        drawBlocks(cond,ret);
        ///@cleanup como esta todo tirado aca en vez de en input no se puede cerrar la ventana, pero bueno
        while(true){
            sleep(milliseconds(mil));
            if(!window->hasFocus()) continue;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
                if(!ZPressed){
                    ZPressed=true;
                    break;
                }
            }else
                ZPressed=false;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)){
                if(mil>10) mil-=1;
                break;
            }else
                mil=25;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
                mil=0;
                break;
            }
        }
        return ret;
    }
    virtual void debug(){}
};

inline void drawDebugPos(condt* cond,bool ret){
    v posAct=static_cast<vacio*>(cond)->pos+offset;
    if(ret){
        posActGood.setPosition(posAct.x*32*escala,posAct.y*32*escala);
        tileActDebug=&posActGood;
        textDebug.setColor(sf::Color(78,84,68,100));
    }else{
        posActBad.setPosition(posAct.x*32*escala,posAct.y*32*escala);
        tileActDebug=&posActBad;
        textDebug.setColor(sf::Color(240,70,40,240));
    }
    posPieza.setPosition(actualHolder.tile->pos.x*32*escala,actualHolder.tile->pos.y*32*escala);
    drawDebugTiles=true;
    drawScreen();
    drawDebugTiles=false;
}
typedef debugWrapper<drawDebugPos> debugMov;
bool drawMemDebug;
getterCond* getterMemDebug1;
getterCond* getterMemDebug2;
inline void drawDebugMem(condt* cond,bool ret){
    struct mock:public condt{
        getterCond* i1;
        getterCond* i2;
    };
    if(ret)
        textDebug.setColor(sf::Color(78,84,68,100));
    else
        textDebug.setColor(sf::Color(240,70,40,240));
    posPieza.setPosition(actualHolder.tile->pos.x*32*escala,actualHolder.tile->pos.y*32*escala);

    getterMemDebug1=static_cast<mock*>(cond)->i1;
    getterMemDebug2=static_cast<mock*>(cond)->i2;
    drawMemDebug=true;
    drawScreen();
    drawMemDebug=false;
    getterMemDebug1=nullptr;
}
typedef debugWrapper<drawDebugMem> debugMem;


Text asterisco;
bool drawAsterisco=false;
debugInicial::debugInicial():condt(&strNil){}
bool debugInicial::check(){
    drawAsterisco=true;
    return true;
}


