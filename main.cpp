#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

void init(void);
void display(void);
void keyboard(unsigned char, int, int);
void resize(int, int);
vector<int> v;
unsigned int ID;

GLfloat car1Step = 1;
GLfloat car1Initial = 180;
GLfloat car2Step = 1;
GLfloat car2Initial = -180;

static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,1,5,7},  //front
    {6,4,0,2},  //back
    {2,3,7,6},  //top
    {1,0,4,5},  //bottom
    {7,5,4,6},  //right
    {2,0,1,3}   //left
};

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void cube(float R=0.5, float G=0.5, float B=0.5, float val=1)
{
    GLfloat m_no[] = {0, 0, 0, 1.0};
    GLfloat m_amb[] = {R,G,B,1};
    GLfloat m_diff[] = {R,G,B,1};
    GLfloat m_spec[] = {1,1,1,1};
    GLfloat m_sh[] = {30};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0,val);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val,val);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}
// TEXTURE
class BmpLoader
{
    public:
        unsigned char* textureData;
        int iWidth, iHeight;

        BmpLoader(const char*);
        ~BmpLoader();

    private:
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;
};

BmpLoader::BmpLoader(const char* filename)
{
    FILE *file=0;
    file=fopen(filename, "rb");
    if(!file)
        cout<<"File "<<filename<<" not found"<<endl;
    fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
        cout<<"Not a valid bitmap"<<endl;
    fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage=bih.biHeight*bih.biWidth*3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for(int i=0; i<bih.biSizeImage; i+=3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2] = temp;
    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete [] textureData;
}

void LoadTexture(const char*filename)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

void texture_image()
{
    /// building texture
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\building-1.bmp");
    v.push_back(ID);

    /// grass texture
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\grass-texture.bmp");
    v.push_back(ID);

    ///Building 2
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\building-2.bmp");
    v.push_back(ID);

    ///Door
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\2_door.bmp");
    v.push_back(ID);

    /// building texture short
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\building-1 (2).bmp");
    v.push_back(ID);

    ///Building 3
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\glass.bmp");
    v.push_back(ID);

    ///Road z
    LoadTexture("D:\\3D City Scenario Images\\Textures\\Roads\\BMPFormat\\road-z.bmp");
    v.push_back(ID);
}

void pohon()
{


}

