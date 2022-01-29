#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform samplerCube skybox;
uniform Light light;
uniform bool blinn;
uniform vec3 basecolor;

void only_light()
{    
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 tcolor = vec3(texture(skybox, R).rgb);

    vec3 ambient = 0.5 * basecolor * light.ambient;
    // diffuse
    vec3 lightDir = normalize(lightPos - Position);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * basecolor * light.diffuse;
    // specular
    vec3 viewDir = normalize(cameraPos - Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = spec * light.specular * basecolor; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0)*0.7 + vec4(tcolor, 1.0)*0.3;
}

float coeffs(int i)
{
    return 1.0/(3.0+0.1*float(i));
}

void other_render()
{
    vec3 N = normalize(Normal);
    vec3 I = normalize(cameraPos - Position);
    float IdotN = dot(I,N);
    float scales[6];
    vec3 C[6];
    for(int i = 0; i < 6; i++) {
        scales[i] = (IdotN - sqrt(1.0 - coeffs(i) + coeffs(i)*(IdotN*IdotN) ));
        C[i] = texture(skybox, (-I+coeffs(i)*N)).xyz;
    }
    vec4 refractedColor = 0.25*vec4( C[5].x + 2.0*C[0].x + C[1].x, C[1].y + 2.0*C[2].y + C[3].y, C[3].z + 2.0*C[4].z +C[5].z, 4.0 );
    vec3 R = 2.0 * dot(-(cameraPos - Position), N)*N + (cameraPos - Position);
    vec4 reflectedColor = vec4(basecolor, 1.0);
    FragColor = mix(reflectedColor, refractedColor, 0.02 + 0.02 * pow(1.0 - IdotN, 2.0));
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 I = normalize(cameraPos - Position);
    vec3 R = reflect(I, normalize(Normal));
    float cosTheta = dot(I,N);
    vec3 lightDir = normalize(lightPos - Position);
    vec3 halfwayDir = normalize(lightDir + I);
    vec3 F0 = vec3(0.2);
    vec3 F = F0 + (vec3(1.0)-F0) * pow(1.0 - cosTheta, 5);

    // Reflection color component
    vec3 reflectedColor = texture(skybox, R).xyz;

    // Refracted color component
    vec3 refractedColor = basecolor;

    FragColor = vec4(F*reflectedColor + (vec3(1.0)-F)*refractedColor, 1.0);
    vec3 color = F*reflectedColor + (vec3(1.0)-F)*refractedColor;

    /* 光照 */
    /*vec3 ambient = 0.5 * color * light.ambient;
    // diffuse
    // vec3 lightDir = normalize(lightPos - Position);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * light.diffuse;
    // specular
    vec3 viewDir = normalize(cameraPos - Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)   //需要把.cpp里的开头bool变量注释去掉和键盘回调函数的注释去掉
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = spec * light.specular * color; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);*/  
}
