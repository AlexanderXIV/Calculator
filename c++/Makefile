all:
	g++ -Wall -Wextra -Wconversion -Wmaybe-uninitialized -fsanitize=undefined,address -D_GLIBCXX_DEBUG -g tokenizer.cpp mathematics.cpp parser.cpp visualisation_svg.cpp main.cpp -o calculator
	./calculator

allPNG:
	g++ -Wall -Wextra -Wconversion -Wmaybe-uninitialized -fsanitize=undefined,address -D_GLIBCXX_DEBUG -g tokenizer.cpp mathematics.cpp parser.cpp visualisation_png.cpp visualisation_svg.cpp main.cpp -o calculator -lpng
	./calculator

performance:
	g++ -O2 -g parser.cpp visualisation_svg.cpp main.cpp -o calculator
	./calculator

run:
	./calculator

clean:
	rm -f calculator *.out