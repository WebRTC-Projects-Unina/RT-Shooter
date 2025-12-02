

const char* PhongShaderVertex = R"(#version 300 es
precision mediump float;

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos; 
out vec3 Normal;
out vec3 LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
     // normal matrix FIX (WebGL2-safe)
    mat4 mv = view * model;
    mat3 normalMatrix = mat3(transpose(inverse(mv)));
    Normal = normalize(normalMatrix * aNormal);
    LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position

}
)";


const char* PhongShaderFragment = R"(#version 300 es
precision mediump float;

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 LightPos; 
 
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

void main()
{
    
    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * ambientColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor;
        
    //specular
    //float specularStrength = 0.9;
    vec3 viewDir = normalize(- FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessVal);
    //vec3 specular = specularStrength * spec * specularColor; 
    vec3 specular = spec * specularColor;

    vec3 result = (ambient * ambientEnable * Ka + diffuse * diffuseEnable * Kd + specular * specularEnable * Ks);
    FragColor = vec4(result, 1.0);
}   
)";
