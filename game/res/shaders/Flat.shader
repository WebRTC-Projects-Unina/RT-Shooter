const char* FlatShaderVertex = R"(#version 300 es
precision mediump float;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

flat out vec3 FlatNormal;
flat out vec3 FlatFragPos;
flat out vec3 FlatLightPos;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;

    FlatFragPos = vec3(view * worldPos);
     // normal matrix FIX (WebGL2-safe)
    mat4 mv = view * model;
    mat3 normalMatrix = mat3(transpose(inverse(mv)));
    FlatNormal = normalize(normalMatrix * aNormal);
    FlatLightPos = vec3(view * vec4(lightPos, 1.0));
}
)";
const char* FlatShaderFragment = R"(#version 300 es
precision mediump float;

out vec4 FragColor;

flat in vec3 FlatNormal;
flat in vec3 FlatFragPos;
flat in vec3 FlatLightPos;

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shininessVal;

uniform float ambientEnable;
uniform float diffuseEnable;
uniform float specularEnable;

void main()
{
    vec3 norm = normalize(FlatNormal);
    vec3 lightDir = normalize(FlatLightPos - FlatFragPos);
    vec3 viewDir = normalize(-FlatFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    vec3 ambient = ambientColor * Ka;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseColor * diff * Kd;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessVal);
    vec3 specular = specularColor * spec * Ks;

    vec3 result = ambientEnable * ambient +
        diffuseEnable * diffuse +
        specularEnable * specular;

    FragColor = vec4(result, 1.0);
}

)";
