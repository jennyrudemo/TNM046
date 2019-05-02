# version 330 core
out vec4 finalcolor ;
//in vec3 interpolatedColor;

in vec3 interpolatedNormal;
in vec2 st;

uniform float time;
uniform mat4 M;
void main () {
    //finalcolor = vec4 (interpolatedColor, 1.0) ;

    finalcolor = vec4 (interpolatedNormal, 1.0);
}

