#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec3 position;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(0.0, -1.0, -0.5));

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants{
    mat4 model;
    mat4 normal;
} pushConstants;

struct PointLight{
    vec4 position;
    vec4 color;
};

struct SpotLight{
    vec4 position;
    vec4 color;
    vec4 direction;
    float cutOff;
    float outerCutOff;
};

struct DirectionalLight{
    vec4 direction;
    vec4 color;
};


layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 camera_pos;
    vec4 ambient_light;
    DirectionalLight directional_light;
    PointLight point_lights[16];
    SpotLight spot_lights[2];
} ubo;

float START_DECREASE = 1500;
float MAX_DISTANCE = 3000.0;
float gamma = 64.0;
float att = 0.032;

vec3 getDiffuse(vec3 light_dir, vec3 light_color, vec3 normal, float distance){
    float attenuation = 1.0 / (att * distance * distance);
    return light_color * max(dot(light_dir, normal), 0.0) * attenuation;
}

vec3 getSpecular(vec3 d, vec3 color, vec3 w, vec3 n){
    vec3 h = normalize(d + w);
    return color * pow(max(dot(n, h), 0.0), gamma);
}

void main() {
    vec3 position_world = (pushConstants.model * vec4(position, 1.0)).xyz;
    vec3 normal = normalize(fragNormalWorld);
    vec3 view_direction = normalize(vec3(ubo.camera_pos.xyz - position_world));

    vec3 total_light = vec3(0.0);

    float diffuse_intensity = max(dot(normal, normalize(ubo.directional_light.direction.xyz)), 0.0);
    vec3 color = ubo.directional_light.color.xyz * ubo.directional_light.color.w;

    vec3 light_dir = normalize(ubo.directional_light.direction.xyz);
    vec3 light_color = ubo.directional_light.color.xyz * ubo.directional_light.color.w;

    vec3 specular_light = getSpecular(light_dir, light_color, view_direction, normal);

    total_light += diffuse_intensity * light_color + specular_light;

    vec3 direction_to_light = ubo.point_lights[0].position.xyz - position_world;
    float distance = length(direction_to_light);

    float dist_sq = distance * distance;

    float fade_out = 1;

    if(!(dist_sq > MAX_DISTANCE)){
        if(dist_sq > START_DECREASE){
            fade_out -= (dist_sq - START_DECREASE) / (MAX_DISTANCE - START_DECREASE);
        }


        vec3 light_dir = normalize(direction_to_light);
        vec3 light_color = ubo.point_lights[0].color.xyz * ubo.point_lights[0].color.w;

        vec3 specular_light = getSpecular(light_dir, light_color, view_direction, normal);
        vec3 diffuse_light = getDiffuse(light_dir, light_color, normal, distance);

        total_light += (diffuse_light + specular_light) * fade_out;
    } 


    vec3 ambient_light = ubo.ambient_light.xyz * min(ubo.ambient_light.w / 4.0, 0.3);
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = texColor * vec4((total_light + ambient_light) * fragColor, 1.0);
} 
