/*
 * A C++ framework for OpenGL programming in TNM046 for MT1 2015.
 *
 * This is a small, limited framework, designed to be easy to use
 * for students in an introductory computer graphics course in
 * the first year of a M Sc curriculum. It uses custom code
 * for some things that are better solved by external libraries
 * like GLEW and GLM, but the emphasis is on simplicity and
 * readability, not generality.
 * For the window management, GLFW 3.x is used for convenience.
 * The framework should work in Windows, MacOS X and Linux.
 * Some Windows-specific stuff for extension loading is still
 * here. GLEW could have been used instead, but for clarity
 * and minimal dependence on other code, we rolled our own extension
 * loading for the things we need. That code is short-circuited on
 * platforms other than Windows. This code is dependent only on
 * the GLFW and OpenGL libraries. OpenGL 3.3 or higher is required.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2013-2015
 * This code is in the public domain.
 */

// File and console I/O for logging and error reporting
#include <iostream>
// Math header for trigonometric functions
//#define _USE_MATH_DEFINES
#include <cmath>

#include <cstdio>

// In MacOS X, tell GLFW to include the modern OpenGL headers.
// Windows does not want this, so we make this Mac-only.
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#endif

// In Linux, tell GLFW to include the modern OpenGL functions.
// Windows does not want this, so we make this Linux-only.
#ifdef __linux__
#define GL_GLEXT_PROTOTYPES
#endif

// GLFW 3.x, to handle the OpenGL window
#include <GLFW/glfw3.h>

#include "Utilities.hpp"
#include "Shader.hpp"
#include "TriangleSoup.hpp"
#include "Texture.hpp"
#include "Rotator.hpp"

//FUNCTION DECLERATION//
void createVertexBuffer(int location, int dimensions, const float *data, int datasize);

void createIndexBuffer(const unsigned int *data, int datasize);

void mat4mult(float M1[], float M2[], float Mout[]);

void mat4print ( float M []);

void mat4identity(float M[]);

void mat4rotx(float M[], float angle);

void mat4roty(float M[], float angle);

void mat4rotz(float M[], float angle);

void mat4scale(float M[], float scale);

void mat4translate(float M[], float x, float y, float z);

void mat4perspective(float M[], float vfov, float aspect, float znear, float zfar);

/*
 * main(argc, argv) - the standard C++ entry point for the program
 */
