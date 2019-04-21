# version 330 core
layout (location = 0) in vec3 Position ;
layout (location = 1) in vec3 Color;
out vec3 interpolatedColor;

uniform float time;
uniform mat4 T, R, M;

void main () {
   // vec3 addVec = vec3 (0, sin(time), 0);
    //gl_Position = vec4 (Position + addVec , 1.0) ;
    //gl_Position = T * R * vec4(Position, 1.0);
    gl_Position = M * vec4(Position, 1.0);
    interpolatedColor = Color;
}

