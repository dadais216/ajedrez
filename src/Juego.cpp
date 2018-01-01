#include "Juego.h"
#include "Estado.h"
#include <global.h>

Juego* j;
Input* input;
RenderWindow* window;
Manager<Texture>* imagen;

Juego::Juego()
:_window(VideoMode(640, 512), "ajedrez")
,_input(&_window)
{
    _imagen.adddir("sprites\\");
    time=seconds(0);
    _input.check();
    font.loadFromFile("sprites\\VL-PGothic-Regular.ttf");

    j=this;
    input=&this->_input;
    window=&this->_window;
    imagen=&this->_imagen;
    actual= new Arranque();
}
void Juego::gameloop(){
    while(true){
        //time=clock.getElapsedTime();
        //t=time.asSeconds();
        _input.check();
        actual->update();
        _window.clear();
        actual->draw(); //en realidad no hay necesidad de dibujar en cada tick, podría delegarse a update esto
        _window.display();
        while (_window.pollEvent(event)){
            if (event.type == Event::Closed){
                _window.close();
                return;
            }
        }
    }
}

void Juego::change(Estado* est){
    delete actual;
    actual=est;
}
