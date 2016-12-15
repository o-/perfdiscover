all : bin/run

src/gen.h : ./utils/codegen.rb
	./utils/codegen.rb 12 28 8 64

bin/run : src/gen.h src/main.cpp src/lib.h
	g++ -Isrc -O3 -std=c++11 src/main.cpp -o bin/run

clean :
	rm -f src/gen.h
	rm -f bin/*

run : all
	bin/run

bench :
	$(MAKE) clean
	$(MAKE) run
	$(MAKE) clean
	$(MAKE) run
	$(MAKE) clean
	$(MAKE) run

plot:
	R -f utils/plot.R

distclean : clean
	rm -f out/*

.PHONY: all run clean bench plot
