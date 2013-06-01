#version 330 core

in vec3 normal; // vertex normal
in vec3 eyeVec;
//comment this
in vec3 pos;
//until this
uniform vec3 lightDir;

const vec4 material_ambient = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 light_ambient = vec4(0.0, 0.0, 0.0, 1.0);

const vec4 material_diffuse = vec4(0.6, 0.3, 0.3, 1.0);
const vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);

const vec4 material_specular = vec4(1.0);
const vec4 light_specular = vec4(1.0);
const float shininess = 5.0;

out vec4 gl_Color;

void main()
{

	// Output color = red 
    vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = max(dot(N,L), 0.0);

    vec3 E = normalize(eyeVec);
	vec3 R = reflect(-L, N);
	float specular = pow(max(dot(R, E), 0.0), shininess );

    vec4 diffuse = material_diffuse;

    //comment next lines

    vec3 pos_frac = fract(pos*6.0) - vec3(0.5);
    if (length(pos_frac) > 0.5) {
        diffuse = vec4(0.8, 0.1, 0.1, 1.0);
    }

	gl_Color = material_ambient*light_ambient + diffuse*light_diffuse*lambertTerm + material_specular*light_specular*specular;

}