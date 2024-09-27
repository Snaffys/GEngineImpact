#version 330 core

// output
layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

// material structure
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
    sampler2D normal_map1;
    sampler2D displacement_map1;
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

in VS_OUT{
	vec3 frag_pos_tbn;
	vec4 frag_pos_light;
    vec4 frag_pos_light_spot;
    mat3 TBN;
    vec3 frag_pos;
	vec3 normal;
	vec2 tex_coords;
} fs_in;

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

// structures objects
uniform Material material;


uniform bool normal_map;
uniform bool parallax_map;


uniform float height_scale;

uniform sampler2D shadow_map_direct;
uniform sampler2D shadow_map_spot;
uniform samplerCube shadow_map_point[NR_POINT_LIGHTS];



// functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, float shadow, mat3 TBN, vec2 tex_coords);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow, mat3 TBN, vec2 tex_coords);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow, mat3 TBN, vec2 tex_coords);

float CalcAttenuation(vec3 position, vec3 frag_pos, float constant, float linear, float quadratic);

vec3 CalcDiffuse(vec3 normal, vec3 light_direction, sampler2D material_diffuse, vec3 diffuse, vec2 tex_coords);
vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, sampler2D material_specular, vec3 light_specular, vec2 tex_coords);

float CalcShadow(vec4 frag_pos_light, sampler2D shadow_map, vec3 normal);

float CalcShadowPoint(PointLight light, samplerCube shadow_map, vec3 fragPos);

vec2 ParallaxMapping(vec2 tex_coords, vec3 view_dir);




void main()
{
    vec3 norm;
    vec3 view_direction;
    vec3 fragment_position;
    vec2 texCoords;
    mat3 TBN;

    if(normal_map){
        vec3 viewPos = fs_in.TBN * view_pos;
        view_direction = normalize(viewPos - fs_in.frag_pos_tbn);
        TBN = fs_in.TBN;

        if(parallax_map){
            texCoords = ParallaxMapping(fs_in.tex_coords, view_direction);
            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
                discard;
            norm = texture(material.normal_map1, texCoords).rgb;
            norm = normalize(norm * 2.0f - 1.0f);
            fragment_position = fs_in.frag_pos_tbn;
        }
        else{
            texCoords = fs_in.tex_coords;
            norm = texture(material.normal_map1, fs_in.tex_coords).rgb;
            norm = normalize(norm * 2.0f - 1.0f);
            fragment_position = fs_in.frag_pos_tbn;
        }
    }
    else{
            view_direction = normalize(view_pos - fs_in.frag_pos);
            texCoords = fs_in.tex_coords;
            fragment_position = fs_in.frag_pos;  
            norm = normalize(fs_in.normal);
            TBN = mat3(1.0f);
    }

    
    float shadow_direct = CalcShadow(fs_in.frag_pos_light, shadow_map_direct, norm);
    // directional light
    vec3 result_color = CalcDirLight(dir_light, norm, view_direction, shadow_direct, TBN, texCoords);
    // point light
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        float shadow_point = CalcShadowPoint(point_lights[i], shadow_map_point[i], fs_in.frag_pos);
        result_color += CalcPointLight(point_lights[i], norm, fragment_position, view_direction, shadow_point, TBN, texCoords); 
    }
    float shadow_spot = CalcShadow(fs_in.frag_pos_light_spot, shadow_map_spot, norm);
    // spot light
    result_color += CalcSpotLight(spot_light, norm, fragment_position, view_direction, shadow_spot, TBN, texCoords);

    // result
    frag_color = vec4(result_color, 1.0);

    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_color = vec4(frag_color.rgb, 1.0);
    else
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 view_dir, float shadow, mat3 TBN, vec2 tex_coords){
    vec3 light_direction = normalize(-(TBN * light.direction));
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse, tex_coords);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular, tex_coords);
    vec3 ambient = texture(material.texture_diffuse1, tex_coords).rgb * light.ambient;
    // result
    return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow, mat3 TBN, vec2 tex_coords){
    vec3 light_direction = normalize(TBN * light.position - frag_pos);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse, tex_coords);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular, tex_coords);
    vec3 ambient = texture(material.texture_diffuse1, tex_coords).rgb * light.ambient;
    // attenuation
    float attenuation = CalcAttenuation(TBN * light.position, frag_pos, light.constant, light.linear, light.quadratic);
    // result
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    shadow = (1.0f - shadow) * attenuation;
    return (ambient + shadow * (diffuse + specular));
}



vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, float shadow, mat3 TBN, vec2 tex_coords){
    vec3 light_direction = normalize(TBN * light.position - frag_pos);
    // lighting maps
    vec3 diffuse = CalcDiffuse(normal, light_direction, material.texture_diffuse1, light.diffuse, tex_coords);
    vec3 specular = CalcSpecular(light_direction, normal, view_dir, material.shininess, material.texture_specular1, light.specular, tex_coords);
    vec3 ambient = texture(material.texture_diffuse1, tex_coords).rgb * light.ambient;
    // attenuation
    float attenuation = CalcAttenuation(TBN * light.position, frag_pos, light.constant, light.linear, light.quadratic);
    // spotlight (soft edges)
    float theta = dot(light_direction, normalize(TBN * - light.direction));
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

vec3 CalcDiffuse(vec3 normal, vec3 light_direction, sampler2D material_diffuse, vec3 light_diffuse, vec2 tex_coords){
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_diffuse * texture(material_diffuse, tex_coords).rgb;
    return diffuse;
}

vec3 CalcSpecular(vec3 light_direction, vec3 normal, vec3 view_dir, float material_shininess, sampler2D material_specular, vec3 light_specular, vec2 tex_coords){
    vec3 halfway_dir = normalize(light_direction + view_dir);
    float spec = pow(max(dot(normal, halfway_dir),0.0), material_shininess);
    vec3 specular = spec * light_specular * texture(material_specular, tex_coords).rgb;
    return specular;
}





float CalcShadow(vec4 frag_pos_light, sampler2D shadow_map, vec3 normal){
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
        float bias = max(0.0001f * (1.0 - dot(normal, dir_light.direction)), 0.00005f);

        // PCF (gets the average value of texels in a certain radius)
        int sampleRadius = 5;
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


vec2 ParallaxMapping(vec2 tex_coords, vec3 view_dir){
    // looking straight on the surface
    const float min_layers = 8.0f;
    // looking on the surface from an angle
    const float max_layers = 32.0f;
    float layers_amount = mix(max_layers, min_layers, max(dot(vec3(0.0f, 0.0f, 1.0f), view_dir),0.0f));
    float layer_depth = 1.0f/ layers_amount;
    float current_layer_depth = 0.0f;
    
    vec2 p = view_dir.xy * height_scale;
    vec2 delta_tex_coords = p / layers_amount;

    vec2 current_tex_coords = tex_coords;
    float current_depth_map_value = texture(material.displacement_map1, current_tex_coords).r;

    // finds proper layer depth
    while(current_layer_depth < current_depth_map_value){
        current_tex_coords -= delta_tex_coords;
        current_depth_map_value = texture(material.displacement_map1, current_tex_coords).r;
        current_layer_depth += layer_depth;
    }

    vec2 prev_tex_coords = current_tex_coords + delta_tex_coords;

    float after_depth = current_depth_map_value - current_layer_depth;
    float before_depth = texture(material.displacement_map1, prev_tex_coords).r - current_layer_depth + layer_depth;

    float interpolation = after_depth / (after_depth - before_depth);

    vec2 final_tex_coords = prev_tex_coords * interpolation  + current_tex_coords * (1.0f - interpolation);

    return final_tex_coords;
}
