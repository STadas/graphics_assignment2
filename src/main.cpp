/** main.h
 * main file
 * Tadas Saltenis November 2021
 * NOTE: modified example from Iain Martin
 */

/* TODO: fixed framerate */

#include "main.h"

/* window variables */
GLfloat aspectRatio;

/* camera */
OrbitCamera *camera;

/* lights */
Light *mainLight;

/* shader programs */
Shader *defaultShader;
Shader *emissiveShader;
Shader *texturedShader;
Shader *texturedWithNormalsShader;

/* imported models */
Model *curtainModel;
Model *floorModel;
Model *stageLightsModel;
Model *sphereModel;

/* animation helper variables */
GLfloat speed;

/**
 * initialize glew, set variables etc.
 */
void init(GLWrapper *glw)
{
    /* enable depth test  */
    glEnable(GL_DEPTH_TEST);

    /* make sure multisampling is enabled */
    glEnable(GL_MULTISAMPLE);

    /* blending (transparency stuff) */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    /* set basic variables */
    aspectRatio = GLfloat(glw->getWidth()) / GLfloat(glw->getHeight());

    /* make camera */
    camera = new OrbitCamera(glm::vec3(0.f, 0.f, 1.f));

    /* make lights */
    mainLight = new Light(glm::vec4(0.f, 1.f, 1.f, 1.f), 0.01f, 1.f, 1.f);

    /* set animation variables */
    speed = 1.f;

    /* load shaders */
    defaultShader =
        new Shader("assets/shaders/default.vert", "assets/shaders/default.frag");
    texturedShader =
        new Shader("assets/shaders/default.vert", "assets/shaders/textured.frag");
    texturedWithNormalsShader =
        new Shader("assets/shaders/tangents.vert", "assets/shaders/textured_normalmap.frag");
    emissiveShader =
        new Shader("assets/shaders/basic.vert", "assets/shaders/emissive.frag");

    /* load models */
    curtainModel = new Model("assets/objs/curtain/curtain.obj");
    floorModel = new Model("assets/objs/floor/floor.obj");
    stageLightsModel = new Model("assets/objs/stage_lights/stage_lights.obj");
    sphereModel = new Model("assets/objs/light_sphere/light_sphere.obj");
}

/**
 * the draw loop that runs until the application is stopped
 */
void render()
{
    /* set background color to blue */
    glClearColor(0.f, 0.f, 1.f, 1.f);

    /* /1* set background color to white *1/ */
    /* glClearColor(1.f, 1.f, 1.f, 1.f); */

    /* set background color to black */
    /* glClearColor(0.f, 0.f, 0.f, 1.f); */

    /* clear the color and frame buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* reset projection */
    camera->projection = glm::perspective(glm::radians(70.f), aspectRatio, 0.3f, 100.f);

    /* orbit camera */
    camera->view = lookAt(camera->position, camera->target, camera->up);
    camera->view = translate(camera->view, glm::vec3(0.f, 0.f, -camera->radius));
    camera->view = rotate(camera->view, glm::radians(camera->pitch), glm::vec3(1, 0, 0));
    camera->view = rotate(camera->view, glm::radians(camera->yaw), glm::vec3(0, 1, 0));

    /* model matrix */
    std::stack<glm::mat4> model;
    model.push(glm::mat4(1.0f));

    /* draw stuff */
    model.push(model.top());
    drawStatic(model);
    model.pop();

    model.push(model.top());
    drawMainLightSphere(model);
    model.pop();

    glDepthMask(GL_FALSE);

    /* draw non-opaque stuff */

    glDepthMask(GL_TRUE);
}

/**
 * draw imported .obj models that we're not planning on moving
 */
void drawStatic(std::stack<glm::mat4> &model)
{
    model.top() = translate(model.top(), glm::vec3(0.f, 0.f, 0.f));

    floorModel->draw(texturedWithNormalsShader, camera, mainLight, model.top());
    curtainModel->draw(defaultShader, camera, mainLight, model.top());
    stageLightsModel->draw(defaultShader, camera, mainLight, model.top());
}

/**
 * draw main light sphere
 */
void drawMainLightSphere(std::stack<glm::mat4> &model)
{
    model.top() = translate(model.top(), glm::vec3(mainLight->position));
    model.top() = scale(model.top(), glm::vec3(0.1f, 0.1f, 0.1f));

    sphereModel->draw(emissiveShader, camera, mainLight, model.top());
}

/**
 * handle window resizing. The new window size is given, in pixels.
 */
void reshape(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    /* store aspect ratio to use for our perspective projection */
    aspectRatio = GLfloat(w) / GLfloat(h);
}

/**
 * handle key presses
 */
void keyCallback(GLFWwindow *window, int key, int s, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (mods == GLFW_MOD_SHIFT)
        speed *= 2.f;
    
    if (mods == GLFW_MOD_CONTROL)
        speed /= 2.f;

    if (key == 'W' && camera->pitch < 90.f)
        camera->pitch += 1.f * speed;
    if (key == 'S' && camera->pitch > -90.f)
        camera->pitch -= 1.f * speed;
    if (key == 'A')
        camera->yaw += 1.f * speed;
    if (key == 'D')
        camera->yaw -= 1.f * speed;
    if (key == 'E' && camera->radius > 0.f && action == GLFW_PRESS)
        camera->radius -= 1.f * speed;
    if (key == 'Q' && action == GLFW_PRESS)
        camera->radius += 1.f * speed;

    if (key == 'I')
        mainLight->position.z -= 0.05f * speed;
    if (key == 'K')
        mainLight->position.z += 0.05f * speed;
    if (key == 'J')
        mainLight->position.x -= 0.05f * speed;
    if (key == 'L')
        mainLight->position.x += 0.05f * speed;
    if (key == 'U')
        mainLight->position.y += 0.05f * speed;
    if (key == 'O')
        mainLight->position.y -= 0.05f * speed;
}

int main()
{
    GLWrapper *glw = new GLWrapper(1024, 768, "Lab3 start example");

    /* register the callback functions */
    glw->setRendererCallback(render);
    glw->setKeyCallback(keyCallback);
    glw->setReshapeCallback(reshape);

    /* output the OpenGL vendor and version */
    glw->displayVersion();

    init(glw);

    std::cout
        << "Shift - speed everything up" << std::endl
        << "Ctrl - slow everything down" << std::endl
        << "W A S D - rotate camera" << std::endl
        << "Q E - control zoom" << std::endl
        << "J L - move light in X axis" << std::endl
        << "U O - move light in Y axis" << std::endl
        << "I K - move light in Z axis" << std::endl;

    glw->eventLoop();

    delete glw;
    delete camera;
    delete mainLight;

    delete defaultShader;
    delete emissiveShader;

    delete curtainModel;
    delete sphereModel;

    return 0;
}
