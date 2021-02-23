/*
  ====--- [C++ SOURCE FILE] HEADER ---====
  ----------------------------------------

  @MARK:source

  Creator: James Spratt.
  Notice: (C) Copyright 2021, James Spratt, All rights reserved.

  ----------------------------------------
*/


#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <string>


// const GLint WINDOW_WIDTH = 1280;
// const GLint WINDOW_HEIGHT = 720;
const GLint WINDOW_WIDTH = 800;
const GLint WINDOW_HEIGHT = 600;


void read_shader_source(std::string file_path, std::string &output_source);


int main() {
   printf("Program Begin!!!");

   
   // @@ program variables
   float texture_scale = 1.0f;
   // @!


   // @@ GLFW
   GLFWwindow* window;
   {
      glfwInit();
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
      glfwMakeContextCurrent(window);
   }
   // @!

   
   // @@ GLAD
   {
      gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
   }
   // @!


   // @@ GL setup
   {
      glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
   }
   // @!


   // @@ loading textures and creating them
   GLuint texture_1_id;
   GLuint texture_2_id;
   {
      stbi_set_flip_vertically_on_load(true);
   
      int texture_width = 0;
      int texture_height = 0;
      int no_channels = 0;
      unsigned char *texture_data = stbi_load("container.jpg", &texture_width, &texture_height,
					      &no_channels, 0);

      if(!texture_data) {
	 std::cerr << "Texture failed to  load" << '\n';
	 exit(1);
      }

      glGenTextures(1, &texture_1_id);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_1_id);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB,
		   GL_UNSIGNED_BYTE, texture_data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(texture_data);

   
      texture_data = stbi_load("awesomeface.png", &texture_width, &texture_height, &no_channels, 0);
      if(!texture_data) {
	 std::cerr << "Texture failed to  load" << '\n';
	 exit(1);
      }

      glGenTextures(1, &texture_2_id);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture_2_id);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGBA,
		   GL_UNSIGNED_BYTE, texture_data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(texture_data);
   }
   // @!

   
   // @@ compiling shaders
   GLuint shader_program;
   {
      int success;
      char info_log[2048];
   
      std::string vert_path{"shaders/triangle.vert"};
      std::string frag_path{"shaders/triangle.frag"};
      std::string vert_source{};
      std::string frag_source{};

      read_shader_source(vert_path, vert_source);
      read_shader_source(frag_path, frag_source);

      GLuint vert_shader;
      GLuint frag_shader;
      vert_shader = glCreateShader(GL_VERTEX_SHADER);
      frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

      const GLchar *char_vert_source = vert_source.c_str();
      glShaderSource(vert_shader, 1, &char_vert_source, NULL);
      glCompileShader(vert_shader);
      glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
      if(!success) {
	 glGetShaderInfoLog(vert_shader, 2048, NULL, info_log);
	 std::cerr << "ERROR: vert shader compiling failed." << '\n';
	 std::cout << info_log << '\n';
	 exit(1);
      }

      const GLchar *char_frag_source = frag_source.c_str();
      glShaderSource(frag_shader, 1, &char_frag_source, NULL);
      glCompileShader(frag_shader);
      glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
      if(!success) {
	 glGetShaderInfoLog(frag_shader, 2048, NULL, info_log);
	 std::cerr << "ERROR: frag shader compiling failed." << '\n';
	 std::cout << info_log << '\n';
	 exit(1);
      }
   
      shader_program = glCreateProgram();
      glAttachShader(shader_program, vert_shader);
      glAttachShader(shader_program, frag_shader);
      glLinkProgram(shader_program);

      glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
      if(!success) {
	 glGetProgramInfoLog(shader_program, 2048, NULL, info_log);
	 std::cerr << "ERROR: shader link failed." << '\n';
	 std::cout << info_log << '\n';
	 exit(1);
      }
   
      glDeleteShader(vert_shader);
      glDeleteShader(frag_shader);

      
      glUseProgram(shader_program);
      glUniform1i(glGetUniformLocation(shader_program, "container_texture"), 0);
      glUniform1i(glGetUniformLocation(shader_program, "face_texture"), 1);
   }
   // @!


   // @@ vertex stuff
   GLuint triangle_VAO;
   {
      glGenVertexArrays(1, &triangle_VAO);
      glBindVertexArray(triangle_VAO);
   
      GLuint triangle_VBO;
      {
	 GLfloat vertices[] = {
	    // positions         // colors         // tex coords
	    0.5f,  -0.5f, 0.0f,  1.0f, 1.0f, 0.0f, texture_scale * 0.0f, texture_scale * 0.0f,
	    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f, texture_scale * 1.0f, texture_scale * 0.0f,
	    0.0f,   0.5f, 0.0f,  1.0f, 0.0f, 1.0f, texture_scale * 0.5f, texture_scale * 1.0f,
	 };

	 glGenBuffers(1, &triangle_VBO);
	 glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      }

      // positions
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
      glEnableVertexAttribArray(0);

      // colors
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
			    (void *)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);

      // tex coords
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
			    (void *)(6 * sizeof(GLfloat)));
      glEnableVertexAttribArray(2);
   }
   // @!


   // @@ idk
   glm::mat4 trans;
   GLint transform_location;
   {
      transform_location = glGetUniformLocation(shader_program, "transform");
   }
   // @!
   
   
   while(!glfwWindowShouldClose(window))
   {
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      // input

      // @@ simulation
      trans = glm::mat4(1.0f);
      trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
      trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
      //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
      glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(trans));
      // @!

      // @@ rendering
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_1_id);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture_2_id);
      
      glUseProgram(shader_program);
      glBindVertexArray(triangle_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      // @!

      // check and call events and swap the buffers
      glfwPollEvents();
      glfwSwapBuffers(window);
   }
   

   return 0;
}


// @@ read source of shader and store it in output_source
void read_shader_source(std::string file_path, std::string &output_source) {
   std::ifstream inf{file_path};

   if(!inf) {
      std::cerr << "ERROR" << '\n';
   }

   std::string file_source{""};
   while(inf) {
      std::string temp_str;
      std::getline(inf, temp_str);
      file_source += temp_str += "\n";
   }

   output_source = file_source;
}
