#ifndef INPUT_H
#define INPUT_H




struct{
  bool leftClick;
  bool rightClick;
  bool r;
  bool z;
  bool x;
  bool c;
  int wheelDelta;
  v mouse;

  bool rEnclave;
  bool zEnclave;
}Input;

//en un momento pense en manejar los eventos en un thread pero no sé hasta que punto sería util
//me dejaría hacer cosas como manejar eventos de ventana mientras esta corriendo la ia o el sistema debug
//pero es medio raro porque lo toma de input esta ahi y estaría desincronizado de el codigo que la consume
//y eso llevaría a cosas raras como comerme inputs o bufferear por mas de lo necesario, y al pedo
//ademas no me ahorra la necesidad de hacer un bucle en el codigo del jugador humano, por ejemplo, porque
//aunque la toma de input se haga asincronica tengo que preguntar por esta sincronicamente
//lo ideal sería tener un lugar solo centralizado donde se maneje el input cada frame, pero hacer eso necesitaria
//que guarde informacion para retomarla despues, en el sistema debug por ejemplo, y mantener esos estados intermedios
//solo por esto no tiene mucho sentido. El problema de manejar el input desde varios lugares es que un sistema podría
//comerse el input de otro, pero bueno, en este programa sistema debug toma prioridad asi que esta bien que se ignore
//lo que va al juego normal, y cosas como el manejo de ventana se manejan aca, por fuera de los sistemas, asi que funciona
void handleSystemEvents(){
  Input.leftClick=false;
  Input.rightClick=false;
  Input.r=false;
  Input.z=false;
  Input.wheelDelta=0;
  Input.mouse=v(-1,-1);

  sf::Event event;
  while(window.pollEvent(event)){
    if(event.type == Event::Closed){
      window.close();
      exit(0);
    }else if(event.type == sf::Event::EventType::MouseWheelScrolled){
      Input.wheelDelta=event.mouseWheelScroll.delta;
      auto mousePos=sf::Mouse::getPosition(window);
      Input.mouse.x=mousePos.x;
      Input.mouse.y=mousePos.y;
    }else if(event.type==sf::Event::EventType::MouseButtonPressed){
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        Input.leftClick=true;
        auto mousePos=sf::Mouse::getPosition(window);
        Input.mouse.x=mousePos.x;
        Input.mouse.y=mousePos.y;
      }
      if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
        Input.rightClick=true;
      }
      //if(sf::Mouse::isButtonPressed(sf::Mouse::XButton1)){
      //  rightB=true;
      //}
      //if(sf::Mouse::isButtonPressed(sf::Mouse::XButton2)){
      //  leftB=true;
      //}
      //}else if(event.type==sf::Event::EventType::MouseButtonReleased){
      //if(!sf::Mouse::isButtonPressed(sf::Mouse::Left)){
      //  clickPressed=false;
      //}
    }
  }
  //no uso el evento porque hace lo mismo, en el caso de click si enclava solo
  Input.x=sf::Keyboard::isKeyPressed(sf::Keyboard::X);
  Input.c=sf::Keyboard::isKeyPressed(sf::Keyboard::C);
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){
    if(!Input.zEnclave){
      Input.z=true;
      Input.zEnclave=true;
    }
  }else
    Input.zEnclave=false;
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
    if(!Input.rEnclave){
      Input.r=true;
      Input.rEnclave=true;
    }
  }else
    Input.rEnclave=false;
}


//TODO mirar esto
float escala;

/*
bool isInRange(v a, v b)
{
  return ve.x>=a.x&&ve.x<=b.x&&ve.y>=a.y&&ve.y<=b.y;
}
*/

bool clickInRange(){
  return Input.mouse.x>=0&&Input.mouse.x<=640&&Input.mouse.y>=0&&Input.mouse.y<=512;
}

bool inGameRange(v tam){
  return Input.mouse.x>=0&&Input.mouse.x<tam.x*32*escala&&Input.mouse.y>=0&&Input.mouse.y<tam.y*32*escala;
}

v getClickInGameCoordinates(){
  return v(Input.mouse.x/(32*escala),Input.mouse.y/(32*escala));
}

v getClickInPixelCoordinates(){
  if(clickInRange()){
    return Input.mouse;
  }
  return v(0,0);//bizarro
}

#endif // INPUT_H
