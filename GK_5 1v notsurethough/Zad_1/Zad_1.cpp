// opengl_wektory_normalne.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*
(c) Janusz Ganczarski
http://www.januszg.hg.pl
JanuszG@enter.net.pl
*/

#include <GL/glut.h>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "materials.h"

// wskaźnik dostępności rozszerzenia EXT_rescale_normal

bool rescale_normal = false;

// stałe do obsługi menu podręcznego

enum
{
	BRASS,                // mosiądz
	BRONZE,               // brąz
	POLISHED_BRONZE,      // polerowany brąz
	CHROME,               // chrom
	COPPER,               // miedź
	POLISHED_COPPER,      // polerowana miedź
	GOLD,                 // złoto
	POLISHED_GOLD,        // polerowane złoto
	PEWTER,               // grafit (cyna z ołowiem)
	SILVER,               // srebro
	POLISHED_SILVER,      // polerowane srebro
	EMERALD,              // szmaragd
	JADE,                 // jadeit
	OBSIDIAN,             // obsydian
	PEARL,                // perła
	RUBY,                 // rubin
	TURQUOISE,            // turkus
	BLACK_PLASTIC,        // czarny plastik
	BLACK_RUBBER,         // czarna guma

	NORMALS_SMOOTH,       // jeden wektor normalny na wierzchołek
	NORMALS_FLAT,         // jeden wektor normalny na ścianę

	LIGHTING_DIFFUSE,
	LIGHTING_AMBIENT,
	LIGHTING_SPECULAR,
	LIGHTING_DIFFUSE_SPECULAR,

	FULL_WINDOW,          // aspekt obrazu - całe okno
	ASPECT_1_1,           // aspekt obrazu 1:1
	EXIT                  // wyjście
};

// name explains itself
const int magicNumber = 18;

// aspekt obrazu

int aspect = FULL_WINDOW;

// usunięcie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bryły obcinania

const GLdouble left = -1.0;
const GLdouble right = 1.0;
const GLdouble bottom = -1.0;
const GLdouble top = 1.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// kąty obrotu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaźnik naciśnięcia lewego przycisku myszki

int button_state = GLUT_UP;

// położenie kursora myszki

int button_x, button_y;

// współczynnik skalowania

GLfloat scale = 1.0;

// właściwości materiału - domyślnie mosiądz

const GLfloat *ambient = BrassAmbient;
const GLfloat *diffuse = BrassDiffuse;
const GLfloat *specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// wektory normalne

int normals = NORMALS_FLAT;

// Oświetlenie

GLint lighting = LIGHTING_DIFFUSE;

// współrzędne wierzchołków

GLfloat vertex[(magicNumber + 2) * 3] =
{
	0.00,	1.00,	0.00,	// v0
	1.00,	-0.50,	0.00,	// v1
	0.94,	-0.50,	0.34,	// v2
	0.77,	-0.50,	0.64,	// v3
	0.50,	-0.50,	0.87,	// v4
	0.17,	-0.50,	0.98,	// v5
	-0.17,	-0.50,	0.98,	// v6
	-0.50,	-0.50,	0.87,	// v7
	-0.77,	-0.50,	0.64,	// v8
	-0.94,	-0.50,	0.34,	// v9
	-1.00,	-0.50,	0.00,	// v10
	-0.94,	-0.50,	-0.34,	// v11
	-0.77,	-0.50,	-0.64,	// v12
	-0.50,	-0.50,	-0.87,	// v13
	-0.17,	-0.50,	-0.98,	// v14
	0.17,	-0.50,	-0.98,	// v15
	0.50,	-0.50,	-0.87,	// v16
	0.77,	-0.50,	-0.64,	// v17
	0.94,	-0.50,	-0.34,	// v18
	0.00,	-0.50,	0.00,	// v19
};

// opis ścian

int triangles[magicNumber * 2 * 3] =
{
	1, 0, 2,	// f0
	2, 0, 3,	// f1
	3, 0, 4,	// f2
	4, 0, 5,	// f3
	5, 0, 6,	// f4
	6, 0, 7,	// f5
	7, 0, 8,	// f6
	8, 0, 9,	// f7
	9, 0, 10,	// f8
	10, 0, 11,	// f9
	11, 0, 12,	// f10
	12, 0, 13,	// f11
	13, 0, 14,	// f12
	14, 0, 15,	// f13
	15, 0, 16,	// f14
	16, 0, 17,	// f15
	17, 0, 18,	// f16
	18, 0, 1,	// f17
	19, 1, 2,	// f0
	19, 2, 3,	// f1
	19, 3, 4,	// f2
	19, 4, 5,	// f3
	19, 5, 6,	// f4
	19, 6, 7,	// f5
	19, 7, 8,	// f6
	19, 8, 9,	// f7
	19, 9, 10,	// f8
	19, 10, 11,	// f9
	19, 11, 12,	// f10
	19, 12, 13,	// f11
	19, 13, 14,	// f12
	19, 14, 15,	// f13
	19, 15, 16,	// f14
	19, 16, 17,	// f15
	19, 17, 18,	// f16
	19, 18, 1,	// f17
};

