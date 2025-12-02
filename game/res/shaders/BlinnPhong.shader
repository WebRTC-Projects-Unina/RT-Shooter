
const char* BlinnPhongShaderVertex = R"(#version 300 es
precision mediump float;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aOffset;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos + vec3(aOffset.x, 0.5, aOffset.y), 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
     // normal matrix FIX (WebGL2-safe)
    mat4 mv = view * model;
    mat3 normalMatrix = mat3(transpose(inverse(mv)));
    Normal = normalize(normalMatrix * aNormal);
    LightPos = vec3(view * vec4(lightPos, 1.0)); // trasformazione in view space
    TexCoord = aTexCoord;
}
)";

const char* BlinnPhongShaderFragment = R"(#version 300 es
precision mediump float;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoord;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 viewPos;

uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shininessVal;

uniform float ambientEnable;
uniform float diffuseEnable;
uniform float specularEnable;

uniform sampler2D diffTexture;
uniform sampler2D specTexture;


void main()
{
    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength  * texture(diffTexture, TexCoord).rgb;

    // normali e direzioni
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 viewDir = normalize(-FragPos); // view space

    // Blinn-Phong: half vector
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininessVal);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * texture(diffTexture, TexCoord).rgb;
    vec3 specular = spec * texture(specTexture, TexCoord).rgb;

    vec3 result = (ambient * ambientEnable * Ka +
        diffuse * diffuseEnable * Kd +
        specular * specularEnable * Ks);

    FragColor = vec4(result, 1.0);
}
)";
