/** main.h
 * main file
 * Tadas Saltenis December 2021
 */

/* TODO: fixed framerate */

#include "main.h"

/* window variables */
GLfloat scrWidth;
GLfloat scrHeight;

/* camera */
OrbitCamera *camera;
const GLfloat MIN_ZOOM = 1.f, MAX_ZOOM = 12.f;
const GLfloat MIN_PITCH = -15.f, MAX_PITCH = 90.f;

/* lights */
const GLfloat MIN_LIGHT_Y = -3.f, MAX_LIGHT_Y = 5.f;
bool orbitLight;
Light *mainLight;
glm::vec3 backgroundColor;

/* shadows */
Texture shadowMap;
GLuint depthMapFBO;
const GLfloat SHADOW_FAR = 100.f;
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

/* shader programs */
Shader *depthShader;
Shader *discreteNoiseShader;

Shader *emissiveShader;
Shader *emissiveTexturedShader;

/* Shader *defaultShader; */
/* Shader *texturedShader; */
/* Shader *texturedNMapShader; */

Shader *defaultShadowsShader;
Shader *texturedNMapShadowsShader;

/* pairs of non-special static models with shaders */
std::map<Model*, Shader*> staticModels;

/* special models */
Model *tvBodyModel;
Model *tvScreenModel;
Model *sphereModel;
Model *backgroundCubeModel;

/* additional transform variables */
std::pair<transforms_t, transforms_t> tvTransforms;

/* animation variables */
GLfloat speed;
GLfloat currTime;
std::vector<GLfloat> tvFloatModifiers;

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

    /* cull back-facing triangles */
    glEnable(GL_CULL_FACE);

    /* set basic variables */
    scrWidth = glw->getWidth();
    scrHeight = glw->getHeight();

    /* make camera */
    camera = new OrbitCamera(glm::vec3(0.f, 0.f, MAX_ZOOM * 0.6f));
    camera->pitch = 30.f;

    /* make lights */
    backgroundColor = glm::vec3(0.6f, 0.4f, 0.8f);
    mainLight = new Light(glm::vec4(0.f, 0.0f, 0.f, 1.f), 0.2f * backgroundColor, 1.f, 1.f);
    orbitLight = true;

    /* set animation variables */
    speed = 1.f;

    /* load shaders */
    /* defaultShader = */
    /*     new Shader("assets/shaders/default.vert", "assets/shaders/default.frag"); */
    discreteNoiseShader =
        new Shader("assets/shaders/default.vert", "assets/shaders/discrete_noise.frag");
    depthShader =
        new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag", "assets/shaders/depth.geom");
    emissiveShader =
        new Shader("assets/shaders/default.vert", "assets/shaders/emissive.frag");
    emissiveTexturedShader =
        new Shader("assets/shaders/default.vert", "assets/shaders/emissive_textured.frag");
    /* texturedShader = */
    /*     new Shader("assets/shaders/default.vert", "assets/shaders/textured.frag"); */
    /* texturedNMapShader = */
    /*     new Shader("assets/shaders/tangents.vert", "assets/shaders/textured_nmap.frag"); */
    defaultShadowsShader =
        new Shader("assets/shaders/default_shadows.vert", "assets/shaders/default_shadows.frag");
    texturedNMapShadowsShader =
        new Shader("assets/shaders/tangents_shadows.vert", "assets/shaders/textured_nmap_shadows.frag");

    /* load models */
    staticModels = {
        { new Model("assets/objs/floor/floor.obj"), texturedNMapShadowsShader },
        { new Model("assets/objs/curtain/curtain.obj"), defaultShadowsShader },
        { new Model("assets/objs/stage_lights/stage_lights.obj"), defaultShadowsShader },
        { new Model("assets/objs/skybox_sphere/skybox_sphere.obj"), emissiveTexturedShader },
    };
    sphereModel = new Model("assets/objs/light_sphere/light_sphere.obj");
    tvBodyModel = new Model("assets/objs/tv/tv_body.obj");
    tvScreenModel =  new Model("assets/objs/tv/tv_screen.obj");

    /* parse tv transforms from file */
    tvTransforms = parseTransformsFile("assets/objs/tv/transforms.txt");

    /* prepare depth cubemap texture */
    shadowMap.type = "cube_map";
    glGenTextures(1, &(shadowMap.id));
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap.id);
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /* prepare depth framebuffer */
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap.id, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* add depth texture to all lit models for it to be applied later */
    for (auto &modelShader : staticModels)
        for (auto &mesh : modelShader.first->meshes)
            mesh.textures.push_back(shadowMap);
}

