#ifndef INPUT_H
#define INPUT_H
struct Input
{
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
float escala;
Input* input;

#endif // INPUT_H
