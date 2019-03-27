#include "../include/global.h"
#include "../include/lector.h"
#include "../include/Pieza.h"

list<int> tokens;
lector::lector(){
    extra=0;
    #define rel(T) tabla[#T]=T
    rel(def);
    rel(sprt);
    rel(numShow);
    rel(end);

    rel(mov);
    rel(capt);
    rel(pausa);
    rel(spwn);
    rel(capt);
    rel(pass);

    rel(esp);
    rel(vacio);
    rel(enemigo);
    rel(pieza);
    rel(pass);

    rel(desliz);
    rel(exc);
    rel(isol);
    rel(desopt);

    tabla["c"]=click;
    tabla["or"]=sep;

    #undef rel
    #define rel(T) tablaMem[#T]=T
    rel(mcmp);
    rel(mset);
    rel(madd);
    rel(mless);
    rel(mmore);
    rel(mdist);
    #undef rel
    tablaMem["=="]=mcmp;
    tablaMem["="]=mset;
    tablaMem["+="]=madd;
    tablaMem["<"]=mless;
    tablaMem[">"]=mmore;
    tablaMem["!="]=mdist;
    lista=nullptr;
}

int stringToInt(string& s){
    int a=0,sig=1;
    for(int i=0; i<s.length(); i++){
        if(s[i]=='-'){
            sig=-1;
            continue;
        }
        if(s[i]==' '||s[i]=='\n'||s[i]=='\0'){
            s=s.substr(i+1);
            return a*sig;
        }
        a=a*10+s[i]-'0';
    }
    s="";
    return a*sig;
}

