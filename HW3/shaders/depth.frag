#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

// Light source parameters
uniform bool enablelighting;
uniform float near;
uniform float far;

// Output the frag color
out vec4 fragColor;

void main (void){
    if (!enablelighting){
        // using default lighting when disabled
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        // linearize depth color
        // far, near are from camera's clipping plane
        float ndc_z = 2 * gl_FragCoord.z - 1;
        float lin_z = (2.0 * near * far) / (far + near - ndc_z * (far - near)); 
        fragColor = vec4(vec3(lin_z / (far - near)), 1.0f);
    }
}
