#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform int flashlightActive;
uniform vec2 flashlightPos;
uniform float flashlightRadius;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    if (flashlightActive == 1)
    {
        // Raylib top-left origin, OpenGL bottom-left - flip Y
        vec2 fragPos = vec2(gl_FragCoord.x, 600.0 - gl_FragCoord.y);
        
        // Distance from fragment to flashlight center
        float dist = distance(fragPos, flashlightPos);
        
        // Create smooth gradient light circle
        float intensity;
        if (dist >= flashlightRadius) {
            intensity = 0.0;  
        } else if (dist <= 0.0) {
            intensity = 1.0;  
        } else {
            // Linear falloff from center to edge
            intensity = 1.0 - (dist / flashlightRadius);
        }
        
        float brightness = 0.1 + 0.9 * intensity;
        vec3 result = texelColor.rgb * brightness;
        
        finalColor = vec4(result, texelColor.a) * colDiffuse * fragColor;
    }
    else
    {
        // Flashlight off : darkness
        vec3 result = texelColor.rgb * 0.1;
        finalColor = vec4(result, texelColor.a) * colDiffuse * fragColor;
    }
}