// obliczanie wektora normalnego dla wybranej ściany

void Normal(GLfloat *n, int i)
{
	GLfloat v1[3], v2[3];

	// obliczenie wektorów na podstawie współrzędnych wierzchołków trójkątów
	v1[0] = vertex[3 * triangles[3 * i + 1] + 0] - vertex[3 * triangles[3 * i + 0] + 0];
	v1[1] = vertex[3 * triangles[3 * i + 1] + 1] - vertex[3 * triangles[3 * i + 0] + 1];
	v1[2] = vertex[3 * triangles[3 * i + 1] + 2] - vertex[3 * triangles[3 * i + 0] + 2];
	v2[0] = vertex[3 * triangles[3 * i + 2] + 0] - vertex[3 * triangles[3 * i + 1] + 0];
	v2[1] = vertex[3 * triangles[3 * i + 2] + 1] - vertex[3 * triangles[3 * i + 1] + 1];
	v2[2] = vertex[3 * triangles[3 * i + 2] + 2] - vertex[3 * triangles[3 * i + 1] + 2];

	// obliczenie waktora normalnego przy pomocy iloczynu wektorowego
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v1[2] * v2[0] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// normalizacja wektora

void Normalize(GLfloat *v)
{
	// obliczenie długości wektora
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	// normalizacja wektora
	if (d)
	{
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
	}
}

// funkcja generująca scenę 3D
void Display()
{
	// kolor tła - zawartość bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesunięcie układu współrzędnych obiektu do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// włączenie oświetlenia
	glEnable(GL_LIGHTING);

	// włączenie światła GL_LIGHT0
	glEnable(GL_LIGHT0);

	// Light switches
	GLfloat nothings[4] = { 0, 0, 0, 1 };

	switch (lighting) {
	case LIGHTING_DIFFUSE:
		glDisable(GL_COLOR_MATERIAL);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nothings);
		glMaterialfv(GL_FRONT, GL_SPECULAR, nothings);

		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		break;
	case LIGHTING_AMBIENT:
		glDisable(GL_LIGHT0);
		
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		glEnable(GL_COLOR_MATERIAL);
		break;
	case LIGHTING_SPECULAR:
		glDisable(GL_COLOR_MATERIAL);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nothings);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, nothings);
		
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		break;
	case LIGHTING_DIFFUSE_SPECULAR:
		glDisable(GL_COLOR_MATERIAL);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nothings);

		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		break;
	}

	// włączenie automatycznej normalizacji wektorów normalnych
	// lub automatycznego skalowania jednostkowych wektorów normalnych
	if (rescale_normal == true)
		glEnable(GL_RESCALE_NORMAL);
	else
		glEnable(GL_NORMALIZE);

	// początek definicji obiektu
	glBegin(GL_TRIANGLES);

	// generowanie obiektu gładkiego - jeden uśredniony
	// wektor normalny na wierzchołek
	if (normals == NORMALS_SMOOTH)
		for (int i = 0; i < magicNumber * 2; i++)
		{
			// obliczanie wektora normalnego dla pierwszego wierzchołka
			GLfloat n[3];
			n[0] = n[1] = n[2] = 0.0;

			// wyszukanie wszystkich ścian posiadających bieżący wierzchołek
			for (int j = 0; j < magicNumber * 2; j++)
				if (3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 2])
				{
					// dodawanie wektorów normalnych poszczególnych ścian
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);

			// obliczanie wektora normalnego dla drugiego wierzchołka
			n[0] = n[1] = n[2] = 0.0;

			// wyszukanie wszystkich ścian posiadających bieżący wierzchołek
			for (int j = 0; j < magicNumber * 2; j++)
				if (3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 2])
				{
					// dodawanie wektorów normalnych poszczególnych ścian
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);

			// obliczanie wektora normalnego dla trzeciego wierzchołka
			n[0] = n[1] = n[2] = 0.0;

			// wyszukanie wszystkich ścian posiadających bieżący wierzchołek
			for (int j = 0; j < magicNumber * 2; j++)
				if (3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 2])
				{
					// dodawanie wektorów normalnych poszczególnych ścian
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		}
	else

		// generowanie obiektu "płaskiego" - jeden wektor normalny na ścianę
		for (int i = 0; i < magicNumber * 2; i++)
		{
			GLfloat n[3];
			Normal(n, i);

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
			glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
			glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		}

	// koniec definicji obiektu
	glEnd();

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// zmiana wielkości okna