/**
 * wrapper function for stuff needed to be done outside of renders but inside the loop.
 * needs to have at least one render() call to actually draw anything
 */
void loop()
{
    currTime = glfwGetTime();

    /* set background color to blue */
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.f);

    /* clear the color and frame buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* depth render */
    glm::mat4 shadowProjection = glm::perspective(glm::radians(90.f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 0.1f, SHADOW_FAR);
    std::vector<glm::mat4> shadowTransforms;

    shadowTransforms.push_back(shadowProjection * glm::lookAt(mainLight->position, mainLight->position + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(mainLight->position, mainLight->position + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(mainLight->position, mainLight->position + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(mainLight->position, mainLight->position + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(mainLight->position, mainLight->position + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(mainLight->position, mainLight->position + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader->use();
        for (unsigned int i = 0; i < 6; ++i)
            depthShader->setMat4("shadow_mat[" + std::to_string(i) + "]", shadowTransforms[i]);
        depthShader->setFloat("far_plane", SHADOW_FAR);
        depthShader->setVec3("light_pos", mainLight->position);
        render(depthShader); // render depth
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* normal render */
    glViewport(0, 0, scrWidth, scrHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->projection = glm::perspective(glm::radians(90.f), scrWidth / scrHeight, 0.1f, 200.f);
    /* orbit camera */
    camera->view = lookAt(camera->position, camera->target, camera->up);
    camera->view = rotate(camera->view, glm::radians(camera->pitch), glm::vec3(1, 0, 0));
    camera->view = rotate(camera->view, glm::radians(camera->yaw), glm::vec3(0, 1, 0));

    /* orbit light on Z axis over time */
    if (orbitLight){
        mainLight->position.x = sin(currTime * speed);
        mainLight->position.z = cos(currTime * speed);
    }
    render(); // render normally
}

/**
 * the draw function that should be used in a loop that runs until the application is stopped
 */
void render(Shader *overrideShader)
{
    /* model matrix */
    std::stack<glm::mat4> model;
    model.push(glm::mat4(1.0f));

    /* draw opaque stuff */

    model.push(model.top());
    drawStatic(model, overrideShader);
    model.pop();

    model.push(model.top());
    drawTVs(model, overrideShader);
    model.pop();

    model.push(model.top());
    drawMainLightSphere(model);
    model.pop();

    /* glDepthMask(GL_FALSE); */

    /* /1* draw non-opaque stuff *1/ */

    /* glDepthMask(GL_TRUE); */
}

/**
 * draw imported .obj models that we're not planning on moving
 */
void drawStatic(std::stack<glm::mat4> &model, Shader *overrideShader)
{
    if (overrideShader != nullptr)
        for (auto &modelShader : staticModels)
            modelShader.first->draw(overrideShader, camera, mainLight, model.top(), SHADOW_FAR);
    else
        for (auto &modelShader : staticModels)
            modelShader.first->draw(modelShader.second, camera, mainLight, model.top(), SHADOW_FAR);
}

/**
 * draw TV models using transforms read from a file
 * and apply a time-based modifier that pulses from the centre except Z axis
 **/
void drawTVs(std::stack<glm::mat4> &model, Shader *overrideShader)
{
    GLfloat timeMod = 1.5 + glm::sin(currTime * speed) / 16.f;
    glm::vec3 translationMod = glm::vec3(timeMod, 1.f, timeMod);

    discreteNoiseShader->use();
    discreteNoiseShader->setFloat("time", currTime);

    for (int i = 0; i < tvTransforms.first.size(); i++)
    {
        model.push(model.top());
        {
            model.top() = translate(model.top(), tvTransforms.first[i] * translationMod);
            model.top() = rotate(model.top(), tvTransforms.second[i].x, glm::vec3(1.f, 0.f, 0.f));
            model.top() = rotate(model.top(), tvTransforms.second[i].y, glm::vec3(0.f, 1.f, 0.f));
            model.top() = rotate(model.top(), tvTransforms.second[i].z, glm::vec3(0.f, 0.f, 1.f));

            if (overrideShader != nullptr)
            {
                tvBodyModel->draw(overrideShader, camera, mainLight, model.top(), SHADOW_FAR);
                tvScreenModel->draw(overrideShader, camera, mainLight, model.top(), SHADOW_FAR);
            }
            else
            {
                tvBodyModel->draw(defaultShadowsShader, camera, mainLight, model.top(), SHADOW_FAR);
                tvScreenModel->draw(discreteNoiseShader, camera, mainLight, model.top(), SHADOW_FAR);
            }
        }
        model.pop();
    }
}

/**
 * draw main light sphere
 */
void drawMainLightSphere(std::stack<glm::mat4> &model)
{
    model.top() = translate(model.top(), mainLight->position);
    model.top() = scale(model.top(), glm::vec3(0.1f, 0.1f, 0.1f));

    sphereModel->draw(emissiveShader, camera, mainLight, model.top(), SHADOW_FAR);
}

/**
 * handle window resizing. The new window size is given, in pixels.
 */
void reshape(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    /* store the size to use later */
    scrWidth = GLfloat(w);
    scrHeight = GLfloat(h);
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

    if (key == 'W')
    {
        GLfloat newPitch = camera->pitch + 1.f * speed;
        camera->pitch = newPitch < MAX_PITCH ? newPitch : MAX_PITCH;
    }
    if (key == 'S')
    {
        GLfloat newPitch = camera->pitch - 1.f * speed;
        camera->pitch = newPitch > MIN_PITCH ? newPitch : MIN_PITCH;
    }
    if (key == 'A')
        camera->yaw += 1.f * speed;
    if (key == 'D')
        camera->yaw -= 1.f * speed;
    if (key == 'E')
    {
        GLfloat newZ = camera->position.z - 0.3f * speed;
        camera->position.z = newZ > MIN_ZOOM ? newZ : MIN_ZOOM;
    }
    if (key == 'Q')
    {
        GLfloat newZ = camera->position.z + 0.3f * speed;
        camera->position.z = newZ < MAX_ZOOM ? newZ : MAX_ZOOM;
    }

    if (key == 'N' && action == GLFW_PRESS)
        orbitLight = !orbitLight;

    if (key == 'K' && !orbitLight)
        mainLight->position.z += 0.05f * speed;
    if (key == 'I' && !orbitLight)
        mainLight->position.z -= 0.05f * speed;
    if (key == 'L' && !orbitLight)
        mainLight->position.x += 0.05f * speed;
    if (key == 'J' && !orbitLight)
        mainLight->position.x -= 0.05f * speed;
    if (key == 'U' && !orbitLight)
        mainLight->position.y += 0.05f * speed;
    if (key == 'O' && !orbitLight)
        mainLight->position.y -= 0.05f * speed;
}

int main()
{
    GLWrapper *glw = new GLWrapper(1024, 768, "Assignment 2");

    /* register the callback functions */
    glw->setLoopCallback(loop);
    glw->setKeyCallback(keyCallback);
    glw->setReshapeCallback(reshape);

    /* output the OpenGL vendor and version */
    glw->displayVersion();

    init(glw);

    std::cout
        << "Shift - speed everything up" << std::endl
        << "Ctrl - slow everything down" << std::endl
        << "W A S D - orbit camera" << std::endl
        << "Q E - control camera orbit radius" << std::endl
        << "N - toggle light orbit animation" << std::endl
        << "J L - move light along world X axis" << std::endl
        << "I K - move light along world Z axis" << std::endl
        << "U O - move light along world Y axis" << std::endl;

    glw->eventLoop();

    /* delete misc pointers */
    delete glw;
    delete camera;
    delete mainLight;

    /* delete shader pointers */
    /* delete defaultShader; */
    delete emissiveShader;
    delete emissiveTexturedShader;
    delete discreteNoiseShader;
    /* delete texturedShader; */
    /* delete texturedNMapShader; */
    delete depthShader;
    delete defaultShadowsShader;
    delete texturedNMapShadowsShader;

    /* delete model pointers */
    delete sphereModel;
    delete tvBodyModel;
    delete tvScreenModel;

    /* https://stackoverflow.com/a/27175070 */
    for (auto it = staticModels.begin(); it != staticModels.end();)
    {
        delete it->first;
        it = staticModels.erase(it);
    }

    return 0;
}
