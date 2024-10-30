#version 330 core

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

in VS_OUT{
    vec2 tex_coords;
} fs_in;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedospec;
uniform sampler2D ssao;


// LIGHT STRUCTS____________________________________________________________
struct DirLight{
    vec3 direction;
    vec3 diffuse_col;
    vec3 spec_col;
};

struct PointLight{
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

struct SpotLight{
    vec3 direction;
    vec3 position;
    float linear; 
    float quadratic;
    float constant;
    float cut_off;
    float outer_cut_off;  
    vec3 diffuse_col;
    vec3 spec_col;
};

uniform DirLight dir_light;
const int NR_POINT_LIGHTS = 4;
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform SpotLight spot_light;


// SHADOWS______________________________________________________________________
uniform mat4 light_space_matrix_directional;
uniform sampler2D shadow_map_direct;

uniform sampler2D shadow_map_spot;

vec3 sample_offset_directions[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  
uniform float point_map_far_plane;
uniform samplerCube shadow_map_point[NR_POINT_LIGHTS];


// ADDITIONAL___________________________________________________________________
uniform vec3 view_pos;
uniform mat4 view;
uniform mat4 projection;

uniform float shininess;


// LIGHT CASTERS________________________________________________________________________________________________________________________
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, vec3 diffuse, float specular, float shadow);
vec3 CalcSpotLight(SpotLight light, vec3 frag_pos, vec3 normal, vec3 view_dir, vec3 diffuse, float specular, vec3 ambient, float shadow);
vec3 CalcPointLight(vec3 light_pos, vec3 frag_pos, vec3 normal, vec3 view_dir, vec3 diffuse, float specular, 
                    vec3 light_color, vec3 ambient, float radius, float linear, float quadratic, float shadow);


// LIGHTING MAPS_________________________________________________________________________________________________________________________
vec3 CalcDiffuse(vec3 normal, vec3 light_direction, vec3 diffuse_tex, vec3 light_color);
vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, float specular_tex, vec3 light_color);
float CalcAttenuation(float distance, float constant, float linear, float quadratic);


// SHADOWS______________________________________________________________________________________________________________________________
float CalcShadowDir(vec4 frag_pos_light, sampler2D shadow_map, vec3 normal, DirLight light);
float CalcShadowSpot(vec4 frag_pos_light, sampler2D shadow_map, vec3 normal, SpotLight light);
float CalcShadowPoint(PointLight light, samplerCube shadow_map, vec3 frag_pos);


void main(){
    vec3 frag_pos = texture(g_position, fs_in.tex_coords).rgb;
    vec3 normal = normalize(texture(g_normal, fs_in.tex_coords).rgb);
    vec3 diffuse = texture(g_albedospec, fs_in.tex_coords).rgb;
    float specular = texture(g_albedospec, fs_in.tex_coords).a;
    float ambient_occlusion = texture(ssao, fs_in.tex_coords).r;
    vec3 ambient = vec3(0.3 * diffuse * ambient_occlusion);
    //vec3 ambient = vec3(0.3 * diffuse);






    vec3 view_dir = normalize(-frag_pos);
    vec3 lighting = vec3(0.0f);

    // DIR LIGHT_________________________________________________________________________________
    vec4 frag_pos_shadow_dir = light_space_matrix_directional * inverse(view) * vec4(frag_pos,1.0f);
    float shadow_dir = CalcShadowDir(frag_pos_shadow_dir, shadow_map_direct, normal, dir_light);
    lighting += ambient + CalcDirLight(dir_light, normal, view_dir, diffuse, specular, shadow_dir);

    // POINT LIGHTS_________________________________________________________________________________________
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        float shadow_point = CalcShadowPoint(point_lights[i], shadow_map_point[i], frag_pos);
        lighting += CalcPointLight(point_lights[i].position, frag_pos, normal, view_dir, diffuse, specular, point_lights[i].color,
                                   ambient, point_lights[i].radius, point_lights[i].linear, point_lights[i].quadratic, shadow_point);
    }

    // SPOT LIGHT__________________________________________________________________________________
    vec4 frag_pos_shadow_spot = projection * vec4(frag_pos, 1.0f);
    float shadow_spot = CalcShadowSpot(frag_pos_shadow_spot, shadow_map_spot, normal, spot_light);
    lighting += CalcSpotLight(spot_light, frag_pos, normal, view_dir, diffuse, specular, ambient, shadow_spot);

    // OUTPUT______________________________________________________________________________________
    frag_color = vec4(lighting, 1.0);
    bright_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}



vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, vec3 diffuse, float specular, float shadow){
    vec3 light_dir = normalize((view * -vec4(light.direction,0.0f)).xyz);

    vec3 light_diff = CalcDiffuse(normal, light_dir, diffuse, light.diffuse_col);
    vec3 light_spec = CalcSpecular(light_dir, normal, view_dir, shininess, specular, light.spec_col);
    
    return (1.0f - shadow) * light_diff + light_spec;
}

