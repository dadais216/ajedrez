
struct botonGraphics{
  Sprite sprite;
  Text text;
  noSizeAttribute(firstButton,boton);
}

struct boton{
  string name;//aca hay una indireccion(y leak) que eventualmente podria sacar
  int n,x,y;//no es que lo necesite pero ya que estamos
  boton* next;//next es para eso, si lo dejo como ahora no sirve para nada
}

void drawButton(boton* b){
  getStruct(botonGraphics,bg,stateBucket);
  bg->text.setString(b->name);
  window.draw(bg->sprite);
  window.draw(bg->text);
}

Boton::Boton(string nomb,int n_,int x_,int y_,int escala_){
  text.setFont(j->font);
    text.setString(nomb);
    text.setColor(Color::Black);
    sprite.setTexture(imagen->get("tiles.png"));
    sprite.setTextureRect(IntRect(0,32,64,32));

    escala=escala_;
    sprite.setScale(escala_,escala_);
    text.setScale(escala_/2.,escala_/2.);
    y=y_;
    x=x_;
    sprite.setPosition(x,y);
    text.setPosition(x+5,y+10);
    n=n_+1;
}
int Boton::clicked(){
    v ve=input->pixel();
    if(ve.x>=x&&ve.x<=x+64*escala&&ve.y>=y&&ve.y<=y+32*escala)
        return n;
    else
        return 0;
}
void Boton::draw(){
    window->draw(sprite);
    window->draw(text);
}

SelJugador::SelJugador(int bando_){
    bando=bando_;
    if(bando==1)
        botones.push_back(Boton("nadie",0,540,320,1));
    botones.push_back(Boton("humano",1,500+40*bando,360,1));
    botones.push_back(Boton("aleatorio",2,500+40*bando,400,1));
    botones.push_back(Boton("IA",3,500+40*bando,440,1));

    cuadrado.setFillColor(Color(0,0,0,0));
    cuadrado.setSize(sf::Vector2f(64, 32));
    cuadrado.setOutlineColor(Color::White);
    cuadrado.setOutlineThickness(1);

    selected=1;
}
void SelJugador::clicked(){
    for(Boton& b:botones){
        int n;
        if((n=b.clicked())){
            selected=n-1;
            drawScreen();
            return;
        }
    }
}
void SelJugador::draw(){
    for(Boton& b:botones)
        b.draw();
    cuadrado.setPosition(500+40*bando,320+40*selected);
    window->draw(cuadrado);
}
