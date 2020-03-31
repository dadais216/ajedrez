#ifndef VEC_H_INCLUDED
#define VEC_H_INCLUDED

struct v{
    int x;
    int y;
    v(){
        x=216;
        y=216;
    };
    v(int x,int y):x(x),y(y){}
    v show(){
      std::cout<<"("<<x<<","<<y<<")"<<std::endl;
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
  friend std::ostream& operator<<(std::ostream&,v);
};

inline int sign(int val){
  return val>0?1:-1;
}

inline std::ostream& operator<<(std::ostream& out,v a){
  std::cout<<" ("<<a.x<<","<<a.y<<") ";
    return out;
}



#endif // VEC_H_INCLUDED
