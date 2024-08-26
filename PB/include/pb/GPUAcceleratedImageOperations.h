#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <pb/image/ImageReader.h>
#include <pb/util/Traits.h>

namespace PB {
class GPUAcceleratedImageOperations final {
public:
  GPUAcceleratedImageOperations() = default;
  ~GPUAcceleratedImageOperations() = default;

  void initOpenGL()
  {
    if (!glfwInit()) {
      PBDev::basicAssert(false);
    }
    // Create an invisible window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
    if (!window) {
      glfwTerminate();
      PBDev::basicAssert(false);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
      PBDev::basicAssert(false);
    }
  }

  void createFrameBuffer()
  {
    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    mShaderProgram = createShaderProgram();
    glUseProgram(mShaderProgram);

    glGenTextures(1, &mRenderTextureId);
    glBindTexture(GL_TEXTURE_2D, mRenderTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &mRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, mRbo);

    glGenTextures(1, &mLutTexture);
    glBindTexture(GL_TEXTURE_3D, mLutTexture);

    test1();
  }

  void matToTexture(std::shared_ptr<cv::Mat> image)
  {

    GLenum format = (image->channels() == 3) ? GL_RGB : GL_LUMINANCE;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->cols, image->rows, 0, format,
                 GL_UNSIGNED_BYTE, image->data);
  }

  std::shared_ptr<cv::Mat> textureToMat(int width, int height)
  {
    std::shared_ptr<cv::Mat> output =
        std::make_shared<cv::Mat>(height, width, CV_8UC3);

    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, output->data);

    return output;
  }

  void createTextureToRenderTo(
      std::shared_ptr<cv::Mat>                          inputImage,
      std::vector<std::vector<std::vector<cv::Vec3f>>> &lutData,
      unsigned                                          lutSize)
  {
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, lutSize, lutSize, lutSize, 0,
                 GL_RGB, GL_FLOAT, lutData.data());

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          inputImage->cols, inputImage->rows);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, mRbo);

    glViewport(0, 0, inputImage->cols, inputImage->rows);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inputImage->cols, inputImage->rows,
                 0, GL_BGR, GL_UNSIGNED_BYTE, inputImage->data);

    glBindVertexArray(VAO); // Ensure VAO is bound
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           mRenderTextureId, 0);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      PBDev::basicAssert(false);
    }
    glfwPollEvents();
  }

  void test1()
  {
    // Define a simple quad that covers the entire screen
    float        vertices[] = {// positions       // texture coords
                        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                        0.0f,  0.0f, 0.0f, 1.0f, -1.0f, 0.0f,  1.0f,
                        0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f};
    unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  void cleanup()
  {
    glDeleteTextures(1, &mTextureId);
    glDeleteTextures(1, &mRenderTextureId);
    glDeleteFramebuffers(1, &mFbo);
    glDeleteProgram(mShaderProgram);

    glfwTerminate();
  }

  std::shared_ptr<cv::Mat>
  processImage(std::shared_ptr<cv::Mat>                          inputImage,
               std::vector<std::vector<std::vector<cv::Vec3f>>> &lutData,
               unsigned                                          lutSize)
  {
    createTextureToRenderTo(inputImage, lutData, lutSize);
    return textureToMat(inputImage->cols, inputImage->rows);
  }

  GLuint compileShader(GLenum type, const char *source)
  {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compile errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      PBDev::basicAssert(false);
    }
    return shader;
  }

  GLuint createShaderProgram()
  {
    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);

    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      PBDev::basicAssert(false);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
  }

private:
  GLuint mFbo;
  GLuint mRbo;
  GLuint mTextureId;
  GLuint mRenderTextureId;
  GLuint VBO, VAO, EBO;
  GLuint mLutTexture;

  GLint mShaderProgram;

  const char *fragmentShaderSource = R"(
    #version 330 core
    out vec3 FragColor;
    
    in vec2 TexCoord;
    uniform sampler2D texture1;
    uniform sampler3D textureSampler;      // The 3D LUT
/*
    void main() {
        vec3 color = texture(texture1, TexCoord).rgb;

        ivec3 size = textureSize(textureSampler, 0);

        ivec3 lutCoords = ivec3(color * size);

        ivec3 nextlutCoords;
		nextlutCoords.x = min(lutCoords.x + 1, size.x - 1);
		nextlutCoords.y = min(lutCoords.y + 1, size.y - 1);
        nextlutCoords.z = min(lutCoords.z + 1, size.z - 1);

        

        // Fetch the color from the LUT
        //vec4 gradedColor = texture(textureSampler, lutCoords);
        FragColor = vec3(lutCoords);
    }
*/

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

)";

  const char *vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    
    out vec2 TexCoord;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
)";
};
} // namespace PB
