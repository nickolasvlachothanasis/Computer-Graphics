
#include <stdio.h>
#include <GL/gl.h>		   // Open Graphics Library (OpenGL) header
#include <GL/glut.h>	   // The GL Utility Toolkit (GLUT) Header

#include <GL/gl.h>		   // Open Graphics Library (OpenGL) header
#include <GL/glut.h>	   // The GL Utility Toolkit (GLUT) Header
#include <iostream>

//#include <GL/gl.h>


#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define KEY_ESCAPE 27

#define ROCK 0
#define PAPER 1
#define SCISSORS 2
#define BLUE 3
#define RED 4
#define MPEZ 5
#define EMPTY 6

#define N 15

using namespace std;

typedef struct {
    int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;

bool game = false;
GLuint textureRock, texturePaper, textureScissors;

void loadTextureFromFile(const char *filename)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//glShadeModel(GL_FLAT);
	//glEnable(GL_DEPTH_TEST);

	unsigned int texture;
        if(strcmp(filename, "rock.bmp") == 0){
            glGenTextures(1, &textureRock);
            glBindTexture(GL_TEXTURE_2D, textureRock);
        }
        else if(strcmp(filename, "paper.bmp") == 0){
            glGenTextures(1, &texturePaper);
            glBindTexture(GL_TEXTURE_2D, texturePaper);
        }
        else{
            glGenTextures(1, &textureScissors);
            glBindTexture(GL_TEXTURE_2D, textureScissors);
        }
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void FreeTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

class Info{
private:
    int moves, score;
    bool isGameOn;
public:
    Info(){
        moves = 30;
        score = 0;
        isGameOn = false;
    }
    void resetGame(){
        moves = 30;
        score = 0;
        isGameOn = true;
    }
    void makeMove(){
        if(isGameOn)
            moves--;
    }

    bool gameOver(){
        if(moves == 0)
            return true;
        return false;
    }
    void incrementScore(int score){
        this->score += score;
    }
    void changeGameStatus(){
        isGameOn = !isGameOn;
    }
    void displayScore(){
        char str[30];
        char *c;
        if(moves > 20)
            glColor3f(0, 1, 0);
        else if(moves > 10)
            glColor3f(1, 1, 0);
        else
            glColor3f(1, 0, 0);
        if(moves == 0){
            sprintf(str, "Game Over Final Score: %d", score);
        }
        else{
            sprintf(str, "Moves: %d Score: %d", moves, score);
        }
	glRasterPos3f(22, 35.5,0);

	for (c=str; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
    }
};

Info *info = new Info();

class Camera{
private:
    float x, z, y;
    float lx, lz;
    float deltaAngle;
    float deltaMove;
    int xOrigin;
    float angle;
    bool isCameraRotating;
public:
    Camera(float x, float z){
        this->x = x;
        this->z = z;
        this->lx = 0.0;
        this->lz = -1.0;
        this->y = 18;
        this->xOrigin = -1;
        this->angle = 0.0;
        this->deltaAngle = 0.0f;
        this->deltaMove = 0.0f;
        this->isCameraRotating = false;
    }

    float getX(){ return x;}
    float getY(){ return y;}
    float getZ(){ return z;}
    float getLX(){ return lx;}
    float getLZ(){ return lz;}

    void changeCameraStatus(bool isCameraRotating){
        this->isCameraRotating = isCameraRotating;
    }
    bool getCameraRotating(){
        return isCameraRotating;
    }
    void resetCamera(){

        this->lx = 0.0;
        this->lz = -1.0;
        this->y = 18;
        this->xOrigin = -1;
        this->angle = 0.0;
        this->deltaAngle = 0.0f;
        this->deltaMove = 0.0f;
        this->isCameraRotating = false;
        this->x = 18;
        this->z = 50;

    }
    void computePos(float deltaMove) {

	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
     }

     void update(int x){
         if (xOrigin >= 0) {
		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
                //glutPostRedisplay();
	}
    }

    void updateAngle(){
        angle += deltaAngle;
    }

    void setXOrigin(int x){
                     printf("xOrigin = %d\n", xOrigin);

        xOrigin = x;
    }

    void computePos(){
        if (deltaMove)
		computePos(deltaMove);
    }

    void setX(float x){
        this->x = x;
    }
    void setY(float y){
        this->y = y;
    }
    void setZ(float z){
        this->z = z;
    }

};

Camera *camera = new Camera(18, 50);

class Cube{
private:
    double x, y, z;
    int type;

public:
    Cube(double x, double y, double z, int type){
        this->x = x;
        this->y = y;
        this->z = z;
        this->type = type;
    }

    void setType(int type){
        this->type = type;
    }

    int getType(){
        return type;
    }
    bool areNeighbour(Cube *other){
        if(this->x == other->x && abs(this->y - other->y) == 1)
            return true;
        if(this->y == other->y && abs(this->x - other->x) == 1)
            return true;
        return false;

    }
    bool equals(Cube *other){
        if(this->type != other->type)
            return false;
        return true;
    }

    bool canEat(Cube *other){
        if(this->type > SCISSORS)
            return false;
        if(this->type - other->type == 1 || (this->type == 0 && other->type == 2))
            return true;
        return false;
    }
    void display(){


        glPushMatrix();

        glTranslatef(2.4*x, 2.4*y, z);

        if(type == BLUE){
            glColor3f(0,0,1);
            DrawCube(false);
        }
        else if(type == RED){
            glColor3f(1,0,0);
            DrawCube(false);

        }
        else if(type == MPEZ){
            glColor3f(0.96, 0.96, 0.86);
            DrawCube(false);
        }
        else if(type == ROCK){
            //loadTextureFromFile("rock.bmp");

            glBindTexture(GL_TEXTURE_2D, textureRock);
            DrawCube(true);
        }
        else if(type == PAPER){

            glBindTexture(GL_TEXTURE_2D, texturePaper);
//            loadTextureFromFile("paper.bmp");
            DrawCube(true);
        }
        else if(type == SCISSORS){
            glBindTexture(GL_TEXTURE_2D, textureScissors);
            //loadTextureFromFile("scissors.bmp");
            DrawCube(true);
        }
	glPopMatrix();
    }


    void DrawCube(bool flag)
    {
            //glMatrixMode(GL_MODELVIEW);

            if(flag == true){
                glColor3f(1.0f, 1.0f, 1.0f);

                glEnable(GL_TEXTURE_2D);
            }

                    //gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            glBegin(GL_QUADS);        // Draw The Cube Using quads
            //glColor3f(0.0f, 1.0f, 0.0f);    // Color Blue
            glTexCoord2d(1.0, 1.0);glVertex3f(1.0f, 1.0f, -1.0f);    // Top Right Of The Quad (Top)
            glTexCoord2d(0.0, 1.0);glVertex3f(-1.0f, 1.0f, -1.0f);    // Top Left Of The Quad (Top)
            glTexCoord2d(0.0, 0.0);glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
            glTexCoord2d(1.0, 0.0);glVertex3f(1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)
            //glEnd(GL_QUADS);

            //glColor3f(1.0f, 0.5f, 0.0f);    // Color Orange
            glTexCoord2d(1.0, 1.0);glVertex3f(1.0f, -1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
            glTexCoord2d(0.0, 1.0);glVertex3f(-1.0f, -1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
            glTexCoord2d(0.0, 0.0);glVertex3f(-1.0f, -1.0f, -1.0f);    // Bottom Left Of The Quad (Bottom)
            glTexCoord2d(1.0, 0.0);glVertex3f(1.0f, -1.0f, -1.0f);    // Bottom Right Of The Quad (Bottom)
            //glColor3f(1.0f, 0.0f, 0.0f);    // Color Red
            glTexCoord2d(1.0, 1.0);glVertex3f(1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
            glTexCoord2d(0.0, 1.0);glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
            glTexCoord2d(0.0, 0.0);glVertex3f(-1.0f, -1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
            glTexCoord2d(1.0, 0.0);glVertex3f(1.0f, -1.0f, 1.0f);    // Bottom Right Of The Quad (Front)
            //glColor3f(1.0f, 1.0f, 0.0f);    // Color Yellow
            glTexCoord2d(1.0, 1.0);glVertex3f(1.0f, -1.0f, -1.0f);    // Top Right Of The Quad (Back)
            glTexCoord2d(0.0, 1.0);glVertex3f(-1.0f, -1.0f, -1.0f);    // Top Left Of The Quad (Back)
            glTexCoord2d(0.0, 0.0);glVertex3f(-1.0f, 1.0f, -1.0f);    // Bottom Left Of The Quad (Back)
            glTexCoord2d(1.0, 0.0);glVertex3f(1.0f, 1.0f, -1.0f);    // Bottom Right Of The Quad (Back)
            //glColor3f(0.0f, 0.0f, 1.0f);    // Color Blue
            glTexCoord2d(1.0, 1.0);glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
            glTexCoord2d(0.0, 1.0);glVertex3f(-1.0f, 1.0f, -1.0f);    // Top Left Of The Quad (Left)
            glTexCoord2d(0.0, 0.0);glVertex3f(-1.0f, -1.0f, -1.0f);    // Bottom Left Of The Quad (Left)
            glTexCoord2d(1.0, 0.0);glVertex3f(-1.0f, -1.0f, 1.0f);    // Bottom Right Of The Quad (Left)
            //glColor3f(1.0f, 0.0f, 1.0f);    // Color Violet
            glTexCoord2d(1.0, 1.0);glVertex3f(1.0f, 1.0f, -1.0f);    // Top Right Of The Quad (Right)
            glTexCoord2d(0.0, 1.0);glVertex3f(1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
            glTexCoord2d(0.0, 0.0);glVertex3f(1.0f, -1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
            glTexCoord2d(1.0, 0.0);glVertex3f(1.0f, -1.0f, -1.0f);    // Bottom Right Of The Quad (Right)*/
            glEnd();            // End Drawing The Cube
            glFlush();
            if(flag == true)
                glDisable(GL_TEXTURE_2D);

    }
};

class Grid{
private:
    Cube *cubes[N][N];
    int selectedx1, selectedy1;
    int selectedx2, selectedy2;
    bool hasSelectedCube;
public:
    Grid(){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                //int r = rand()%6;
                cubes[i][j] = new Cube(i, j, 0, MPEZ);
            }
        }
        hasSelectedCube = false;
    }

    void selectCube(int x, int y){
        if(hasSelectedCube == false){

            selectedx1 = x;
            selectedy1 = y;
        }
        else{

            selectedx2 = x;
            selectedy2 = y;
            swapCubes();
        }
        hasSelectedCube = !hasSelectedCube;

        //cubes[x][y]->setType(RED);
    }


    void eatNeighbours(int x, int y){
        if(x - 1 >= 0 && cubes[x][y]->canEat(cubes[x-1][y])==true){
            cubes[x-1][y]->setType(EMPTY);
        }
        if(x + 1 < N && cubes[x][y]->canEat(cubes[x+1][y])==true){
            cubes[x+1][y]->setType(EMPTY);
        }

        if(y - 1 >= 0 && cubes[x][y]->canEat(cubes[x][y-1])==true){
            cubes[x][y-1]->setType(EMPTY);
        }
        if(y + 1 < N && cubes[x][y]->canEat(cubes[x][y+1])==true){
            cubes[x][y+1]->setType(EMPTY);
        }
    }
    void swapCubes(){
        if( cubes[selectedx1][selectedy1]->areNeighbour(cubes[selectedx2][selectedy2])){
            int temp = cubes[selectedx1][selectedy1]->getType();
            cubes[selectedx1][selectedy1]->setType(cubes[selectedx2][selectedy2]->getType());
            cubes[selectedx2][selectedy2]->setType(temp);
            info->makeMove();

            if(cubes[selectedx1][selectedy1]->getType() == EMPTY && cubes[selectedx2][selectedy2]->getType() != EMPTY)
                eatNeighbours(selectedx2, selectedy2);
            else if(cubes[selectedx1][selectedy1]->getType() != EMPTY && cubes[selectedx2][selectedy2]->getType() == EMPTY)
                eatNeighbours(selectedx1, selectedy1);

            explosion();
        }
    }
    void display(){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){

                cubes[i][j]->display();
            }
        }
    }

    void initRandom(){
        bool flag = false;
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                int r = rand()%5;
                cubes[i][j] = new Cube(i, j, 0, r);
            }
        }

        do{
            flag = false;



            for(int i = 0; i < N; i++){
                for(int j = 0; j < N -2; j++){
                    int k = finalSamePositionCols(i, j);
                    if(k - j + 1 >= 3){
                        int r = rand()%5;
                        cubes[i][j] = new Cube(i, j, 0, r);
                        flag = true;
                    }
                }
            }
            if(flag == false){
                for(int i = 0; i < N-2; i++){
                    for(int j = 0; j < N; j++){
                        int k = finalSamePositionRows(i, j);
                        if(k - i + 1 >= 3){
                            int r = rand()%5;
                            cubes[i][j] = new Cube(i, j, 0, r);
                            flag = true;
                        }
                    }
                }
            }
        }while(flag == true);

        //explosion();
        glutPostRedisplay();
    }
//giati einai anapoda
    //ekrh3h stis sthles tou paixnidiou(eswterika grammes)
    int finalSamePositionCols(int i, int j){
        for(int k = j+1; k < N; k++){
            if(cubes[i][j]->equals(cubes[i][k]) == false)
                return k -1;
        }
        return N-1;
    }
    void destroyCombo(int i, int j, int k){
        for(int l = j; l <= k; l++){
            cubes[i][l]->setType(EMPTY);
        }
        info->incrementScore(10);

    }
    void destroyDist1Cols(int i, int j, int k){
        int m, n;
        if(cubes[i][j]->getType() == BLUE || cubes[i][j]->getType() == RED|| cubes[i][j]->getType() == EMPTY)
            return;
        if(j-1>=0)
            m = j-1;
        else
            m = j;
        if(k+1<N)
            n = k+1;
        else
            n = k;


        for(int l = m; l <= n; l++){
            if(i-1>=0 && cubes[i-1][l]->canEat(cubes[i][j]) == false){
                cubes[i-1][l]->setType(EMPTY);
                info->incrementScore(2);
            }
            else{
                info->incrementScore(-1);
            }


            if(i+1<N && cubes[i+1][l]->canEat(cubes[i][j]) == false){
                cubes[i+1][l]->setType(EMPTY);
                info->incrementScore(2);
            }
            else{
                info->incrementScore(-1);
            }

        }

        if(j-1>=0 && cubes[i][j-1]->canEat(cubes[i][j]) == false){
            cubes[i][j-1]->setType(EMPTY);
            info->incrementScore(2);
        }
        else{
            info->incrementScore(-1);
        }

        if(k+1<N && cubes[i][k+1]->canEat(cubes[i][j]) == false){
                cubes[i][k+1]->setType(EMPTY);
            info->incrementScore(2);
        }
        else{
            info->incrementScore(-1);
        }
    }

    void destroyDist2Cols(int i, int j, int k){
        int m, n;
        if(cubes[i][j]->getType() == BLUE || cubes[i][j]->getType() == RED|| cubes[i][j]->getType() == EMPTY)
            return;
        if(j-2>=0)
            m = j-2;
        else if(j-1>=0)
            m = j-1;
        else
            m = j;
        if(k+2<N)
            n = k+2;
        if(k+1<N)
            n = k+1;
        else
            n = k;


        for(int l = m; l <= n; l++){
            if(i-2>=0 && cubes[i][j]->canEat(cubes[i-2][l]) == true){
                cubes[i-2][l]->setType(EMPTY);
                info->incrementScore(3);
            }
            if(i+2<N && cubes[i][j]->canEat(cubes[i+2][l]) == true){
                cubes[i+2][l]->setType(EMPTY);
                info->incrementScore(3);
            }

        }

        if(j-2>=0){
            for(int l = i - 2; l <= i + 2; l++){
                if(l>=0 && l<N && cubes[i][j]->canEat(cubes[l][j-2]) == true){
                    cubes[l][j-2]->setType(EMPTY);
                    info->incrementScore(3);
                }
            }
        }

        if(k+2 < N){
            for(int l = i - 2; l <= i + 2; l++){
                if(l>=0 && l<N && cubes[i][j]->canEat(cubes[l][k+2]) == true){
                    cubes[l][k+2]->setType(EMPTY);
                    info->incrementScore(3);
                }
            }
        }
    }

    void destroyDist3Cols(int i, int j, int k){
        int m, n;
        if(cubes[i][j]->getType() == BLUE || cubes[i][j]->getType() == RED|| cubes[i][j]->getType() == EMPTY)
            return;
        if(j-3>=0)
            m = j -3;
        else if(j-2>=0)
            m = j-2;
        else if(j-1>=0)
            m = j-1;
        else
            m = j;
        if(k+3<N)
            n = k+3;
        else if(k+2<N)
            n = k+2;
        if(k+1<N)
            n = k+1;
        else
            n = k;


        for(int l = m; l <= n; l++){
            if(i-3>=0 && cubes[i][j]->canEat(cubes[i-3][l]) == true){
                cubes[i-3][l]->setType(EMPTY);
                info->incrementScore(3);
            }
            if(i+3<N && cubes[i][j]->canEat(cubes[i+3][l]) == true){
                cubes[i+3][l]->setType(EMPTY);
                info->incrementScore(3);
            }

        }

        if(j-3>=0){
            for(int l = i - 3; l <= i + 3; l++){
                if(l>=0 && l<N && cubes[i][j]->canEat(cubes[l][j-3]) == true){
                    cubes[l][j-3]->setType(EMPTY);
                    info->incrementScore(3);
                }
            }
        }

        if(k+3 < N){
            for(int l = i - 3; l <= i + 3; l++){
                if(l>=0 && l<N && cubes[i][j]->canEat(cubes[l][k+3]) == true){
                    cubes[l][k+3]->setType(EMPTY);
                    info->incrementScore(3);
                }
            }
        }
    }

//ekrh3h stis grammes tou paixnidiou(eswterika sthles)
    int finalSamePositionRows(int i, int j){
        for(int k = i+1; k < N; k++){
            if(cubes[i][j]->equals(cubes[k][j]) == false)
                return k -1;
        }
        return N-1;
    }
    void destroyComboRows(int j, int i, int k){
        for(int l = i; l <= k; l++){
            cubes[l][j]->setType(EMPTY);
        }

        info->incrementScore(10);
    }
    void destroyDist1Rows(int j, int i, int k){
        int m, n;
        if(cubes[i][j]->getType() == BLUE || cubes[i][j]->getType() == RED|| cubes[i][j]->getType() == EMPTY)
            return;
        if(i-1>=0)
            m = i-1;
        else
            m = i;
        if(k+1<N)
            n = k+1;
        else
            n = k;


        for(int l = m; l <= n; l++){
            if(j-1>=0 && cubes[l][j-1]->canEat(cubes[i][j]) == false){
                cubes[l][j-1]->setType(EMPTY);
                info->incrementScore(2);
            }
            else{
                info->incrementScore(-1);
            }


            if(j+1<N && cubes[l][j+1]->canEat(cubes[i][j]) == false){
                cubes[l][j+1]->setType(EMPTY);
                info->incrementScore(2);
            }
            else{
                info->incrementScore(-1);
            }
        }

        if(i-1>=0 && cubes[i-1][j]->canEat(cubes[i][j]) == false){
                cubes[i-1][j]->setType(EMPTY);
                info->incrementScore(2);
        }
        else{
            info->incrementScore(-1);
        }

        if(k+1<N && cubes[k+1][j]->canEat(cubes[i][j]) == false){
            cubes[k+1][j]->setType(EMPTY);
            info->incrementScore(2);
        }
        else{
            info->incrementScore(-1);
        }
    }

    void destroyDist2Rows(int j, int i, int k){
        printf("i %d k %d\n", i, k);
        int m, n;
        if(cubes[i][j]->getType() == BLUE || cubes[i][j]->getType() == RED|| cubes[i][j]->getType() == EMPTY)
            return;
        if(i-2>=0)
            m = i-2;
        else if(i-1>=0)
            m = i-1;
        else
            m = i;
        if(k+2<N)
            n = k+2;
        else if(k+1<N)
            n = k+1;
        else
            n = k;


        for(int l = m; l <= n; l++){
            if(j-2>=0 && cubes[i][j]->canEat(cubes[l][j-2]) == true){
                cubes[l][j-2]->setType(EMPTY);
                info->incrementScore(3);
            }


            if(j+2<N && cubes[i][j]->canEat(cubes[l][j+2]) == true){
                cubes[l][j+2]->setType(EMPTY);
                info->incrementScore(3);
            }

        }
        for(int l = j - 2; l <= j+2; l++){

            if(i-2>=0 && cubes[i][j]->canEat(cubes[i-2][l]) == true){
                cubes[i-2][l]->setType(EMPTY);
                info->incrementScore(3);
            }


            if(k+2<N && cubes[i][j]->canEat(cubes[k+2][l]) == true){
                cubes[k+2][l]->setType(EMPTY);
                info->incrementScore(3);
            }

        }
    }
    void destroyDist3Rows(int j, int i, int k){
        printf("i %d k %d\n", i, k);
        int m, n;
        if(cubes[i][j]->getType() == BLUE || cubes[i][j]->getType() == RED|| cubes[i][j]->getType() == EMPTY)
            return;
        if(i-3>=0)
            m = i-3;

        else if(i-2>=0)
            m = i-2;
        else if(i-1>=0)
            m = i-1;
        else
            m = i;
        if(k+3<N)
            n = k+3;
        else if(k+2<N)
            n = k+2;
        else if(k+1<N)
            n = k+1;
        else
            n = k;


        for(int l = m; l <= n; l++){
            if(j-3>=0 && cubes[i][j]->canEat(cubes[l][j-3]) == true){
                cubes[l][j-3]->setType(EMPTY);
                info->incrementScore(3);
            }


            if(j+3<N && cubes[i][j]->canEat(cubes[l][j+3]) == true){
                cubes[l][j+3]->setType(EMPTY);

                info->incrementScore(3);
            }
        }
        for(int l = j - 3; l <= j+3; l++){

            if(i-3>=0 && cubes[i][j]->canEat(cubes[i-3][l]) == true){
                    cubes[i-3][l]->setType(EMPTY);
                    info->incrementScore(3);
            }

            if(k+3<N && cubes[i][l]->canEat(cubes[k+3][l]) == true){
                cubes[k+3][l]->setType(EMPTY);
                info->incrementScore(3);
            }
        }
    }

    void explosion(){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N -2; j++){
                int k = finalSamePositionCols(i, j);
                if(k - j + 1 >= 3){
                   // if()
                    destroyDist1Cols(i, j, k);
                    destroyDist2Cols(i, j, k);
                    destroyDist3Cols(i, j, k);
                    destroyCombo(i, j, k);
                }
            }
        }
        for(int i = 0; i < N-2; i++){
            for(int j = 0; j < N; j++){
                int k = finalSamePositionRows(i, j);
                if(k - i + 1 >= 3){
                   // if()
                    destroyDist1Rows(j, i, k);
                    destroyDist2Rows(j, i, k);
                    destroyDist3Rows(j, i, k);
                    destroyComboRows(j, i, k);
                }
            }
        }
    }
};

