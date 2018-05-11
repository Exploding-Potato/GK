// OpenGL_TRIANGLE_ConsoleApplication.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "math.h"
#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define GL_PI 3.1415f

enum
{
	EXIT // wyjœcie
};


// Wielkoœæ obrotów
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLboolean iCull, iDepth, iOutline;


// Funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania
void SetupRC()
{
	// Czarne t o
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor rysuj cy - zielony
	glColor3f(0.0f, 1.0f, 0.0f);
	// Model cieniowania kolorów - p aski
	glShadeModel(GL_FLAT);
	// Wielok ty o nawini ciu zgodnym z ruchem wskazówek zegara traktowane s
	// jako skierowane do przodu. Takie ustawienie jest konieczne, poniewa
	// korzystamy z wachlarzy trójk tów.
	glFrontFace(GL_CW);
}


// Wywo ywana w celu przerysowania sceny
void RenderScene(void)
{
	// Some values
	GLint magicNumber = 18;
	GLfloat baseDiameter = 50.;
	GLfloat height = 75.;

	// Calculates angle between two vertices relative to the center vertex
	GLfloat angle = 2 * GL_PI / magicNumber;

	// Wyczyszczenie okna i bufora g³êbi
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// W czenie lub wy czenie mechanizmu eliminowania ukrytych powierzchni
	if (iCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// W³¹czenie lub wy³¹czenie mechanizmu sprawdzania g³êbi
	if (iDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	// Je¿eli ten znacznik bêdzie ustawiony, to wielok¹ty zwrócone ty³em do widza bêd¹ rysowane tylko w postaci szkieletu
	if (iOutline)
		glPolygonMode(GL_BACK, GL_LINE);
	else
		glPolygonMode(GL_BACK, GL_FILL);

	// Zapisanie stanu macierzy i wykonanie obrotu
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	// Rozpoczêcie rysowania œcian sto¿ka
	glBegin(GL_TRIANGLE_FAN);

	// Czubek sto¿ka
	glVertex3f(0.0f, 0.0f, height);

	// Wykonujemy obrót wokó³ osi oznaczamy w równych odstêpach wierzcho³ki tworz¹ce wachlarz trójk¹tów.
	for (int i = 0; i < magicNumber + 1; i++)
	{
		// Wybieranie koloru - zielony lub czerwony
		if (i % 2)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);
		
		// Definiowanie kolejnego wierzcho³ka w wachlarzu trójk¹tów
		glVertex2f(baseDiameter * sin(i * angle), baseDiameter * cos(i * angle));
	}
	glEnd();

	// Rysowanie podstway sto¿ka
	glBegin(GL_TRIANGLE_FAN);

	// Czubek sto¿ka
	glVertex3f(0.0f, 0.0f, 0.0f);

	// Wykonujemy obrót wokó³ osi oznaczamy w równych odstêpach wierzcho³ki tworz¹ce wachlarz trójk¹tów.
	for (int i = 0; i < magicNumber + 1; i++)
	{
		// Wybieranie koloru - zielony lub czerwony
		if (i % 2)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);

		// Definiowanie kolejnego wierzcho³ka w wachlarzu trójk¹tów
		glVertex2f(baseDiameter * sin(-i * angle), baseDiameter * cos(-i * angle));
	}
	glEnd();

	// Odtworzenie macierzy przekszta³ceñ
	glPopMatrix();
	// Wys³anie poleceñ do wykonania
	glutSwapBuffers();
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
	// Od wie enie zawarto ci okna
	glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;
	// Ustalenie wymiarów widoku na zgodnych z wymiarami okna
	glViewport(0, 0, w, h);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Utworzenie przestrzeni ograniczaj cej (lewo, prawo, dó , góra, blisko, daleko)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Menu(int value)
{
	switch (value)
	{
	case GL_CULL_FACE:
		iCull = !iCull;
		glutSetWindowTitle("Stozek - GL_CULL_FACE");
		RenderScene();
		break;
	case GL_DEPTH_TEST:
		iDepth = !iDepth;
		glutSetWindowTitle("Stozek - GL_DEPTH_TEST");
		RenderScene();
		break;
	case GL_LINE:
		iOutline = true;
		glutSetWindowTitle("Stozek - GL_LINE");
		RenderScene();
		break;
	case GL_FILL:
		iOutline = false;
		glutSetWindowTitle("Stozek - GL_LINE");
		RenderScene();
		break;
	case EXIT:
		exit(0);
	}
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Triangles Constructing Fan Example");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	// utworzenie podmenu - Prymitywy
	int MenuPrimitive = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_CULL_FACE", GL_CULL_FACE);
	glutAddMenuEntry("GL_DEPTH_TEST", GL_DEPTH_TEST);
	glutAddMenuEntry("Polygon mode LINE", GL_LINE);
	glutAddMenuEntry("Polygon mode FILL", GL_FILL);
	glutAddMenuEntry("Wyjecie", EXIT);
	// okre lenie przycisku myszki obs uguj cego menu podr czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
	return 0;
}