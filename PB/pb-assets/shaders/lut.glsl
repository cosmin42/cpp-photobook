#version 330 core
out vec3 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler3D textureSampler;      // The 3D LUT

void main() {
    vec3 color = texture(texture1, TexCoord).rgb;

    // Get the integer and fractional parts of the texture coordinates
    vec3 texSize = vec3(textureSize(textureSampler, 0));
    vec3 texCoord = color * texSize;
    vec3 texCoordFloor = floor(texCoord);
    vec3 texCoordFrac = texCoord - texCoordFloor;

    // Convert to texture space (0 to 1)
    vec3 texCoordNorm = texCoordFloor / texSize;
    vec3 texCoordNormStep = 1.0 / texSize;

    // Compute coordinates of the eight surrounding texels
    vec3 texCoord000 = texCoordNorm;
    vec3 texCoord100 = texCoordNorm + vec3(texCoordNormStep.x, 0.0, 0.0);
    vec3 texCoord010 = texCoordNorm + vec3(0.0, texCoordNormStep.y, 0.0);
    vec3 texCoord110 = texCoordNorm + vec3(texCoordNormStep.x, texCoordNormStep.y, 0.0);

    vec3 texCoord001 = texCoordNorm + vec3(0.0, 0.0, texCoordNormStep.z);
    vec3 texCoord101 = texCoordNorm + vec3(texCoordNormStep.x, 0.0, texCoordNormStep.z);
    vec3 texCoord011 = texCoordNorm + vec3(0.0, texCoordNormStep.y, texCoordNormStep.z);
    vec3 texCoord111 = texCoordNorm + texCoordNormStep;

    // Sample the eight surrounding texels
    vec4 c000 = texture(textureSampler, texCoord000);
    vec4 c100 = texture(textureSampler, texCoord100);
    vec4 c010 = texture(textureSampler, texCoord010);
    vec4 c110 = texture(textureSampler, texCoord110);

    vec4 c001 = texture(textureSampler, texCoord001);
    vec4 c101 = texture(textureSampler, texCoord101);
    vec4 c011 = texture(textureSampler, texCoord011);
    vec4 c111 = texture(textureSampler, texCoord111);

    // Perform trilinear interpolation
    vec4 c00 = mix(c000, c100, texCoordFrac.x);
    vec4 c10 = mix(c010, c110, texCoordFrac.x);
    vec4 c01 = mix(c001, c101, texCoordFrac.x);
    vec4 c11 = mix(c011, c111, texCoordFrac.x);

    vec4 c0 = mix(c00, c10, texCoordFrac.y);
    vec4 c1 = mix(c01, c11, texCoordFrac.y);

    FragColor = mix(c0, c1, texCoordFrac.z).rgb;
}