#include "Draw.h"

Draw::Draw(RenderWindow* w)
{
    window=w;
}

void Draw::setText(Texture& tex){
    spt.setTexture(tex);
}

void Draw::pos(int x,int y){
    spt.setPosition(x,y);
}

void Draw::setRect(int pxx,int pyy,int spxx,int spyy){
    spt.setTextureRect(IntRect(pxx,pyy,spxx,spyy));
}

void Draw::draw(){
    window->draw(spt);
}

Draw::~Draw()
{
    //dtor
}

Anim::Anim(RenderWindow* w,int we, int he,int ipxx, int ipyy,int fpxx,int fpyy,float spxx,float spyy)
:Draw(w),h(he),w(we),px(0),py(0),ipx(ipxx),ipy(ipyy),fpx(fpxx),fpy(fpyy),spx(spxx),spy(spyy),end(false)
{
    updateimage();
}

Anim::Anim(RenderWindow* w)
:Draw(w)
{}

void Anim::setdata(int we, int he,int ipxx, int ipyy,int fpxx,int fpyy,float spxx,float spyy){
    h=he;
    w=we;
    px=0;
    py=0;
    ipx=ipxx;
    ipy=ipyy;
    fpx=fpxx;
    fpy=fpyy;
    spx=spxx;
    spy=spyy;
    setRect(ipx*32,ipy*32,spx*32,spy*32);
}

void Anim::updateimage(){
//    cout<<px<<" "<<py<<endl;
    spt.setTextureRect(IntRect(px*32,py*32,spx*32,spy*32));
}

void Anim::sig(){
    beg=false;
    if(py==fpy&&px==fpx){
        end=true;
        return;
    }
    if(px!=w){
        px+=spx;
    }else if(py!=h){
        py+=spy;
        w=0;
    }
    updateimage();
}

void Anim::ant(){
    end=false;
    if(py==ipy&&px==ipx){
        beg=true;
        return;
    }
    if(px==0){
        py-=spy;
        px=w;
    }else{
        px-=spx;
    }
    updateimage();
}

void Anim::coord(int a, int b){
    end=false;
    beg=false;
    px=a;
    py=b;
    updateimage();
}

void Anim::start(){
    end=false;
    beg=true;
    px=0;
    py=0;
    updateimage();
}

void Anim::toend(){
    end=true;
    beg=false;
    px=fpx;
    py=h;
    updateimage();
}

bool Anim::ended(){
    return end;
}
bool Anim::atstart(){
    return beg;
}
