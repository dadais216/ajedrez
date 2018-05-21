#include "../include/Juego.h"
#include "../include/Estado.h"
#include "../include/global.h"

Juego* j;
Input* input;
RenderWindow* window;
Manager<Texture>* imagen;

Juego::Juego()
    :_window(VideoMode(640, 512), "ajedrez")
    ,_input(&_window)
{
    _imagen.adddir("sprites/");
    _input.check();
    font.loadFromFile("sprites/VL-PGothic-Regular.ttf");

    j=this;
    input=&this->_input;
    window=&this->_window;
    imagen=&this->_imagen;
    actual= new Arranque();
    drawScreen();
}
void Juego::gameloop()
{
    clock.restart();
    float dt=0;
    while(true)
    {
        dt+=clock.restart().asSeconds();
        while(dt>1./60.)
        {
            dt-=1./60.;
            _input.check();
            actual->update();
            while (_window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    _window.close();
                    return;
                }
            }
        }

    }
}

void Juego::change(Estado* est)
{
    delete actual;
    actual=est;
    drawScreen();
}

void Juego::draw()
{
    _window.clear(Color(209,177,158));
    actual->draw();
    _window.display();
}

void drawScreen()
{
    j->draw();
}
