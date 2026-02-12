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
float gamma = 4.0;
float att = 0.032;

float specular_contribution = 0.2;

vec3 getDiffuse(vec3 light_dir, vec3 light_color, vec3 normal, float distance){
    float attenuation = 1.0 / (att * distance * distance);
    return light_color * max(dot(light_dir, normal), 0.0) * attenuation;
}

vec3 getSpecular(vec3 d, vec3 color, vec3 w, vec3 n){
    vec3 h = normalize(d + w);
    return color * pow(max(dot(n, h), 0.0), gamma);
}

void main() {
    
    outColor = vec4(0, 0, 0, 0);
} 