void Reshape(int width, int height)
{
	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bryły obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	Display();
}

// obsługa klawiatury

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		scale += 0.05;
	else

		// klawisz -
		if (key == '-' && scale > 0.05)
			scale -= 0.05;

	// narysowanie sceny
	Display();
}

// obsługa klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w lewo
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;

		// kursor w górę
	case GLUT_KEY_UP:
		rotatex -= 1;
		break;

		// kursor w prawo
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;

		// kursor w dół
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obsługa przycisków myszki

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

// obsługa menu podręcznego

void Menu(int value)
{
	switch (value)
	{
		// materiał - mosiądz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		Display();
		break;

		// materiał - brąz
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		Display();
		break;

		// materiał - polerowany brąz
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		Display();
		break;

		// materiał - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		Display();
		break;

		// materiał - miedź
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		Display();
		break;

		// materiał - polerowana miedź
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		Display();
		break;

		// materiał - złoto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		Display();
		break;

		// materiał - polerowane złoto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		Display();
		break;

		// materiał - grafit (cyna z ołowiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		Display();
		break;

		// materiał - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		Display();
		break;

		// materiał - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		Display();
		break;

		// materiał - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		Display();
		break;

		// materiał - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		Display();
		break;

		// materiał - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		Display();
		break;

		// materiał - perła
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		Display();
		break;

		// metariał - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		Display();
		break;

		// materiał - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		Display();
		break;

		// materiał - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		Display();
		break;

		// materiał - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
		Display();
		break;

		// wektory normalne - GLU_SMOOTH
	case NORMALS_SMOOTH:
		normals = NORMALS_SMOOTH;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case NORMALS_FLAT:
		normals = NORMALS_FLAT;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case LIGHTING_DIFFUSE:
		lighting = LIGHTING_DIFFUSE;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case LIGHTING_AMBIENT:
		lighting = LIGHTING_AMBIENT;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case LIGHTING_SPECULAR:
		lighting = LIGHTING_SPECULAR;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case LIGHTING_DIFFUSE_SPECULAR:
		lighting = LIGHTING_DIFFUSE_SPECULAR;
		Display();
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

// sprawdzenie i przygotowanie obsługi wybranych rozszerzeñ

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf_s(version, "%d.%d", &major, &minor) != 2)
	{
		printf("Bledny format wersji OpenGL\n");
		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.2
	if (major > 1 || minor >= 2)
		rescale_normal = true;
	else
		// sprawdzenie czy jest obsługiwane rozszerzenie EXT_rescale_normal
		if (glutExtensionSupported("GL_EXT_rescale_normal"))
			rescale_normal = true;
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
	glutCreateWindow("Wektory normalne");

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(Display);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);

	// dołączenie funkcji obsługi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podręcznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - Materiał
	int MenuMaterial = glutCreateMenu(Menu);

	glutAddMenuEntry("Mosiadz", BRASS);
	glutAddMenuEntry("Braz", BRONZE);
	glutAddMenuEntry("Polerowany braz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedz", COPPER);
	glutAddMenuEntry("Polerowana miedz", POLISHED_COPPER);
	glutAddMenuEntry("Zloto", GOLD);
	glutAddMenuEntry("Polerowane zloto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z olowiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Perla", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

	// utworzenie podmenu - Wektory normalne
	int MenuNormals = glutCreateMenu(Menu);
	glutAddMenuEntry("Jeden wektor normalny na wierzcholek", NORMALS_SMOOTH);
	glutAddMenuEntry("Jeden wektor normalny na sciane", NORMALS_FLAT);

	// utworzenie podmenu - oświetlenie
	int MenuLighting = glutCreateMenu(Menu);
	glutAddMenuEntry("Kierunkowe", LIGHTING_DIFFUSE);
	glutAddMenuEntry("Globalne", LIGHTING_AMBIENT);
	glutAddMenuEntry("Reflektor", LIGHTING_SPECULAR);
	glutAddMenuEntry("Kierunkowe i reflektor", LIGHTING_DIFFUSE_SPECULAR);

	// utworzenie podmenu - aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne
	glutCreateMenu(Menu);
	glutAddSubMenu("Material", MenuMaterial);
	glutAddSubMenu("Wektory normalne", MenuNormals);
	glutAddSubMenu("Oswietlenie", MenuLighting);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);

	// określenie przycisku myszki obsługującej menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// sprawdzenie i przygotowanie obsługi wybranych rozszerzeñ
	ExtensionSetup();

	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();
	return 0;
}
