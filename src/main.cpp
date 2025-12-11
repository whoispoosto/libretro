#include "window.h"
#include <print>

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

int main(void) {
  const auto init_cb = []() {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    /* Bind vertex array before modifying any of its buffers/features */
    glBindVertexArray(VAO);

    /* Bind vertex data to buffer */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* Set VAO attributes */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(0));
    glEnableVertexAttribArray(0);

    /* Unbind for cleanup */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  };

  const auto render_cb = []() { glClear(GL_COLOR_BUFFER_BIT); };

  try {
    Window win{640, 480, "Hello World", init_cb, render_cb};

    // unsigned int VAO;
    // glGenVertexArrays(1, &VAO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
    //                       (void *)0);
    // glEnableVertexAttribArray(0);
    //
    // unsigned int vertexShader;
    // vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // glCompileShader(vertexShader);
    //
    // unsigned int fragmentShader;
    // fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // glCompileShader(fragmentShader);
    //
    // unsigned int shaderProgram;
    // shaderProgram = glCreateProgram();
    //
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);
    //
    // glUseProgram(shaderProgram);
    //
    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);

    while (!win.shouldClose()) {
      Window::pollEvents();
      win.render();
    }
  } catch (const std::exception &e) {
    std::println("Window failure: {}", e.what());
  }

  return 0;
}
