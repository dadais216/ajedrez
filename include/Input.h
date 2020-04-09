#ifndef INPUT_H
#define INPUT_H
struct Input
{
    Input();
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

#endif // INPUT_H
