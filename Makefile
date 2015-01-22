all:
	g++ -o ThreadSync -lpthread ThreadSync.cpp;
	./ThreadSync