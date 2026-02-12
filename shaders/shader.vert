#version 450

layout(push_constant) uniform PushConstants{
    mat4 model;
    mat4 normal;
} pushConstants;

struct PointLight{
    vec4 position;
    vec4 color;
};

struct DirectionalLight{
    vec4 direction;
    vec4 color;
};

struct SpotLight{
    vec4 position;
    vec4 color;
    vec4 direction;
    float cutOff;
    float outerCutOff;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 camera_pos;
    vec4 ambient_light;
    DirectionalLight directional_light;
    PointLight point_lights[12];
    SpotLight spot_lights[2];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec3 position;

void main() {
    gl_Position = ubo.proj * ubo.view * pushConstants.model * vec4(inPosition, 1.0);
    
    fragNormalWorld = mat3(pushConstants.normal) * inNormal;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    position = inPosition;
}
