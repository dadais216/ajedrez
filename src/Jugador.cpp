
struct Proper;
void properDraw();
void properGameInit();

Humano::Humano(bool bando_,tabl& tablero_)
:Jugador(bando_,tablero_) {}

int dt=0;
int clickI=0;
bool confirm;
void Humano::turno(){
    /*
    dt++;//se podría mover adentro del if?
    if(!clickers.empty()){
        if(dt>20){
            dt=0;
            clickI++;
            for(Clicker* cli:clickers)
                cli->activacion(clickI);
        }
        drawScreen();
        confirm=false;
        if(input->click()){
            bool any=false;
            for(Clicker* cli:clickers)
                if(cli->update()){
                    any=true;
                    break;
                }
            if(confirm){
                drawScreen();
                return false;
            }
            clickers.clear();
            if(!any)
                drawScreen();
            return any;
        }
    }
    */
    while(true){
        sleep(milliseconds(20));
        input.check();
        debug(
              if(window.hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                //static_cast<Proper*>(j->actual)->init();///@leaks
                while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
                throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
              }
              )
          if(input.click()&&input.inGameRange(_tablero.tam)){
            v posClicked=input.get();
            for(Clicker& cli:clickers){
            ///@todo @optim esto se pregunta 60hz
            ///Lo mejor seria hacer que se bloquee hasta recibir otro click, hacerlo bien cuando
            ///vuelva a meter solapamiento
                if(posClicked==cli.clickPos){
                    cli.update();//accionar
                    turno1=!turno1;
                    drawScreen(properDraw);
                    return;
                }
            }
            if(!clickers.empty()){
                clickers.clear();
                //drawScreen();
            }
            cout<<"("<<input.get()<<")"<<endl;


            act=_tablero.tile(input.get())->holder;
            if(act&&act->bando==bando){
                act->makeCli();
                //drawScreen();
            }
        }
    }
}

void Nadie::turno(){
    turnoAct++;
    ::turno=turnoAct/2;
}

Aleatorio::Aleatorio(bool bando_,tabl& tablero_)
:Jugador(bando_,tablero_){
    srand(time(NULL));
}
double sProm=0;
int cProm=0;
double minV=10000;
double maxV=0;
void Aleatorio::turno(){
    if(window.hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
        properGameInit();
        while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
        throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
    }
    bool alive=false;
    for(int i=0; i<_tablero.tam.x; i++)
        for(int j=0; j<_tablero.tam.y; j++){
            Holder* act=_tablero.tile(v(i,j))->holder;
            if(act&&act->bando==bando){
                alive=true;
                act->makeCli();
            }
        }
    //drawScreen();
    if(!alive)
        while(true){
            if(window.hasFocus()&&sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                properGameInit();///@leaks
                while(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) sleep(milliseconds(10));
                throw nullptr;//es un longjump para evitar que proper::update llame a segundo en lugar de a primero
            }
        }
    if(clickers.size()>0){
        auto it=clickers.begin();
        advance(it,rand()%clickers.size());

        //sleep(milliseconds(120));


        clock_t t=clock();
        (*it).update();
        double val=clock()-t;
        sProm+=val;
        if(val>maxV)
            maxV=val;
        if(val<minV)
            minV=val;
        cProm++;
        if(cProm==100){
            cout<<"normalSize  "<<sizeof(normalHolder)
            <<"\nbucketMovSize  "<<bucketHolders->head-bucketHolders->data
            <<"\nbucketOpSize  "<<bucketPiezas->head-bucketPiezas->data
            <<"\npromedio: "<<fixed<<sProm/(double)cProm/CLOCKS_PER_SEC<<" segundos"
            <<"\nmin: "<<minV/CLOCKS_PER_SEC
            <<"\nmax: "<<maxV/CLOCKS_PER_SEC<<endl;
            exit(0);
        }

        clickers.clear();
    }
}
