
## Descripción ##
Proyecto (WIP) donde uno puede crear juegos similares al ajedrez o las damas, especificamente juegos en tableros formados por baldosas que pueden ser ocupadas por piezas, estas piezas teniendo una serie de movimientos. El objetivo es que estos juegos arbitrarios sean jugados por una inteligencia artificial de forma razonablemente bien.

El usuario puede definir la forma del tablero, la cantidad, tipo de piezas y los movimientos que pueden hacer estas piezas, usando un lenguaje de descripción de movimientos hecho especificamente con ese fin. Los movimientos pueden ser simples, por ejemplo una torre, hasta piezas considerablemente mas complejas como la dama o el rey, teniendo en cuenta que tiene que implementar las reglas de jaque. El lenguaje es turing completo por lo que en principio permite cualquier accion dependiente del estado actual del tablero y tal vez alguna memoria externa.

## Ejemplo del lenguaje ##
Una torre de ajedrez se mueve en las direcciones ortogonales, siguiendo a traves de espacios vacios y cortando en una pieza enemiga. En caso de llegar al borde del tablero o encontrarse con una pieza amiga corta. Cada paso de este movimiento es valido, no solo el ultimo. El codigo para describir esto es:

\>W=a,w,s,d;
desliz W mover c end W capturar c;

W es un macro que se expande la sentencia en la que se encuentra en 4 versiones, reemplazando en cada una por a,w,s y d. Esos simbolos representan mover el puntero sobre el que se realizan las condiciones y acciones a una baldosa contigua, en una direccion.
mover mueve la pieza si la baldosa esta vacia.
capturar captura una pieza si es enemiga.
desliz es un bucle que repite su contenido siempre que se cumpla las condiciones que contiene, en este caso solo la condicion de encontrar la baldosa vacia y la condicion implicita de no salir del tablero. 
c indica que lo que se evaluó hasta el momento es un movimiento valido, que puede ser seleccionado por el jugador o la maquina.

## Tecnologia ##
El proyecto esta programado en c++ usando SFML para manejar los graficos. El estilo de programacion es estructurado, ya que considero importante priorizar la eficiencia. Programar de esta forma me permite tener un control fino sobre el manejo de memoria y una vision clara del procesamiento. La eficiencia es importante en este caso porque el algoritmo usado por la inteligencia artificial es un min max cercano a fuerza bruta, que es bastante exigente, y por encima de esto el dominio sobre el que se aplica es dinamico al estar definido por el usuario, por lo que muchas optimizaciones que se podrían hacer para juegos especificos no aplican. 

## Pantallazos ##
![p1](https://github.com/dadais216/ajedrez/tree/master/fotos/Selection_555.png)
![p2](https://github.com/dadais216/ajedrez/tree/master/fotos/Selection_556.png)
![p3](https://github.com/dadais216/ajedrez/tree/master/fotos/f5c040f1-ce7e-4352-84c9-fa252c3373d3.jpg)
![p4](https://github.com/dadais216/ajedrez/tree/master/fotos/190069c9-ee3a-48bf-a437-721d3c01b10e.jpg)


