#version 330 core

// material structure
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

// light structures
struct DirLight{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight{
    vec3 position;
    vec3 direction;

    float cut_off;
    float outer_cut_off;  

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// inputs from vertex shader
in VS_OUT{
    //vec3 vert_color;		
    //vec2 tex_coords;    
    vec3 normal;        
    vec3 frag_pos;
    vec4 frag_pos_light;
} fs_in; 

in GS_OUT{
    vec2 tex_coords;
    vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light;
} fs_in_g;

#define NR_POINT_LIGHTS 4

// structures objects
uniform Material material;
uniform DirLight dir_light;
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform SpotLight spot_light;

// textures
uniform sampler2D texture1;
uniform sampler2D texture2;

// view position
uniform vec3 view_pos;

uniform sampler2D shadowMap;

// output
out vec4 frag_color;

// functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

float CalcAttenuation(vec3 position, vec3 frag_pos, float constant, float linear, float quadratic);

vec3 CalcDiffuse(vec3 normal, vec3 light_direction, sampler2D material_diffuse, vec3 diffuse);
vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, sampler2D material_specular, vec3 light_specular);

float CalcShadow();

void main()
{
    vec3 norm = normalize(fs_in_g.normal);
    vec3 view_direction = normalize(view_pos - fs_in_g.frag_pos);

    float shadow = CalcShadow();

    // directional light
    vec3 result_color = CalcDirLight(dir_light, norm, view_direction, shadow);
    // point light
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result_color += CalcPointLight(point_lights[i], norm, fs_in_g.frag_pos, view_direction); 
    // spot light
    result_color += CalcSpotLight(spot_light, norm, fs_in_g.frag_pos, view_direction);

    // result
    frag_color = vec4(result_color, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, float shadow){
    vec3 light_direction = normalize(-light.direction);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular);
    vec3 ambient = texture(material.texture_diffuse1, fs_in_g.tex_coords).rgb * light.ambient;
    // result
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir){
    vec3 light_direction = normalize(light.position - frag_pos);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular);
    vec3 ambient = texture(material.texture_diffuse1, fs_in_g.tex_coords).rgb * light.ambient;
    // attenuation
    float attenuation = CalcAttenuation(light.position, frag_pos, light.constant, light.linear, light. quadratic);
    // result
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir){
    vec3 light_direction = normalize(light.position - frag_pos);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular);
    vec3 ambient = texture(material.texture_diffuse1, fs_in_g.tex_coords).rgb * light.ambient;
    // attenuation
    float attenuation = CalcAttenuation(light.position, frag_pos, light.constant, light.linear, light. quadratic);
    // spotlight (soft edges)
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    // result
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;    
    return (ambient + diffuse + specular);
}

float CalcAttenuation(vec3 position, vec3 frag_pos, float constant, float linear, float quadratic){
    float distance = length(position - frag_pos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
    return attenuation;
}

vec3 CalcDiffuse(vec3 normal, vec3 light_direction, sampler2D material_diffuse, vec3 light_diffuse){
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_diffuse * texture(material_diffuse, fs_in_g.tex_coords).rgb;
    return diffuse;
}

vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, sampler2D material_specular, vec3 light_specular){
    vec3 halfway_dir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfway_dir),0.0), material_shininess);
    vec3 specular = spec * light_specular * texture(material_specular, fs_in_g.tex_coords).rgb;
    return specular;
}


float CalcShadow(){
    // gets coords
    vec3 proj_coords = fs_in_g.frag_pos_light.xyz / fs_in_g.frag_pos_light.w;
    proj_coords = proj_coords * 0.5f + 0.5f;

    float shadow = 0.0f;

    // makes shadows inside depth map range
    if(proj_coords.z <= 1.0f){
        float closestDepth = texture(shadowMap, proj_coords.xy).r;
        float currentDepth = proj_coords.z;

        // anti shadow acne
        float bias = max(0.0001f * (1.0 - dot(fs_in_g.normal, dir_light.direction)), 0.00001f);

        int sampleRadius = 1;

        vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
        for(int x = -sampleRadius; x <= sampleRadius; ++x){
            for(int y = -sampleRadius; y <= sampleRadius; ++y){
                float pcfDepth = texture(shadowMap, proj_coords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
            }
        }
        //shadow /= 9.0f;
        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    return shadow;
}