Cube *c = new Cube(0,0,0,RED);
Cube *c1 = new Cube(0,1,0,MPEZ);

Grid *grid = new Grid();

void menu(int option){
    if(option == 0){
        exit(0);
    }
    if(option == 1){
        if(camera->getCameraRotating() == false){
            grid->initRandom();
            info->changeGameStatus();
            info->resetGame();
        }
    }
    if(option == 2){
        camera->resetCamera();
    }
    if(option == 3){
        camera->changeCameraStatus(true);
    }
    glutPostRedisplay();
}

void display() 
{


// Clear Screen and Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		     
	glLoadIdentity();
 
        camera->computePos();

        float x = camera->getX();
        float z = camera->getZ();
        float lx = camera->getLX();
        float lz = camera->getLZ();
        float y = camera->getY();

	// Define a viewing transformation
	gluLookAt(x, y, z, x+lx, y, z+lz, 0,1,0);
 

        info->displayScore();


	// Push and pop the current matrix stack. 
	// This causes that translations and rotations on this matrix wont influence others.
 
        grid->display();
 
	glutSwapBuffers();
}


void initialize () 
{
   // select projection matrix
    glMatrixMode(GL_PROJECTION);												
 
	// set the viewport
    glViewport(0, 0, win.width, win.height);									
 
	// set matrix mode
    glMatrixMode(GL_PROJECTION);												
 
	// reset projection matrix
    glLoadIdentity();															
    GLfloat aspect = (GLfloat) win.width / win.height;
 
	// set up a perspective projection matrix
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		
 
	// specify which matrix is the current matrix
	glMatrixMode(GL_MODELVIEW);													
    glShadeModel( GL_SMOOTH );
 
	// specify the clear value for the depth buffer
	glClearDepth( 1.0f );														
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
 
	// specify implementation-specific hints
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );						
 
	GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
    GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glEnable( GL_LIGHT0 );
    glEnable( GL_COLOR_MATERIAL );
    glShadeModel( GL_SMOOTH );
    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); 
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

