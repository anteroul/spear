#version 450 core

in vec3 fragNormal;     // Interpolated normal vector
in vec2 fragTexCoord;   // Interpolated texture coordinates

// Uniforms
uniform sampler2D textureSampler;

struct Material {
    float specularExponent; // Ns
    float dissolve;         // d
    int illuminationModel;  // illum
    vec3 ambientColor;      // Ka
    vec3 diffuseColor;      // Kd
    vec3 specularColor;     // Ks
};
uniform Material material;

// Light properties
struct Light {
    vec3 position;       // Light position
    vec3 color;          // Light color
    float intensity;     // Light intensity
};
uniform Light light;

// Camera position
uniform vec3 cameraPosition;

// Output color
out vec4 color;

void main()
{
    // Normalize the normal vector
    vec3 normal = normalize(fragNormal);

    // Calculate light direction
    vec3 lightDir = normalize(light.position - fragNormal);

    // Ambient lighting
    vec3 ambient = material.ambientColor * light.color * light.intensity;

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuseColor * light.color * light.intensity;

    // Specular lighting
    vec3 viewDir = normalize(cameraPosition - fragNormal);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularExponent);
    vec3 specular = spec * material.specularColor * light.color * light.intensity;

    // Combine results
    vec3 lighting = ambient + diffuse + specular;

    // Apply texture
    vec3 texColor = texture(textureSampler, fragTexCoord).rgb;

    // Final color output
    color = vec4(lighting * texColor, 1.0);
}
