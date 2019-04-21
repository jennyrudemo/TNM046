# version 330 core
out vec4 finalcolor ;
in vec3 interpolatedColor;

uniform float time;
uniform mat4 T;
void main () {
    //vec3 colorChange = vec3(sin(time), sin(time), sin(time));
    //finalcolor = vec4 (interpolatedColor + colorChange, 1.0) ;
    finalcolor = vec4 (interpolatedColor, 1.0) ;
}