int main(int argc, char *argv[]) {
    using namespace std;

    int width, height;
	float time;

	Texture myTexture, sphereTexture;
	KeyRotator myKeyRotator;
	MouseRotator myMouseRotator;

	GLint location_time, location_M, location_R, location_MV, location_P, location_tex;

    const GLFWvidmode *vidmode;  // GLFW struct to hold information about the display
	GLFWwindow *window;    // GLFW struct to hold information about the window

	Shader myShader;
	TriangleSoup myShape;
    TriangleSoup mySphere;

	// Vertex coordinates (x,y,z) for three vertices
    GLuint vertexArrayID, vertexBufferID, indexBufferID, colorBufferIS;



    /*Matrices*/

    //GLfloat M[16]; // final matrix
    GLfloat R[16]; //final rotation matrix
    GLfloat T[16]; // translation matrix
    GLfloat R1[16]; // rotation matrix Orbit
    GLfloat R2[16]; // rotation matrix around y axis own axis
    GLfloat Rx[16]; // rotation depending on key rotation
    GLfloat Ry[16]; // rotation depending on mouse rotation
    GLfloat S[16]; // scaling matrix
    GLfloat V[16]; // rotation of viewpoint
    GLfloat MV[16];
    GLfloat P[16];
    GLfloat T2[16];


    // Initialise GLFW
    glfwInit();

    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// Make sure we are getting a GL context of at least version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Exclude old legacy cruft from the context. We don't need it, and we don't want it.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open a square window (aspect 1:1) to fill half the screen height
    window = glfwCreateWindow(vidmode->height/2, vidmode->height/2, "GLprimer", NULL, NULL);
    if (!window)
    {
        cout << "Unable to open window. Terminating." << endl;
        glfwTerminate(); // No window was opened, so we can't continue in any useful way
        return -1;
    }

    // Make the newly created window the "current context" for OpenGL
    // (This step is strictly required, or things will simply not work)
    glfwMakeContextCurrent(window);

    // Load extensions (only needed in Microsoft Windows)
    Utilities::loadExtensions();

    myShader.createShader("vertex.glsl", "fragment.glsl");

    //glUniformMatrix4fv(location_M, 1, GL_FALSE, M); //Copy the value

    // Show some useful information on the GL context
    cout << "GL vendor:       " << glGetString(GL_VENDOR) << endl;
    cout << "GL renderer:     " << glGetString(GL_RENDERER) << endl;
    cout << "GL version:      " << glGetString(GL_VERSION) << endl;
    cout << "Desktop size:    " << vidmode->width << "x" << vidmode->height << " pixels" << endl;



    glfwSwapInterval(0); // Do not wait for screen refresh between frames

    location_time = glGetUniformLocation(myShader.programID, "time");
    //location_M = glGetUniformLocation(myShader.programID, "M");
    location_R = glGetUniformLocation(myShader.programID, "R");
    location_MV = glGetUniformLocation(myShader.programID, "MV");
    location_P = glGetUniformLocation(myShader.programID, "P");
    location_tex = glGetUniformLocation(myShader.programID, "tex"); // Locate the sampler2D uniform in the shader program

    glUseProgram(myShader.programID); //Activate the shader to set its variable

    //If the variable is not found, -1 is returned
    if(location_time == -1){
        cout << "Unable to locate variable 'time' in shader!" << endl;
    }

    myKeyRotator.init(window);
    myMouseRotator.init(window);


    mySphere.createSphere(1.0, 200);
    //myShape.createBox(1.0,1.0,1.0);
    myShape.readOBJ("meshes/trex.obj");
    // Generate one texture object with data from a TGA file
    myTexture.createTexture("textures/trex.tga");
    sphereTexture.createTexture("textures/earth.tga");

    glEnable(GL_DEPTH_TEST);

    // Main loop
    while(!glfwWindowShouldClose(window))
    {
        // Get window size. It may start out different from the requested
        // size, and will change if the user resizes the window.
        glfwGetWindowSize( window, &width, &height );
        // Set viewport. This is the pixel rectangle we want to draw into.
        glViewport( 0, 0, width, height ); // The entire window

		// Set the clear color and depth, and clear the buffers for drawing
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Utilities::displayFPS(window);

        /* ---- Rendering code should go here ---- */
        time = (float)glfwGetTime(); //Number of seconds since the program was started

        glUseProgram(myShader.programID);//Activate the shader to set its variables

        glBindTexture(GL_TEXTURE_2D, myTexture.textureID);

        glUniform1f(location_time, time); //Copy the value to the shade program
        glUniform1i(location_tex, 0);

        myKeyRotator.poll(window);
        myMouseRotator.poll(window);

        //mat4identity(M); // initializing M to identity matrix
        mat4identity(R);
        mat4identity(P);
        mat4identity(MV);


        mat4rotx(Rx, myMouseRotator.theta);
        mat4roty(Ry, myMouseRotator.phi);
        mat4mult(Rx,Ry,R);


        mat4rotx(Rx, -myKeyRotator.theta);
        mat4roty(Ry, myKeyRotator.phi);

        mat4scale(S, 0.5); //setting scaler
        mat4rotx(V, M_PI/10); // view point angle
        mat4translate(T, 0.0, 0.0, -3.0);
        mat4roty(R1, time*M_PI/8); //Orbit rotation
        mat4roty(R2, time*M_PI/2);
        mat4perspective(P, M_PI/4, 1, 0.1, 100.0);

        mat4mult(T, Rx, MV);//Rotation around y-axis
        mat4mult(MV, Ry, MV);
        mat4mult(MV, V, MV);


        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //glUniformMatrix4fv(location_M, 1, GL_FALSE, M); //Copy the value
        glUniformMatrix4fv(location_R, 1, GL_FALSE, R); //Copy the value
        glUniformMatrix4fv(location_MV, 1, GL_FALSE, MV); //Copy the value
        glUniformMatrix4fv(location_P, 1, GL_FALSE, P); //Copy the value


        myShape.render();
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, sphereTexture.textureID);
        glUniform1i(location_tex, 0);

        mat4identity(MV);
        mat4identity(T2);

        mat4scale(S, 0.2); //setting scaler
        mat4rotx(V, M_PI/10); // view point angle
        mat4translate(T, 0.0, 0.0, -3.0);
        mat4roty(R1, time*M_PI/3); //Orbit rotation
        mat4translate(T2, 1.0, 0.0, 0.0);

        mat4mult(T, V, MV);//Rotation around y-axis
        mat4mult(MV,R1,MV);
        //mat4mult(MV, R1, MV);
        mat4mult(MV, T2, MV);
        mat4mult(MV,R2,MV);
        mat4mult(MV, S, MV);

        //glUniformMatrix4fv(location_R, 1, GL_FALSE, R); //Copy the value
        glUniformMatrix4fv(location_MV, 1, GL_FALSE, MV); //Copy the value
        //glUniformMatrix4fv(location_P, 1, GL_FALSE, P); //Copy the value

        mySphere.render();


        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

		// Swap buffers, i.e. display the image and prepare for next frame.
        glfwSwapBuffers(window);

		// Poll events (read keyboard and mouse input)
		glfwPollEvents();

        // Exit if the ESC key is pressed (and also if the window is closed).
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
          glfwSetWindowShouldClose(window, GL_TRUE);
        }

    }

    // Close the OpenGL window and terminate GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


