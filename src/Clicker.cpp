#include "Clicker.h"
Clicker::Clicker(v ve)
:ve(ve),tocado(false),rectangle(Vector2f(64,64)){
    rectangle.setFillColor(Color::Green);
    rectangle.setPosition(ve.x*64,ve.y*64);
}

void Clicker::draw(){
    window->draw(rectangle);
}