void lampu_lalu_lintas(){
    //TIANG LAMPU
     glPushMatrix();
     glColor3f(0.15,0.15,0.15);
     glTranslatef(-5,15,123.8);
     glScalef(2,60,2);
     glutSolidCube(0.5);
     glPopMatrix();

     glPushMatrix();
     glColor3f(0.1,0.1,0.1);
     glTranslatef(-5,30,123.8);
     glScalef(10,32,4);
     glutSolidCube(0.5);
     glPopMatrix();
     //AKHIR TIAANG LAMPU

    //LAMPU MERAH
     glPushMatrix();
     glTranslatef(-5,35,124.8);
     glScalef(5.5, 5.5, 1.5);
     glColor3f(1.0,0.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU MERAH

    //LAMPU KUNING
     glPushMatrix();
     glTranslatef(-5,30,124.8);
     glScalef(5.5,5.5,1.5);
     glColor3f(0.8,0.8,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU KUNING

    //LAMPU HIJAU
     glPushMatrix();
     glTranslatef(-5,25,124.8);
     glScalef(5.5,5.5,1.5);
     glColor3f(0.0,1.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU HIJAU
}


void lampu_lalu_lintas_2(){
    //TIANG LAMPU
     glPushMatrix();
     glColor3f(0.15,0.15,0.15);
     glTranslatef(-5,15,97.8);
     glScalef(2,60,2);
     glutSolidCube(0.5);
     glPopMatrix();

     glPushMatrix();
     glColor3f(0.1,0.1,0.1);
     glTranslatef(-5,30,97.5);
     glScalef(4,32,10);
     glutSolidCube(0.5);
     glPopMatrix();
     //AKHIR TIAANG LAMPU

    //LAMPU MERAH
     glPushMatrix();
     glTranslatef(-6,35,97.5);
     glScalef(1.5, 5.5, 5.5);
     glColor3f(1.0,0.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU MERAH

    //LAMPU KUNING
     glPushMatrix();
     glTranslatef(-6,30,97.5);
     glScalef(1.5,5.5,5.5);
     glColor3f(0.8,0.8,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU KUNING

    //LAMPU HIJAU
     glPushMatrix();
     glTranslatef(-6,25,97.5);
     glScalef(1.5,5.5,5.5);
     glColor3f(0.0,1.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU HIJAU
}

void lampu_lalu_lintas_3(){
    //TIANG LAMPU
     glPushMatrix();
     glColor3f(0.15,0.15,0.15);
     glTranslatef(82,15,2.8);
     glScalef(2,60,2);
     glutSolidCube(0.5);
     glPopMatrix();

     glPushMatrix();
     glColor3f(0.1,0.1,0.1);
     glTranslatef(82,30,2.8);
     glScalef(4,32,10);
     glutSolidCube(0.5);
     glPopMatrix();
     //AKHIR TIAANG LAMPU

    //LAMPU MERAH
     glPushMatrix();
     glTranslatef(83,35,2.8);
     glScalef(1.5, 5.5, 5.5);
     glColor3f(1.0,0.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU MERAH

    //LAMPU KUNING
     glPushMatrix();
     glTranslatef(83,30,2.8);
     glScalef(1.5,5.5,5.5);
     glColor3f(0.8,0.8,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU KUNING

    //LAMPU HIJAU
     glPushMatrix();
     glTranslatef(83,25,2.8);
     glScalef(1.5,5.5,5.5);
     glColor3f(0.0,1.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU HIJAU
}

void lampu_lalu_lintas_4(){
    //TIANG LAMPU
     glPushMatrix();
     glColor3f(0.15,0.15,0.15);
     glTranslatef(77,15,-20.8);
     glScalef(2,60,2);
     glutSolidCube(0.5);
     glPopMatrix();

     glPushMatrix();
     glColor3f(0.1,0.1,0.1);
     glTranslatef(77,30,-20.8);
     glScalef(10,32,4);
     glutSolidCube(0.5);
     glPopMatrix();
     //AKHIR TIAANG LAMPU

    //LAMPU MERAH
     glPushMatrix();
     glTranslatef(77,35,-21.8);
     glScalef(5.5, 5.5, 1.5);
     glColor3f(1.0,0.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU MERAH

    //LAMPU KUNING
     glPushMatrix();
     glTranslatef(77,30,-21.8);
     glScalef(5.5,5.5,1.5);
     glColor3f(0.8,0.8,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU KUNING

    //LAMPU HIJAU
     glPushMatrix();
     glTranslatef(77,25,-21.8);
     glScalef(5.5,5.5,1.5);
     glColor3f(0.0,1.0,0.0);
     glutSolidSphere(0.3,30,30);
     glPopMatrix();
     //AKHIR LAMPU HIJAU
}


void duagedung(){
    // -------------------------------------- 2 GEDUNG KUNING -----------------------------------------
    //GEDUNG 1
    glPushMatrix();
    glColor3f(0.0f, 0.1f, 0.1f);
    glTranslatef(165,35,53.8);
    glScalef(60,150,195);
    glutSolidCube(0.5);
    glPopMatrix();

    //GEDUNG 2
    glPushMatrix();
    glColor3f(0.0f, 0.1f, 0.1f);
    glTranslatef(120,35,53.8);
    glScalef(60,150,195);
    glutSolidCube(0.5);
    glPopMatrix();

    // halaman gedung
    glPushMatrix();
    glTranslatef(143,0,80);
    glScalef(153,1,310);
    glColor3f(0.3,0.3,0.3);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    // jalan ke gedung dari jalan raya
    glPushMatrix();
    glTranslatef(143,0,170);
    glScalef(50,0.5,50);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    //-------------------------------------------- DINDING PEMBATAS GEDUNG ---------------------------------
    //Dinding kiri
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(106.4,4,130);
    glScalef(5,15,110);
    glutSolidCube(0.5);
    glPopMatrix();

    //Dinding kanan
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(178.7,4,130);
    glScalef(5,15,110);
    glutSolidCube(0.5);
    glPopMatrix();

    //Dinding depan kanan
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(118,4,156.3);
    glScalef(50,15,5);
    glutSolidCube(0.5);
    glPopMatrix();

    //Dinding depan kiri
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(167.5,4,156.3);
    glScalef(49,15,5);
    glutSolidCube(0.5);
    glPopMatrix();

    //------------------------------------------------------ AKHIR --------------------------------------------------
    //---------------------------------------------------- BUNDARAN JALAN -------------------------------------------
    glPushMatrix();
    glTranslatef(142.5,0.5,136.3);
    glScalef(44,1,35);
    glColor3f(0.0,0.2,0.0);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    //dinding1-belakang
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(142.5,1,128.3);
    glScalef(50,3,3);
    glutSolidCube(0.5);
    glPopMatrix();

    //dinding2-depan
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(142.5,1,146.3);
    glScalef(50,3,3);
    glutSolidCube(0.5);
    glPopMatrix();

    //dinding-samping
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(130.8,1,137.2);
    glScalef(3,3,39);
    glutSolidCube(0.5);
    glPopMatrix();

    //dinding4-samping
    glPushMatrix();
    glColor3f(0.7,0.7,0.7);
    glTranslatef(154.2,1,137.2);
    glScalef(3,3,39);
    glutSolidCube(0.5);
    glPopMatrix();

    // JENDELA GEDUNG KUNING-KIRI
    // samping kiri

    int x = 0;
    int y = 0;
    int z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<12; j++)
        {
            glPushMatrix();
            glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

            glPushMatrix();
            glColor3f(0.5f, 1.0f, 1.0f);
            glTranslatef(104.9,60+y,16.8+z);
            glScalef(0.1,17,15);
            glutSolidCube(0.5);
            glPopMatrix();
            glPopMatrix();

            z = z+15;
            j = j+1;
        }
        y = y-15;
        z = 0;
        i = i+1;
    }

    //samping kanan
    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<12; j++)
        {
            glPushMatrix();
            glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

            glPushMatrix();
            glColor3f(0.5f, 1.0f, 1.0f);
            glTranslatef(135.1,60+y,16.8+z);
            glScalef(0.1,17,15);
            glutSolidCube(0.5);
            glPopMatrix();
            glPopMatrix();

            z = z+15;
            j = j+1;
        }
        y = y-15;
        z = 0;
        i = i+1;
    }

    // jendela belakang
    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<6; j++)
        {
            glPushMatrix();
            glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

            glPushMatrix();
            glColor3f(0.5f, 1.0f, 1.0f);
            glTranslatef(111+x,60+y,4.9);
            glScalef(12,14,0.1);
            glutSolidCube(0.5);
            glPopMatrix();
            glPopMatrix();

            x = x+9;
            j = j+1;
        }
        y = y-15;
        x = 0;
        i = i+1;
    }

    // jendela depan
    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<6; j++)
        {
            if(j==2 && i==2)
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

                glPushMatrix();
                glColor3f(0.5f, 1.0f, 1.0f);
                glTranslatef(111+x,57.5+y,103.6);
                glScalef(12,14,0.1);
                glutSolidCube(0.5);
                glPopMatrix();
                glPopMatrix();
            }

            x = x+9;
            j = j+1;

        }
        y = y-13;
        x = 0;
        i = i+1;
    }

    // JENDELA GEDUNG DUA
    //sebelah kiri

    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<12; j++)
        {
            glPushMatrix();
            glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

            glPushMatrix();
            glColor3f(0.5f, 1.0f, 1.0f);
            glTranslatef(149.9,60+y,16.8+z);
            glScalef(0.1,17,15);
            glutSolidCube(0.5);
            glPopMatrix();
            glPopMatrix();

            z = z+15;
            j = j+1;
        }
        y = y-15;
        z = 0;
        i = i+1;
    }

    //samping kanan
    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<12; j++)
        {
            glPushMatrix();
            glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

            glPushMatrix();
            glColor3f(0.5f, 1.0f, 1.0f);
            glTranslatef(180.1,60+y,16.8+z);
            glScalef(0.1,17,15);
            glutSolidCube(0.5);
            glPopMatrix();
            glPopMatrix();

            z = z+15;
            j = j+1;
        }
        y = y-15;
        z = 0;
        i = i+1;
    }

    // jendela belakang
    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<6; j++)
        {
            glPushMatrix();
            glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

            glPushMatrix();
            glColor3f(0.5f, 1.0f, 1.0f);
            glTranslatef(156+x,60+y,4.9);
            glScalef(12,14,0.1);
            glutSolidCube(0.5);
            glPopMatrix();
            glPopMatrix();

            x = x+9;
            j = j+1;
        }
        y = y-15;
        x = 0;
        i = i+1;
    }

    // jendela depan
    x = 0;
    y = 0;
    z = 0;
    for(int i=0; i<=6; i++)
    {
        for(int j=0; j<6; j++)
        {
            if(j==2 && i==2)
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(0.0,-(j/10.0)*1.5,-(i/10.0)*2.0);

                glPushMatrix();
                glColor3f(0.5f, 1.0f, 1.0f);
                glTranslatef(156+x,57.5+y,103.6);
                glScalef(12,14,0.1);
                glutSolidCube(0.5);
                glPopMatrix();
                glPopMatrix();
            }

            x = x+9;
            j = j+1;

        }
        y = y-13;
        x = 0;
        i = i+1;
    }

    // LOBBY GEDUNG 1
    //atap lobby
    glPushMatrix();
    glTranslatef(120,22,106.8);
    glScalef(50,4,30);
    glColor3f(1.0,0.4,0.1);
    glutSolidCube(0.3);
    glPopMatrix();
    glPopMatrix();

    //lantai lobby
    glPushMatrix();
    glColor3f(0.5,0.5,0.35);
    glTranslatef(120,0.5,106.8);
    glScalef(50,3,30);
    glutSolidCube(0.3);
    glPopMatrix();

    //tiang lobby 1 gedung 1
    glPushMatrix();
    glColor3f(0.9,0.9,0.9);
    glTranslatef(115,10,110.8);
    glScalef(1,50,1);
    glutSolidCube(0.5);
    glPopMatrix();

    //tiang lobby 2 gedung 1
    glPushMatrix();
    glColor3f(0.9,0.9,0.9);
    glTranslatef(125,10,110.8);
    glScalef(1,50,1);
    glutSolidCube(0.5);
    glPopMatrix();

    //pintu lobby gedung 1
    glPushMatrix();
    glColor3f(1.0,1.0,1.0);
    glTranslatef(120,10,102.7);
    glScalef(15,40,0.0);
    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glutSolidCube(0.5);
    glPopMatrix();

    // LOBBY GEDUNG 2
    //atap lobby
    glPushMatrix();
    glTranslatef(165,22,106.8);
    glScalef(50,4,30);
    glColor3f(1.0,0.4,0.1);
    glutSolidCube(0.3);
    glPopMatrix();

    //lantai lobby
    glPushMatrix();
    glColor3f(0.5,0.5,0.35);
    glTranslatef(165,0.5,106.8);
    glScalef(50,3,30);
    glutSolidCube(0.3);
    glPopMatrix();

    //tiang lobby 1 gedung 1
    glPushMatrix();
    glColor3f(0.9,0.9,0.9);
    glTranslatef(160,10,110.8);
    glScalef(1,50,1);
    glutSolidCube(0.5);
    glPopMatrix();

    //tiang lobby 2 gedung 1
    glPushMatrix();
    glColor3f(0.9,0.9,0.9);
    glTranslatef(170,10,110.8);
    glScalef(1,50,1);
    glutSolidCube(0.5);
    glPopMatrix();

    //pintu lobby gedung 1
    glPushMatrix();
    glColor3f(1.0,1.0,1.0);
    glTranslatef(165,10,102.7);
    glScalef(15,40,0.0);
    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glutSolidCube(0.5);
    glPopMatrix();
}

void kursi(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

    }

    void kursi2(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

    }

    void kursi3(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}

void kursi4(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}

void kursi5(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}

void kursi6(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}

void kursi7(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}

void kursi8(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}

