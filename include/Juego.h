#ifndef JUEGO_H
#define JUEGO_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
using namespace std;
using namespace sf;


#include "Input.h"
#include "Manager.h"
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

void auxDraw();



#endif // JUEGO_H
