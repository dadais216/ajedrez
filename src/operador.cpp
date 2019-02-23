#include "../include/operador.h"
#include "../include/lector.h"
#include "../include/Clicker.h"
#include "../include/tablero.h"
#include "../include/Pieza.h"
#include "movs.cpp"
#include "memGetters.cpp"

bool separator;
bool clickExplicit;///cuando se usa click explicitamente no se pone un click implicitamente
///al final del movimiento si este termina en una no normal
///@detail una condicion mejor sería no poner click implicito si el ultimo operador no normal contiene algun click explicito

string str_cmp="cmp";
string str_set="set";
string str_add="add";
string str_less="less";
string str_more="more";

int memLocalSizeAct;

normal::normal(bool make){
    vector<acct*> accsTemp;
    vector<condt*> condsTemp;
    vector<colort*> colorsTemp;
    vector<normal::setupTrigInfo> setUpMemTriggersPerNormalHolderTemp;
    auto setupBarrays=[&](){
        //cargo listas de punteros despues del resto de memoria de la normal.
        //Lo pongo al final en vez de al principio para evitar tener que hacer un analisis
        //previo a la carga para saber hasta donde llega la normal y que cosas tiene.
        //supongo que que este antes o despues tiene el mismo efecto en la cache
        accs.init(accsTemp.size(),accsTemp.data());
        conds.init(condsTemp.size(),condsTemp.data());
        colors.init(colorsTemp.size(),colorsTemp.data());
        setUpMemTriggersPerNormalHolder.init(setUpMemTriggersPerNormalHolderTemp.size(),setUpMemTriggersPerNormalHolderTemp.data());
    };

    movSize+=sizeof(normalHolder)+memLocalSizeAct;
    tipo=NORMAL;
    hasClick=makeClick=false;
    sig=nullptr;
    relPos=v(0,0);
    doEsp=false;
    bool posTriggerableCond=false;
    if(make){
        bool changeInLocalMem=false;
        while(true){
            if(tokens.empty()) return;///@optim creo que no se activa nunca, no deberia
            int tok=tokens.front();
            tokens.pop_front();
            switch(tok){
            case lector::W:
            case lector::S:
            case lector::D:
            case lector::A:

                //antes nomas cortaba en cuando hubo una cond posicional antes. Ahora lo hago por cualquiera
                //porque sino cosas como mcmp p0 1 w mover ponen un trigger en w aun cuando mcmp es falso
                //por ahi no vale la pena cortar por eso igual.
                if(!condsTemp.empty()||!accsTemp.empty()){
                    tokens.push_front(tok);
                    setupBarrays();
                    sig=bucketAdd<normal>(true);
                    return;
                    //nueva normal
                }else{
                    switch(tok){
                    case lector::W: relPos.y+=bandoAct;break;
                    case lector::S: relPos.y-=bandoAct;break;
                    case lector::D: relPos.x++;break;
                    case lector::A: relPos.x--;break;
                    }
                }
                break;
                //cout<<#TOKEN<<endl;
    #define cond(TOKEN)  case lector::TOKEN: if(debugMode) condsTemp.push_back(bucketAdd<debugMov>(&TOKEN)); else condsTemp.push_back(&TOKEN);
                cond(vacio);break;
                cond(pieza);break;
                cond(enemigo);break;
                cond(pass);break;
            case lector::esp:
            doEsp=true;
            break;
    #define acc(TOKEN) case lector::TOKEN: accsTemp.push_back(&TOKEN);break

                acc(mov);
                acc(capt);
                acc(pausa);
            case lector::spwn:
                accsTemp.push_back(bucketAdd<spwn>((tokens.front()-1000)*bandoAct));tokens.pop_front();break;
                //spwn n con n positivo quiere decir mismo bando, negativo bando enemigo
            case lector::color:
                colorsTemp.push_back(crearColor());
                break;
    //       colorr(sprt);
    //       colorr(numShow);
            case lector::mcmp:
            case lector::mset:
            case lector::madd:
            case lector::mless:
            case lector::mmore:
                {
                    ///hay 2 tipos de operaciones en memoria no local
                    ///condiciones que leen
                    ///acciones que escriben la variable izquierda y leen la derecha
                    getter* g[2];
                    bool write=tok==lector::mset||tok==lector::madd;
                    bool action;
                    bool writeOnTile=false;
                    bool isLocalAcc=false;
                    for(int i=0;i<2;i++){
                        bool left=i==0;
                        int tg[5],j;
                        for(j=0;;j++){
                            assert(j<5);
                            tg[j]=tokens.front();tokens.pop_front();
                            if(tg[j]>=900||tg[j]==lector::turno||tg[j]==lector::posX||tg[j]==lector::posY)///@sospechoso no 1000 para tomar algunos numeros negativos
                                break;
                        }
                        if(left)
                            action=write&&tg[0]!=lector::mlocal;
                        if(j==0){
                            assert(!(write&&left)&&"escritura en constante");
                            switch(tg[0]){
                            case lector::turno:
                                g[i]=&turnoa;
                                setUpMemTriggersPerNormalHolderTemp.push_back({2,0});
                                break;
                            case lector::posX:
                                g[i]=&posX;break;
                            case lector::posY:
                                g[i]=&posY;break;
                            default:
                                g[i]=bucketAdd<ctea>(tg[0]-1000);
                            }
                        }else{
                            tg[j]-=1000;
                            bool hasIndirection=j>1;
                            if(tg[j-1]==lector::mlocal)
                                if(left){
                                    g[i]=bucketAdd<locala>(tg[j]);
                                    changeInLocalMem=changeInLocalMem||write;
                                }else
                                    if(action){
                                        g[i]=bucketAdd<localaAcc>(tg[j]);
                                        isLocalAcc=true;
                                    }else
                                        g[i]=bucketAdd<locala>(tg[j]);
                            else
                                if(action)
                                    if(hasIndirection)
                                        switch(tg[j-1]){
                                        case lector::mglobal: g[i]=bucketAdd<globalaReadNT>(tg[j]); break;
                                        case lector::mpieza: g[i]=bucketAdd<piezaaReadNT>(tg[j]);break;
                                        case lector::mtile: g[i]=bucketAdd<tileaReadNT>(tg[j]);break;
                                        case lector::mother: g[i]=bucketAdd<otheraReadNT>(tg[j]);break;
                                        }
                                    else
                                        if(left)
                                            switch(tg[j-1]){
                                            case lector::mglobal: g[i]=bucketAdd<globalaWrite>(tg[j]); break;
                                            case lector::mpieza: g[i]=bucketAdd<piezaaWrite>(tg[j]);break;
                                            case lector::mtile: g[i]=bucketAdd<tileaWrite>(tg[j]);writeOnTile=true;break;
                                            case lector::mother: g[i]=bucketAdd<otheraWrite>(tg[j]);break;
                                            }
                                        else
                                            switch(tg[j-1]){
                                            case lector::mglobal: g[i]=bucketAdd<globalaReadNT>(tg[j]); break;
                                            case lector::mpieza: g[i]=bucketAdd<piezaaReadNT>(tg[j]);break;
                                            case lector::mtile: g[i]=bucketAdd<tileaReadNT>(tg[j]);break;
                                            case lector::mother: g[i]=bucketAdd<otheraReadNT>(tg[j]);break;
                                            }
                                else
                                    switch(tg[j-1]){
                                    case lector::mglobal:
                                        g[i]=bucketAdd<globalaRead>(tg[j]);
                                        setUpMemTriggersPerNormalHolderTemp.push_back({0,tg[j]});
                                    break;case lector::mpieza:
                                        g[i]=bucketAdd<piezaaRead>(tg[j]);
                                        setUpMemTriggersPerNormalHolderTemp.push_back({1,tg[j]});
                                    break;
                                    case lector::mtile: g[i]=bucketAdd<tileaRead>(tg[j]);break;
                                    case lector::mother: g[i]=bucketAdd<otheraRead>(tg[j]);break;
                                    }

                            for(int k=j-2;k>=0;k--){//getters indirectos
                                hasIndirection=k>0;
                                if(tg[k]==lector::mlocal)
                                    if(left){
                                        g[i]=bucketAdd<localai>(static_cast<getterCond*>(g[i]));
                                        changeInLocalMem=changeInLocalMem||write;
                                    }else
                                        if(action){
                                            g[i]=bucketAdd<localaiAcc>(g[i]);
                                            isLocalAcc=true;
                                        }else
                                            g[i]=bucketAdd<localai>(static_cast<getterCond*>(g[i]));
                                else
                                    if(action)
                                        if(hasIndirection)
                                            switch(tg[k]){
                                            case lector::mglobal: g[i]=bucketAdd<globalaiReadNT>(g[i]); break;
                                            case lector::mpieza: g[i]=bucketAdd<piezaaiReadNT>(g[i]);break;
                                            case lector::mtile: g[i]=bucketAdd<tileaiReadNT>(g[i]);break;
                                            case lector::mother: g[i]=bucketAdd<otheraiReadNT>(g[i]);break;
                                            }
                                        else
                                            if(left)
                                                switch(tg[k]){
                                                case lector::mglobal: g[i]=bucketAdd<globalaiWrite>(g[i]); break;
                                                case lector::mpieza: g[i]=bucketAdd<piezaaiWrite>(g[i]);break;
                                                case lector::mtile: g[i]=bucketAdd<tileaiWrite>(g[i]);break;
                                                case lector::mother: g[i]=bucketAdd<otheraiWrite>(g[i]);break;
                                                }
                                            else
                                                switch(tg[k]){
                                                case lector::mglobal: g[i]=bucketAdd<globalaiReadNT>(g[i]); break;
                                                case lector::mpieza: g[i]=bucketAdd<piezaaiReadNT>(g[i]);break;
                                                case lector::mtile: g[i]=bucketAdd<tileaiReadNT>(g[i]);break;
                                                case lector::mother: g[i]=bucketAdd<otheraiReadNT>(g[i]);break;
                                                }
                                    else
                                        switch(tg[k]){
                                        case lector::mglobal:
                                            g[i]=bucketAdd<globalaiRead>(static_cast<getterCond*>(g[i]));
                                            setUpMemTriggersPerNormalHolderTemp.push_back({true,tg[j]});
                                        break;case lector::mpieza:
                                            g[i]=bucketAdd<piezaaiRead>(static_cast<getterCond*>(g[i]));
                                            setUpMemTriggersPerNormalHolderTemp.push_back({false,tg[j]});
                                        break;
                                        case lector::mtile: g[i]=bucketAdd<tileaiRead>(static_cast<getterCond*>(g[i]));break;
                                        case lector::mother: g[i]=bucketAdd<otheraiRead>(static_cast<getterCond*>(g[i]));break;
                                        }
                            }
                        }
                    }
                    if(action){
                        #define memCase(F) case lector::m##F: if(writeOnTile) \
                                                                    accsTemp.push_back(new macc<m##F##AccTile,&str_##F>(g[0],g[1]));\
                                                                else \
                                                                    accsTemp.push_back(new macc<m##F##Acc,&str_##F>(g[0],g[1]));break;
                            switch(tok){
                            memCase(set);
                            memCase(add);
                        }
                        #undef memCase
                        ///manejo de locales en acciones que usen memoria local que haya cambiado
                        if(isLocalAcc&&changeInLocalMem){
                            auto* localAccTemp=accsTemp.back();accsTemp.pop_back();
                            setupBarrays();
                            accsTemp.push_back(localAccTemp);
                            sig=bucketAdd<normal>(true);
                            return;
                        }
                    }
                    else{
                        #define memCase(F) case lector::m##F: if(debugMode) \
                        condsTemp.push_back(new debugMem(new mcond<m##F##Cond,&str_##F>(g[0],g[1]))); \
                        else condsTemp.push_back(new mcond<m##F##Cond,&str_##F>(g[0],g[1])); break;
                        switch(tok){
                            memCase(cmp);
                            memCase(set);
                            memCase(add);
                            memCase(less);
                            memCase(more);
                        }
                        #undef memCase
                    }
                }
                break;

    #undef acc //(TOKEN)
    #undef cond //(TOKEN)
    #undef colorr //(TOKEN)
    #undef caseT //(TIPO,TOKEN)

            case lector::sep:
                //cout<<"sep"<<endl;
                separator=true;
                setupBarrays();
                return;
            case lector::eol:
                hasClick=makeClick=true;
                setupBarrays();
                return;
            case lector::end:
                //cout<<"lim"<<endl;
                setupBarrays();
                return;
            case lector::click:
                hasClick=makeClick=true;
                clickExplicit=true;
                setupBarrays();
                sig=tomar();
                ///@todo mirar casos raros como dos clicks seguidos
                return;
            default:
                tokens.push_front(tok);
                sig=tomar();
                setupBarrays();
                return;
            }
        }
    }
}

desliz::desliz(){
    tipo=DESLIZ;
    makeClick=false;

    int movSizeTemp=movSize;
    movSize=0;
    inside=tomar();
    v& tam=tablptr->tam;
    //iteraciones necesarias para recorrer el tablero en linea recta.
    iterSize=movSize;
    insideSize=movSize*((tam.x>tam.y?tam.x:tam.y))*2;///@todo agregar posibilidad de elegir cuando se reserva
    movSize=movSizeTemp+sizeof(deslizHolder)+insideSize;

    sig=keepOn(&makeClick);

    if(makeClick)
        hasClick=true;
    else
        for(operador* op=inside;op!=nullptr;op=op->sig)
            if(op->hasClick){
                hasClick=true;
                break;
            }
}
exc::exc(){
    tipo=EXC;
    int movSizeTemp=movSize;
    movSize=0;

    vector<operador*> opsTemp;
    do{
        separator=false;
        operador* op=tomar();
        opsTemp.push_back(op);
    }while(separator);
    ops.init(opsTemp.size(),opsTemp.data());
    movSize+=ops.size();

    insideSize=movSize;
    movSize=movSizeTemp+sizeof(excHolder)+insideSize;

    makeClick=false;
    sig=keepOn(&makeClick);
    if(makeClick)
        hasClick=true;
    else{
        hasClick=false;
        for(operador* op:ops)
            if(op->hasClick){
                hasClick=true;
                break;
            }
    }

}
isol::isol(){
    tipo=ISOL;
    hasClick=true;
    makeClick=false;
    bool clickExplicitBack=clickExplicit;

    movSize+=sizeof(isolHolder);
    inside=tomar();
    if(!clickExplicit)
        makeClick=true;
    clickExplicit=clickExplicitBack;
    sig=keepOn(&makeClick);
}
desopt::desopt(){
    tipo=DESOPT;
    int movSizeTemp=movSize;
    movSize=0;
    //vector<operador*> opsTemp;
    do{
        separator=false;
        operador* op=tomar();
        ops.push_back(op);
        //opsTemp.push_back(op);
    }while(separator);
    //ops.init(opsTemp.size(),opsTemp.data());

    int ramasEnProfundidad=1;
    int sumRamas=0;
    for(int i=0;i<3;i++){///@todo 3 es profundidad. Como con desliz, agregar opcion de editar
        ramasEnProfundidad*=ops.size();
        sumRamas+=ramasEnProfundidad;
    }
    desoptInsideSize=movSize*sumRamas;
    //va a ser un valor muy desproporcionado para profundidades altas. Puede que sea mejor usar alguna mecanica
    //de reutilizacion. Las piezas que usen todas las ramas a la vez serían mas rapidas que lo mismo usando memoria
    //dinamica. El problema esta en piezas que reserven mucho y usen poco

    makeClick=false;
    sig=keepOn(&makeClick);
    if(makeClick)
        hasClick=makeClick;
    else{
        hasClick=false;
        for(operador* op:ops)
            if(op->hasClick){
                hasClick=true;
                break;
            }
    }
}

//mira si hay algun token adelante que genere un operador
operador* keepOn(bool* makeClick){
    if(tokens.empty())
        return nullptr;
    switch(tokens.front())
    {
    case lector::click:
        *makeClick=true;
        tokens.pop_front();
        return keepOn(makeClick);
    case lector::sep:
        separator=true;
        tokens.pop_front();
        return nullptr;
    case lector::eol:
        if(!clickExplicit)
            *makeClick=true;
    case lector::end:
        tokens.pop_front();
        return nullptr;
    }
    return tomar();
}

operador* tomar(){
    if(tokens.empty()) return nullptr;
    int tok=tokens.front();
    tokens.pop_front();
#define caseTomar(TOKEN) case lector::TOKEN: return bucketAdd<TOKEN>()
    switch(tok)
    {
    caseTomar(desliz);
    caseTomar(exc);
    caseTomar(isol);
    caseTomar(desopt);
    case lector::sep:
        separator=true;
    case lector::eol:
    case lector::end:
        return nullptr;
    default:
        tokens.push_front(tok);
        return bucketAdd<normal>(true);
    }
}