vec3 CalcSpotLight(SpotLight light, vec3 frag_pos, vec3 normal, vec3 view_dir, vec3 diffuse, float specular, vec3 ambient, float shadow){
    vec3 light_dir = normalize((view * vec4(light.position, 1.0f)).xyz-frag_pos); 
    
    vec3 light_diff = CalcDiffuse(normal, light_dir, diffuse,  light.diffuse_col);
    vec3 light_spec = CalcSpecular(light_dir, normal, view_dir, shininess, specular, light.spec_col);
    float light_distance = length((view * vec4(light.position, 1.0f)).xyz-frag_pos); //
    float attenuation = CalcAttenuation(light_distance, light.constant, light.linear, light.quadratic);
    
    // soft edges
    float theta = dot(light_dir, normalize((view * -vec4(light.direction, 0.0f)).xyz)); 
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    
    light_diff *= attenuation * intensity;
    light_spec *= attenuation * intensity;   
    ambient *= attenuation * intensity;
    shadow = (1.0f - shadow);

    return ambient + shadow * (light_diff + light_spec);
}

vec3 CalcPointLight(vec3 light_pos, vec3 frag_pos, vec3 normal, vec3 view_dir, vec3 diffuse, float specular, vec3 light_color, vec3 ambient, float radius, float linear, float quadratic, float shadow){
    float distance = length((view * vec4(light_pos, 1.0f)).xyz - frag_pos);//
    if(distance < radius)
    {
        vec3 light_dir = normalize((view * vec4(light_pos, 1.0f)).xyz - frag_pos);//
    
        vec3 light_diff = CalcDiffuse(normal, light_dir, diffuse, light_color);
        vec3 light_spec = CalcSpecular(light_dir, normal, view_dir, shininess, specular, light_color);
        float attenuation = CalcAttenuation(distance, 1.0, linear, quadratic);
        
        light_diff *= attenuation;
        light_spec *= attenuation; 
        ambient *= attenuation;
        shadow = (1.0f - shadow) * attenuation;
    
        return ambient + shadow * (light_diff + light_spec); 
    }
}


float CalcAttenuation(float distance, float constant, float linear, float quadratic){
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
    return attenuation;
}

vec3 CalcDiffuse(vec3 normal, vec3 light_direction, vec3 diffuse_tex, vec3 light_color){
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = light_color * diff * diffuse_tex;
    return diffuse;
}

vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, float specular_tex, vec3 light_color){
    vec3 halfway_dir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfway_dir),0.0), material_shininess);
    vec3 specular = light_color * spec * specular_tex;
    return specular;
}


float CalcShadowSpot(vec4 frag_pos_light, sampler2D shadow_map, vec3 normal, SpotLight light){
    // gets light-spzce position in range [-1,1]
    vec3 proj_coords = frag_pos_light.xyz / frag_pos_light.w;
    // transforms coordinates to the range [0,1], because depth map is in range [0,1]
    proj_coords = proj_coords * 0.5f + 0.5f;

    float shadow = 0.0f;

    // makes shadows inside depth map range
    if(proj_coords.z <= 1.0)
    {
        // gets depths from the lights perspective
        float closestDepth = texture(shadow_map, proj_coords.xy).r;
        float currentDepth = proj_coords.z;

        // anti shadow acne
        float bias = max(0.0001f * (1.0 - dot(normal, light.direction)), 0.00005f);

        // PCF (gets the average value of texels in a certain radius)
        int sampleRadius = 4;
        vec2 texelSize = 1.0f / textureSize(shadow_map, 0);
        for(int x = -sampleRadius; x <= sampleRadius; ++x){
            for(int y = -sampleRadius; y <= sampleRadius; ++y){
                float pcfDepth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
            }
        }
        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    return shadow;
}

float CalcShadowDir(vec4 frag_pos_light, sampler2D shadow_map, vec3 normal, DirLight light){
    // gets light-spzce position in range [-1,1]
    vec3 proj_coords = frag_pos_light.xyz / frag_pos_light.w;
    // transforms coordinates to the range [0,1], because depth map is in range [0,1]
    proj_coords = proj_coords * 0.5f + 0.5f;

    float shadow = 0.0f;

    // makes shadows inside depth map range
    if(proj_coords.z <= 1.0)
    {
        // gets depths from the lights perspective
        float closestDepth = texture(shadow_map, proj_coords.xy).r;
        float currentDepth = proj_coords.z;

        // anti shadow acne
        float bias = max(0.0001f * (1.0 - dot(normal, light.direction)), 0.00005f);

        // PCF (gets the average value of texels in a certain radius)
        int sampleRadius = 2;
        vec2 texelSize = 1.0f / textureSize(shadow_map, 0);
        for(int x = -sampleRadius; x <= sampleRadius; ++x){
            for(int y = -sampleRadius; y <= sampleRadius; ++y){
                float pcfDepth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
            }
        }
        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    return shadow;
}

float CalcShadowPoint(PointLight light, samplerCube shadow_map, vec3 frag_pos)
{
    // get vector between fragment position and light position
    vec3 frag_to_light = (inverse(view) * vec4(frag_pos, 1.0f)).xyz - light.position;

    float current_depth = length(frag_to_light);

    float shadow = 0.0f;
    float bias = 0.05; 
    float view_distance = length(-frag_pos);
    int samples = 20;
    float disk_radius = (1.0f + (view_distance / point_map_far_plane)) / 25.0f;

    float closest_depth;

    for(int i = 0 ; i < samples; i++){
        closest_depth = texture(shadow_map, frag_to_light + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= point_map_far_plane;
        shadow += current_depth - bias > closest_depth ? 1.0f : 0.0f;
    }

    shadow /= float(samples);

    return shadow;
}