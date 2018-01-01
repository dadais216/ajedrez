#include "Input.h"
Input::Input(RenderWindow* window)
:window(window),clickCont(true){}

void Input::check(){
    Vector2i vec=Mouse::getPosition(*window);
    ve.x= vec.x;
    ve.y= vec.y;
    clicked=Mouse::isButtonPressed(Mouse::Left);
    if(clickCont){
        clickCont=clicked;
    }
}

void Input::show(){
    ve.show();
}

bool Input::click(){
    if(!clickCont&&clicked){
        clickCont=true;
        return true;
    }
    return false;
}

bool Input::isInRange(v a, v b){
    return ve.x>=a.x&&ve.x<=b.x&&ve.y>=a.y&&ve.y<=b.y;
}

bool Input::inRange(){
    return ve.x>=0&&ve.x<=640&&ve.y>=0&&ve.y<=512;
}

bool Input::inGameRange(v tam){
    return ve.x>=0&&ve.x<tam.x*64&&ve.y>=0&&ve.y<tam.y*64;
}

v Input::get(){
    return v(ve.x/64,ve.y/64);
}

v Input::pixel(){
    if(inRange()){
        return ve;
    }
    return v(0,0);
}