void lector::generarIdsTablero(int n){
    archTablero.open("tableros.txt");

    for(vector<int>& ve:matriz)
        ve.clear();
    matriz.clear();//limpio en caso de que se este regenerando

    string linea;
    int i=0;
    while(getline(archTablero,linea)){
        if(linea.empty()) continue;
        if(linea[0]=='"'){
            if(i!=n){
                i++;
                continue;
            }
            while(getline(archTablero,linea)){
                if(linea=="") continue;
                if(linea[0]=='"'||linea[0]==':') break;
                vector<int> vec;
                while(!linea.empty())
                    vec.push_back(stringToInt(linea));
                matriz.push_back(vec);
            }
            archTablero.close();
            return;
        }
    }
}
bool tileMemGrowth=false;
Holder* lector::crearPieza(int n,v pos){
    for(Pieza* p:piezas)
        if(p->id==n)
            return new Holder(sgn(n),p,pos);

    archPiezas.clear();
    archPiezas.seekg(0, ios::beg);
    string linea;///@final mirar los leaks y eso
    int sn;
    tileMemGrowth=false;
    memPiezaSize=0;
    memLocalSizes.resize(1);
    memLocalSizes[0]=0;
    while(getline(archPiezas,linea)){
        if(linea.empty()) continue;
        if(linea[0]==':'){
            linea=linea.substr(1,linea.length());
            if(stringToInt(linea)==abso(n)){
                sn=stringToInt(linea);
                goto enPieza;
            }
        }
    }
    cout<<"una pieza no esta";
    exit(EXIT_FAILURE);
enPieza:
    doEsp=hayAtras=false;
    cout<<"p"<<n<<endl;
    while(getline(archPiezas,linea)){
        if(linea.empty()) continue;
        if(linea[0]==':') break;
        tokenizarLinea(linea);
    }
    procesarTokens(tokens);
    for(int s:tokens){
        switch(s){
            #define CASE(A) case A: cout<<#A" "; break;
            CASE(W);
            CASE(A);
            CASE(S);
            CASE(D);
            CASE(N);
            CASE(mov);
            CASE(capt);
            CASE(spwn);
            CASE(pausa);
            CASE(vacio);
            CASE(pieza);
            CASE(enemigo);
            CASE(pass);
            CASE(esp);
            CASE(msize);
            CASE(mcmp);
            CASE(mset);
            CASE(madd);
            CASE(mless);
            CASE(mmore);
            CASE(mdist);
            CASE(mlocal);
            CASE(mglobal);
            CASE(mpieza);
            CASE(mtile);
            CASE(mother);
            CASE(turno);
            CASE(posX);
            CASE(posY);
            CASE(desliz);
            CASE(exc);
            CASE(isol);
            CASE(desopt);
            CASE(click);
            CASE(color);
            CASE(sprt);
            CASE(numShow);
            case eol: cout<<"eol"<<endl;break;
            CASE(sep);
            CASE(end);
            default:
                cout<<s-1000<<" ";
        }
    }
    cout<<endl;

    memMov.resize(maxMemMovSize);
    memGlobal.resize(memGlobalSize);
    memGlobalTriggers.resize(memGlobalSize);
    if(tileMemGrowth)
        for(auto& vec:tablptr->matriz)
            for(Tile* t:vec){
                t->memTile.resize(memTileSize);
                t->memTileTrigs.resize(memTileSize);
            }

    Holder* h=new Holder(sgn(n),new Pieza(n,sn),pos);
    tokens.clear();
    return h;
}
void lector::procesarTokens(list<int>& tokens){
    //aplicar def
    for(auto it=tokens.begin(); it!=tokens.end(); it++)///@optim tendria mas sentido cargarlo directamente en vez de en 2 pasos
        for(auto ent:defs)
            if(ent.first==*it){
                it=tokens.erase(it);
                auto aux=it;
                aux--;
                tokens.insert(it,ent.second.begin(),ent.second.end());
                it=aux;
                break;
            }

    //aplicar llaves
    bool loop=true;
    int numeroLinea;//para manejar incersion de memLocalSize en movimientos con llaves
    while(loop){
        loop=false;
        numeroLinea=0;
        list<int>::iterator inicioLinea=tokens.begin();
        for(auto izq=tokens.begin();izq!=tokens.end();++izq){
            if(*izq==eol){
                inicioLinea=izq;
                ++inicioLinea;
                numeroLinea++;
            }
            else if(*izq==llaveizq){
                loop=true;
                int anid=0;
                ++izq;
                for(auto der=izq; der!=tokens.end();++der){
                    if(*der==llaveizq)
                        ++anid;
                    else if(*der==llaveder){
                        if(anid)
                            --anid;
                        else{
                            auto pos=inicioLinea;
                            --pos;
                            ++der;
                            auto finLinea=der;
                            for(;*finLinea!=eol&&finLinea!=tokens.end();++finLinea);
                            if(*finLinea==eol) ++finLinea;
                            list<int> act,pre,post;
                            act.splice(act.begin(),tokens,inicioLinea,finLinea);
                            --izq;
                            pre.splice(pre.begin(),act,act.begin(),izq);
                            post.splice(post.begin(),act,der,act.end());
                            act.pop_back();
                            act.pop_front();
                            auto jt=act.begin(),it=act.begin();
                            ++pos;
                            --it;
                            do{
                                ++it;
                                if(*it==llaveizq)
                                    ++anid;
                                else if(*it==llaveder)
                                    --anid;
                                else if(*it==coma&&anid==0||it==act.end()){
                                    list<int> exp;
                                    exp.assign(pre.begin(),pre.end());
                                    exp.insert(exp.end(),jt,it);
                                    exp.insert(exp.end(),post.begin(),post.end());
                                    jt=it;
                                    ++jt;
                                    tokens.splice(pos,exp,exp.begin(),exp.end());

                                    memLocalSizes.insert(memLocalSizes.begin()+numeroLinea,memLocalSizes[numeroLinea]);
                                }
                            }
                            while(it!=act.end());
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    //aplicar reglas especiales, como limpiar los eol con lineJoins
    for(list<int>::iterator it=tokens.begin();it!=tokens.end();++it){
        //list<int>::iterator jt=it++;
        list<int>::iterator jt=it;
        ++jt;
        if(*it==N&&*jt==esp){
            tokens.erase(jt);
            it=tokens.erase(it);
            ----it;
            continue;
        }
        if(*it==sep&&*jt==end)
            it=tokens.erase(it);
    }
}
void lector::tokenizarLinea(string& linea){
    i=0,j=0;
    for(;j<linea.length()+1;j++){
        switch(linea[j]){
        case '#': centinela(linea,'#'); return;
        case ' ': centinela(linea,' '); break;
        case '{': centinela(linea,'{'); break;
        case ',': centinela(linea,','); break;
        case '}': centinela(linea,'}'); break;
        case '\0': centinela(linea,'\0'); break;
        case '\\': centinela(linea,'\\'); break;
        default: hayAtras=true;
        }
    }
}
void lector::centinela(string& linea,char c){
    if(hayAtras){
        tokenizarPalabra(linea);
        hayAtras=false;
    }
    i++;
    tokenizarCaracter(c);
}
int lector::getNum(string& linea){//arranca con j en el primer numero
    i=j;
    do{j++;}while(linea[j]>='0'&&linea[j]<='9');
    string numstr=linea.substr(i,j-1);
    return stringToInt(numstr);
}
int memGlobalSize;
int memTileSize;
void lector::tokenizarPalabra(string& linea){
    string palabra=linea.substr(i,j-i);
    //cout<<">>>"<<palabra<<"<<<"<<linea[i]<<"\n";
    i=j;
    if(palabra=="def"&&lista==&tokens){
        linea="";
        return;
    }
    bool esMov=true;
    for(char c:palabra)
        if(c!='w'&&c!='a'&&c!='s'&&c!='d'&&c!='n'){
                esMov=false;
                break;
        }
    if(esMov){
        for(char c:palabra)
            tokenizarCaracter(c);
        doEsp=true;
        return;
    }
    if(palabra=="color"){
        lista->push_back(color);
        for(int k=0;k<3;k++){
            do{j++;}while(linea[j]==' ');
            int num=getNum(linea);
            lista->push_back(num+1000);
            }
        ///@maybe 4 numero opcional para transparencia
        i=j;
        if(linea[j]=='\0')
            cerrarLinea();
        return;
    }
    if(palabra=="msize"){
        do{j++;}while(linea[j]==' ');
        char c=linea[j];
        do{j++;}while(linea[j]==' ');
        int num=getNum(linea);
        if(lista==&tokens){//@hack para evitar que cargarDef entre por aca cuando esta parseando para buscar defs. Puede que se
                //quede para evitar crashes en caso de que haya un msize en un def, lo que no deberia hacerse
                //se podria hacer andar pero no se si lo vale
            switch(c){
            //los tamaños de memorias se determinan aca
            case 'l': //uso un vector porque una pieza tiene un localSize por movimiento.
                if(num>memLocalSizes.back()){
                    memLocalSizes.back()=num;
                    if(num>maxMemMovSize)
                        maxMemMovSize=num;
                }
                break;
            case 'g':
                if(num>memGlobalSize)
                    memGlobalSize=num;
                break;
            case 'p':
                if(num>memPiezaSize)
                    memPiezaSize=num;
                break;
            case 't':
                if(num>memTileSize){
                    memTileSize=num;
                    tileMemGrowth=true;
                }
                break;
            default:
                assert(false&&"msize invalido");
            }
            i=j;
            if(linea[j]=='\0')
                cerrarLinea();
            return;
        }
    }
    if(palabra=="spwn"){
        lista->push_back(spwn);
        do{j++;}while(linea[j]==' ');
        int num=getNum(linea);
        lista->push_back(num+1000);
        i=j;
        if(linea[j]=='\0')
            cerrarLinea();
        return;
    }
    for(auto t:tabla)
        if(palabra==t.first){
            lista->push_back(t.second);
            return;
        }
    for(auto t:tablaMem)
        if(palabra==t.first){
            lista->push_back(t.second);
            for(int k=0;k<2;k++){
                int directGetter=0;
                while(true){
                    j++;
                    switch(linea[j]){
                    case ' ': continue;
                    case 'l': lista->push_back(directGetter=mlocal);break;
                    case 'g': lista->push_back(directGetter=mglobal);break;
                    case 'p': lista->push_back(directGetter=mpieza);break;
                    case 't': lista->push_back(directGetter=mtile);break;
                    case 'o': lista->push_back(directGetter=mother);break;

                    case 'T':lista->push_back(lector::turno);goto handleSpecialMem;
                    case 'x':lista->push_back(lector::posX);goto handleSpecialMem;
                    case 'y':lista->push_back(lector::posY);goto handleSpecialMem;
                    default:
                        goto gnum;
                    }
                }
                handleSpecialMem:
                directGetter=0;
                if(k==1){
                    j++;
                    goto finalArr;
                }
                continue;
                gnum:
                int num=getNum(linea);
                if(lista==&tokens){
                    switch(directGetter){
                    case mlocal:
                        if(num>=memLocalSizes.back()){
                            memLocalSizes.back()=num+1;
                            if(num>=maxMemMovSize)
                                maxMemMovSize=num+1;
                        }
                        break;
                    case mglobal:
                        if(num>=memGlobalSize)
                            memGlobalSize=num+1;
                        break;
                    case mpieza:
                        if(num>=memPiezaSize)
                            memPiezaSize=num+1;
                        break;
                    case mtile:
                        if(num>=memTileSize){
                            memTileSize=num+1;
                            tileMemGrowth=true;
                        }
                    }
                }
                lista->push_back(num+1000);
            }
            finalArr:
            i=j;
            if(linea[j]=='\0')
                cerrarLinea();
            return;
        }
    if(lista!=&tokens){ //se esta llamando desde cargarDefs
        cout<<"palabra nueva:"<<palabra<<endl;
        tabla[palabra]=last+extra;
        lista->push_back(last+extra);
        extra++;
    }else{
        cout<<"palabra no reconocida:"<<palabra;
        exit(EXIT_FAILURE);
    }
}
void lector::tokenizarCaracter(char c){
    switch(c){
    case '{':lista->push_back(llaveizq);break;
    case ',':lista->push_back(coma);break;
    case '}':lista->push_back(llaveder);break;
    case 'w':lista->push_back(W);break;
    case 'a':lista->push_back(A);break;
    case 's':lista->push_back(S);break;
    case 'd':lista->push_back(D);break;
    case 'n':lista->push_back(N);break;
    case '\\':lista->push_back(lineJoin);break;
    case ' ':
        if(doEsp){
            lista->push_back(esp);
            doEsp=false;
        }
        break;
    case '\0':
    case '#':
        if(doEsp){
            lista->push_back(esp);
            doEsp=false;
            cerrarLinea();
        }else{
            if(!lista->empty()&&lista->back()!=eol){//no es una linea vacia
                cerrarLinea();
            }
        }
        break;
    }
}
void lector::cerrarLinea(){
    if(lista->back()==lineJoin)
        lista->pop_back();
    else{
        if(lista==&tokens)
            memLocalSizes.push_back(0);
        lista->push_back(eol);
    }
}

void lector::cargarDefs(){
    archPiezas.clear();
    archPiezas.seekg(0, ios::beg);

    doEsp=hayAtras=false;
    string linea;
    lista=new list<int>;
    while(getline(archPiezas,linea)){

        ///@maybe cargar todo en una linea y procesarlo, para tener defs, llaves y linejoins en def

        if(linea.empty()) continue;
        if(!lista)
            lista=new list<int>;
        tokenizarLinea(linea);
        ///@optim ineficiente ahora que dejo tener defs en todos lados, estoy tokenizando todo el archivo solo
        ///para buscar defs. No cambio nada ahora porque supongo que en un futuro voy a cambiar todo esto por
        ///un parser con scanner comun

        if(lista->empty()) continue;
        if(lista->front()==def){
            lista->pop_front();
            if(lista->back()==eol)
                lista->pop_back();
            int defval=lista->front();
            lista->pop_front();
            defs[defval]=*lista;
            lista=nullptr;
        }else{
            lista->clear();
            continue;
        }
    }
    if(lista)
        delete lista;
    lista=&tokens;
}
