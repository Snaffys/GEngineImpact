#version 330 core

// output
layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

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
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct SpotLight{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    
    float cut_off;
    float outer_cut_off;  
};

in GS_OUT{
    vec2 tex_coords;
    vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light;
    vec4 frag_pos_light_spot;
} fs_in_g;

#define NR_POINT_LIGHTS 4


vec3 sample_offset_directions[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

layout (std140) uniform lightData{
    // view position
    vec3 view_pos;

    
    DirLight dir_light;
    PointLight point_lights[NR_POINT_LIGHTS];
    SpotLight spot_light;

    float point_map_far_plane;
};

uniform Material material;

// textures

uniform sampler2D shadow_map_direct;
uniform sampler2D shadow_map_spot;
uniform samplerCube shadow_map_point[NR_POINT_LIGHTS];




// functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow);

float CalcAttenuation(vec3 position, vec3 frag_pos, float constant, float linear, float quadratic);

vec3 CalcDiffuse(vec3 normal, vec3 light_direction, sampler2D material_diffuse, vec3 diffuse);
vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, sampler2D material_specular, vec3 light_specular);

float CalcShadow(vec4 frag_pos_light, sampler2D shadow_map);

float CalcShadowPoint(PointLight light, samplerCube shadow_map, vec3 fragPos);

void main()
{
    vec3 norm = normalize(fs_in_g.normal);
    vec3 view_direction = normalize(view_pos - fs_in_g.frag_pos);

    float shadow_direct = CalcShadow(fs_in_g.frag_pos_light, shadow_map_direct);
    // directional light
    vec3 result_color = CalcDirLight(dir_light, norm, view_direction, shadow_direct);
    // point light



    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        float shadow_point = CalcShadowPoint(point_lights[i], shadow_map_point[i], fs_in_g.frag_pos);
        result_color += CalcPointLight(point_lights[i], norm, fs_in_g.frag_pos, view_direction, shadow_point); 
    }
    float shadow_spot = CalcShadow(fs_in_g.frag_pos_light_spot, shadow_map_spot);
    // spot light
    result_color += CalcSpotLight(spot_light, norm, fs_in_g.frag_pos, view_direction, shadow_spot);

    // result
    frag_color = vec4(result_color, 1.0);

    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_color = vec4(frag_color.rgb, 1.0);
    else
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow){
    vec3 light_direction = normalize(light.position - frag_pos);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular);
    vec3 ambient = texture(material.texture_diffuse1, fs_in_g.tex_coords).rgb * light.ambient;
    // attenuation
    float attenuation = CalcAttenuation(light.position, frag_pos, light.constant, light.linear, light.quadratic);
    // result
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    shadow = (1.0f - shadow) * attenuation;
    return (ambient + shadow * (diffuse + specular));
}



vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow){
    vec3 light_direction = normalize(light.position - frag_pos);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular);
    vec3 ambient = texture(material.texture_diffuse1, fs_in_g.tex_coords).rgb * light.ambient;
    // attenuation
    float attenuation = CalcAttenuation(light.position, frag_pos, light.constant, light.linear, light.quadratic);
    // spotlight (soft edges)
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    // result
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;   
    shadow = (1.0f - shadow);// * attenuation * intensity;
    return (ambient + shadow * (diffuse + specular));
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





float CalcShadow(vec4 frag_pos_light, sampler2D shadow_map){
    // gets light-spzce position in range [-1,1]
    vec3 proj_coords = frag_pos_light.xyz / frag_pos_light.w;
    // transforms coordinates to the range [0,1], because depth map is in range [0,1]
    proj_coords = proj_coords * 0.5f + 0.5f;

    float shadow = 0.0f;

    // makes shadows inside depth map range
    if(proj_coords.z <= 1.0f){
        // gets depths from the lights perspective
        float closestDepth = texture(shadow_map, proj_coords.xy).r;
        float currentDepth = proj_coords.z;

        // anti shadow acne
        float bias = max(0.0001f * (1.0 - dot(fs_in_g.normal, dir_light.direction)), 0.00005f);

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


float CalcShadowPoint(PointLight light, samplerCube shadow_map, vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 frag_to_light = fragPos - light.position;

    float current_depth = length(frag_to_light);

    float shadow = 0.0f;
    float bias = 0.05; 
    float view_distance = length(view_pos - fragPos);
    int samples = 20;
    float disk_radius = (1.0f + (view_distance / point_map_far_plane)) / 25.0f;
    //float disk_radius = 0.05f;

    for(int i = 0 ; i < samples; i++){
        float closest_depth = texture(shadow_map, frag_to_light + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= point_map_far_plane;
        shadow += current_depth - bias > closest_depth ? 1.0f : 0.0f;
    }

    shadow /= float(samples);

    return shadow;
}
