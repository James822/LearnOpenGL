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


GLint WINDOW_WIDTH = 1280;
GLint WINDOW_HEIGHT = 720;
// const GLint WINDOW_WIDTH = 800;
// const GLint WINDOW_HEIGHT = 600;


struct InputData {
   bool w_key_press;
   bool s_key_press;
   bool a_key_press;
   bool d_key_press;

   float last_mouse_xpos;
   float last_mouse_ypos;
   float mouse_xpos;
   float mouse_ypos;
};

struct ConfigData {
   glm::vec3 ambient_light_color;
   float mouse_sensitivity;
   float texture_scale;
   float fov_degrees;
   float move_speed;
};


void read_shader_source(std::string file_path, std::string &output_source);
void compile_shader_program(std::string vert_path, std::string frag_path, GLuint *shader_program);


int main() {
   printf("Program Begin!!!");

   
   // @@ program variables
   ConfigData config_data;
   config_data.ambient_light_color = glm::vec3(0.1f, 0.1f, 0.1f);
   config_data.mouse_sensitivity = 0.003f;
   config_data.texture_scale = 1.0f;
   config_data.fov_degrees = 70.0f;
   config_data.move_speed = 0.6f;
   // @!


   // @@ GLFW
   GLFWwindow* window;
   {
      glfwInit();
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
      WINDOW_WIDTH = video_mode->width;
      WINDOW_HEIGHT = video_mode->height;
      window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", primary_monitor, NULL);
      glfwMakeContextCurrent(window);
   }

   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   if(glfwRawMouseMotionSupported()) {
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
   } else {
      printf("@DEV_WARNING: GLFW_RAW_MOUSE_MOTION not supported.\n");
   }
   // @!

   
   // @@ GLAD loading procedures
   {
      gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
   }
   // @!


   // @@ GL setup
   {
      glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
      glEnable(GL_DEPTH_TEST);
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
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGBA,
		   GL_UNSIGNED_BYTE, texture_data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(texture_data);
   }
   // @!

   
   // @@ compiling shaders
   GLuint toy_box_shader_program;
   GLuint light_shader_program;
   GLint toy_box_shader_MVP_id;
   GLint light_shader_MVP_id;
   {
      // @@ toy box shader
      std::string toy_box_vert_path{"shaders/object.vert"};
      std::string toy_box_frag_path{"shaders/object.frag"};
      compile_shader_program(toy_box_vert_path, toy_box_frag_path, &toy_box_shader_program);
      
      glUseProgram(toy_box_shader_program);
      toy_box_shader_MVP_id = glGetUniformLocation(toy_box_shader_program, "MVP");
      // @!


      // @@ light shader
      std::string light_vert_path{"shaders/light.vert"};
      std::string light_frag_path{"shaders/light.frag"};
      compile_shader_program(light_vert_path, light_frag_path, &light_shader_program);

      glUseProgram(light_shader_program);
      light_shader_MVP_id = glGetUniformLocation(light_shader_program, "MVP");
      // @!
   }
   // @!

   
   // @@ creating light source
   GLuint light_VAO;
   glm::mat4 light_model_matrix;
   glm::mat4 light_MVP;
   glm::vec3 light_color;
   {
      glGenVertexArrays(1, &light_VAO);
      glBindVertexArray(light_VAO);
   
      GLuint light_VBO;
      {
	 float vertices[] = {
	    -0.5f, -0.5f, -0.5f,
	    0.5f, -0.5f, -0.5f,
	    0.5f,  0.5f, -0.5f,
	    0.5f,  0.5f, -0.5f,
	    -0.5f,  0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,

	    -0.5f, -0.5f,  0.5f,
	    0.5f, -0.5f,  0.5f,
	    0.5f,  0.5f,  0.5f,
	    0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f, -0.5f,  0.5f,

	    -0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f, -0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,

	    0.5f,  0.5f,  0.5f,
	    0.5f,  0.5f, -0.5f,
	    0.5f, -0.5f, -0.5f,
	    0.5f, -0.5f, -0.5f,
	    0.5f, -0.5f,  0.5f,
	    0.5f,  0.5f,  0.5f,

	    -0.5f, -0.5f, -0.5f,
	    0.5f, -0.5f, -0.5f,
	    0.5f, -0.5f,  0.5f,
	    0.5f, -0.5f,  0.5f,
	    -0.5f, -0.5f,  0.5f,
	    -0.5f, -0.5f, -0.5f,

	    -0.5f,  0.5f, -0.5f,
	    0.5f,  0.5f, -0.5f,
	    0.5f,  0.5f,  0.5f,
	    0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f, -0.5f,
	 };

	 glGenBuffers(1, &light_VBO);
	 glBindBuffer(GL_ARRAY_BUFFER, light_VBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      }

      // @@ vertex attributes
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
      glEnableVertexAttribArray(0);
      // @!
      
      // @@ model matrix setup
      light_model_matrix = glm::mat4(1.0f);
      light_model_matrix = glm::translate(light_model_matrix, glm::vec3(4.0f, 1.0f, -2.0f));
      // @!

      // @@ shader stuff
      light_color = glm::vec3(1.0f, 1.0f, 1.0f);
      glUseProgram(light_shader_program);
      glUniform3f(glGetUniformLocation(light_shader_program, "light_color"),
		  light_color.r, light_color.g, light_color.b);
      // @!
   }
   // @!


   // @@ loading and creating toy box
   GLuint toy_box_VAO;
   glm::mat4 toy_box_model_matrix;
   glm::mat4 toy_box_MVP;
   {
      glGenVertexArrays(1, &toy_box_VAO);
      glBindVertexArray(toy_box_VAO);
   
      GLuint toy_box_VBO;
      {
	 float vertices[] = {
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 };
	 // GLfloat vertices[] = {
	 //    // positions         // colors         // tex coords
	 //    0.5f,  -0.5f, 0.0f,  1.0f, 1.0f, 0.0f, texture_scale * 0.0f, texture_scale * 0.0f,
	 //    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f, texture_scale * 1.0f, texture_scale * 0.0f,
	 //    0.0f,   0.5f, 0.0f,  1.0f, 0.0f, 1.0f, texture_scale * 0.5f, texture_scale * 1.0f,
	 // };

	 glGenBuffers(1, &toy_box_VBO);
	 glBindBuffer(GL_ARRAY_BUFFER, toy_box_VBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      }

      // @@ vertex attributes
      // positions
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
      glEnableVertexAttribArray(0);

      // tex coords
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
			    (void *)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
      // @!

      // @@ model matrix setup
      toy_box_model_matrix = glm::mat4(1.0f);
      // @!

      // @@ shader stuff
      glUseProgram(toy_box_shader_program);
      glUniform1i(glGetUniformLocation(toy_box_shader_program, "container_texture"), 0);
      glUniform1i(glGetUniformLocation(toy_box_shader_program, "face_texture"), 1);
      glUniform3f(glGetUniformLocation(toy_box_shader_program, "light_color"),
		  light_color.r, light_color.g, light_color.b);
      // @!
   }
   // @!

   
   // @@ camera VP setup
   glm::mat4 view = glm::mat4(1.0f);
   view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
   glm::mat4 projection;
   projection = glm::perspective(glm::radians(config_data.fov_degrees),
				 (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
   // @!


   // @@ loop variables
   float delta_time = 0.0f;
   float last_frame_time = 0.0f;

   float pitch = 0.0f;
   float yaw = 0.0f;
   
   glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
   glm::vec3 camera_face_dir = glm::vec3(0.0f, 0.0f, -1.0f);
   
   InputData input_data;
   {
      double d_mouse_xpos;
      double d_mouse_ypos;
      glfwGetCursorPos(window, &d_mouse_xpos, &d_mouse_ypos);
      
      input_data.last_mouse_xpos = d_mouse_xpos;
      input_data.last_mouse_ypos = d_mouse_ypos;
   }
   // @!
   
   
   while(!glfwWindowShouldClose(window))
   {
      // @@ delta time calculations, must be done at BEGINNING OF FRAME!
      float current_frame_time = glfwGetTime();
      delta_time = current_frame_time - last_frame_time;
      last_frame_time = current_frame_time;
      // @!

      
      // @@ input
      {
	 input_data.w_key_press = false;
	 input_data.s_key_press = false;
	 input_data.a_key_press = false;
	 input_data.d_key_press = false;
      
	 if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
	    input_data.w_key_press = true;
	 }
	 if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
	    input_data.s_key_press = true;
	 }
	 if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
	    input_data.a_key_press = true;
	 }
	 if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
	    input_data.d_key_press = true;
	 }

	 double d_mouse_xpos;
	 double d_mouse_ypos;
	 glfwGetCursorPos(window, &d_mouse_xpos, &d_mouse_ypos);
	 input_data.mouse_xpos = d_mouse_xpos;
	 input_data.mouse_ypos = d_mouse_ypos;
      }
      // @!


      // @@ simulation
      yaw += config_data.mouse_sensitivity * (input_data.mouse_xpos - input_data.last_mouse_xpos);
      pitch += config_data.mouse_sensitivity * (input_data.mouse_ypos - input_data.last_mouse_ypos);
      if(pitch > 1.5533) { pitch = 1.5533; }
      else if(pitch < -1.5533) { pitch = -1.5533; }

      input_data.last_mouse_xpos = input_data.mouse_xpos;
      input_data.last_mouse_ypos = input_data.mouse_ypos;
      
      camera_face_dir = glm::vec3(sin(yaw) * cos(pitch), -sin(pitch), -cos(yaw) * cos(pitch));
      
      
      glm::vec3 move_dir = glm::vec3(0.0f, 0.0f, 0.0f);
      glm::vec3 camera_right_dir =
	 glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camera_face_dir));
      if(input_data.w_key_press == true) {
	 move_dir += glm::vec3(camera_face_dir.x, camera_face_dir.y, camera_face_dir.z);
      }
      if(input_data.s_key_press == true) {
	 move_dir += glm::vec3(-camera_face_dir.x, -camera_face_dir.y, -camera_face_dir.z);
      }

      if(input_data.a_key_press == true) {
	 move_dir += glm::vec3(camera_right_dir.x, 0.0f, camera_right_dir.z);
      }
      if(input_data.d_key_press == true) {
	 move_dir += glm::vec3(-camera_right_dir.x, 0.0f, -camera_right_dir.z);
      }

      if(glm::length(move_dir) > 0.0f) {
	 move_dir = glm::normalize(move_dir);
	 camera_pos += move_dir * config_data.move_speed * delta_time;
      }
      // @!

      
      // @@ rendering
      glm::mat4 view;
      view = glm::lookAt(camera_pos,
			 camera_pos + camera_face_dir,
			 glm::vec3(0.0, 1.0, 0.0));

      
      glClearColor(0.1f, 0.4f, 0.5f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_1_id);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture_2_id);
      
      glUseProgram(toy_box_shader_program);
      toy_box_MVP = projection * view * toy_box_model_matrix;
      glUniformMatrix4fv(toy_box_shader_MVP_id, 1, GL_FALSE, glm::value_ptr(toy_box_MVP));
      glBindVertexArray(toy_box_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      
      glUseProgram(light_shader_program);
      light_MVP = projection * view * light_model_matrix;
      glUniformMatrix4fv(light_shader_MVP_id, 1, GL_FALSE, glm::value_ptr(light_MVP));
      glBindVertexArray(light_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
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


void compile_shader_program(std::string vert_path, std::string frag_path, GLuint *shader_program) {
   int success;
   char info_log[2048];
   
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
   
   *shader_program = glCreateProgram();
   glAttachShader(*shader_program, vert_shader);
   glAttachShader(*shader_program, frag_shader);
   glLinkProgram(*shader_program);

   glGetProgramiv(*shader_program, GL_LINK_STATUS, &success);
   if(!success) {
      glGetProgramInfoLog(*shader_program, 2048, NULL, info_log);
      std::cerr << "ERROR: shader link failed." << '\n';
      std::cout << info_log << '\n';
      exit(1);
   }
   
   glDeleteShader(vert_shader);
   glDeleteShader(frag_shader);

}
