main: main.cpp curve.cpp
	g++ -o test main.cpp curve.cpp -l GL -l GLU -l glut

clean:
	rm test
