# version 330 core
out vec4 finalcolor ;
//in vec3 interpolatedColor;

in vec3 interpolatedNormal;
in vec2 st;
in vec3 lightDirection;

uniform float time;
uniform mat4 M, R;
uniform sampler2D tex;
void main () {
    //finalcolor = vec4 (interpolatedColor, 1.0) ;
    //float shading = dot(interpolatedNormal, normalize(lightDirection));
    //shading = max(0.0, shading);

    /* LABB 4

    vec3 V = vec3(0.0, 0.0, 1.0);
    V = normalize(V);
    vec3 L = normalize(lightDirection);
    vec3 N = normalize(interpolatedNormal);

    vec3 Ia = vec3(0.5, 0.5, 0.5);
    vec3 ka = vec3(0.5, 0.0, 0.0);
    vec3 Id = vec3(1.0, 1.0, 1.0);
    vec3 kd = vec3(0.5, 0.0, 0.0);
    vec3 Is = vec3(1.0, 1.0, 1.0);
    vec3 ks = vec3(1.0, 1.0, 1.0);

    float n = 10.0;


    // vec3 L is the light direction
    // vec3 V is the view direction - (0,0,1) in view space
    // vec3 N is the normal
    // vec3 Ref is the computed reflection direction
    // float n is the "shininess" parameter
    // vec3 ka is the ambient  reflection color
    // vec3 Ia is the ambient illumination color
    // vec3 kd is the diffuse surface reflection color
    // vec3 Id is the diffuse illumination color
    // vec3 ks is the specular surface reflection color
    // vec3 Is is the specular illumination color

    //This assumes that N, L and V are normalized
    vec3 Ref = 2.0 * dot(N,L)*N -L; //Could have also used the function reflect()
    float dotNL = max(dot(N,L), 0.0); //If negative, set to zero
    float dotRV = max(dot(Ref,V), 0.0);
    if(dotNL == 0.0) dotRV = 0.0;
    vec3 shadedcolor = Ia*ka + Id*kd *dotNL + Is*ks *pow(dotRV, n);
    finalcolor = vec4 (shadedcolor, 1.0);

    */

    finalcolor = texture(tex, st);
}

