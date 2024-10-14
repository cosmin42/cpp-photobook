#include <pb/components/OGLEngine.h>

#include <fstream>

#include <pb/entities/LutImageProcessingData.h>
#include <pb/util/Traits.h>

namespace PB::Service {

void OGLEngine::configurePlatformInfo(
    std::shared_ptr<PlatformInfo> platformInfo)
{
  mPlatformInfo = platformInfo;
}

void OGLEngine::start(std::stop_token stopToken)
{
  mStopToken = stopToken;
  mThread = std::jthread([this] { mainloop(); });
}

void OGLEngine::stop(std::stop_source stopSource)
{
  stopSource.request_stop();
  mWorkQueue.enqueue(LutImageProcessingData());
}

void OGLEngine::initOpenGL()
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

void OGLEngine::initFrameBuffer()
{
  glGenFramebuffers(1, &mFrameBufferObject);
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
}

std::string OGLEngine::readShaderSource(Path path)
{
  std::ifstream file(path);
  std::string   source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
  return source;
}

GLuint OGLEngine::compileShader(GLenum type, std::string source)
{
  GLuint      shader = glCreateShader(type);
  const char *c_str = source.c_str();
  glShaderSource(shader, 1, &c_str, NULL);
  glCompileShader(shader);

  // Check for compilation errors
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    PBDev::basicAssert(false);
  }

  return shader;
}

void OGLEngine::generateRenderTexture()
{
  glGenTextures(1, &mRenderTextureId);
  glBindTexture(GL_TEXTURE_2D, mRenderTextureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenRenderbuffers(1, &mRenderBufferObject);
  glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferObject);

  glGenTextures(1, &mLutTextureId);
  glBindTexture(GL_TEXTURE_3D, mLutTextureId);

  glGenVertexArrays(1, &mVertexArrayObject);
  glGenBuffers(1, &mVertexBufferObject);
  glGenBuffers(1, &mElementBufferObject);

  glBindVertexArray(mVertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(mImageVertices), mImageVertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBufferObject);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices), mIndices,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coordinate attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

GLint OGLEngine::createProgram(Path fragmentShaderPath, std::string name)
{
  std::string fragmentShaderSource =
      readShaderSource(mPlatformInfo->installationPath / fragmentShaderPath);
  std::string vertexShaderSource =
      readShaderSource(mPlatformInfo->installationPath / VERTEX_SHADER_PATH);

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

void OGLEngine::loadPrograms()
{
  // Load shaders.
  for (auto const &[name, path] : FRAGMENT_SHADERS_PATHS) {
    mShaderPrograms[name] = createProgram(path, name);

    glUseProgram(mShaderPrograms.at(name));
  }
}

void OGLEngine::mainloop()
{
  initOpenGL();
  initFrameBuffer();
  loadPrograms();
  generateRenderTexture();

  while (!mStopToken.stop_requested()) {
    auto imageProcessingData = mWorkQueue.dequeue();
    loadTextureAndRender(imageProcessingData);

    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
    glReadPixels(0, 0, imageProcessingData.inImage->cols,
                 imageProcessingData.inImage->rows, GL_BGR, GL_UNSIGNED_BYTE,
                 imageProcessingData.outImage->data);
    mFinishedWork = true;
    mFinishedWorkCondition.notify_one();
  }

  glDeleteTextures(1, &mRenderTextureId);
  glDeleteFramebuffers(1, &mFrameBufferObject);
  for (auto const &[name, program] : mShaderPrograms) {
    glDeleteProgram(program);
  }

  glfwTerminate();
}

Path OGLEngine::vertexShaderPath() const
{
  return mPlatformInfo->installationPath / VERTEX_SHADER_PATH;
}

void OGLEngine::loadTextureAndRender(
    ImageProcessingData const &imageProcessingData)
{
  if (imageProcessingData.type() == ImageProcessingType::LUT) {
    auto lutImageProcessingData =
        dynamic_cast<LutImageProcessingData const &>(imageProcessingData);

    GLsizei lutSize = (GLsizei)std::cbrt(lutImageProcessingData.lut.size());
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, lutSize, lutSize, lutSize, 0,
                 GL_RGB, GL_FLOAT, lutImageProcessingData.lut.data());

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          lutImageProcessingData.inImage->cols,
                          lutImageProcessingData.inImage->rows);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, mRenderBufferObject);

    glViewport(0, 0, lutImageProcessingData.inImage->cols,
               lutImageProcessingData.inImage->rows);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lutImageProcessingData.inImage->cols,
                 lutImageProcessingData.inImage->rows, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, lutImageProcessingData.inImage->data);

    glBindVertexArray(mVertexArrayObject); // Ensure VAO is bound
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
}

void OGLEngine::applyLut(LutImageProcessingData const &imageProcessingData)
{
  std::unique_lock lock(mWorkMutex);
  mFinishedWork = false;
  mWorkQueue.enqueue(imageProcessingData);
  mFinishedWorkCondition.wait(lock, [this] { return mFinishedWork; });
}

} // namespace PB
