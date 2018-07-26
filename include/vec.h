#ifndef VEC_H_INCLUDED
#define VEC_H_INCLUDED

#include <iostream>
using namespace std;
struct v
{
    int x;
    int y;
    v(){
        x=216;
        y=216;
    };
    v(int x,int y)
        :x(x),y(y) {}
    v show()
    {
        cout<<"("<<x<<","<<y<<")"<<endl;
        return *this;
    }
    friend v operator+(v a,v b)
    {
        //cout<<a.x+b.x<<","<<a.y+b.y<<endl;
        return v(a.x+b.x,a.y+b.y);
    }

    friend bool operator==(v a,v b)
    {
        return a.x==b.x&&a.y==b.y;
    }
    friend bool operator!=(v a,v b)
    {
        return a.x!=b.x||a.y!=b.y;
    }
    v operator=(v vec)
    {
        x=vec.x;
        y=vec.y;
        return vec;
    }
    friend ostream& operator<<(ostream&,v);
};

inline int abso(int n){
    if(n>=0)
        return n;
    return -n;
}

inline int sgn(int n){
    if(n==0)
        return 0;
    if(n>0)
        return 1;
    return -1;
}

inline ostream& operator<<(ostream& out,v a){
    cout<<" ("<<a.x<<","<<a.y<<") ";
    return out;
}



#endif // VEC_H_INCLUDED
