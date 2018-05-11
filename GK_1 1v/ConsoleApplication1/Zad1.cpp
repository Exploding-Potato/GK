// OpenGL_Animacja_Prostokat_ConsoleApplication1.cpp : Defines the entry point for the console application.
//
// Bounce.c
// Demonstrates a simple animated rectangle program with GLUT
// OpenGL SuperBible, 2nd Edition
// Richard S. Wright Jr.

#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream> using std::cout; using std::endl;
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Pocztkowy rozmiar i pozycja osiemnastok¹ta
GLfloat cX = 100.0f;
GLfloat cY = 150.0f;
GLsizei radius = 30;
GLbyte vertices = 18;

// Rozmiar kroku (liczba pikseli) w osi x i y
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;
// Dane zmieniajcych siê rozmiarów okna
GLfloat windowWidth;
GLfloat windowHeight;


///////////////////////////////////////////////////////////
// Wywo³ywana w celu przerysowania sceny
void RenderScene(void) {
	// Wyczyszczenie okna aktualnym kolorem czyszcz¹cym
	glClear(GL_COLOR_BUFFER_BIT);

	// Aktualny kolor rysuj¹cy - czerwony
	glColor3f(1.0f, 0.0f, 0.0f);
	
	// Calculates the angle in radians between two vertices seen from the center of the figure
	GLfloat angle = 2 * M_PI / vertices;

	// Draws a triangle fan
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < vertices; i++)
	{
		glVertex3f(cX + radius * cos(i * angle), cY + radius * sin(i * angle), 0.0);
	}
	glEnd();

	// Wys³anie poleceñ do wykonania - !!! dla animacji to jest inne polecenie
	glutSwapBuffers();
}


///////////////////////////////////////////////////////////
// Wywo³ywana przez bibliotek GLUT w czasie, gdy okno nie
// jest przesuwane ani nie jest zmieniana jego wielkoœæ
void TimerFunction(int value) {
	// Odwrócenie kierunku, je¿eli osi¹gniêto lew¹ lub praw¹ krawêdŸ
	if (cX > windowWidth - radius || cX < radius)
		xstep = -xstep;

	// Odwrócenie kierunku, je¿eli osi¹gniêto doln¹ lub górn¹ krawêdŸ
	if (cY > windowHeight - radius || cY < radius)
		ystep = -ystep;


	// Kontrola obramowania. Wykonywana jest na wypadek, gdyby okno     
	// zmniejszy³o swoj wielkoœæ w czasie, gdy kwadrat odbija³ siê od     
	// krawêdzi, co mog³oby spowodowaæ, ¿e znalaz³ by siê poza      
	// przestrzeni¹ ograniczajc¹.     
	if (cX > windowWidth - radius)
		cX = windowWidth - radius - 1;

	if (cY > windowHeight - radius)
		cY = windowHeight - radius - 1;

	// Wykonanie przesuniêcia kwadratu
	cX += xstep;
	cY += ystep;

	// Ponowne rysowanie sceny z nowymi wspó³rzêdnymi  
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}


///////////////////////////////////////////////////////////
// Konfigurowanie stanu renderowania
void SetupRC(void) {
	// Ustalenie niebieskiego koloru czyszcz¹cego     
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}


///////////////////////////////////////////////////////////
// Wywo³ywana przez bibliotek GLUT przy ka¿dej zmianie wielkoœci okna
void ChangeSize(GLsizei w, GLsizei h) {
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;

	// Ustalenie wielkoœci widoku zgodnego z rozmiarami okna
	glViewport(0, 0, w, h);

	// Ustalenie uk³adu wspó³rzêdnych
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Keep the square square, this time, save calculated
	// width and height for later use
	if (w <= h)
	{
		windowHeight = 250.0f*h / w;
		windowWidth = 250.0f;
	}
	else
	{
		windowWidth = 250.0f*w / h;
		windowHeight = 250.0f;
	}

	// Ustanowienie przestrzeni ograniczaj¹cej (lewo, prawo, dó³, góra, blisko, daleko)
	glOrtho(0.0f, windowWidth, 0.0f, windowHeight, 1.0f, -1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


///////////////////////////////////////////////////////////
// G³ówny punkt wejœcia programu
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Bounce");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(33, TimerFunction, 1);
	SetupRC();
	glutMainLoop();
}
