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

vector<int> memMov;
int maxMemMovSize=0;

struct locala{
    int ind;
    locala(int ind_):ind(ind_){
        if(ind>=memLocalSize)
            memLocalSize=ind+1;
    }
    int* val(){
        return &memMov[ind];
    }
};
struct ctea{
    int v;
    ctea(int v_):v(v_){}
    int* val(){
        return &v;
    }
};

///version condt, la version acct necesita un clone() que le guarde el holder.
template<void(*t2)(int*)> struct piezaa{
    int ind;
    piezaa(){
        ind=tokens.front()-1000;
        tokens.pop_front();
    }
    int* val(){
        t2(&ind);//version con y sin triggers
        return &hAct->memPieza[ind];
    }
};
///reemplazar int* por triggermem*
inline void dont(int* a){}
inline void addMemTrigger(int* a){
    *a+=1;
}



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
    acc(v pos_):acct(n,pos_){}
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

fabAcc(mov,
    h->tile->step++;
    h->tile->holder=nullptr;
    h->tile=tablptr->tile(pos);
    h->tile->holder=h;
)
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
    tablptr->tile(pos+offset)->triggers.push_back(triggerInfo);
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
        return tablptr->tile(pos+offset)->holder->bando!=hAct->bando;
    return false;
)
fabCond(esp,
    v posAct=pos+offset;
    hAct->outbounds=!(posAct.x>=0&&posAct.x<tablptr->tam.x&&posAct.y>=0&&posAct.y<tablptr->tam.y);
    return !hAct->outbounds;
)
fabCond(outbounds,
    return hAct->outbounds;
)

RectangleShape backGroundMemLocal;
Text textValMemLocal;

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
    v pos;
    condt* cond;
    debugMov(condt* cond_):condt(&strNil){
        cond=cond_;
        pos=static_cast<debugMov*>(cond)->pos;
        ///sospechoso pero es la unica instancia donde se necesita saber la pos de un cond, no vale la pena agregar
        ///un nivel mas de herencia por esto nomas. Antes de llegar aca hay que filtrar las no posicionales
    }
    virtual bool check(){
        v posAct=pos+offset;
        bool ret=cond->check();
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
        posPieza.setPosition(hAct->tile->pos.x*32*escala,hAct->tile->pos.y*32*escala);
        drawDebugTiles=true;
        drawScreen();
        drawDebugTiles=false;

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
    virtual void debug(){
    }
};

Text asterisco;
bool drawAsterisco=false;
debugInicial::debugInicial():condt(&strNil){}
bool debugInicial::check(){
    drawAsterisco=true;
    return true;
}


