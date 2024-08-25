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

  void createTextureToRenderTo(std::shared_ptr<cv::Mat> inputImage)
  {
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
    glDrawArrays(GL_TRIANGLES, 0,
                 6); // Or glDrawElements, depending on your setup



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
    float vertices[] = {
        // Positions        // Colors
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom left
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // Top
    };

    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
  }

  void cleanup()
  {
    glDeleteTextures(1, &mTextureId);
    glDeleteTextures(1, &mRenderTextureId);
    glDeleteFramebuffers(1, &mFbo);
    glDeleteProgram(mShaderProgram);

    glfwTerminate();
  }

  std::shared_ptr<cv::Mat> processImage(std::shared_ptr<cv::Mat> inputImage)
  {
    createTextureToRenderTo(inputImage);
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
  GLuint VBO, VAO;

  GLint mShaderProgram;

  const char *fragmentShaderSource = R"(
    #version 330 core
    out vec3 FragColor;
    
    in vec2 TexCoord;
    uniform sampler2D texture1;

    void main() {
        vec3 color = texture(texture1, TexCoord).rgb;
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        FragColor = vec3(gray);
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
