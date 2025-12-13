#include "window.h"
#include <print>

float vertices[] = {
    0.5f,  0.5f,  0.0f, // top right
    0.5f,  -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f  // top left
};
unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource1 =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

const char *fragmentShaderSource2 =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "  FragColor = vec4(0.5f, 0.5f, 0.9f, 1.0f);\n"
    "}\0";

int main(void) {
  unsigned int VAO, VBO, EBO, vertexShader, fragmentShader1, fragmentShader2,
      shaderProgram1, shaderProgram2;

  const auto init_cb = [&]() noexcept {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    /* Bind vertex array before modifying any of its buffers/features */
    glBindVertexArray(VAO);

    /* Bind vertex buffer object to vertex array object */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* Bind element buffer object to vertex array object (VAO) */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    /* Set VAO attributes */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void *>(0));
    glEnableVertexAttribArray(0);

    /* Unbind for cleanup */
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);

    /* Create vertex shader */
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    /* Create fragment shader */
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, nullptr);
    glCompileShader(fragmentShader1);

    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, nullptr);
    glCompileShader(fragmentShader2);

    /* Link shaders into program */
    shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);

    shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);

    /* Delete shaders for cleanup */
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader1);
    glDeleteShader(fragmentShader2);
  };

  const auto render_cb = [&]() noexcept {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    glUseProgram(shaderProgram1);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    glUseProgram(shaderProgram2);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,
                   reinterpret_cast<void *>(3 * sizeof(unsigned int)));
  };

  const auto cleanup_cb = [&]() noexcept {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);
  };

  try {
    Window win{640, 480, "Hello World", init_cb, render_cb, cleanup_cb};

    while (!win.shouldClose()) {
      Window::pollEvents();
      win.render();
    }
  } catch (const std::exception &e) {
    std::println("Window failure: {}", e.what());
  }

  return 0;
}
