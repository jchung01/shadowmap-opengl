#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

smooth in vec4 position_light; 

uniform mat4 model;
// manually assign the modelview later.
// uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// shadowmap
uniform sampler2D shadowMap;

// set to true to test task3/sampled depth.
const bool debugDepth = false;

// Output the frag color
out vec4 fragColor;

vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

// return 1 if in shadow
float computeShadow() {
    // naive bias, may not work correctly for all cases?
    const float delta = 0.01f;
    vec3 ndc = position_light.xyz / position_light.w;	// you can skip the division if using orthographic projection
	ndc = position_light.xyz * 0.5 + 0.5;	// shift from [-1, 1] to [0, 1]

    // light's perspective
	float sampledDepth = texture(shadowMap, ndc.xy).r;
    float newDepth = ndc.z;
    float bias = max(0.005 * (1.0 - dot(normal, vec3(position_light))), 0.005);
    float shadow = newDepth - bias > sampledDepth ? 1.f : 0.f;

    return shadow;

//    if (newDepth - sampledDepth > delta) {
//        return 1.0f;
//    }
//    return 0.0f;
}

// PCF code taken from learnOpenGL
float computePCF() {
    // poisson disc vals taken from: https://www.asawicki.info/Download/Productions/Applications/PoissonDiscGenerator/2D.txt
    const vec2 poissonDiscs[9] = vec2[9](vec2(0.493393f, 0.394269f),
	    vec2(0.798547f, 0.885922f),
	    vec2(0.247322f, 0.92645f),
	    vec2(0.0514542f, 0.140782f),
	    vec2(0.831843f, 0.00955229f),
	    vec2(0.428632f, 0.0171514f),
	    vec2(0.015656f, 0.749779f),
	    vec2(0.758385f, 0.49617f),
	    vec2(0.223487f, 0.562151f));
    // total samples = (2*sampleRange + 1)^2
    const int sampleRange = 1;
    const float SPREAD = 2.0f;

    vec3 ndc = position_light.xyz / position_light.w;	// you can skip the division if using orthographic projection
	ndc = position_light.xyz * 0.5 + 0.5;	// shift from [-1, 1] to [0, 1]
    float newDepth = ndc.z;
    float bias = max(0.005 * (1.0 - dot(normal, vec3(position_light))), 0.005);

    float shadow = 0.0f;
    
    int i = 0;
    for(int x = -1 * sampleRange; x <= sampleRange; ++x) {
        for(int y = -1 * sampleRange; y <= sampleRange; ++y, ++i) {
            vec2 poisson = poissonDiscs[i] / SPREAD;
            float pcfDepth = texture(shadowMap, ndc.xy + vec2(x, y) * texelSize + poisson * texelSize).r; 
            shadow += newDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }
    }
    return shadow / ((2*sampleRange + 1) * (2*sampleRange + 1));
}

void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else if (debugDepth) {
        vec3 ndc = position_light.xyz / position_light.w;	// you can skip the division if using orthographic projection
	    ndc = position_light.xyz * 0.5 + 0.5;	// shift from [-1, 1] to [0, 1]
        float sampledDepth = texture(shadowMap, ndc.xy).r;
        fragColor = vec4(vec3(sampledDepth), 1.0f);
    } else {
        mat4 modelview = view * model;

        // We will reference the camera (eye) coordinate for all vectors/positions.
        // keep vec4 for all positions

        // At the end, fragColor = emision + for all lights i, sum(sum_comps * lightcolors[i])
        // where sum_comps is the sum of ambient, diffuse, specular components
        fragColor = emision;

        // transform norm to reference eye/camera
        // vector: model -> eye
        // A = mat3(modelview)
        vec3 iref_norm = normalize( transpose( inverse( mat3(modelview)) ) * normal);

        // transform position to reference eye
        // position: model -> eye
        vec4 iref_position = modelview * position;

        // compute direction to viewer (p=position to q=(0, 0, 0, 1))
        // p.w * q.xyz - q.w * p.xyz
        vec3 iref_vdir = normalize( (iref_position.w * vec3(0.0f)) - (1.0f * vec3(iref_position)) );

        // Compute Blinn-Phong shading for each light
        for (int i = 0; i < nlights; ++i) {
            // transform light position to reference eye
            vec4 iref_lposition = view * lightpositions[i];
            // compute direction to light (p=position to q=lposition)
            // p.w * q.xyz - q.w * p.xyz
            vec3 iref_ldir = normalize( (iref_position.w * vec3(iref_lposition)) -
                                        (iref_lposition.w * vec3(iref_position)) );

            // compute halfway direction (reference to eye)
            vec3 iref_halfdir = normalize(iref_vdir + iref_ldir);

            vec4 shading = lightcolors[i];
            vec4 sum_comps = ambient;
            // change shadow function if you want PCF or not.
            sum_comps += (1.0f - computePCF()) * (
                            ( diffuse * max( dot(iref_norm, iref_ldir), 0.0f ) ) +
                            ( specular * pow( max( dot(iref_norm, iref_halfdir), 0.0f ), shininess ) )
                            );
            shading = sum_comps * shading;
            fragColor = fragColor + shading;
        }
    }
}
