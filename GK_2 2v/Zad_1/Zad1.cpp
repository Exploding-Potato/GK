// OpenGL_POINTS_ConsoleApplication.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
// Definiujemy na sta�o warto�ci liczby PI
#define GL_PI 3.1415f

static GLfloat rotations = 18;
static GLfloat pointSizeStep = 0.007;

// Wielko�ci obrot�w
static GLfloat xRot = 0.0f;
static GLfloat yStepSize = 0.14;
static GLfloat yRot = 0.0f;

// Wywo�ywana w celu przerysowania sceny
void RenderScene(void)
{
	GLfloat x, y, z, angle, pointSize = 1; // Zmienne przechowuj�ce wsp�rz�dne i k�ty
	
	// Wyczyszczenie okna aktualnym kolorem czyszcz�cym
	glClear(GL_COLOR_BUFFER_BIT);

	// Zapisanie stanu macierzy i wykonanie obrotu
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	
	// Wyliczenie pocz�tkowego y - czysty chaos ale dzia�a
	y = -(50.0f * yStepSize * 2 * rotations) / 3;
	for (angle = 0.0f; angle <= (2.0f*GL_PI)*rotations; angle += 0.1f)
	{
		glPointSize(pointSize);
		glBegin(GL_POINTS);

		x = 50.0f*sin(angle);
		z = 50.0f*cos(angle);
		// Okre�lenie punktu i przesuni�cie wsp�rz�dnej Z
		glVertex3f(x, y, z);
		y += yStepSize;

		// Zako�czenie rysowania punkt�w
		glEnd();

		pointSize += pointSizeStep;
	}

	// Odtworzenie macierzy przekszta�ce�
	glPopMatrix();
	// Wykonanie polece� rysowania
	glutSwapBuffers();
}


// Ta funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania
void SetupRC()
{
	// Czarne t�o
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// B�dziemy rysowa� kolorem zielonym
	glColor3f(0.0f, 1.0f, 0.0f);
}


void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;
	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;

	if (key > 356.0f)
		xRot = 0.0f;
	if (key < -1.0f)
		xRot = 355.0f;
	if (key > 356.0f)
		yRot = 0.0f;
	if (key < -1.0f)
		yRot = 355.0f;
	// Od�wie�enie zawarto�ci okna
	glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;
	// Ustalenie wymiar�w widoku na zgodnych z wymiarami okna
	glViewport(0, 0, w, h);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Utworzenie przestrzeni ograniczaj�cej (lewo, prawo, d�, g�ra, blisko, daleko)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Points Example");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}