#include "lector.h"
#include <Pieza.h>

list<int> tokens;

lector::lector(){
    extra=0;
    #define rel(T) tabla[#T]=T
    rel(def);
    rel(color);
    rel(lim);
    rel(pausa);
    rel(posRestore);
    rel(posRemember);


    rel(mov);
    rel(capt);

    rel(vacio);
    rel(enemigo);
    rel(outbounds);

    rel(desliz);
    rel(opt);
    rel(click);

    rel(nc);
    rel(t);

    tabla["<<"]=bloque;
    tabla[">>"]=lim;
    tabla["or"]=sep;

    archPiezas.open("piezas.txt");
    archTablero.open("tableros.txt");

    lista=nullptr;
}

int lector::stringToIntR(string& s){
    int a=0,sig=1;
    for(int i=0;i<s.length();i++){
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

void lector::leer(int n){
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
                while(!linea.empty()) vec.push_back(stringToIntR(linea));
                matriz.push_back(vec);
            }
            archTablero.close();
            return;
        }
    }
}

void lector::mostrar(){
    //cout<<matriz[0].size()<<" "<<matriz.size()<<endl;
    for(int j=0;j<matriz.size();j++){
        for(int i=0;i<matriz[j].size();i++){
            cout<<matriz[j][i];
        }
        cout<<endl;
    }
}

Pieza* lector::crearPieza(int n){
    archPiezas.clear();
    archPiezas.seekg(0, ios::beg);
    string linea;
    int sn;
    while(getline(archPiezas,linea)){
        if(linea.empty()) continue;
        if(linea[0]==':'){
            int j;
            for(j=1;j<linea.length()&&linea[j]!='"';j++);
            linea=linea.substr(1,j-1);
            if(stringToIntR(linea)==abso(n)){
                sn=stringToIntR(linea);
                goto enPieza;
            }
        }
    }
    cout<<"una pieza no esta";
    exit(EXIT_FAILURE);
    enPieza:
    hayAtras=false;
    cout<<"p"<<n<<endl;
    while(getline(archPiezas,linea)){
        //cout<<">"<<linea<<endl;
        if(linea.empty()) continue;
        if(linea[0]==':') break;
        tokenizarLinea(linea);
    }

//    for(int s:tokens)
//        cout<<"|"<<s<<"|";
//    cout<<endl;
//    for(auto i:defs){
//        cout<<i.first<<">>";
//        for(auto j:i.second){
//            cout<<j<<"-";
//        }
//        cout<<endl;
//    }

    //aplicar def
    for(auto it=tokens.begin();it!=tokens.end();it++){
        for(auto ent:defs){
            if(ent.first==*it){
                it=tokens.erase(it);
                auto aux=it;
                aux--;
                tokens.insert(it,ent.second.begin(),ent.second.end());
                it=aux;
                break;
            }
        }
    }



    //aplicar llaves
    bool loop=true;
    while(loop){
        loop=false;
        list<int>::iterator bizq=tokens.begin(),pos;
        for(auto izq=tokens.begin();izq!=tokens.end();izq++){
            if(*izq==eol){
                bizq=izq;
                bizq++;
            }else if(*izq==llaveizq){
                loop=true;
                int anid=0;
                izq++;
                for(auto der=izq;der!=tokens.end();der++){
                    if(*der==llaveizq)
                        anid++;
                    else if(*der==llaveder){
                        if(anid)
                            anid--;
                        else{
                            pos=bizq;
                            pos--;
                            der++;
                            auto bder=der;
                            for(;*bder!=eol&&bder!=tokens.end();bder++);
                            if(*bder==eol) bder++;
                            list<int> act,pre,post;
                            act.splice(act.begin(),tokens,bizq,bder);
                            izq--;
                            pre.splice(pre.begin(),act,act.begin(),izq);
                            post.splice(post.begin(),act,der,act.end());
                            act.pop_back();
                            act.pop_front();
                            auto jt=act.begin(),it=act.begin();
                            pos++;
                            it--;
                            do{
                                it++;
                                if(*it==llaveizq)
                                    anid++;
                                else if(*it==llaveder)
                                    anid--;
                                else if(*it==coma&&anid==0||it==act.end()){
                                    list<int> exp;
                                    exp.assign(pre.begin(),pre.end());
                                    exp.insert(exp.end(),jt,it);
                                    exp.insert(exp.end(),post.begin(),post.end());
                                    jt=it;
                                    jt++;
                                    tokens.splice(pos,exp,exp.begin(),exp.end());
                                }
                            }while(it!=act.end());
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    for(int s:tokens)
        cout<<"|"<<s<<"|";
    cout<<endl;




    Pieza* p=new Pieza(abso(n),sgn(n),sn);

    tokens.clear();
    return p;
}

void lector::tokenizarLinea(string linea){
    i=0,j=0;
    for(;j<linea.length()+1;j++){
        //cout<<"="<<linea[j]<<endl;
        if(token(linea,'#')) break;
        if(token(linea,' ')) continue;
        if(token(linea,'{')) continue;
        if(token(linea,',')) continue;
        if(token(linea,'}')) continue;
        if(token(linea,'=')) continue;
        if(token(linea,'\0')) continue;
        hayAtras=true;
    }
}

bool lector::token(string linea,char c){
    if(linea[j]==c){
        if(hayAtras){
            token(linea);
            hayAtras=false;
        }
        i++;
        token(c);
        return true;
    }
    return false;
}

void lector::token(string linea){
    string palabra=linea.substr(i,j-i);

    //cout<<"++"<<palabra<<"++"<<i<<" "<<j<<"\n";
    i=j;

    if(palabra=="pass")
        return;

    bool esMov=true,esNum=true;
    for(int k=0;k<palabra.length();k++){
        if(palabra[k]!='w'&&palabra[k]!='a'&&palabra[k]!='s'&&palabra[k]!='d')
            esMov=false;
        if(palabra[k]<'0'||palabra[k]>'9')
            esNum=false;
    }
    if(esMov){
        for(int k=0;k<palabra.length();k++)
            token(palabra[k]);
        lista->push_back(esp);
        return;
    }
    if(esNum){
        lista->push_back(-stringToIntR(palabra));
        return;
    }

    for(auto i:tabla)
        if(palabra==i.first){
            lista->push_back(i.second);
            return;
        }
    if(lista!=&tokens){
        cout<<"palabra nueva: "<<palabra<<endl;
        tabla[palabra]=last+extra;
        lista->push_back(last+extra);
        extra++;
    }else{
        cout<<"palabra no reconocida: "<<palabra;
        exit(EXIT_FAILURE);
    }
}

void lector::token(char c){
    switch(c){
    case '{':lista->push_back(llaveizq);break;
    case ',':lista->push_back(coma);break;
    case '}':lista->push_back(llaveder);break;
    case '=':lista->push_back(igual);break;
    case '\0':lista->push_back(eol);break;
    case 'w':lista->push_back(W);break;
    case 'a':lista->push_back(A);break;
    case 's':lista->push_back(S);break;
    case 'd':lista->push_back(D);break;
    //' ' y # no hacer nada
    }
}

void lector::cargarDefs(){
    string linea;
    while(getline(archPiezas,linea)){
        if(linea.empty()) continue;
        if(linea[0]==':') break;
        if(!lista)
            lista=new list<int>;
        tokenizarLinea(linea);

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
