#ifndef JUEGO_H
#define JUEGO_H

struct Estado;

struct Juego
{
    Estado* actual;
    Clock clock;
    Font font;
    Manager<Texture> _imagen;
    RenderWindow _window;
    Input _input;
    Event event;
    Juego();
    void draw();
    void gameloop();
    void change(Estado*);
};

Juego* j;
RenderWindow* window;
Manager<Texture>* imagen;

void drawScreen(){
    j->draw();
}

#endif // JUEGO_H
