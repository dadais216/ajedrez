#include "Matriz.h"

c::c(int id, int x, int y)
:id(id),x(x),y(y){}

Matriz::Matriz()
{
    for(int i=0;i<7;i++){
        for(int j=0;j<7;j++){
            self[i][j]=0;
        }
    }
}

void Matriz::armar(array<array<int,8>,8> m){
    self=m;
}