// -----------------------------------
//             MOUSE
// -----------------------------------

void mouseMove(int x, int y) {

	// this will only be true when the left button is down
        if(camera->getCameraRotating())
            camera->update(x);
}

void mouseButton(int button, int state, int x, int y) {
    printf("%d %d\n", x, y);
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
                    if(camera->getCameraRotating() == false){

                        if(info->gameOver() == false && x >= 20 && x <= 560 && y >= 55 && y <= 580){
                            if(y <= 277){

                                int sizex = 540/15;
                                int sizey = (277 - 35)/7;

                                int cubex = (x - 20)/sizex;
                                int cubey = (600 - y - 35)/sizey;
                                printf("%d %d\n", cubex, cubey);
                                grid->selectCube(cubex, cubey);
                            }
                            else if(y >=282){

                                int sizex = 540/15;
                                int sizey = (560 - 282)/8;

                                int cubex = (x - 20)/sizex;
                                int cubey = (600 - y - 35)/sizey;
                                printf("%d %d\n", cubex, cubey);
                                grid->selectCube(cubex, cubey);
                            }
                        }
                    }
                    else{
                    //info->makeMove();
                        camera->updateAngle();
                        camera->setXOrigin(-1);
                    }
		}
		else {// state = GLUT_DOWN
			camera->setXOrigin(x);
		}
	}
}


