# version 330 core
layout (location = 0) in vec3 Position ;
layout (location = 1) in vec3 Color;

/*LABB 3*/
/*
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec3 interpolatedNormal;
out vec2 st;
*/
out vec3 interpolatedColor;

uniform float time;
uniform mat4 M;

void main () {

    /*LABB 2*/

    gl_Position = M * vec4(Position, 1.0);
    interpolatedColor = Color;

/*
    gl_Postition = M * vec4(Position, 1.0);
    interpolatedNormal = Normal;
    st = TexCoord;
    */
}

