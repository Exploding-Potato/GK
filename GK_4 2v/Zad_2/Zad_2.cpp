// opengl_tekstura_alfa.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include "glext.h"
#ifndef WIN32
#define GLX_GLXEXT_LEGACY
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif
#include <stdlib.h>
#include <stdio.h>
#include "colors.h"
#include "targa.h"

// Wskaźnik na funkcję glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// Stałe do obsługi menu podręcznego

enum
{
	FULL_WINDOW,    // aspekt obrazu - całe okno
	ASPECT_1_1,     // aspekt obrazu 1:1
	EXIT            // wyjście
};

// Aspekt obrazu

int aspect = FULL_WINDOW;

// Usunięcie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// Fuck
GLint magicNumber = 18;

// Rozmiary bryły obcinania
const GLdouble left = -1.2;
const GLdouble right = 1.2;
const GLdouble bottom = -1.2;
const GLdouble top = 1.2;
const GLdouble near = 2.0;
const GLdouble far = 30.0;

// Kąty obrotu
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// Wskaźnik naciśnięcia lewego przycisku myszki
int button_state = GLUT_UP;

// Położenie kursora myszki
int button_x, button_y;

// Współczynnik skalowania
GLfloat scale = 1.5;

// Identyfikatory tekstur
GLuint GRASS, TREE;

// Identyfikatory list wyświetlania
GLint GRASS_LIST, TREE_LIST;

// Włączenie/wyłączenie
bool alpha_test = true;
bool blend = true;

// Funkcja rysująca napis w wybranym miejscu (wersja korzystająca z funkcji glWindowPos2i)
void DrawString(GLint x, GLint y, char *string)
{
	// Położenie napisu
	glWindowPos2i(x, y);

	// Wyświetlenie napisu
	int len = strlen(string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}

// Funkcja generująca scenę 3D

void DisplayScene()
{
	// Kolor tła - zawartość bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// Macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// Przesunięcie układu współrzędnych obiektów do środka bryły odcinania
	glTranslatef(0.0, 0.0, -(near + far) / 2);

	// Obroty obiektu
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);

	// Skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// Włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// Włączenie teksturowania dwuwymiarowego
	glEnable(GL_TEXTURE_2D);

	// Filtr powiększający
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Filtr pomniejszający
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Wskazówki do korekcji perspektywy przy renderingu tekstur
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Ustawienie parametów środowiska tekstur
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Włączenie/wyłączenie testowania kanału alfa
	if (alpha_test == GL_TRUE)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
	}
	else
		glDisable(GL_ALPHA_TEST);

	// Włączenie/wyłączenie mieszania kolorów
	if (blend == GL_TRUE)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);

	// Rysowanie trawy
	glBindTexture(GL_TEXTURE_2D, GRASS);
	glPushMatrix();
	glTranslatef(0.0, -0.5, 0.0);
	glCallList(GRASS_LIST);
	glPopMatrix();

	// Rysowanie trzech drzew
	glBindTexture(GL_TEXTURE_2D, TREE);
	glPushMatrix();

	glScalef(0.5, 0.5, 0.5);

	glTranslatef(-1. * magicNumber / 2, .0, -.5 * magicNumber / 2);
	for (GLint i = 0; i < magicNumber; i++)
	{
		glCallList(TREE_LIST);
		glTranslatef(1.0, 0.0, 0.5);
	}
	glPopMatrix();

	// Wyłączenie teksturowania dwuwymiarowego
	glDisable(GL_TEXTURE_2D);

	// Wyświetlenie wybranych informacje
	char string[200];
	GLboolean var;
	glColor3fv(Black);

	// Pobranie inforacji o włączeniu testowania kanału alfa
	glGetBooleanv(GL_ALPHA_TEST, &var);
	if (var == GL_TRUE)
		strcpy_s(string, "GL_ALPHA_TEST = GL_TRUE");
	else
		strcpy_s(string, "GL_ALPHA_TEST = GL_FALSE");
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 17, string);

	// Pobranie informacji o włączeniu mieszania kolorów
	glGetBooleanv(GL_BLEND, &var);
	if (var == GL_TRUE)
		strcpy_s(string, "GL_BLEND = GL_TRUE");
	else
		strcpy_s(string, "GL_BLEND = GL_FALSE");
	DrawString(2, glutGet(GLUT_WINDOW_HEIGHT) - 33, string);

	// Skierowanie poleceń do wykonania
	glFlush();

	// Zamiana buforów koloru
	glutSwapBuffers();
}

