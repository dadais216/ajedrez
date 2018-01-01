#ifndef VEC_H_INCLUDED
#define VEC_H_INCLUDED

#include <iostream>
using namespace std;
struct v{
    int x;
    int y;
    int tec; //para pasar datos tecnicos, para que los casos especiales se menejen con esto en vez de numeros especificos
    v(){};
    v(int x,int y)
    :x(x),y(y),tec(0){}
    void show(){
        cout<<"("<<x<<","<<y<<")"<<endl;
    }
    friend v operator+(v a,v b){
        //cout<<a.x+b.x<<","<<a.y+b.y<<endl;
        return v(a.x+b.x,a.y+b.y);
    }
    friend bool operator==(v a,v b){
        return a.x==b.x&&a.y==b.y;
    }
};

inline int abso(int n){
    if(n>=0){
        return n;
    }
    return -n;
}

inline int sgn(int n){
    if(n==0){
        return 0;
    }if(n>0){
        return 1;
    }
    return -1;
}



#endif // VEC_H_INCLUDED