void kursi9(void){
    // Batang Tiang Kanan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Tiang Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,380.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang depan knan
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(43, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // Batang Depan Kiri
    glPushMatrix();
    glScaled(0.06, 0.2,0.06);
    glTranslatef(3, 0,390.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();

    // atas kursi
    glPushMatrix();
    glScaled(0.6, 0.05,0.3);
    glTranslatef(2.4,8,77);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0000, 0.5252, 0.0157);
    glutSolidCube(5.0);
    glPopMatrix();

}


void mobilKotak(int X){
    glPushMatrix();
    //bagian atas
    glColor3f(0.7,0.7,0.7);
    glTranslatef(0+X,20,0);
    glScalef(10,8,10);
    glutSolidCube(2);
    glPopMatrix();
    //bagian bawah
    glPushMatrix();
    glTranslatef(0+X,15,0);
    glScalef(25,5,10);
    glutSolidCube(2);
    glPopMatrix();
    //kaca depan
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(10+X,20,0);
    glScalef(0.1,6,7);
    glutSolidCube(2);
    glPopMatrix();
    // kaca kanan
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(0+X,23,10);
    glScalef(8,3,0.1);
    glutSolidCube(2);
    glPopMatrix();
    // kaca kiri
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(0+X,23,-10);
    glScalef(8,3,0.1);
    glutSolidCube(2);
    glPopMatrix();

    //lampu
    glColor3f(0.8, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(25+X,16,-6);
    glScalef(0.1,6,7);
    glutSolidSphere(0.3,20,20);
    glPopMatrix();

    glColor3f(0.8, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(25+X,16, 6);
    glScalef(0.1,6,7);
    glutSolidSphere(0.3,20,20);
    glPopMatrix();

    //bemper
    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(25+X,11, 0);
    glScalef(1,1,10);
    glutSolidCube(2);
    glPopMatrix();

    //ban
    glColor3f(1,0,1);
    glPushMatrix();
    glTranslatef(10+X,20,0);
    glScalef(0.1,6,7);
    glutSolidCube(2);
    glPopMatrix();

    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(-15+X,13,11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15+X,13,11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15+X,13,-11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15+X,13,-11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();
}

void mobilKotak2(int X){
    glPushMatrix();
    //bagian atas
    glColor3f(0.7,0.7,0.7);
    glTranslatef(0+X,20,0);
    glScalef(10,8,10);
    glutSolidCube(2);
    glPopMatrix();
    //bagian bawah
    glPushMatrix();
    glTranslatef(0+X,15,0);
    glScalef(25,5,10);
    glutSolidCube(2);
    glPopMatrix();
    //kaca depan
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(-10+X,20,0);
    glScalef(0.1,6,7);
    glutSolidCube(2);
    glPopMatrix();
    //kaca belakang
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(10+X,20,0);
    glScalef(0.1,6,7);
    glutSolidCube(2);
    glPopMatrix();
    // kaca kanan
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(0+X,23,10);
    glScalef(8,3,0.1);
    glutSolidCube(2);
    glPopMatrix();
    // kaca kiri
    glColor3f(0.9,0.9,0.9);
    glPushMatrix();
    glTranslatef(0+X,23,-10);
    glScalef(8,3,0.1);
    glutSolidCube(2);
    glPopMatrix();

    //lampu
    glColor3f(0.8, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-25+X,16,-6);
    glScalef(0.1,6,7);
    glutSolidSphere(0.3,20,20);
    glPopMatrix();

    glColor3f(0.8, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-25+X,16, 6);
    glScalef(0.1,6,7);
    glutSolidSphere(0.3,20,20);
    glPopMatrix();

    //bemper
    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(-25+X,11, 0);
    glScalef(1,1,10);
    glutSolidCube(2);
    glPopMatrix();

    //ban
    glColor3f(1,0,1);
    glPushMatrix();
    glTranslatef(10+X,20,0);
    glScalef(0.1,6,7);
    glutSolidCube(2);
    glPopMatrix();

    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(-15+X,13,11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15+X,13,11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15+X,13,-11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(15+X,13,-11);
    glScalef(1.2,1.2,1.2);
    glutSolidTorus(2,3,40,20);
    glPopMatrix();
}

void car(){
    // mobil
    glPushMatrix();
    glTranslatef(20,-7,car1Initial-=car1Step);
    if(car1Initial < -180){
        car1Initial = 180;
    }
    glRotatef(90,0,1,0);
    mobilKotak(0);
	glPopMatrix();
}

void car2(){
    // mobil
    glPushMatrix();
    glTranslatef(55,-7,car2Initial+=car2Step);
    if(car2Initial > 180){
        car2Initial = -180;
    }
    glRotatef(90,0,1,0);
    mobilKotak2(0);
	glPopMatrix();
}

void markajalan(void) {

    // marka jalan
    glPushMatrix();
    glScaled(0.25, 0.01, 1);
    glTranslatef(2.4,2.5,67);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1,1,1);
    glutSolidCube(5.0);
    glPopMatrix();
}
void apartkiri1(float x,float y,float z)
{
	int i;
	int j;
	glColor3f(1,1,1);
	/* DINDING apartkiri1 */
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[0]);
	glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(45,y+200,45);
    cube(1, 1, 1, 1);
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);

    /* PINTU apartkiri1 */
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[3]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0); glVertex3f(x+45.1,y+0.1,z+17.5);
    glTexCoord2f(1, 0); glVertex3f(x+45.1,y+0.1,z+27.5);
    glTexCoord2f(1, 1); glVertex3f(x+45.1,y+20,z+27.5);
    glTexCoord2f(0, 1); glVertex3f(x+45.1,y+20,z+17.5);
    glEnd();
	glDisable(GL_TEXTURE_2D);

    /* JALAN apartkiri1 */
    glPushMatrix();
    glTranslatef(x+57.5,0,z+22.5);
    glScalef(50,0.5,20);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glColor3f(0.8,0.4,0.4);
    /* LANTAI apartkiri1 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartkiri1 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+200.1,z);
    glVertex3f(x+45,y+200.1,z);
    glVertex3f(x+45,y+200.1,z+45);
    glVertex3f(x,y+200.1,z+45);
    glEnd();

}
void apartkiri2(float x,float y,float z)
{
	int i;
	int j;
    glColor3f(1,1,1);
    /* DINDING apartkiri2 */
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[2]);
	glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(45,y+140,45);
    cube(1, 1, 1, 1);
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);

    /* JALAN apartkiri2*/
    glPushMatrix();
    glTranslatef(x+57.5,0,z+22.5);
    glScalef(50,0.5,20);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glColor3f(0.9,0.5,0.3);
    /* LANTAI apartkiri2 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartkiri2 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+140.1,z);
    glVertex3f(x+45,y+140.1,z);
    glVertex3f(x+45,y+140.1,z+45);
    glVertex3f(x,y+140.1,z+45);
    glEnd();
}
void apartkiri3(float x,float y,float z)
{
	int i;
	int j;
    /* DINDING apartkiri3 */
	glColor3f(0.8,1,0.8);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[4]);glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(45,y+90,45);
    cube(1, 1, 1, 1);
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);

    /* PINTU apartkiri3 */
    glColor3f(0.5,0.5,0.5);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[3]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0); glVertex3f(x+45.1,y+0.1,z+17.5);
    glTexCoord2f(1, 0); glVertex3f(x+45.1,y+0.1,z+27.5);
    glTexCoord2f(1, 1); glVertex3f(x+45.1,y+20,z+27.5);
    glTexCoord2f(0, 1); glVertex3f(x+45.1,y+20,z+17.5);
    glEnd();
	glDisable(GL_TEXTURE_2D);

    /* JALAN apartkiri3*/
    glPushMatrix();
    glTranslatef(x+57.5,0,z+22.5);
    glScalef(50,0.5,20);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glColor3f(0.8,0.3,0);
    /* LANTAI apartkiri3 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartkiri3 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+90.1,z);
    glVertex3f(x+45,y+90.1,z);
    glVertex3f(x+45,y+90.1,z+45);
    glVertex3f(x,y+90.1,z+45);
    glEnd();
}
void apartkiri4(float x,float y,float z)
{
	int i;
	int j;
    /* DINDING apartkiri4 KIRI */
	glColor3f(0.2,0.2,0.2);
	glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y+230,z); //ini code tingginya di atur di bagian y
    glVertex3f(x,y+230,z);// ini juga
	glEnd();

	/* JENDELA apartkiri4 KIRI */
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(11.5*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+2,y+224,z-0.1);
            glVertex3f(x+8,y+224,z-0.1);
            glVertex3f(x+8,y+214,z-0.1);
            glVertex3f(x+2,y+214,z-0.1);
            glEnd();
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x+2,y+224,z-0.1);
            glVertex3f(x+8,y+224,z-0.1);
            glVertex3f(x+8,y+214,z-0.1);
            glVertex3f(x+2,y+214,z-0.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartkiri4 KANAN */
    glColor3f(0.2,0.2,0.2);
	glBegin(GL_POLYGON);
    glVertex3f(x,y,z+45);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+230,z+45);
    glVertex3f(x,y+230,z+45);
	glEnd();
	glColor3f(1,1,1);

	/* JENDELA apartkiri4 KANAN */
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(11.5*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+2,y+224,z+45.1);
            glVertex3f(x+8,y+224,z+45.1);
            glVertex3f(x+8,y+214,z+45.1);
            glVertex3f(x+2,y+214,z+45.1);
            glEnd();
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x+2,y+224,z+45.1);
            glVertex3f(x+8,y+224,z+45.1);
            glVertex3f(x+8,y+214,z+45.1);
            glVertex3f(x+2,y+214,z+45.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartkiri4 BELAKANG */
    glColor3f(0.2,0.2,0.2);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+45);
    glVertex3f(x,y+230,z+45);
    glVertex3f(x,y+230,z);
    glEnd();

    /* JENDELA apartkiri4 BELAKANG */
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11.5*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+224,z+2);
            glVertex3f(x-0.1,y+224,z+8);
            glVertex3f(x-0.1,y+214,z+8);
            glVertex3f(x-0.1,y+214,z+2);
            glEnd();
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+224,z+2);
            glVertex3f(x-0.1,y+224,z+8);
            glVertex3f(x-0.1,y+214,z+8);
            glVertex3f(x-0.1,y+214,z+2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartkiri4 DEPAN */
    glColor3f(0.2,0.2,0.2);
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+230,z+45);
    glVertex3f(x+45,y+230,z);
    glEnd();

    /* PINTU apartkiri4 */
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x+45.1,y+0.1,z+17.5);
    glVertex3f(x+45.1,y+0.1,z+27.5);
    glVertex3f(x+45.1,y+20,z+27.5);
    glVertex3f(x+45.1,y+20,z+17.5);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
    glVertex3f(x+45.1,y+0.1,z+17.5);
    glVertex3f(x+45.1,y+0.1,z+27.5);
    glVertex3f(x+45.1,y+20,z+27.5);
    glVertex3f(x+45.1,y+20,z+17.5);
    glEnd();

    /* JALAN apartkiri4*/
    glPushMatrix();
    glTranslatef(x+57.5,0,z+22.5);
    glScalef(50,0.5,20);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    /* JENDELA apartkiri4 DEPAN */
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            if(j==14 && (i==1 | i==2))
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(0,0,11.5*i);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+45.1,y+224,z+2);
                glVertex3f(x+45.1,y+224,z+8);
                glVertex3f(x+45.1,y+214,z+8);
                glVertex3f(x+45.1,y+214,z+2);
                glEnd();
                glColor3f(1,1,1);
                glBegin(GL_POLYGON);
                glVertex3f(x+45.1,y+224,z+2);
                glVertex3f(x+45.1,y+224,z+8);
                glVertex3f(x+45.1,y+214,z+8);
                glVertex3f(x+45.1,y+214,z+2);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }

    glColor3f(0.2,0.2,0.2);
    /* LANTAI apartkiri4 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartkiri4 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+230,z);
    glVertex3f(x+45,y+230,z);
    glVertex3f(x+45,y+230,z+45);
    glVertex3f(x,y+230,z+45);
    glEnd();
}
void apartkiri5(float x,float y,float z)
{

    int i;
    int j;
    /* DINDING apartkiri5 KIRI */
    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y+100,z); //ini code tingginya di atur di bagian y
    glVertex3f(x,y+100,z);// ini juga
    glEnd();

    /* JENDELA apartkiri5 KIRI */
    for(j=0;j<6;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(11.5*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+2,y+94,z-0.1);
            glVertex3f(x+8,y+94,z-0.1);
            glVertex3f(x+8,y+84,z-0.1);
            glVertex3f(x+2,y+84,z-0.1);
            glEnd();
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x+2,y+94,z-0.1);
            glVertex3f(x+8,y+94,z-0.1);
            glVertex3f(x+8,y+84,z-0.1);
            glVertex3f(x+2,y+84,z-0.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartkiri5 KANAN */
    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z+45);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+100,z+45);
    glVertex3f(x,y+100,z+45);
    glEnd();

    /* JENDELA apartkiri5 KANAN */
    for(j=0;j<6;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(11.5*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+2,y+94,z+45.1);
            glVertex3f(x+8,y+94,z+45.1);
            glVertex3f(x+8,y+84,z+45.1);
            glVertex3f(x+2,y+84,z+45.1);
            glEnd();
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x+2,y+94,z+45.1);
            glVertex3f(x+8,y+94,z+45.1);
            glVertex3f(x+8,y+84,z+45.1);
            glVertex3f(x+2,y+84,z+45.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartkiri5 BELAKANG */
    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+45);
    glVertex3f(x,y+100,z+45);
    glVertex3f(x,y+100,z);
    glEnd();

    /* JENDELA apartkiri5 BELAKANG */
    for(j=0;j<6;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11.5*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+94,z+2);
            glVertex3f(x-0.1,y+94,z+8);
            glVertex3f(x-0.1,y+84,z+8);
            glVertex3f(x-0.1,y+84,z+2);
            glEnd();
            glColor3f(1,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+94,z+2);
            glVertex3f(x-0.1,y+94,z+8);
            glVertex3f(x-0.1,y+84,z+8);
            glVertex3f(x-0.1,y+84,z+2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartkiri5 DEPAN */
    glColor3f(0,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+100,z+45);
    glVertex3f(x+45,y+100,z);
    glEnd();

    /* PINTU apartkiri5 */
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x+45.1,y+0.1,z+17.5);
    glVertex3f(x+45.1,y+0.1,z+27.5);
    glVertex3f(x+45.1,y+20,z+27.5);
    glVertex3f(x+45.1,y+20,z+17.5);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
    glVertex3f(x+45.1,y+0.1,z+17.5);
    glVertex3f(x+45.1,y+0.1,z+27.5);
    glVertex3f(x+45.1,y+20,z+27.5);
    glVertex3f(x+45.1,y+20,z+17.5);
    glEnd();

    /* JALAN apartkiri5*/
    glPushMatrix();
    glTranslatef(x+57.5,0,z+22.5);
    glScalef(50,0.5,20);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    /* JENDELA apartkiri5 DEPAN */
    for(j=0;j<6;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            if(j==5 && (i==1 | i==2))
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(0,0,11.5*i);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+45.1,y+94,z+2);
                glVertex3f(x+45.1,y+94,z+8);
                glVertex3f(x+45.1,y+84,z+8);
                glVertex3f(x+45.1,y+84,z+2);
                glEnd();
                glColor3f(1,1,1);
                glBegin(GL_POLYGON);
                glVertex3f(x+45.1,y+94,z+2);
                glVertex3f(x+45.1,y+94,z+8);
                glVertex3f(x+45.1,y+84,z+8);
                glVertex3f(x+45.1,y+84,z+2);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }

    glColor3f(0,0,0);
    /* LANTAI apartkiri5 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartkiri5 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+100,z);
    glVertex3f(x+45,y+100,z);
    glVertex3f(x+45,y+100,z+45);
    glVertex3f(x,y+100,z+45);
    glEnd();
}
void aparttengah1(float x, float y, float z)
{
    int i;
	int j;
    //jalan ke gedung
     glPushMatrix();
     glTranslatef(-5,0,-150);
     glScalef(40,0.01,70);
     glColor3f(0.2,0.2,0.2);
     glutSolidCube(0.5);
     glPopMatrix();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     //tiang1
     glPushMatrix();
     glTranslatef(0,10,-73);
     glScalef(0.5,20,0.5);
     glColor3f(0.5,0.8,0.8);
     glutSolidCube(1);
     glPopMatrix();
     //tiang1
     glPushMatrix();
     glTranslatef(0,10,-47);
     glScalef(0.5,20,0.5);
     glColor3f(0.5,0.8,0.8);
     glutSolidCube(1);
     glPopMatrix();
     //tutupan
     glPushMatrix();
     glTranslatef(-10 ,20,-150);
     glScalef(10,1,50);
     glColor3f(0.3,0,0);
     glutSolidCube(1);
     glPopMatrix();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);

	/* BAGIAN DINDING */
    glColor3f(0.1,0.0,0.0);
	// dinding belakang
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z);
	glVertex3f(x+45,y,z);
	glVertex3f(x+45,y+200,z);
 	glVertex3f(x,y+200,z);
	glEnd();
    // dinding depan
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z+60);
	glVertex3f(x+45,y,z+60);
	glVertex3f(x+45,y+200,z+60);
 	glVertex3f(x,y+200,z+60);
	glEnd();
	// dinding samping kiri
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+60);
    glVertex3f(x,y+200,z+60);
    glVertex3f(x,y+200,z);
    glEnd();
    // dinding samping kanan
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+60);
    glVertex3f(x+45,y+200,z+60);
    glVertex3f(x+45,y+200,z);
    glEnd();

    /* BAGIAN PINTU DEPAN */
    glColor4d(0.89, 0.97, 1, 0.6);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x+45.1,y+0.1,z+25);
    glVertex3f(x+45.1,y+0.1,z+35);
    glVertex3f(x+45.1,y+20,z+35);
    glVertex3f(x+45.1,y+20,z+25);
    glEnd();
    glColor4d(0.89, 0.97, 1, 0.6);
    glBegin(GL_POLYGON);
    glVertex3f(x+45.1,y+0.1,z+25);
    glVertex3f(x+45.1,y+0.1,z+35);
    glVertex3f(x+45.1,y+20,z+35);
    glVertex3f(x+45.1,y+20,z+25);
    glEnd();

    /* BAGIAN JENDELA */
    // jendela samping kanan
    for(j=0;j<14;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<5;i++)
        {
            if(j==14 && i==2)
            {
                continue;
            }
             else
             {
                glPushMatrix();
                glTranslatef(0,95,11*i);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+45.1,y+90,z+5);
                glVertex3f(x+45.1,y+90,z+10);
                glVertex3f(x+45.1,y+85,z+10);
                glVertex3f(x+45.1,y+85,z+5);
                glEnd();
                glColor3f(1.0,1.0,0.5);
                glBegin(GL_POLYGON);
                glVertex3f(x+45.1,y+90,z+5);
                glVertex3f(x+45.1,y+90,z+10);
                glVertex3f(x+45.1,y+85,z+10);
                glVertex3f(x+45.1,y+85,z+5);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }
    // jendela samping kiri
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<5;i++)
        {
            glPushMatrix();
            glTranslatef(0,95,11*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+90,z+5);
            glVertex3f(x-0.1,y+90,z+10);
            glVertex3f(x-0.1,y+85,z+10);
            glVertex3f(x-0.1,y+85,z+5);
            glEnd();
            glColor3f(1.0,1.0,0.5);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+90,z+5);
            glVertex3f(x-0.1,y+90,z+10);
            glVertex3f(x-0.1,y+85,z+10);
            glVertex3f(x-0.1,y+85,z+5);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }
    // jendela belakang
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(10*i,95,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+5,y+90,z-0.1);
            glVertex3f(x+10,y+90,z-0.1);
            glVertex3f(x+10,y+85,z-0.1);
            glVertex3f(x+5,y+85,z-0.1);
            glEnd();
            glColor3f(1.0,1.0,0.5);
            glBegin(GL_POLYGON);
            glVertex3f(x+5,y+90,z-0.1);
            glVertex3f(x+10,y+90,z-0.1);
            glVertex3f(x+10,y+85,z-0.1);
            glVertex3f(x+5,y+85,z-0.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }
    // jendela depan
    for(j=0;j<15;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0); // 12 = jarak antar kotak jendela
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(10*i,95,0); // 95 = posisi kotak jendela paling atas
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+5,y+90,z+60.1);
            glVertex3f(x+10,y+90,z+60.1);
            glVertex3f(x+10,y+85,z+60.1);
            glVertex3f(x+5,y+85,z+60.1);
            glEnd();
            glColor3f(1.0,1.0,0.5);
            glBegin(GL_POLYGON);
            glVertex3f(x+5,y+90,z+60.1);
            glVertex3f(x+10,y+90,z+60.1);
            glVertex3f(x+10,y+85,z+60.1);
            glVertex3f(x+5,y+85,z+60.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* BAGIAN GARIS ATAP */
	glColor3f(1,1,1);
	// garis atap belakang
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+197,z-0.1);
	glVertex3f(x+45.1,y+197,z-0.1);
	glVertex3f(x+45.1,y+203,z-0.1);
 	glVertex3f(x-0.1,y+203,z-0.1);
    glEnd();
	// garis atap depan
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+197,z+60.1);
	glVertex3f(x+45.1,y+197,z+60.1);
	glVertex3f(x+45.1,y+203,z+60.1);
 	glVertex3f(x-0.1,y+203,z+60.1);
    glEnd();
	// garis atap samping kiri
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+197,z-0.1);
	glVertex3f(x-0.1,y+197,z+60.1);
	glVertex3f(x-0.1,y+203,z+60.1);
 	glVertex3f(x-0.1,y+203,z-0.1);
    glEnd();
	// garis atap samping kanan
    glBegin(GL_POLYGON);
 	glVertex3f(x+45.1,y+197,z-0.1);
	glVertex3f(x+45.1,y+197,z+60.1);
	glVertex3f(x+45.1,y+203,z+60.1);
 	glVertex3f(x+45.1,y+203,z-0.1);
    glEnd();

}
// apart navy
void aparttengah3(float x, float y, float z)
{
    int i;
	int j;
    //jalan ke gedung
     glPushMatrix();
     glTranslatef(-5,0,-60);
     glScalef(40,0.01,70);
     glColor3f(0.2,0.2,0.2);
     glutSolidCube(0.5);
     glPopMatrix();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     //tiang1
     glPushMatrix();
     glTranslatef(0,10,-73);
     glScalef(0.5,20,0.5);
     glColor3f(0.5,0.8,0.8);
     glutSolidCube(1);
     glPopMatrix();
     //tiang1
     glPushMatrix();
     glTranslatef(0,10,-47);
     glScalef(0.5,20,0.5);
     glColor3f(0.5,0.8,0.8);
     glutSolidCube(1);
     glPopMatrix();
     //tutupan
     glPushMatrix();
     glTranslatef(-5,20,-60);
     glScalef(20,1,30);
     glColor3f(0.2,1,1);
     glutSolidCube(1);
     glPopMatrix();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);

	/* BAGIAN DINDING */
    glColor3f(0.0,0.1,0.1);
	// dinding belakang
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z);
	glVertex3f(x+45,y,z);
	glVertex3f(x+45,y+100,z);
 	glVertex3f(x,y+100,z);
	glEnd();
    // dinding depan
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z+60);
	glVertex3f(x+45,y,z+60);
	glVertex3f(x+45,y+100,z+60);
 	glVertex3f(x,y+100,z+60);
	glEnd();
	// dinding samping kiri
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+60);
    glVertex3f(x,y+100,z+60);
    glVertex3f(x,y+100,z);
    glEnd();
    // dinding samping kanan
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+60);
    glVertex3f(x+45,y+100,z+60);
    glVertex3f(x+45,y+100,z);
    glEnd();

    /* BAGIAN PINTU DEPAN */
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x+45.1,y+0.1,z+25);
    glVertex3f(x+45.1,y+0.1,z+35);
    glVertex3f(x+45.1,y+20,z+35);
    glVertex3f(x+45.1,y+20,z+25);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
    glVertex3f(x+45.1,y+0.1,z+25);
    glVertex3f(x+45.1,y+0.1,z+35);
    glVertex3f(x+45.1,y+20,z+35);
    glVertex3f(x+45.1,y+20,z+25);
    glEnd();

    /* BAGIAN JENDELA */
    // jendela samping kanan
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<5;i++)
        {
            if(j==6 && i==2)
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(0,0,11*i);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+45.1,y+90,z+5);
                glVertex3f(x+45.1,y+90,z+10);
                glVertex3f(x+45.1,y+85,z+10);
                glVertex3f(x+45.1,y+85,z+5);
                glEnd();
                glColor3f(0.8,0.8,0.8);
                glBegin(GL_POLYGON);
                glVertex3f(x+45.1,y+90,z+5);
                glVertex3f(x+45.1,y+90,z+10);
                glVertex3f(x+45.1,y+85,z+10);
                glVertex3f(x+45.1,y+85,z+5);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }
    // jendela samping kiri
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<5;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+90,z+5);
            glVertex3f(x-0.1,y+90,z+10);
            glVertex3f(x-0.1,y+85,z+10);
            glVertex3f(x-0.1,y+85,z+5);
            glEnd();
            glColor3f(0.8,0.8,0.8);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+90,z+5);
            glVertex3f(x-0.1,y+90,z+10);
            glVertex3f(x-0.1,y+85,z+10);
            glVertex3f(x-0.1,y+85,z+5);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }
    // jendela belakang
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(10*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+5,y+90,z-0.1);
            glVertex3f(x+10,y+90,z-0.1);
            glVertex3f(x+10,y+85,z-0.1);
            glVertex3f(x+5,y+85,z-0.1);
            glEnd();
            glColor3f(0.8,0.8,0.8);
            glBegin(GL_POLYGON);
            glVertex3f(x+5,y+90,z-0.1);
            glVertex3f(x+10,y+90,z-0.1);
            glVertex3f(x+10,y+85,z-0.1);
            glVertex3f(x+5,y+85,z-0.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }
    // jendela depan
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(10*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+5,y+90,z+60.1);
            glVertex3f(x+10,y+90,z+60.1);
            glVertex3f(x+10,y+85,z+60.1);
            glVertex3f(x+5,y+85,z+60.1);
            glEnd();
            glColor3f(0.8,0.8,0.8);
            glBegin(GL_POLYGON);
            glVertex3f(x+5,y+90,z+60.1);
            glVertex3f(x+10,y+90,z+60.1);
            glVertex3f(x+10,y+85,z+60.1);
            glVertex3f(x+5,y+85,z+60.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* BAGIAN GARIS ATAP */
	glColor3f(1,1,1);
	// garis atap belakang
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+97,z-0.1);
	glVertex3f(x+45.1,y+97,z-0.1);
	glVertex3f(x+45.1,y+103,z-0.1);
 	glVertex3f(x-0.1,y+103,z-0.1);
    glEnd();
	// garis atap depan
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+97,z+60.1);
	glVertex3f(x+45.1,y+97,z+60.1);
	glVertex3f(x+45.1,y+103,z+60.1);
 	glVertex3f(x-0.1,y+103,z+60.1);
    glEnd();
	// garis atap samping kiri
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+97,z-0.1);
	glVertex3f(x-0.1,y+97,z+60.1);
	glVertex3f(x-0.1,y+103,z+60.1);
 	glVertex3f(x-0.1,y+103,z-0.1);
    glEnd();
	// garis atap samping kanan
    glBegin(GL_POLYGON);
 	glVertex3f(x+45.1,y+97,z-0.1);
	glVertex3f(x+45.1,y+97,z+60.1);
	glVertex3f(x+45.1,y+103,z+60.1);
 	glVertex3f(x+45.1,y+103,z-0.1);
    glEnd();

    /* BAGIAN ATAP */
    glColor3f(0.3,0.1,0.1);
    glBegin(GL_POLYGON);
 	glVertex3f(x,y+100,z);
 	glVertex3f(x,y+100,z+60);
    glVertex3f(x+22.1,y+115,z+50);
    glVertex3f(x+22.1,y+115,z+10);
 	glEnd();
    glBegin(GL_POLYGON);
 	glVertex3f(x+45,y+100,z);
 	glVertex3f(x+45,y+100,z+60);
    glVertex3f(x+22.1,y+115,z+50);
    glVertex3f(x+22.1,y+115,z+10);
 	glEnd();
    glBegin(GL_TRIANGLES);
 	glVertex3f(x,y+100,z);
    glVertex3f(x+45,y+100,z);
    glVertex3f(x+22.1,y+115,z+10);
    glEnd();
    glBegin(GL_TRIANGLES);
 	glVertex3f(x,y+100,z+60);
    glVertex3f(x+45,y+100,z+60);
    glVertex3f(x+22.1,y+115,z+50);
    glEnd();
}
// apart biru
void aparttengah4(float x, float y, float z)
{
    int i;
	int j;

	//jalan ke gedung
     glPushMatrix();
     glTranslatef(-5,0,50);
     glScalef(40,0.01,70);
     glColor3f(0.2,0.2,0.2);
     glutSolidCube(0.5);
     glPopMatrix();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);
     //tiang1
     glPushMatrix();
     glTranslatef(0,10,63);
     glScalef(0.5,20,0.5);
     glColor3f(0.5,0.8,0.8);
     glutSolidCube(1);
     glPopMatrix();
     //tiang1
     glPushMatrix();
     glTranslatef(0,10,37);
     glScalef(0.5,20,0.5);
     glColor3f(0.5,0.8,0.8);
     glutSolidCube(1);
     glPopMatrix();
     //tutupan
     glPushMatrix();
     glTranslatef(-5,20,50);
     glScalef(20,1,30);
     glColor3f(0.2,0.2,1);
     glutSolidCube(1);
     glPopMatrix();
     glDisable(GL_LIGHTING);
     glDisable(GL_LIGHT0);

	/* BAGIAN DINDING */
    glColor3f(0.2,0.2,0.8);
	// dinding belakang
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z);
	glVertex3f(x+45,y,z);
	glVertex3f(x+45,y+100,z);
 	glVertex3f(x,y+100,z);
	glEnd();
    // dinding depan
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z+60);
	glVertex3f(x+45,y,z+60);
	glVertex3f(x+45,y+100,z+60);
 	glVertex3f(x,y+100,z+60);
	glEnd();
	// dinding samping kiri
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+60);
    glVertex3f(x,y+100,z+60);
    glVertex3f(x,y+100,z);
    glEnd();
    // dinding samping kanan
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+60);
    glVertex3f(x+45,y+100,z+60);
    glVertex3f(x+45,y+100,z);
    glEnd();

    /* BAGIAN PINTU DEPAN */
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x+45.1,y+0.1,z+25);
    glVertex3f(x+45.1,y+0.1,z+35);
    glVertex3f(x+45.1,y+20,z+35);
    glVertex3f(x+45.1,y+20,z+25);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
    glVertex3f(x+45.1,y+0.1,z+25);
    glVertex3f(x+45.1,y+0.1,z+35);
    glVertex3f(x+45.1,y+20,z+35);
    glVertex3f(x+45.1,y+20,z+25);
    glEnd();

    /* BAGIAN JENDELA */
    // jendela samping kanan
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<5;i++)
        {
            if(j==6 && i==2)
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(0,0,11*i);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+45.1,y+90,z+5);
                glVertex3f(x+45.1,y+90,z+10);
                glVertex3f(x+45.1,y+85,z+10);
                glVertex3f(x+45.1,y+85,z+5);
                glEnd();
                glColor3f(0.8,1.0,1.0);
                glBegin(GL_POLYGON);
                glVertex3f(x+45.1,y+90,z+5);
                glVertex3f(x+45.1,y+90,z+10);
                glVertex3f(x+45.1,y+85,z+10);
                glVertex3f(x+45.1,y+85,z+5);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }
    // jendela samping kiri
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<5;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+90,z+5);
            glVertex3f(x-0.1,y+90,z+10);
            glVertex3f(x-0.1,y+85,z+10);
            glVertex3f(x-0.1,y+85,z+5);
            glEnd();
            glColor3f(0.8,1.0,1.0);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+90,z+5);
            glVertex3f(x-0.1,y+90,z+10);
            glVertex3f(x-0.1,y+85,z+10);
            glVertex3f(x-0.1,y+85,z+5);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }
    // jendela belakang
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(10*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+5,y+90,z-0.1);
            glVertex3f(x+10,y+90,z-0.1);
            glVertex3f(x+10,y+85,z-0.1);
            glVertex3f(x+5,y+85,z-0.1);
            glEnd();
            glColor3f(0.8,1.0,1.0);
            glBegin(GL_POLYGON);
            glVertex3f(x+5,y+90,z-0.1);
            glVertex3f(x+10,y+90,z-0.1);
            glVertex3f(x+10,y+85,z-0.1);
            glVertex3f(x+5,y+85,z-0.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }
    // jendela depan
    for(j=0;j<7;j++)
    {
        glPushMatrix();
        glTranslatef(0,-12*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(10*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+5,y+90,z+60.1);
            glVertex3f(x+10,y+90,z+60.1);
            glVertex3f(x+10,y+85,z+60.1);
            glVertex3f(x+5,y+85,z+60.1);
            glEnd();
            glColor3f(0.8,1.0,1.0);
            glBegin(GL_POLYGON);
            glVertex3f(x+5,y+90,z+60.1);
            glVertex3f(x+10,y+90,z+60.1);
            glVertex3f(x+10,y+85,z+60.1);
            glVertex3f(x+5,y+85,z+60.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* BAGIAN GARIS ATAP */
	glColor3f(1,1,1);
	// garis atap belakang
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+97,z-0.1);
	glVertex3f(x+45.1,y+97,z-0.1);
	glVertex3f(x+45.1,y+103,z-0.1);
 	glVertex3f(x-0.1,y+103,z-0.1);
    glEnd();
	// garis atap depan
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+97,z+60.1);
	glVertex3f(x+45.1,y+97,z+60.1);
	glVertex3f(x+45.1,y+103,z+60.1);
 	glVertex3f(x-0.1,y+103,z+60.1);
    glEnd();
	// garis atap samping kiri
    glBegin(GL_POLYGON);
 	glVertex3f(x-0.1,y+97,z-0.1);
	glVertex3f(x-0.1,y+97,z+60.1);
	glVertex3f(x-0.1,y+103,z+60.1);
 	glVertex3f(x-0.1,y+103,z-0.1);
    glEnd();
	// garis atap samping kanan
    glBegin(GL_POLYGON);
 	glVertex3f(x+45.1,y+97,z-0.1);
	glVertex3f(x+45.1,y+97,z+60.1);
	glVertex3f(x+45.1,y+103,z+60.1);
 	glVertex3f(x+45.1,y+103,z-0.1);
    glEnd();

    /* BAGIAN ATAP */
    glColor3f(0.3,0.1,0.1);
    glBegin(GL_POLYGON);
 	glVertex3f(x,y+100,z);
 	glVertex3f(x,y+100,z+60);
    glVertex3f(x+22.5,y+115,z+50);
    glVertex3f(x+22.5,y+115,z+10);
 	glEnd();
    glBegin(GL_POLYGON);
 	glVertex3f(x+45,y+100,z);
 	glVertex3f(x+45,y+100,z+60);
    glVertex3f(x+22.1,y+115,z+50);
    glVertex3f(x+22.1,y+115,z+10);
 	glEnd();
    glBegin(GL_TRIANGLES);
 	glVertex3f(x,y+100,z);
    glVertex3f(x+45,y+100,z);
    glVertex3f(x+22.1,y+115,z+10);
    glEnd();
    glBegin(GL_TRIANGLES);
 	glVertex3f(x,y+100,z+60);
    glVertex3f(x+45,y+100,z+60);
    glVertex3f(x+22.1,y+115,z+50);
    glEnd();
}
void apartnyamping1(float x,float y,float z)
{
	int i;
	int j;
    /* DINDING apartnyamping1 DEPAN */
	glColor3f(0.2,0.2,0.2);
	glBegin(GL_POLYGON);
	glVertex3f(x,y,z);
	glVertex3f(x+45,y,z);
	glVertex3f(x+45,y+260,z);
 	glVertex3f(x,y+260,z);
	glEnd();

    /* PINTU apartnyamping1 */
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
	glVertex3f(x+17.5,y+0.1,z-0.1);
	glVertex3f(x+27.5,y+0.1,z-0.1);
	glVertex3f(x+27.5,y+20,z-0.1);
 	glVertex3f(x+17.5,y+20,z-0.1);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
	glVertex3f(x+17.5,y+0.1,z-0.1);
	glVertex3f(x+27.5,y+0.1,z-0.1);
	glVertex3f(x+27.5,y+20,z-0.1);
 	glVertex3f(x+17.5,y+20,z-0.1);
    glEnd();

    /* JALAN apartnyamping1 */
    glPushMatrix();
    glTranslatef(x+22.5,0,z-10);
    glScalef(20,0.5,40);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    /* JENDELA apartnyamping1 DEPAN */
	for(j=0;j<17;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            if(j==16 && (i==1 | i==2))
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(11.5*i,0,0);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+2,y+254,z-0.1);
                glVertex3f(x+8,y+254,z-0.1);
                glVertex3f(x+8,y+244,z-0.1);
                glVertex3f(x+2,y+244,z-0.1);
                glEnd();
                glColor3f(1,1,0);
                glBegin(GL_POLYGON);
                glVertex3f(x+2,y+254,z-0.1);
                glVertex3f(x+8,y+254,z-0.1);
                glVertex3f(x+8,y+244,z-0.1);
                glVertex3f(x+2,y+244,z-0.1);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }

    /* DINDING apartnyamping1 BELAKANG */
    glColor3f(0.2,0.2,0.2);
	glBegin(GL_POLYGON);
    glVertex3f(x,y,z+45);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+260,z+45);
    glVertex3f(x,y+260,z+45);
	glEnd();

    /* JENDELA apartnyamping1 BELAKANG */
    for(j=0;j<17;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(11.5*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+2,y+254,z+45.1);
            glVertex3f(x+8,y+254,z+45.1);
            glVertex3f(x+8,y+244,z+45.1);
            glVertex3f(x+2,y+244,z+45.1);
            glEnd();
            glColor3f(1,1,0);
            glBegin(GL_POLYGON);
            glVertex3f(x+2,y+254,z+45.1);
            glVertex3f(x+8,y+254,z+45.1);
            glVertex3f(x+8,y+244,z+45.1);
            glVertex3f(x+2,y+244,z+45.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartnyamping1 KANAN */
    glColor3f(0.2,0.2,0.2);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+45);
    glVertex3f(x,y+260,z+45);
    glVertex3f(x,y+260,z);
    glEnd();

    /* JENDELA apartnyamping1 KANAN */
    for(j=0;j<17;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11.5*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+254,z+2);
            glVertex3f(x-0.1,y+254,z+8);
            glVertex3f(x-0.1,y+244,z+8);
            glVertex3f(x-0.1,y+244,z+2);
            glEnd();
            glColor3f(1,1,0);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+254,z+2);
            glVertex3f(x-0.1,y+254,z+8);
            glVertex3f(x-0.1,y+244,z+8);
            glVertex3f(x-0.1,y+244,z+2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartnyamping1 KIRI */
    glColor3f(0.2,0.2,0.2);
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+260,z+45);
    glVertex3f(x+45,y+260,z);
    glEnd();

    /* JENDELA apartnyamping1 KIRI */
    for(j=0;j<17;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11.5*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+45.1,y+254,z+2);
            glVertex3f(x+45.1,y+254,z+8);
            glVertex3f(x+45.1,y+244,z+8);
            glVertex3f(x+45.1,y+244,z+2);
            glEnd();
            glColor3f(1,1,0);
            glBegin(GL_POLYGON);
            glVertex3f(x+45.1,y+254,z+2);
            glVertex3f(x+45.1,y+254,z+8);
            glVertex3f(x+45.1,y+244,z+8);
            glVertex3f(x+45.1,y+244,z+2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    glColor3f(0.2,0.2,0.2);
    /* LANTAI apartnyamping1 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartnyamping1 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+260,z);
    glVertex3f(x+45,y+260,z);
    glVertex3f(x+45,y+260,z+45);
    glVertex3f(x,y+260,z+45);
    glEnd();
}
void apartnyamping2(float x,float y,float z)
{
	int i;
	int j;
    /* DINDING apartnyamping2 DEPAN */
	glColor3f(0.5,0,0);
	glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y+200,z);
    glVertex3f(x,y+200,z);
	glEnd();

    /* PINTU apartnyamping2 */
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x+17.5,y+0.1,z-0.1);
    glVertex3f(x+27.5,y+0.1,z-0.1);
    glVertex3f(x+27.5,y+20,z-0.1);
    glVertex3f(x+17.5,y+20,z-0.1);
    glEnd();
    glColor3f(1,1,1);
    glBegin(GL_POLYGON);
    glVertex3f(x+17.5,y+0.1,z-0.1);
    glVertex3f(x+27.5,y+0.1,z-0.1);
    glVertex3f(x+27.5,y+20,z-0.1);
    glVertex3f(x+17.5,y+20,z-0.1);
    glEnd();

    /* JALAN apartnyamping2 */
    glPushMatrix();
    glTranslatef(x+22.5,0,z-10);
    glScalef(20,0.5,40);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

	/* JENDELA apartnyamping2 DEPAN */
    for(j=0;j<13;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            if(j==12 && (i==1 | i==2))
            {
                continue;
            }
            else
            {
                glPushMatrix();
                glTranslatef(11.5*i,0,0);
                glColor3f(0,0,0);
                glBegin(GL_LINE_LOOP);
                glVertex3f(x+2,y+194,z-0.1);
                glVertex3f(x+8,y+194,z-0.1);
                glVertex3f(x+8,y+184,z-0.1);
                glVertex3f(x+2,y+184,z-0.1);
                glEnd();
                glColor3f(0,1,1);
                glBegin(GL_POLYGON);
                glVertex3f(x+2,y+194,z-0.1);
                glVertex3f(x+8,y+194,z-0.1);
                glVertex3f(x+8,y+184,z-0.1);
                glVertex3f(x+2,y+184,z-0.1);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }

    /* DINDING apartnyamping2 BELAKANG */
    glColor3f(0.5,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z+45);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+200,z+45);
    glVertex3f(x,y+200,z+45);
    glEnd();

    /* JENDELA apartnyamping2 BELAKANG */
	for(j=0;j<13;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(11.5*i,0,0);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+2,y+194,z+45.1);
            glVertex3f(x+8,y+194,z+45.1);
            glVertex3f(x+8,y+184,z+45.1);
            glVertex3f(x+2,y+184,z+45.1);
            glEnd();
            glColor3f(0,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x+2,y+194,z+45.1);
            glVertex3f(x+8,y+194,z+45.1);
            glVertex3f(x+8,y+184,z+45.1);
            glVertex3f(x+2,y+184,z+45.1);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartnyamping2 KANAN */
    glColor3f(0.5,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x,y,z+45);
    glVertex3f(x,y+200,z+45);
    glVertex3f(x,y+200,z);
    glEnd();

    /* JENDELA apartnyamping2 KANAN */
    for(j=0;j<13;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11.5*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x-0.1,y+194,z+2);
            glVertex3f(x-0.1,y+194,z+8);
            glVertex3f(x-0.1,y+184,z+8);
            glVertex3f(x-0.1,y+184,z+2);
            glEnd();
            glColor3f(0,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x-0.1,y+194,z+2);
            glVertex3f(x-0.1,y+194,z+8);
            glVertex3f(x-0.1,y+184,z+8);
            glVertex3f(x-0.1,y+184,z+2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    /* DINDING apartnyamping2 KIRI */
    glColor3f(0.5,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x+45,y+200,z+45);
    glVertex3f(x+45,y+200,z);
    glEnd();

    /* JENDELA apartnyamping2 KIRI */
    for(j=0;j<13;j++)
    {
        glPushMatrix();
        glTranslatef(0,-14.5*j,0);
        for(i=0;i<4;i++)
        {
            glPushMatrix();
            glTranslatef(0,0,11.5*i);
            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x+45.1,y+194,z+2);
            glVertex3f(x+45.1,y+194,z+8);
            glVertex3f(x+45.1,y+184,z+8);
            glVertex3f(x+45.1,y+184,z+2);
            glEnd();
            glColor3f(0,1,1);
            glBegin(GL_POLYGON);
            glVertex3f(x+45.1,y+194,z+2);
            glVertex3f(x+45.1,y+194,z+8);
            glVertex3f(x+45.1,y+184,z+8);
            glVertex3f(x+45.1,y+184,z+2);
            glEnd();
            glPopMatrix();
        }
        glPopMatrix();
    }

    glColor3f(0.5,0,0);
    /* LANTAI apartnyamping2 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartnyamping2 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+200,z);
    glVertex3f(x+45,y+200,z);
    glVertex3f(x+45,y+200,z+45);
    glVertex3f(x,y+200,z+45);
    glEnd();
}
void apartnyamping3(float x,float y,float z)
{
	int i;
	int j;
    /* DINDING apartnyamping3 DEPAN */
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[5]);glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(45,y+230,45);
    cube(1, 1, 1, 4);
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);

    /* PINTU apartnyamping3 */
    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,v[3]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0); glVertex3f(x+17.5,y+0.1,z-0.1);
    glTexCoord2f(1, 0); glVertex3f(x+27.5,y+0.1,z-0.1);
    glTexCoord2f(1, 1); glVertex3f(x+27.5,y+20,z-0.1);
    glTexCoord2f(0, 1); glVertex3f(x+17.5,y+20,z-0.1);
    glEnd();
	glDisable(GL_TEXTURE_2D);

    /* JALAN apartnyamping3 */
    glPushMatrix();
    glTranslatef(x+22.5,0,z-10);
    glScalef(20,0.5,40);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glColor3f(0.4,0.5,0.6);
    /* LANTAI apartnyamping3 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y,z);
    glVertex3f(x+45,y,z);
    glVertex3f(x+45,y,z+45);
    glVertex3f(x,y,z+45);
    glEnd();

    /* ATAP apartnyamping3 */
    glBegin(GL_POLYGON);
    glVertex3f(x,y+230.1,z);
    glVertex3f(x+45,y+230.1,z);
    glVertex3f(x+45,y+230.1,z+45);
    glVertex3f(x,y+230.1,z+45);
    glEnd();
}
static void idle(void)
{
    glutPostRedisplay();
}
int main (int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1200, 1200);
	glutInitWindowPosition(100, 10);
	glutCreateWindow("3D City");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	texture_image();

	glutMainLoop();
	return 0;
}
void init(void)
{
    glClearColor(0.0,0.0,0.0,0.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,1,1);

	/* the floor */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[1]);
    glPushMatrix();
    glTranslatef(-200,0, -200);
    glScalef(400,0,400);
    cube(100, 100, 100, 100);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /* main road */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[6]);
	glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0,0.1,-200);
    glTexCoord2f(0, 1); glVertex3f(0,0.1,0);
    glTexCoord2f(1, 1); glVertex3f(80,0.1,0);
    glTexCoord2f(1, 0); glVertex3f(80,0.1,-200);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[6]);
	glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0,0.1,0);
    glTexCoord2f(0, 1); glVertex3f(0,0.1,200);
    glTexCoord2f(1, 1); glVertex3f(80,0.1,200);
    glTexCoord2f(1, 0); glVertex3f(80,0.1,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    /* marka jalan main road */
    /*
    int m;
    float pos = 25;
    for(m=0;m<12;m++)
    {
        pos -= 34;
        glPushMatrix();
        glTranslatef(37.5,0,pos);
        glScalef(3, 3, 3);
        //glBindTexture(GL_TEXTURE_2D, texture[0]);
        markajalan();
        glPopMatrix();
    }
    */

    glColor3f(0.2, 0.2, 0.2);
    /* road (kanan)*/
    glBegin(GL_QUADS);
    glVertex3f(35,0.01,-20);
    glVertex3f(35,0.01, 0);
    glVertex3f(200,0.01,0);
    glVertex3f(200,0.01,-20);
    glEnd();
    /* road (kanan) 2*/
    glBegin(GL_QUADS);
    glVertex3f(70,0.01,180);
    glVertex3f(70,0.01, 190);
    glVertex3f(200,0.01,190);
    glVertex3f(200,0.01,180);
    glEnd();
    /* road (kiri 1)*/
    glBegin(GL_QUADS);
    glVertex3f(-200,0.01,120);
    glVertex3f(-200,0.01,100);
    glVertex3f(0,0.01,100);
    glVertex3f(0,0.01,120);
    glEnd();
    /* road (kiri 2)*/
    glBegin(GL_QUADS);
    glVertex3f(-90,0.01,-200);
    glVertex3f(-90,0.01,110);
    glVertex3f(-80,0.01,110);
    glVertex3f(-80,0.01,-200);
    glEnd();

    glPopMatrix();

    apartkiri1(-160,0,-180);
    apartkiri2(-160,0,-130);
    apartkiri3(-160,0,-80);
    apartkiri1(-160,0,-30);
    apartkiri2(-160,0,20);
    apartnyamping1(-160,0,140);
    apartnyamping2(-110,0,140);
    apartnyamping3(-60,0,140);
    aparttengah1(-60,0,-180);
    aparttengah3(-60,0,-90);
    aparttengah4(-60,0,20);

    car();
    car2();
    //jalan taman
    glPushMatrix();
    glTranslatef(135,0,-91);
    glScalef(260,0.01,25);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(135,0,-114);
    glScalef(260,0.01,25);
    glColor3f(0.2,0.2,0.2);
    glutSolidCube(0.5);
    glPopMatrix();

    //lampu taman
    glPushMatrix();
    glTranslatef(105,15,-102);
    glScalef(10,300,10);
    glColor3f(0.8,0.8,0.8);
    glutSolidCube(0.1);
    glPopMatrix();
        glPushMatrix();
        glTranslatef(105,30,-102);
        glScalef(6,2,6);
        glColor3f(1,1,0);
        glutSolidSphere(1,3,4);
        glPopMatrix();

    glPushMatrix();
    glTranslatef(125,15,-102);
    glScalef(10,300,10);
    glColor3f(0.8,0.8,0.8);
    glutSolidCube(0.1);
    glPopMatrix();
        glPushMatrix();
        glTranslatef(125,30,-102);
        glScalef(6,2,6);
        glColor3f(1,1,0);
        glutSolidSphere(1,3,4);
        glPopMatrix();
        glPushMatrix();

    glTranslatef(145,15,-102);
    glScalef(10,300,10);
    glColor3f(0.8,0.8,0.8);
    glutSolidCube(0.1);
    glPopMatrix();
        glPushMatrix();
        glTranslatef(145,30,-102);
        glScalef(6,2,6);
        glColor3f(1,1,0);
        glutSolidSphere(1,3,4);
        glPopMatrix();
        glPushMatrix();

    glTranslatef(165,15,-102);
    glScalef(10,300,10);
    glColor3f(0.8,0.8,0.8);
    glutSolidCube(0.1);
    glPopMatrix();
        glPushMatrix();
        glTranslatef(165,30,-102);
        glScalef(6,2,6);
        glColor3f(1,1,0);
        glutSolidSphere(1,3,4);
        glPopMatrix();

    glPushMatrix();
    glTranslatef(185,15,-102);
    glScalef(10,300,10);
    glColor3f(0.8,0.8,0.8);
    glutSolidCube(0.1);
    glPopMatrix();
        glPushMatrix();
        glTranslatef(185,30,-102);
        glScalef(6,2,6);
        glColor3f(1,1,0);
        glutSolidSphere(1,3,4);
        glPopMatrix();




    //kursi taman menyamping
    glPushMatrix();
    glTranslatef(5,5,-50);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,5,-50);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi2();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(55,5,-50);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi3();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5,5,-130);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi4();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,5,-130);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi5();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(55,5,-130);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi6();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5,5,-170);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi7();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(30,5,-170);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi8();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(55,5,-170);
    glScalef(5, 5, 5);
    glRotated(90, 0, 1, 0);
    kursi9();
    glPopMatrix();

    //Dinding taman 1
    glPushMatrix();
    glColor3f(0.35,0.35,0.35);
    glTranslatef(83.7,3,-160.2);
    glScalef(15,12.5,160);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0,0.35,0.0);
    glTranslatef(83.7,8,-160.2);
    glScalef(10,10,160);
    glutSolidCube(0.5);
    glPopMatrix();

    //Dinding taman 2
    glPushMatrix();
    glColor3f(0.35,0.35,0.35);
    glTranslatef(83.7,3,-52.5);
    glScalef(15,12.5,130);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0,0.35,0.0);
    glTranslatef(83.7,8,-52.5);
    glScalef(10,10,130);
    glutSolidCube(0.5);
    glPopMatrix();

    //Dinding taman 3
    glPushMatrix();
    glColor3f(0.35,0.35,0.35);
    glTranslatef(140,3,-23.8);
    glScalef(235,12.5,15);
    glutSolidCube(0.5);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0,0.35,0.0);
    glTranslatef(140,8,-23.8);
    glScalef(230,10,10);
    glutSolidCube(0.5);
    glPopMatrix();

    duagedung();
    lampu_lalu_lintas();
    lampu_lalu_lintas_2();
    lampu_lalu_lintas_3();
    lampu_lalu_lintas_4();
    pohon();

    glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
    // kiri
	case 'a':
	case 'A':
		glTranslatef(5.0, 0.0, 0.0);
		display();
		break;
    // kanan
	case 'd':
	case 'D':
		glTranslatef(-5.0, 0.0, 0.0);
		display();
		break;
    // maju
	case 'w':
	case 'W':
		glTranslatef(0.0, 0.0, 5.0);
		display();
		break;
    // mundur
	case 's':
	case 'S':
		glTranslatef(0.0, 0.0, -5.0);
		display();
		break;
    // rotate
	case 'q':
	case 'Q':
        glRotatef(-2,1.0,0.0,0.0);
		//display();
    case 'e':
    case 'E':
        glRotatef(2,0.0,1.0,0.0);
	}
    display();
}

void resize(int width, int height)
{
	if (height == 0) height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    gluPerspective(120.0, width / height, 1.0, 600.0);

	glTranslatef(0.0, -15.0, -320.0);

	glMatrixMode(GL_MODELVIEW);
}