// Zmiana wielkości okna
void Reshape(int width, int height)
{
	// Obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// Wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// Macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// Parametry bryły obcinania
	if (aspect == ASPECT_1_1)
	{
		// Wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// Szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// Generowanie sceny 3D
	DisplayScene();
}

// Obsługa klawiatury
void Keyboard(unsigned char key, int x, int y)
{
	// Klawisz +
	if (key == '+')
		scale += 0.05;
	else

		// Klawisz -
		if (key == '-' && scale > 0.05)
			scale -= 0.05;

	// Narysowanie sceny
	DisplayScene();
}

// Obsługa przycisków myszki
void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiętanie położenia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obsługa ruchu kursora myszki
void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// utworzenie tekstur
void GenerateTextures()
{
	// zmienne użyte przy obsłudze plików TARGA
	GLsizei width, height;
	GLenum format, type;
	GLvoid *pixels;

	// tryb upakowania bajtów danych tekstury
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// wczytanie tekstury grass_messy.tga
	GLboolean error = load_targa("grass_messy.tga", width, height, format, type, pixels);

	// błąd odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku grass_messy.tga");
		exit(0);
	}

	// utworzenie identyfikatora tekstury
	glGenTextures(1, &GRASS);

	// dowiązanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, GRASS);

	// włączenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, type, pixels);

	// porządki
	delete[](unsigned char*)pixels;

	// wczytanie tekstury oak_tree_medium.tga
	error = load_targa("oak_tree_medium.tga", width, height, format, type, pixels);

	// błąd odczytu pliku
	if (error == GL_FALSE)
	{
		printf("Niepoprawny odczyt pliku oak_tree_medium.tga");
		exit(0);
	}

	// utworzenie identyfikatora tekstury
	glGenTextures(1, &TREE);

	// dowiązanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, TREE);

	// włączenie automatycznego generowania mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// definiowanie tekstury (z mipmapami)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, pixels);

	// porządki
	delete[](unsigned char*)pixels;
}

// obsługa menu podręcznego

void Menu(int value)
{
	switch (value)
	{
		// test kanału alfa: włącz/wyłącz
	case GL_ALPHA_TEST:
		alpha_test = !alpha_test;
		DisplayScene();
		break;

		// mieszanie kolorów: włącz/wyłącz
	case GL_BLEND:
		blend = !blend;
		DisplayScene();
		break;

		// obszar renderingu - całe okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjście
	case EXIT:
		exit(0);
	}
}

// utworzenie list wyświetlania

void GenerateDisplayLists()
{
	// generowanie identyfikatora pierwszej listy wyświetlania
	GRASS_LIST = glGenLists(1);

	// pierwsza lista wyświetlania - trawa
	glNewList(GRASS_LIST, GL_COMPILE);

	// czworokąt
	glBegin(GL_QUADS);
	glTexCoord2f(16.0, 0.0);
	glVertex3f(-8.0, 0.0, -8.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-8.0, 0.0, 8.0);
	glTexCoord2f(0.0, 16.0);
	glVertex3f(8.0, 0.0, 8.0);
	glTexCoord2f(16.0, 16.0);
	glVertex3f(8.0, 0.0, -8.0);
	glEnd();

	// koniec pierwszej listy wyświetlania
	glEndList();

	// generowanie identyfikatora drugiej listy wyświetlania
	TREE_LIST = glGenLists(1);

	// druga lista wyświetlania - drzewo
	glNewList(TREE_LIST, GL_COMPILE);

	// dwa trójkąty
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0, 1.0, 0.0);
	glEnd();

	// koniec drugiej listy wyświetlania
	glEndList();
}

// sprawdzenie i przygotowanie obsługi wybranych rozszerzeń
void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf_s(version, "%d.%d", &major, &minor) != 2)
	{
#ifdef WIN32
		printf("Błędny format wersji OpenGL\n");
#else

		printf("Bledny format wersji OpenGL\n");
#endif

		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.4 OpenGL lub
	// czy jest obsługiwane rozszerzenie GL_SGIS_generate_mipmap
	if (!(major > 1 || minor >= 4) &&
		!glutExtensionSupported("GL_SGIS_generate_mipmap"))
	{
		printf("Brak rozszerzenia GL_SGIS_generate_mipmap!\n");
		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.4
	if (major > 1 || minor >= 4)
	{
		// pobranie wskaźnika wybranej funkcji OpenGL 1.4
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
	}
	else
		// sprawdzenie czy jest obsługiwane rozszerzenie ARB_window_pos
		if (glutExtensionSupported("GL_ARB_window_pos"))
		{
			// pobranie wskaźnika wybranej funkcji rozszerzenia ARB_window_pos
			glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress
			("glWindowPos2iARB");
		}
		else
		{
			printf("Brak rozszerzenia ARB_window_pos!\n");
			exit(0);
		}
}

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie głównego okna programu
	glutCreateWindow("Tekstura alfa");

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(DisplayScene);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
#ifdef WIN32

	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);
#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
#endif

	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne
	glutCreateMenu(Menu);

#ifdef WIN32

	glutAddMenuEntry("Test kanału alfa: włącz/wyłącz", GL_ALPHA_TEST);
	glutAddMenuEntry("Mieszanie kolorów: włącz/wyłącz", GL_BLEND);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjście", EXIT);
#else

	glutAddMenuEntry("Test kanalu alfa: wlacz/wylacz", GL_ALPHA_TEST);
	glutAddMenuEntry("Mieszanie kolorow: wlacz/wylacz", GL_BLEND);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);
#endif

	// określenie przycisku myszki obsługującego menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie tekstur
	GenerateTextures();

	// sprawdzenie i przygotowanie obsługi wybranych rozszerzeń
	ExtensionSetup();

	// utworzenie list wyświetlania
	GenerateDisplayLists();

	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();

	system("pause>nul");
	return 0;
}