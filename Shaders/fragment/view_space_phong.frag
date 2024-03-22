#version 330 core

in vec3 normal;         // gets normal from vertex shader
in vec3 frag_pos;
in vec3 light_p;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 object_color;
uniform vec3 light_color;

out vec4 frag_color;	// retutns color

void main()
{
    // ambient lightning
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

    // diffuse lightning
    vec3 norm = normalize(normal);    
    vec3 light_direction = normalize(light_p - frag_pos);
	float diff = max(dot(norm,light_direction), 0.0);
	vec3 diffuse = diff * light_color;

    // specular lightning
    float specular_strength = 1.5;
    vec3 view_direction = normalize(-frag_pos);
    vec3 reflect_direction = reflect(-light_direction, norm);   // "-" because light_direction points from the fragment's position
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 64);
    vec3 specular = specular_strength * spec * light_color;

    // result
    vec3 result_color = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(result_color, 1.0);
}