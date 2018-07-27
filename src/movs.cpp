#include "movs.h"

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

//usando herencia podría evitar tener 800 constructores iguales, pero de todas formas va a haber 800 clone
//y con esto puedo hacer lo de debug
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
    NOMB(v pos_){\
        pos=pos_;\
    };\
    Func(TIPO,FUNC)\
    virtual void debug(){\
        cout<<#NOMB<<" "<<pos<<endl;\
    } \
    Clone(TIPO,NOMB)\
}\

bool addTrigger;

fabMov(mov,acct,
       h->tile->step++;
       h->tile->holder=nullptr;
       h->tile=tablptr->tile(pos);
       h->tile->holder=h;
      );

fabMov(pausa,acct,
       drawScreen();
       sleep(milliseconds(500));
      );
vector<Holder*> capturados;
fabMov(capt,acct,
       Tile* captT=tablptr->tile(pos);
       delete captT->holder;
       captT->holder=nullptr;
       captT->step++;
       h->pisados.push_back(captT);
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
///list<v> limites;
fabMov(esp,condt,
        if(pos.x>=0&&pos.x<tablptr->tam.x&&pos.y>=0&&pos.y<tablptr->tam.y){
            h->outbounds=false;
            /*
            for(v vec:limites){ //si realentiza mover a otra cond
                if(pos==vec){
                    return false;
            }
            }
            */
            ///mirar el tema de los limites, ahora tienen que ser locales
            ///probablemente esten en holder y listo
            return true;
        }else{
            h->outbounds=true;
            return false;
        }
      );
/*
fabMov(prob,condt,
        limites.emplace_back(pos);
);
*/
fabMov(outbounds,condt,
       return h->outbounds;
      );
fabMov(inicial,condt,
       return h->inicial;
      );
/*
fabMov(ori,movt,
        pos=org;
);
*/
