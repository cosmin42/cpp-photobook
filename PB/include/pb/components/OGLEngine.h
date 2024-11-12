#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS
// TODO: Add the OpenGL ES libraries
#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include <pb/Platform.h>
#ifdef __APPLE__
#include <pb/components/ThreadScheduler.h>
#endif
#include <pb/components/TSQueue.h>
#include <pb/entities/LutImageProcessingData.h>
#include <pb/util/Traits.h>

DECLARE_STRONG_STRING(OGLRenderId)

namespace PB::Service {
class OGLEngine final {
public:
  ~OGLEngine() = default;

#ifdef __APPLE__
  void configureThreadScheduler(PBDev::ThreadScheduler* threadScheduler);
#endif
  void configurePlatformInfo(std::shared_ptr<PlatformInfo> platformInfo);

  void start(std::stop_token stopToken);
  void stop(std::stop_source stopSource);

  void applyLut(LutImageProcessingData const &imageProcessingData);

private:
  void initOpenGL();

  void initFrameBuffer();

  GLint createProgram(Path path, std::string name);

  std::string readShaderSource(Path path);

  GLuint compileShader(GLenum type, std::string source);

  void generateRenderTexture();

  void loadPrograms();

  void mainloop();

  Path vertexShaderPath() const;

  void loadTextureAndRender(ImageProcessingData const &imageProcessingData);

#ifdef __APPLE__
  PBDev::ThreadScheduler* mThreadScheduler = nullptr;
#endif

  std::shared_ptr<PlatformInfo> mPlatformInfo = nullptr;

  TSQueue<LutImageProcessingData> mWorkQueue;
  std::jthread                    mThread;

  std::stop_token mStopToken;

  GLuint mRenderTextureId;
  GLuint mLutTextureId;

  GLuint mVertexBufferObject;
  GLuint mVertexArrayObject;
  GLuint mElementBufferObject;
  GLuint mFrameBufferObject;
  GLuint mRenderBufferObject;

  std::unordered_map<std::string, GLint> mShaderPrograms;

  float mImageVertices[20] = {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                              0.0f,  0.0f, 0.0f, 1.0f, -1.0f, 0.0f,  1.0f,
                              0.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f};
  unsigned int mIndices[6] = {0, 1, 2, 0, 2, 3};

  std::mutex              mWorkMutex;
  std::condition_variable mFinishedWorkCondition;
  bool                    mFinishedWork = false;

  const std::unordered_map<std::string, Path> FRAGMENT_SHADERS_PATHS = {
      {"lut", Path("shaders") / "lut.glsl"}};

  const Path VERTEX_SHADER_PATH = Path("shaders") / "vertex.glsl";
};
} // namespace PB
