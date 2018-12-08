#include "movs.h"
#include "Pieza.h"

///este archivo se compila a traves de operador.cpp

color::color(RectangleShape* rs_,v pos_){
    rs=rs_;
    pos=pos_;
}
void color::draw(){
    rs->setPosition(pos.x*32*escala,pos.y*32*escala);
    window->draw(*rs);
}
/*
void color::debug(){
    cout<<"color "<<(int)rs->getFillColor().r<<" "<<(int)rs->getFillColor().g<<" "<<(int)rs->getFillColor().b<<" "<<pos<<endl;
}
*/
color* color::clone(){
    return new color(rs,pos);
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

///si tarda mucho en compilar hacerlo por polimorfismo, total esta no va a ser la version definitiva
///mirar de que deberian heredar las acc m
template<typename T,typename ma1,typename ma2,T(*chck)(ma1,ma2),string* n> struct mcond:public mcondt{
    ma1 i1;
    ma2 i2;
    mcond(ma1 i1_,ma2 i2_):mcondt(n),i1(i1_),i2(i2_){}
    virtual bool check(){
        return chck(i1,i2);
    }
};
template<typename ma1,typename ma2> bool mcmp(ma1 a1,ma2 a2){
    return *a1.val()==*a2.val();
}
template<typename ma1,typename ma2> void mset(ma1 a1,ma2 a2){
    *a1.val()=*a2.val();
}

struct locala{
    int ind;
    locala(int ind_):ind(ind_){}
    int* val(){
        return &memMov[ind];
    }
};
struct piezaa{
    int ind;
    Holder* h;
    piezaa(int ind_,Holder* h_):ind(ind_),h(h_){}
    int* val(){
        //asumo que h es valida
        return &h->memPieza[ind];
    }
};

string mcondstr="mcond";
typedef mcond<bool,locala,piezaa,mcmp<locala,piezaa>,&mcondstr> mcmplp;

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

template<void(*funct)(v,Holder*),string* n> struct acc:public acct{
    acc(v pos_):acct(pos_,n){}
    virtual void func(){
        funct(pos,h);
    }
    virtual acct* clone(Holder* h_){
        acct* a=new acc(pos);
        a->h=h_;
        return a;
    }
};
#define fabAcc(NAME,FUNC) \
inline void NAME##func(v pos,Holder* h){ \
    FUNC \
}\
extern string str##NAME=#NAME;\
typedef acc<NAME##func,&str##NAME> NAME;

void movfunc(v pos,Holder* h){
    h->tile->step++;
    h->tile->holder=nullptr;
    h->tile=tablptr->tile(pos);
    h->tile->holder=h;
}
extern string strmov="mov";
typedef acc<movfunc,&strmov> mov;


fabAcc(pausa,
    drawScreen();
    sleep(milliseconds(40));
)
fabAcc(capt,
    Tile* captT=tablptr->tile(pos);
    delete captT->holder;
    captT->holder=nullptr;
    captT->step++;
    pisados.push_back(captT);
);


template<bool(*chck)(v,Holder*),string* n> struct cond:public condt{
    cond(v pos_):condt(pos_,n){}
    virtual bool check(Holder* h){
        return chck(pos,h);
    }
};
#define fabCond(NAME,FUNC)\
inline bool NAME##check(v pos,Holder* h){ \
    FUNC \
}\
extern string str##NAME=#NAME;\
typedef cond<NAME##check,&str##NAME> NAME;

fabCond(vacio,
    return tablptr->tile(pos+offset)->holder==nullptr;
)
fabCond(pieza,
    return tablptr->tile(pos+offset)->holder;
)
fabCond(enemigo,
    if(tablptr->tile(pos+offset)->holder)
        return tablptr->tile(pos+offset)->holder->bando!=h->bando;
    return false;
)
fabCond(esp,
    v posAct=pos+offset;
    h->outbounds=!(posAct.x>=0&&posAct.x<tablptr->tam.x&&posAct.y>=0&&posAct.y<tablptr->tam.y);
    return !h->outbounds;
)
fabCond(outbounds,
    return h->outbounds;
)

RectangleShape posPieza;
RectangleShape posActGood;
RectangleShape posActBad;
RectangleShape* tileActDebug;
Text textDebug;
bool drawDebugTiles;
bool ZPressed=false;
int mil=25;
extern string strNil="-";
struct debugMov:public condt{
    condt* cond;
    debugMov(condt* cond_):condt(v(0,0),&strNil){
        cond=cond_;
        pos=cond->pos;
    }
    virtual bool check(Holder* h){
        v posAct=pos+offset;
        bool ret=cond->check(h);
        textDebug.setString(*cond->nomb);
        if(ret){
            posActGood.setPosition(posAct.x*32*escala,posAct.y*32*escala);
            tileActDebug=&posActGood;
            textDebug.setColor(sf::Color(78,84,68,100));
        }else{
            posActBad.setPosition(posAct.x*32*escala,posAct.y*32*escala);
            tileActDebug=&posActBad;
            textDebug.setColor(sf::Color(240,70,40,240));
        }
        posPieza.setPosition(h->tile->pos.x*32*escala,h->tile->pos.y*32*escala);
        drawDebugTiles=true;
        drawScreen();
        drawDebugTiles=false;

        //como esta todo tirado aca en vez de en input no se puede cerrar la ventana, pero bueno
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
    virtual void debug(){
    }
};

Text asterisco;
bool drawAsterisco=false;
debugInicial::debugInicial():condt(v(0,0),&strNil){}
bool debugInicial::check(Holder* h){
    drawAsterisco=true;
    return true;
}

