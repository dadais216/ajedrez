#despues de testear tiempos resulta que compilar todo de una es mas rapido que compilar en pedazos y linkear. La makefile de arriba tardó 5.23 segundos en compilar cambios en 3 archivos (23 en compilar todo), esta tardó 5.02

Debug:
	gcc -g -pipe -Wall -std=c++14 -o game all.cpp -I include/. -I src/. -lsfml-graphics -lsfml-window -lsfml-system -lstdc++ 

release:
	gcc -O3 -std=c++14 -o game all.cpp -I include/. -I src/. -lsfml-graphics -lsfml-window -lsfml-system -lstdc++ 

clean:
	rm -f $(obj) game


#src := main.cpp $(filter-out src/memGetters.cpp src/movs.cpp, $(wildcard src/*.cpp))
#obj := $(addprefix obj/,$(subst src/,,$(patsubst %.cpp, %.o, $(src))))

#obj/%.o: src/%.cpp
#	gcc -std=c++14 -c -o $@ $^ -I include/.

#obj/main.o: main.cpp
#	gcc -std=c++14 -c -o $@ $^ -I include/.


#compile: $(obj)
#	gcc -o game $^ -lsfml-graphics -lsfml-window -lsfml-system -lstdc++


