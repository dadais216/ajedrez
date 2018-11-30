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
void color::debug(){
    cout<<"color "<<(int)rs->getFillColor().r<<" "<<(int)rs->getFillColor().g<<" "<<(int)rs->getFillColor().b<<" "<<pos<<endl;
}
color* color::clone()
{
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

array<int,20> numeros;
bool memcambios;
/*
struct posRemember:public acm{
    int index,jndex;
    posRemember(){
        tipo=movt;
        if(!tokens.empty()&&tokens.front()>=1000){
            index=tokens.front()-1000; tokens.pop_front();
            jndex=tokens.front()-1000; tokens.pop_front();
        }else{
            index=0;
            jndex=1;
        }
    }
    virtual void func(){
        memcambios=true;
        numeros[index]=pos.x;
        numeros[jndex]=pos.y;
    }
    virtual void debug(){
        cout<<"posRemember ";
    }
};

struct posRestore:public acm{
    int index,jndex;
    posRestore(){
        tipo=movt;
        if(!tokens.empty()&&tokens.front()>=1000){
            index=tokens.front()-1000; tokens.pop_front();
            jndex=tokens.front()-1000; tokens.pop_front();
        }else
            index=0;
            jndex=1;
    }
    virtual void func(){
        if(memcambios){
            pos.x=numeros[index];
            pos.y=numeros[jndex];
        }
    }
    virtual void debug(){
        cout<<"posRestore ";pos.show();cout<<" -> ";v(numeros[index],numeros[jndex]).show();
    }
};
*/
/*
#define numFab(NOMB,TIPO,FUNC) \
struct NOMB:public acm{ \
    int val,index; \
    NOMB(){ \
        tipo=TIPO; \
        index=tokens.front()-1000; tokens.pop_front();  \
        val=tokens.front()-1000; tokens.pop_front(); \
    } \
    virtual void func(){ \
        FUNC \
    } \
    virtual void debug(){ \
        cout<<#NOMB<<" "<<index<<" "<<val<<" "; \
    } \
}

numFab(numSet,movt,numeros[index]=val;);
numFab(numAdd,movt,numeros[index]+=val;);
numFab(numCmp,condt,cond=numeros[index]==val;);
numFab(numDst,condt,cond=numeros[index]!=val;);
numFab(numLess,condt,cond=numeros[index]<val;);

numFab(numSeti,movt,numeros[index]=numeros[val];);
numFab(numAddi,movt,numeros[index]+=numeros[val];);
numFab(numCmpi,condt,cond=numeros[index]==numeros[val];);
numFab(numDsti,condt,cond=numeros[index]!=numeros[val];);
numFab(numLessi,condt,cond=numeros[index]<numeros[val];);

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

//puede que cambie a un sistema que no use herencia si lo necesito, el problema es que algunos movs toman parametros,
//pero se podria poner un puntero a informacion extra en un struct generico y listo
///@optim h en acciones podría estar fijado como esta pos
#define Func(TIPO,FUNC) Func##TIPO(FUNC)
#define Funcacct(FUNC)\
    virtual void func(Holder* h){\
        FUNC\
    }
#define Funccondt(FUNC)\
    virtual bool check(Holder* h,v pos){\
        FUNC\
    }
#define Clone(TIPO,NOMB) Clone##TIPO(NOMB)
#define Cloneacct(NOMB) \
    virtual NOMB* clone(){\
        return new NOMB(pos);\
    }
#define Clonecondt(NOMB)
#define fabMov(NOMB,TIPO,FUNC)\
struct NOMB:public TIPO{\
    NOMB(v pos_):TIPO(#NOMB){\
        pos=pos_;\
    };\
    Func(TIPO,FUNC)\
    Clone(TIPO,NOMB)\
}

bool addTrigger;

fabMov(mov,acct,
       h->tile->step++;
       h->tile->holder=nullptr;
       h->tile=tablptr->tile(pos);
       h->tile->holder=h;
      );

fabMov(pausa,acct,
       drawScreen();
       sleep(milliseconds(40));
      );
fabMov(capt,acct,
       Tile* captT=tablptr->tile(pos);
       delete captT->holder;
       captT->holder=nullptr;
       captT->step++;
       pisados.push_back(captT);
      );

fabMov(vacio,condt,
       addTrigger=true;
       return tablptr->tile(pos)->holder==nullptr;
      );
fabMov(pieza,condt,
       addTrigger=true;
       return tablptr->tile(pos)->holder;
      );
fabMov(enemigo,condt,
       if(tablptr->tile(pos)->holder)
            return tablptr->tile(pos)->holder->bando!=h->bando;
       return false;
      );
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tablptr->tam.x&&pos.y>=0&&pos.y<tablptr->tam.y){
            h->outbounds=false;
            return true;
        }else{
            h->outbounds=true;
            return false;
        }
      );
fabMov(outbounds,condt,
       return h->outbounds;
      );

RectangleShape posPieza;
RectangleShape posActGood;
RectangleShape posActBad;
RectangleShape* tileActDebug;
Text textDebug;
bool drawDebugTiles;
bool ZPressed=false;
int mil=25;
struct debugMov:public condt{
    condt* cond;
    debugMov(condt* cond_):condt("debugMov"){
        cond=cond_;
        pos=cond->pos; //necesita tener pos para no ser especial en la logica de operar
    }
    virtual bool check(Holder* h,v pos){
        bool ret=cond->check(h,pos);
        textDebug.setString(cond->nomb);
        if(ret){
            posActGood.setPosition(pos.x*32*escala,pos.y*32*escala);
            tileActDebug=&posActGood;
            textDebug.setColor(sf::Color(78,84,68,100));
        }else{
            posActBad.setPosition(pos.x*32*escala,pos.y*32*escala);
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
        cond->debug();
    }
};

Text asterisco;
bool drawAsterisco=false;
debugInicial::debugInicial(v pos_):condt("-"){
    pos=pos_;
}
bool debugInicial::check(Holder* h,v pos_){
    drawAsterisco=true;
    return true;
}
passCond::passCond(v pos_):condt("passCond"){
    pos=pos_;
}
bool passCond::check(Holder* h,v pos_){
    return true;
}



passAcc::passAcc(v pos_):acct("passAcc"){
    pos=pos_;
}
void passAcc::func(Holder* h){
}
acct* passAcc::clone(){
    return new passAcc(pos);
}
