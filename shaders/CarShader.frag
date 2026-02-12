#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec3 position;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
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

float mu = 0.1;
float mu_2 = mu * mu;
float F0 = 0.91;
float k = 0.3;

float att = 0.032;
float START_DECREASE = 1500.0;
float MAX_DISTANCE = 3000.0;

// Mix betwenn ggx and Schlick-Beckmann with IBL
float ggx(vec3 n, vec3 v) {
    float dotNV = max(dot(n, v), 0.0);
    float k = mu_2 / 2.0;
    return dotNV / (dotNV * (1.0 - k) + k);
}


vec3 getDiffuse(vec3 light_dir, vec3 light_color, vec3 normal, float distance){
    float attenuation = 1.0 / (att * distance * distance);
    return light_color * max(dot(light_dir, normal), 0.0) * attenuation;
}

vec3 getSpecular(vec3 light_dir, vec3 light_color, vec3 normal, vec3 view_direction){
    vec3 half_dir = normalize(light_dir + view_direction);

    float D = mu_2 / (3.14 * pow(pow(dot(normal, half_dir), 2) * (mu_2 - 1.0) + 1.0, 2));

    float F = F0 + (1.0 - F0) * pow(1.0 - max(dot(view_direction, half_dir), 0.0), 5);

    float G = ggx(normal, view_direction) * ggx(normal, light_dir);

    return light_color * D * F * G / (4 * max(dot(view_direction, normal), 0.0));

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

    vec3 specular_light = getSpecular(light_dir, light_color, normal, view_direction);

    total_light += k * diffuse_intensity * light_color + (1 - k) * specular_light;

    int i;


    for (i = 0; i < 2; i++) {
        vec3 direction_to_light = ubo.point_lights[i].position.xyz - position_world;
        float distance = length(direction_to_light);

        float dist_sq = distance * distance;

        float fade_out = 1;

        if(dist_sq > MAX_DISTANCE) continue;

        if(dist_sq > START_DECREASE){
            fade_out -= (dist_sq - START_DECREASE) / (MAX_DISTANCE - START_DECREASE);
        }


        vec3 light_dir = normalize(direction_to_light);
        vec3 light_color = ubo.point_lights[i].color.xyz * ubo.point_lights[i].color.w;

        vec3 specular_light = getSpecular(light_dir, light_color, normal, view_direction);
        vec3 diffuse_light = getDiffuse(light_dir, light_color, normal, distance);

        total_light += (k * diffuse_light + (1 - k) * specular_light) * fade_out;
    }

    if(ubo.ambient_light.w <= 0.7){
        for (i = 2; i < 12; i++) {
            vec3 direction_to_light = ubo.point_lights[i].position.xyz - position_world;
            float distance = length(direction_to_light);
            float dist_sq = distance * distance;

            float fade_out = 1;

            if(dist_sq > MAX_DISTANCE) continue;

            if(dist_sq > START_DECREASE){
                fade_out -= (dist_sq - START_DECREASE) / (MAX_DISTANCE - START_DECREASE);
            }


            vec3 light_dir = normalize(direction_to_light);
            vec3 light_color = ubo.point_lights[i].color.xyz * ubo.point_lights[i].color.w;

            vec3 specular_light = getSpecular(light_dir, light_color, normal, view_direction);
            vec3 diffuse_light = getDiffuse(light_dir, light_color, normal, distance);

            total_light += (k * diffuse_light + (1 - k) * specular_light) * fade_out;
        }
    }

    for (i = 12; i < 16; i++) {
        vec3 direction_to_light = ubo.point_lights[i].position.xyz - position_world;
        float distance = length(direction_to_light);
        float dist_sq = distance * distance;

        float fade_out = 1;

        if(dist_sq > MAX_DISTANCE / 2) continue;

        if(dist_sq > START_DECREASE / 2){
            fade_out -= (dist_sq - START_DECREASE / 2) / (MAX_DISTANCE / 2 - START_DECREASE / 2);
        }


        vec3 light_dir = normalize(direction_to_light);
        vec3 light_color = ubo.point_lights[i].color.xyz * ubo.point_lights[i].color.w;

        vec3 specular_light = getSpecular(light_dir, light_color, normal, view_direction);
        vec3 diffuse_light = getDiffuse(light_dir, light_color, normal, distance);

        total_light += (k * diffuse_light + (1 - k) * specular_light) * fade_out;
    }

    
    vec3 ambient_light = ubo.ambient_light.xyz * min(ubo.ambient_light.w / 4.0, 0.3);
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = texColor * vec4((total_light + ambient_light) * fragColor, 1.0);
}
