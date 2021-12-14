/**
wrapper_glfw.h
Modified from the OpenGL GLFW example to provide a wrapper GLFW class
Iain Martin August 2014
*/
#pragma once

#include <string>

/* Inlcude GL_Load and GLFW */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GLWrapper
{
private:
    int width;
    int height;
    const char *title;
    double fps;
    void (*loop)();
    bool running;
    GLFWwindow *window;

public:
    GLWrapper(int width, int height, const char *title);
    ~GLWrapper();

    void displayVersion();

    /* Callback registering functions */
    void setLoopCallback(void (*f)());
    void setReshapeCallback(void (*f)(GLFWwindow *window, int w, int h));
    void setKeyCallback(void (*f)(GLFWwindow *window, int key, int scanCode, int action,
                                  int mods));
    void setErrorCallback(void (*f)(int error, const char *description));

    int eventLoop();
    GLFWwindow *getWindow();

    int getWidth() { return this->width; }
    /* void setWidth(int width) { this->width = width; } */

    int getHeight() { return this->height; }
    /* void setHeight(int height) { this->height = height; } */

    double getFPS() { return this->fps; }
    void setFPS(double fps) { this->fps = fps; }
};
