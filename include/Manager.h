#ifndef MANAGER_H
#define MANAGER_H
#include <iostream>
#include <map>
#include <list>
using namespace std;
template<class T>
class Manager{
private:
    map<string,T> archivos;
    list<string> dir;
public:
    Manager(){};
    void adddir(string dire){
        for(list<string>::iterator it=dir.begin();it!=dir.end();it++){
            if(*it==dire){
                cout<<"la direccion "<<dire<<" ya esta agregada"<<endl;
                return;
            }
        }
        dir.push_back(dire);
    }
    void removedir(string dire){
        for(list<string>::iterator it=dir.begin();it!=dir.end();it++){
            if(*it==dire){
                dir.erase(it);
                return;
            }
        }
        cout<<"la direccion "<<dire<<" ya estaba borrada o nunca estuvo"<<endl;
    }
    T& get(string filename){
        for(typename map<string,T>::iterator it=archivos.begin();it!=archivos.end();it++){
            if(filename==it->first){
                return it->second;
            }
        }
        cout<<"agregando "<<filename<<endl;
        T t;
        if(t.loadFromFile(filename)){
            archivos[filename]=t;
            return archivos[filename]; //no entiendo por que return t no anda pero esto si
        }
        for(list<string>::iterator it=dir.begin();it!=dir.end();it++){
            if(t.loadFromFile(*it+filename)){
                archivos[filename]=t;
                return archivos[filename];
            }
        }
        cout<<"ERROR el archivo "<<filename<<" no existe"<<endl;
        archivos[filename]=t;
        return archivos[filename];
    }
    void remove(string filename){
        typename map<string,T>::iterator it=archivos.find(filename);
        if(it!=archivos.end()){
            archivos.erase(it);
            return;
        }
        cout<<"ERROR el archivo "<<filename<<" ya fue borrado o nunca estuvo"<<endl;
    }
};
#endif // MANAGER_H
