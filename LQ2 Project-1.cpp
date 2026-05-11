"""
    The OpenGL Space Scene program is a 3D space simulation with interactive elements. 
    It features a planet, a space character, a rotating eye, and a starry background. 
    The program supports fullscreen mode, distinct coloring for different objects, 
    and uses OpenGL primitives such as spheres, cubes, and points. 
    It employs Vertex Buffer Objects (VBO) for rendering stars and allows the user to manipulate the scene. 
    Actions include toggling fullscreen, moving the space character horizontally and vertically, rotating 
    the eye with mouse clicks, and scaling the planet with the mouse wheel. 
    The program's structure includes initialization functions, display functions, 
    and event handlers for keyboard and mouse input.

    Created by: Marie Kristela Frances Monje and Caila Jenine Ruado
    Section: TN32
"""


#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Camera Distance
GLfloat cameraPositionX = 0.0f;
GLfloat cameraPositionY = 0.0f;
GLfloat cameraPositionZ = 25.0f;

GLfloat object1TranslateX = 0.0f;
GLfloat object2TranslateY = 0.0f;

GLfloat object3Scale = 2.0f;

GLfloat armAngle = 0.0f;

GLfloat eyeRotation = 0.0f;

GLfloat planetScale = 1.0f;
GLfloat planetDistance = 50.0f;  

GLfloat targetScaleValue = 1.0f;
GLfloat currentScaleValue = 1.0f;
GLfloat scaleSpeed = 0.1f;

GLfloat planetScaleFactor = 1.0f;

int lastMouseY = 0;

const int numStars = 1000;
float stars[numStars][3];

bool isFullScreen = false;

