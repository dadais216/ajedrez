
bool Clicker::drawClickers;

vector<Clicker> clickers;
v getActualPos(v,v);

Clicker::Clicker(vector<normalHolder*>* normales_,Holder* h_){
    normales=*normales_;
    ///@optim estaria bueno no copiar el ultimo vector
    h=h_;

    normalHolder* lastN=normales[normales.size()-1];
    /*
    caso para manejar cosas donde se genere un clicker donde la ultima normal no tenga acciones.
    Lo dejo aparte porque no se me ocurre un caso donde sea util, y no me parece que valga la pena
    Para funcionar se deberia hacer posAct una variable de normalHolder (ahora es local)
    if(lastN->accs.empty())
        clickPos=lastN->posAct;
    else
    */

    actualHolder.h=h;
    clickPos=lastN->pos;

    //clickPos.show();
    ///solapamientos
    /*
    val=0;
    mod=1;
    int conflictos=0;
    for(Clicker* c:clickers)
        if(c->clickPos==clickPos){
            conflictos++;
            c->mod++;
        }
    if(conflictos!=0){
        val=conflictos;
        mod=conflictos+1;
    }
    */
}

void Clicker::draw(){
    for(normalHolder* n:normales){
        n->draw();
    }
    /*
    if(!activo) return;
    for(pair<drawable,v> c:colores){
        if(get<0>(c).tipo==0){
            RectangleShape* rs=(RectangleShape*)get<0>(c).obj;
            rs->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*rs);
        }else if(get<0>(c).tipo==1){
            Sprite* s=(Sprite*)get<0>(c).obj;
            s->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*s);
        }else{
            Text* t=(Text*)get<0>(c).obj;
            t->setPosition(get<1>(c).x*32*escala,get<1>(c).y*32*escala);
            window->draw(*t);
        }
        //seguro que se puede simplificar con cast y esas boludeces

        //cout<<c->cuadrado.getPosition().x/32/escala<<" "<<c->cuadrado.getPosition().y/32/escala<<endl;
    }
    */
}

vector<Tile*> pisados;

void Clicker::update(){
    /*
    //esto es para confirmar el toque
    if(mod>1){
        clickers.clear();
        clickers.push_back(this);
        confirm=true;
        val=0;
        mod=1;
        return true;
    }
    */
    Tile* tileBef=h->tile;
    int stepBef=tileBef->step;
    accionar();

    ///@optim esto esta para movimientos que no mueven la pieza, que son una minoria
    if(tileBef->step!=stepBef){
        pisados.push_back(tileBef);
        pisados.push_back(h->tile);
        ///@optim piezas que no se mueven no deberian generar todo
    }
    for(Tile* tile:pisados)
        tile->chargeTriggers();
    for(turnTrigInfo& tti:turnoTrigs[h->bando])///@todo mirar que este bien
        if(tti.h!=h)
            trigsActivados.push_back(tti.nh);
    turnoAct++;
    turno=turnoAct/2;
    activateTriggers();
    pisados.clear();

    try{
        h->generar();
    }catch(...){}//lngjmp para kamikases

    ///una pieza nunca activa sus propios triggers porque al moverse los invalida
    ///necesita generar todos sus movimientos devuelta de forma explicita
    ///piezas que no se mueven no pisan


    //si no se quiere tener este if se podría generar en spwn, pero eso genera recalculos y algunos bugs oscuros
    //(creo que el bug era que una pieza se capturaba a si misma, ponia triggers, se spawneaba a si misma y los
    //activaba. Esto por algun motivo rompia a veces)

    /*
    cout<<endl;
    for(int i=0; i<tablptr->tam.y; i++){
        for(int j=0; j<tablptr->tam.x; j++){
            cout<<tablptr->tile(v(j,i))->triggers.size()<<"  ";
        }
        cout<<endl;
    }
    */

    clickers.clear();
}

void Clicker::accionar(){
    Clicker::drawClickers=false;
    for(normalHolder* n:normales){
        n->accionar();
    }
}

/*
void Clicker::activacion(int clickI)
{
    activo=val==clickI%mod;
}

*/

