#ifndef VEC_H_INCLUDED
#define VEC_H_INCLUDED

#include <iostream>
using namespace std;
struct v{
    int x;
    int y;
    v(){
        x=216;
        y=216;
    };
    v(int x,int y):x(x),y(y){}
    v show(){
        cout<<"("<<x<<","<<y<<")"<<endl;
        return *this;
    }


    inline friend v operator+(v a,v b){
        return v(a.x+b.x,a.y+b.y);
    }
    inline friend v operator-(v a,v b){
        return v(a.x-b.x,a.y-b.y);
    }
    inline friend bool operator==(v a,v b){
        return a.x==b.x&&a.y==b.y;
    }
    inline friend bool operator!=(v a,v b){
        return a.x!=b.x||a.y!=b.y;
    }
    inline v operator=(v vec){
        x=vec.x;
        y=vec.y;
        return vec;
    }
    friend ostream& operator<<(ostream&,v);
};

inline int abso(int val){
    return val>0?val:-val;
}

inline ostream& operator<<(ostream& out,v a){
    cout<<" ("<<a.x<<","<<a.y<<") ";
    return out;
}



#endif // VEC_H_INCLUDED