//FUNCTIONS//
void createVertexBuffer(int location, int dimensions, const float *data, int datasize)
{

	GLuint bufferID;

	// Generate buffer, activate it and copy the data
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, datasize, data, GL_STATIC_DRAW);
	// Tell OpenGL how the data is stored in our buffer
	// Attribute location (must match layout(location=#) statement in shader)
	// Number of dimensions (3 -> vec3 in the shader, 2-> vec2 in the shader),
	// type GL_FLOAT, not normalized, stride 0, start at element 0
	glVertexAttribPointer(location, dimensions, GL_FLOAT, GL_FALSE, 0, NULL);
	// Enable the attribute in the currently bound VAO
	glEnableVertexAttribArray(location);
}

void createIndexBuffer(const unsigned int *data, int datasize) {

	GLuint bufferID;

	// Generate buffer, activate it and copy the data
	glGenBuffers(1, &bufferID);
    // Activate (bind) the index buffer and copy data to it.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, datasize, data, GL_STATIC_DRAW);
}

void mat4mult(float M1[], float M2[], float Mout[]){
    float Mtemp[16];

    Mtemp[0] = M2[0]*M1[0] + M2[1]* M1[4] + M2[2]*M1[8] + M2[3]* M1[12];
    Mtemp[1] = M2[0]*M1[1] + M2[1]* M1[5] + M2[2]*M1[9] + M2[3]* M1[13];
    Mtemp[2] = M2[0]*M1[2] + M2[1]* M1[6] + M2[2]*M1[10] + M2[3]* M1[14];
    Mtemp[3] = M2[0]*M1[3] + M2[1]* M1[7] + M2[2]*M1[11] + M2[3]* M1[15];

    Mtemp[4] = M2[4]*M1[0] + M2[5]* M1[4] + M2[6]*M1[8] + M2[7]* M1[12];
    Mtemp[5] = M2[4]*M1[1] + M2[5]* M1[5] + M2[6]*M1[9] + M2[7]* M1[13];
    Mtemp[6] = M2[4]*M1[2] + M2[5]* M1[6] + M2[6]*M1[10] + M2[7]* M1[14];
    Mtemp[7] = M2[4]*M1[3] + M2[5]* M1[7] + M2[6]*M1[11] + M2[7]* M1[15];

    Mtemp[8] = M2[8]*M1[0] + M2[9]* M1[4] + M2[10]*M1[8] + M2[11]* M1[12];
    Mtemp[9] = M2[8]*M1[1] + M2[9]* M1[5] + M2[10]*M1[9] + M2[11]* M1[13];
    Mtemp[10] = M2[8]*M1[2] + M2[9]* M1[6] + M2[10]*M1[10] + M2[11]* M1[14];
    Mtemp[11] = M2[8]*M1[3] + M2[9]* M1[7] + M2[10]*M1[11] + M2[11]* M1[15];

    Mtemp[12] = M2[12]*M1[0] + M2[13]* M1[4] + M2[14]*M1[8] + M2[15]* M1[12];
    Mtemp[13] = M2[12]*M1[1] + M2[13]* M1[5] + M2[14]*M1[9] + M2[15]* M1[13];
    Mtemp[14] = M2[12]*M1[2] + M2[13]* M1[6] + M2[14]*M1[10] + M2[15]* M1[14];
    Mtemp[15] = M2[12]*M1[3] + M2[13]* M1[7] + M2[14]*M1[11] + M2[15]* M1[15];



    for(int i = 0; i<16; i++){
        Mout[i] = Mtemp[i];
    }
}

