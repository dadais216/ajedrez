#ifndef INPUT_H
#define INPUT_H
#include <SFML/Graphics.hpp>
#include <vec.h>
using namespace std;
using namespace sf;
struct Input{
    RenderWindow* window;
    Input(RenderWindow*);
    void check();
    v get();
    v pixel();
    void show();
    bool inRange();
    bool isInRange(v,v);
    bool inGameRange(v);
    v ve;
    bool clicked;
    bool clickCont;
    bool click();
};
#endif // INPUT_H
