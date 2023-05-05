#include <GL/glut.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define WINDOW_SIZE 600
#define NB_SQUARE 3

#define DELAY 500

typedef struct Square {
    bool isBlack;
    bool nextState;
} Square;

Square grid[NB_SQUARE][NB_SQUARE];
int mouseX = -1, mouseY = -1;
bool simulationRunning = false;
int generationCount = 0;

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}


void createGrid() {
    for (int i = 0; i < NB_SQUARE; i++) {
        for (int j = 0; j < NB_SQUARE; j++) {
            grid[i][j].isBlack = true;
        }
    }
}

int countAliveNeighbors(int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int neighborX = x + i;
            int neighborY = y + j;
            if (neighborX >= 0 && neighborX < NB_SQUARE && neighborY >= 0 && neighborY < NB_SQUARE) {
                count += !grid[neighborY][neighborX].isBlack; // Changez l'ordre des indices ici
            }
        }
    }
    return count;
}

void updateGrid() {
    for (int i = 0; i < NB_SQUARE; i++) {
        for (int j = 0; j < NB_SQUARE; j++) {
            int aliveNeighbors = countAliveNeighbors(i, j);
            if (!grid[i][j].isBlack) { // Si le carré est vivant (blanc)
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    grid[i][j].nextState = true; // Le carré meurt (devient noir)
                } else {
                    grid[i][j].nextState = false; // Le carré reste vivant (blanc)
                }
            } else { // Si le carré est mort (noir)
                if (aliveNeighbors == 3) {
                    grid[i][j].nextState = false; // Le carré naît (devient blanc)
                } else {
                    grid[i][j].nextState = true; // Le carré reste mort (noir)
                }
            }
        }
    }

    for (int i = 0; i < NB_SQUARE; i++) {
        for (int j = 0; j < NB_SQUARE; j++) {
            grid[i][j].isBlack = grid[i][j].nextState;
        }
    }
}



void drawSquare(int x, int y, bool isBlack, bool isHighlighted) {
    if (isHighlighted && isBlack) {
        glColor3f(0.5, 0.5, 0.5);
    } else {
        glColor3f(isBlack ? 0.0 : 1.0, isBlack ? 0.0 : 1.0, isBlack ? 0.0 : 1.0);
    }
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + 1, y);
    glVertex2i(x + 1, y + 1);
    glVertex2i(x, y + 1);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int i = 0; i < NB_SQUARE; i++) {
        for (int j = 0; j < NB_SQUARE; j++) {
            bool isHighlighted = (mouseX == i && mouseY == j);
            drawSquare(i, j, grid[i][j].isBlack, isHighlighted && grid[i][j].isBlack);
        }
    }

    // Affiche le nombre de générations en haut à gauche.
    glColor3f(0.0, 0.0, 1.0);
    char generationText[100];
    sprintf(generationText, "Generation: %d", generationCount);
    drawText(generationText, 5, WINDOW_SIZE - 20);
    //usleep(DELAY * 1000);
    glFlush();
}


void mouseMove(int x, int y) {
    mouseX = x / (500 / NB_SQUARE);
    mouseY = NB_SQUARE - 1 - y / (500 / NB_SQUARE);
    glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int clickedX = x / (500 / NB_SQUARE);
        int clickedY = NB_SQUARE - 1 - y / (500 / NB_SQUARE);
        grid[clickedX][clickedY].isBlack = false;
        glutPostRedisplay();
    }
}

void keyPressed(unsigned char key, int x, int y) {
    if (key == ' ') {
        simulationRunning = !simulationRunning;
        if (simulationRunning) {
            glutIdleFunc(updateGrid);
        } else {
            glutIdleFunc(NULL);
        }
    }
}

void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, NB_SQUARE, 0, NB_SQUARE);
}

int main(int argc, char** argv) {
    createGrid();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Grille de carres");
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyPressed);

    init();

    glutMainLoop();

    return 0;
}
