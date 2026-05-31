#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#undef DrawText

const int MAX_X = 100;
const int MAX_Y = 100;

struct Point {
    float x, y;
};

float playerX = 0.0f;

std::vector<Point> bullets;
std::vector<Point> enemies;

int score = 0;
int lives = 5;

bool gameOver = false;
bool isPaused = false;

int playTimeSeconds = 0;
int timeAccumulator = 0;

void Draw();
void Set_Transformations();
void Initialize(int argc, char *argv[]);
void Timer(int extra);
void SpecialKeyboard(int key, int x, int y);
void NormalKeyboard(unsigned char key, int x, int y);
void Draw_Object(float x, float y, float r, float g, float b, float sizeW, float sizeH);
void Draw_Spaceship(float x, float y);
void SpawnEnemy();
void DrawText(std::string text, float x, float y, float r, float g, float b);

int main(int argc, char *argv[]) {
    Initialize(argc, argv);
    return 0;
}

void Initialize(int argc, char *argv[]) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowPosition(100, 100);

    glutInitWindowSize(600, 600);

    glutCreateWindow("Space Invaders OpenGL");

    srand(time(0));

    Set_Transformations();

    glutDisplayFunc(Draw);

    glutSpecialFunc(SpecialKeyboard);

    glutKeyboardFunc(NormalKeyboard);

    glutTimerFunc(30, Timer, 0);

    glutMainLoop();
}

void SpawnEnemy() {
    float randomX = (rand() % 18) * 10 - 90;

    enemies.push_back({randomX, 100.0f});
}

void Timer(int extra) {

    if (!gameOver && !isPaused) {

        timeAccumulator += 30;

        if (timeAccumulator >= 1000) {

            playTimeSeconds++;

            timeAccumulator = 0;

            if (rand() % 2 == 0)
                SpawnEnemy();
        }

        for (size_t i = 0; i < bullets.size();) {

            bullets[i].y += 7.0f;

            if (bullets[i].y > 100)
                bullets.erase(bullets.begin() + i);

            else
                i++;
        }

        for (size_t i = 0; i < enemies.size();) {

            enemies[i].y -= 2.0f;

            if (enemies[i].y < -100 ||
                (abs(enemies[i].x - playerX) < 12 && enemies[i].y < -80)) {

                lives--;

                enemies.erase(enemies.begin() + i);

                if (lives <= 0)
                    gameOver = true;
            }

            else
                i++;
        }

        for (size_t i = 0; i < bullets.size(); i++) {

            for (size_t j = 0; j < enemies.size(); j++) {

                if (abs(bullets[i].x - enemies[j].x) < 10 &&
                    abs(bullets[i].y - enemies[j].y) < 10) {

                    enemies.erase(enemies.begin() + j);

                    bullets.erase(bullets.begin() + i);

                    score += 100;

                    break;
                }
            }
        }
    }

    glutPostRedisplay();

    glutTimerFunc(30, Timer, 0);
}

void SpecialKeyboard(int key, int x, int y) {

    if (isPaused || gameOver)
        return;

    if (key == GLUT_KEY_LEFT && playerX > -90)
        playerX -= 10;

    if (key == GLUT_KEY_RIGHT && playerX < 90)
        playerX += 10;
}

void NormalKeyboard(unsigned char key, int x, int y) {

    if (key == ' ' && !gameOver && !isPaused) {

        bullets.push_back({playerX, -80.0f});
    }

    if (key == 'p' || key == 'P')
        isPaused = !isPaused;
}

void Draw_Spaceship(float x, float y) {

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_TRIANGLES);

    glVertex2f(x, y + 15);

    glVertex2f(x - 8, y);

    glVertex2f(x + 8, y);

    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);

    glVertex2f(x - 5, y);

    glVertex2f(x + 5, y);

    glVertex2f(x + 3, y - 5);

    glVertex2f(x - 3, y - 5);

    glEnd();
}

void Draw_Object(float x, float y,
                 float r, float g, float b,
                 float sizeW, float sizeH) {

    glColor3f(r, g, b);

    glBegin(GL_QUADS);

    glVertex2f(x - sizeW, y - sizeH);

    glVertex2f(x + sizeW, y - sizeH);

    glVertex2f(x + sizeW, y + sizeH);

    glVertex2f(x - sizeW, y + sizeH);

    glEnd();
}

void DrawText(std::string text,
              float x, float y,
              float r, float g, float b) {

    glColor3f(r, g, b);

    glRasterPos2f(x, y);

    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void Set_Transformations() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(-100, 100, -100, 100);

    glMatrixMode(GL_MODELVIEW);
}

void Draw() {

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    Draw_Spaceship(playerX, -90.0f);

    for (auto& b : bullets)
        Draw_Object(b.x, b.y,
                    1.0f, 1.0f, 0.0f,
                    1.5f, 4.0f);

    for (auto& e : enemies)
        Draw_Object(e.x, e.y,
                    0.7f, 0.2f, 0.5f,
                    7.0f, 7.0f);

    DrawText("Score: " + std::to_string(score),
             -95, 90,
             1, 1, 1);

    DrawText("Lives: " + std::to_string(lives),
             60, 90,
             1, 1, 1);

    DrawText("Time: " + std::to_string(playTimeSeconds) + "s",
             -15, 90,
             1, 1, 1);

    if (gameOver)
        DrawText("GAME OVER!",
                 -20, 0,
                 1, 0, 0);

    if (isPaused)
        DrawText("PAUSED",
                 -10, 0,
                 1, 1, 0);

    if (score >= 1000) {

        DrawText("YOU WIN! CONGRATULATIONS!",
                 -35.0f, 20.0f,
                 0, 1, 0);

        isPaused = true;
    }

    glutSwapBuffers();
}