// Print the elements of a matrix M
void mat4print ( float M []) {
    printf (" Matrix :\n");
    printf (" %6.2f %6.2f %6.2f %6.2f\n", M[0] , M[4] , M[8] , M[12]) ;
    printf (" %6.2f %6.2f %6.2f %6.2f\n", M[1] , M[5] , M[9] , M[13]) ;
    printf (" %6.2f %6.2f %6.2f %6.2f\n", M[2] , M[6] , M[10] , M[14]) ;
    printf (" %6.2f %6.2f %6.2f %6.2f\n", M[3] , M[7] , M[11] , M[15]) ;
    printf ("\n") ;
}

void mat4identity(float M[]) {

    M[0] = 1.0f;
    M[1] = 0.0f;
    M[2] = 0.0f;
    M[3] = 0.0f;

    M[4] = 0.0f;
    M[5] = 1.0f;
    M[6] = 0.0f;
    M[7] = 0.0f;

    M[8] = 0.0f;
    M[9] = 0.0f;
    M[10] = 1.0f;
    M[11] = 0.0f;

    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = 0.0f;
    M[15] = 1.0f;
}

void mat4rotx(float M[], float angle) {

    M[0] = 1.0f;
    M[1] = 0.0f;
    M[2] = 0.0f;
    M[3] = 0.0f;

    M[4] = 0.0f;
    M[5] = cos(angle);
    M[6] = sin(angle);
    M[7] = 0.0f;

    M[8] = 0.0f;
    M[9] = -sin(angle);
    M[10] = cos(angle);
    M[11] = 0.0f;

    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = 0.0f;
    M[15] = 1.0f;
}

void mat4roty(float M[], float angle) {

    M[0] = cos(angle);
    M[1] = 0.0f;
    M[2] = -sin(angle);
    M[3] = 0.0f;

    M[4] = 0.0f;
    M[5] = 1.0f;
    M[6] = 0.0f;
    M[7] = 0.0f;

    M[8] = sin(angle);
    M[9] = 0.0f;
    M[10] = cos(angle);
    M[11] = 0.0f;

    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = 0.0f;
    M[15] = 1.0f;
}

void mat4rotz(float M[], float angle) {

    M[0] = cos(angle);
    M[1] = sin(angle);
    M[2] = 0.0f;
    M[3] = 0.0f;

    M[4] = -sin(angle);
    M[5] = cos(angle);
    M[6] = 0.0f;
    M[7] = 0.0f;

    M[8] = 0.0f;
    M[9] = 0.0f;
    M[10] = 1.0f;
    M[11] = 0.0f;

    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = 0.0f;
    M[15] = 1.0f;
}

void mat4scale(float M[], float scale){

    M[0] = scale;
    M[1] = 0.0f;
    M[2] = 0.0f;
    M[3] = 0.0f;

    M[4] = 0.0f;
    M[5] = scale;
    M[6] = 0.0f;
    M[7] = 0.0f;

    M[8] = 0.0f;
    M[9] = 0.0f;
    M[10] = scale;
    M[11] = 0.0f;

    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = 0.0f;
    M[15] = 1.0f;
}

void mat4translate(float M[], float x, float y, float z){

    M[0] = 1.0f;
    M[1] = 0.0f;
    M[2] = 0.0f;
    M[3] = 0.0f;

    M[4] = 0.0f;
    M[5] = 1.0f;
    M[6] = 0.0f;
    M[7] = 0.0f;

    M[8] = 0.0f;
    M[9] = 0.0f;
    M[10] = 1.0f;
    M[11] = 0.0f;

    M[12] = x;
    M[13] = y;
    M[14] = z;
    M[15] = 1.0f;
}

// M is the matrix we want to create (an output argument )
// vfov is the vertical field of view (in the y direction )
// aspect is the aspect ratio of the viewport ( width / height )
// znear is the distance to the near clip plane ( znear > 0)
// zfar is the distance to the far clip plane ( zfar > znear
void mat4perspective(float M[], float vfov, float aspect, float znear, float zfar){
    float f = cos(vfov/2)/sin(vfov/2);

    M[0] = f/aspect;
    M[1] = 0.0f;
    M[2] = 0.0f;
    M[3] = 0.0f;

    M[4] = 0.0f;
    M[5] = f;
    M[6] = 0.0f;
    M[7] = 0.0f;

    M[8] = 0.0f;
    M[9] = 0.0f;
    M[10] = -(zfar + znear)/(zfar-znear);
    M[11] = -1.0f;

    M[12] = 0.0f;
    M[13] = 0.0f;
    M[14] = -(2*znear*zfar)/(zfar-znear);
    M[15] = 0.0f;
}