void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )		
{ 
  switch ( key ) 
  {
    case KEY_ESCAPE:        
      exit ( 0 );   
      break;      

    default:      
      break;
  }
}

void special_keys(int keys, int x1, int y1) {
	switch(keys) {

		case GLUT_KEY_UP:       // F1 toggles fullscreen mode
			printf("Hello\n");

                        camera->setZ(camera->getZ() - 0.5);
                        camera->changeCameraStatus(true);
			break;
                case GLUT_KEY_DOWN:       // F1 toggles fullscreen mode
			printf("Hello\n");
                        camera->changeCameraStatus(true);

                        camera->setZ(camera->getZ() + 0.5);
                      //  z++;
			break;
                case GLUT_KEY_RIGHT:       // F1 toggles fullscreen mode
			printf("Hello2\n");
                        camera->changeCameraStatus(true);

                        camera->setX(camera->getX() + 0.5);
			break;
                case GLUT_KEY_LEFT:       // F1 toggles fullscreen mode
			printf("Hello2\n");
                        camera->changeCameraStatus(true);
                        camera->setX(camera->getX() - 0.5);
                     //   x--;
			break;
		default:
			break;
	}
	glutPostRedisplay();
}


int main(int argc, char **argv) 
{
	// set window values
	win.width = 600;
	win.height = 600;
	win.title = (char *) "BraXaPsa III";
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;



	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);				// set window size
	glutCreateWindow(win.title);					// create Window
	glutDisplayFunc(display);						// register Display Function
	glutIdleFunc( display );						// register Idle Function
        glutKeyboardFunc( keyboard );						// register Keyboard Handler
        glutSpecialFunc(special_keys);                  // Set the special keyboard function
        glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

        int menuId = glutCreateMenu(menu);
        glutAddMenuEntry("Exit", 0);
        glutAddMenuEntry("Start Game", 1);
        glutAddMenuEntry("Disable Rotate Camera", 2);
        glutAddMenuEntry("Enable Rotate Camera", 3);

        glutAttachMenu(GLUT_RIGHT_BUTTON);
        initialize();
        loadTextureFromFile("rock.bmp");
        loadTextureFromFile("scissors.bmp");
        loadTextureFromFile("paper.bmp");

	glutMainLoop();							// run GLUT mainloop
	return 0;
}
