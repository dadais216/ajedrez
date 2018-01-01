#ifndef JUEGO_H
#define JUEGO_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
using namespace std;
using namespace sf;


#include <Input.h>
#include <Manager.h>
struct Estado;

struct Juego{
    Estado* actual;
    Time time;
    Clock clock;
    Font font;
    Manager<Texture> _imagen;
    float t;
    RenderWindow _window;
    Input _input;
    Event event;
    Juego();
    void gameloop();
    void change(Estado*);
};



#endif // JUEGO_H
