#version 330 core

in vec3 colorVS;

// Fragment shader output
out vec4 FragColor;

in float animation;
in vec3 Normal, LightDir, ViewDir;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 matDiffuse, viewPos, lightColor;
uniform vec3 lightAmbient;
uniform vec3 Emissive;
uniform vec3 objectColor;
uniform vec3 specular;
uniform float shininess;

void main() {
    // normalize and diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseTerm = matDiffuse * lightColor * diff;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambientEmissivterm = ambientStrength * matDiffuse * lightAmbient + Emissive;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float cosBeta = max(0.0, dot(viewDir, reflectDir));
    float spec = pow(cosBeta, shininess);

    vec3 specularTerm = specular * spec * lightColor;

    // final color calculation
    vec3 result = (ambientEmissivterm + diffuseTerm + specularTerm) * objectColor;

    FragColor = vec4(result, 1.0);
}