GLuint starsVBO; // Vertex Buffer Object ID

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    std::srand(static_cast<unsigned int>(std::time(0)));

    glGenBuffers(1, &starsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, starsVBO);

    for (int i = 0; i < numStars; ++i) {
        stars[i][0] = static_cast<float>(std::rand() % 200 - 100);
        stars[i][1] = static_cast<float>(std::rand() % 200 - 100);
        stars[i][2] = static_cast<float>(std::rand() % 200 - 100);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(stars), stars, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 5000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawPlanet() {
    glPushMatrix();
    glTranslatef(-50.0f, 0.0f, -planetDistance);
    glScalef(planetScale * planetScaleFactor, planetScale * planetScaleFactor, planetScale * planetScaleFactor);
    glColor3f(0.0f, 1.0f, 1.0f);  // Cyan color
    glutSolidSphere(15.0f, 30, 40);  
    glPopMatrix();
}

void drawStarBackground() {
    glBindBuffer(GL_ARRAY_BUFFER, starsVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glPointSize(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_POINTS, 0, numStars);

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateStars() {
    for (int i = 0; i < numStars; ++i) {
        stars[i][0] -= 0.05f;

        if (stars[i][0] < -100.0f) {
            stars[i][0] = 100.0f;
            stars[i][1] = static_cast<float>(std::rand() % 200 - 100);
            stars[i][2] = static_cast<float>(std::rand() % 200 - 100);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, starsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(stars), stars);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void moveSpaceGuy(int value) {
    static bool moveRight = true;
    const float moveSpeed = 0.25f;

    if (moveRight) {
        object1TranslateX += moveSpeed;
        if (object1TranslateX > 1.5f) {
            moveRight = false;
        }
    } else {
        object1TranslateX -= moveSpeed;
        if (object1TranslateX < -1.5f) {
            moveRight = true;
        }
    }

    glutTimerFunc(16, moveSpaceGuy, 0);
    glutPostRedisplay();
}

void moveSpaceGuyUpDown(int value) {
    static bool moveUp = true;
    const float moveSpeed = 0.01f;

    if (moveUp) {
        object2TranslateY += moveSpeed;
        if (object2TranslateY > 1.0f) {
            moveUp = false;
        }
    } else {
        object2TranslateY -= moveSpeed;
        if (object2TranslateY < -1.0f) {
            moveUp = true;
        }
    }

    glutTimerFunc(16, moveSpaceGuyUpDown, 0);
    glutPostRedisplay();
}

void drawSpaceGuy() {
    glPushMatrix();
    glTranslatef(object1TranslateX - 4.5f, object2TranslateY, 8.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.45f, -1.5f, 0.0f);
    glScalef(0.75f, 2.0f, 1.0f);
    glutSolidCube(1.0f);
    glTranslatef(1.10f, 0.0f, 0.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-1.5f, 0.5f, 0.0f);
    glRotatef(armAngle, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 2.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.5f, 0.5f, 0.0f);
    glRotatef(-armAngle, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, -2.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    glutSolidSphere(1.0f, 20, 20);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glutSolidCube(2.0f);
    glPopMatrix();

    glPopMatrix();
}

void drawEye() {
    glPushMatrix();
    glTranslatef(50.0f, 0.0f, -500.0f); 

    glRotatef(eyeRotation, 0.0f, 1.0f, 0.0f);

    glScalef(object3Scale * 10.0f, object3Scale * 10.0f, object3Scale * 10.0f);

    // eyeball (white)
    glColor3ub(255, 255, 255);
    glutSolidSphere(5.0f, 30, 40);

    // iris (green)
    glColor3ub(0, 255, 0);
    glTranslatef(0.0f, 0.0f, 2.5f);
    glutSolidSphere(3.0f, 30, 40);
    
    // pupil (black)
    glColor3ub(0, 0, 0);
    glTranslatef(0.0f, 0.0f, 3.0f);
    glutSolidSphere(1.0f, 30, 40);

    glPopMatrix();
}

void rotateEye() {
    // Implement continuous rotation
    eyeRotation += 0.15f;
    glutPostRedisplay();
}

void passiveMotion(int x, int y) {
    cameraPositionX = (x - glutGet(GLUT_WINDOW_WIDTH) / 2.0f) / 50.0f;
    cameraPositionY = (glutGet(GLUT_WINDOW_HEIGHT) / 2.0f - y) / 50.0f;
}

void keyboard(unsigned char key, int x, int y) {
    const float moveSpeed = 0.1f;
    const float rotationSpeed = 5.0f;

    switch (key) {
        case 'a':
            object2TranslateY += moveSpeed;
            armAngle += rotationSpeed;
            break;
        case 'd':
            object2TranslateY -= moveSpeed;
            armAngle -= rotationSpeed;
            break;
        case 27:
            exit(0);
            break;
        case 'F':
        case 'f':
            isFullScreen = !isFullScreen;
            if (isFullScreen) {
                glutFullScreen();
            } else {
                glutReshapeWindow(800, 600);
                glutPositionWindow(100, 100);
            }
            break;
    }

    glutPostRedisplay();
}

bool isMouseButtonDown = false;
bool isMouseDragging = false;
int prevMouseX = 0;

void mouseClick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        isMouseButtonDown = true;
        prevMouseX = x;

        if (button == GLUT_LEFT_BUTTON) {
            eyeRotation += (x - prevMouseX) * 0.2f;
            glutIdleFunc(rotateEye);  // Start continuous rotation
        } else if (button == GLUT_RIGHT_BUTTON) {
            eyeRotation += (x + prevMouseX) * 0.2f;
            glutIdleFunc(rotateEye);
        }
    } else if (state == GLUT_UP) {
        isMouseButtonDown = false;
        glutIdleFunc(NULL);  // Stop continuous rotation when the button is released
    }

    // Handle mouse wheel here
    int deltaY = y - lastMouseY;

    if (deltaY > 0) {
        // Scroll up
        planetScaleFactor += 0.1f;
    } else if (deltaY < 0) {
        // Scroll down
        planetScaleFactor -= 0.1f;
    }

    // Ensure that the scaling factor is within a reasonable range
    planetScaleFactor = std::max(0.1f, std::min(planetScaleFactor, 10.0f));

    lastMouseY = y;

    glutPostRedisplay();
}


void mouseDrag(int x, int y) {
    if (isMouseButtonDown) {
        int deltaX = x - prevMouseX;
        

        prevMouseX = x;
        glutPostRedisplay();
    }
}

void idle() {
    glutPostRedisplay();
}

void registerMouseEvents() {
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseDrag);
    glutPassiveMotionFunc(passiveMotion);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(cameraPositionX, cameraPositionY, cameraPositionZ, 0, 0, 0, 0, 1, 0);

    drawStarBackground();
    drawSpaceGuy();
    
    // Update the scaling factors (planetScaleFactor is controlled by the mouse wheel)
    if (currentScaleValue < targetScaleValue) {
        currentScaleValue += scaleSpeed;
    } else if (currentScaleValue > targetScaleValue) {
        currentScaleValue -= scaleSpeed;
    }

    // Ensure that the scaling factor is within a reasonable range
    currentScaleValue = std::max(0.1f, std::min(currentScaleValue, 2.0f));

    // Apply the scaling factor from the currentScaleValue variable
    planetScale = currentScaleValue;
    
    drawPlanet();

    drawEye();

    updateStars();

    glutSwapBuffers();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("[LONG QUIZ #2] and Project");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed\n";
        return -1; 
    }

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    registerMouseEvents();

    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutTimerFunc(0, moveSpaceGuy, 0);
    glutTimerFunc(0, moveSpaceGuyUpDown, 0);

    glutMainLoop();

    return 0;
}
