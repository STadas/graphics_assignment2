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

/* lights */
Light *mainLight;
glm::vec3 backgroundColor;

/* shadows */
Texture shadowMap;
GLuint depthMapFBO;
const GLfloat farPlane = 200.f;
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

/* shader programs */
Shader *emissiveShader;
Shader *defaultShader;
Shader *depthShader;

/* Shader *texturedShader; */
Shader *texturedNMapShader;

Shader *defaultShadowsShader;
Shader *texturedNMapShadowsShader;

/* pairs of non-special static models with shaders */
std::map<Model*, Shader*> staticWithLighting;

/* special models */
Model *tvBodyModel;
Model *tvScreenModel;
Model *sphereModel;

/* transform helpers */
std::pair<transforms_t, transforms_t> tvTransforms;

/* animation helpers */
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

    /* cull back-facing triangles */
    glEnable(GL_CULL_FACE);

    /* set basic variables */
    scrWidth = glw->getWidth();
    scrHeight = glw->getHeight();

    /* make camera */
    camera = new OrbitCamera(glm::vec3(0.f, 0.f, 1.f));

    /* make lights */
    backgroundColor = glm::vec3(0.4f, 0.4f, 0.8f);
    mainLight = new Light(glm::vec4(0.f, 1.f, 0.f, 1.f), 0.4f * backgroundColor, 1.f, 1.f);

    /* set animation variables */
    speed = 1.f;

    /* load shaders */
    defaultShader =
        new Shader("assets/shaders/default.vert", "assets/shaders/default.frag");
    emissiveShader =
        new Shader("assets/shaders/basic.vert", "assets/shaders/emissive.frag");
    depthShader =
        new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag", "assets/shaders/depth.geom");
    /* texturedShader = */
    /*     new Shader("assets/shaders/default.vert", "assets/shaders/textured.frag"); */
    texturedNMapShader =
        new Shader("assets/shaders/tangents.vert", "assets/shaders/textured_nmap.frag");
    defaultShadowsShader =
        new Shader("assets/shaders/default_shadows.vert", "assets/shaders/default_shadows.frag");
    texturedNMapShadowsShader =
        new Shader("assets/shaders/tangents_shadows.vert", "assets/shaders/textured_nmap_shadows.frag");

    /* load models */
    staticWithLighting = {
        { new Model("assets/objs/floor/floor.obj"), texturedNMapShadowsShader },
        { new Model("assets/objs/curtain/curtain.obj"), defaultShadowsShader },
        { new Model("assets/objs/stage_lights/stage_lights.obj"), defaultShadowsShader },
    };
    sphereModel = new Model("assets/objs/light_sphere/light_sphere.obj");

    tvBodyModel = new Model("assets/objs/tv/tv_body.obj");
    tvScreenModel =  new Model("assets/objs/tv/tv_screen.obj");
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
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap.id, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* add depth texture to all lit models for it to be applied later */
    for (auto &modelShader : staticWithLighting)
    {
        for (auto &mesh : modelShader.first->meshes)
        {
            mesh.textures.push_back(shadowMap);
        }
    }
}

/**
 * wrapper function for stuff needed to be done outside of renders but inside the loop.
 * needs to have at least one render() call to actually draw anything
 */
void loop()
{
    /* set background color to blue */
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.f);

    /* clear the color and frame buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* depth render */
    glm::mat4 shadowProjection = glm::perspective(glm::radians(90.f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 0.1f, farPlane);
    std::vector<glm::mat4> shadowTransforms;
    glm::vec3 lightPosV3 = glm::vec3(mainLight->position);

    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosV3, lightPosV3 + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosV3, lightPosV3 + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosV3, lightPosV3 + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosV3, lightPosV3 + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosV3, lightPosV3 + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPosV3, lightPosV3 + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    depthShader->use();
    for (unsigned int i = 0; i < 6; ++i)
        depthShader->setMat4("shadow_mat[" + std::to_string(i) + "]", shadowTransforms[i]);
    depthShader->setFloat("far_plane", farPlane);
    depthShader->setVec3("light_pos", lightPosV3);
    render(depthShader); // render depth
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* regular render */
    glViewport(0, 0, scrWidth, scrHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* projection matrix */
    camera->projection = glm::perspective(glm::radians(90.f), scrWidth / scrHeight, 0.1f, 100.f);

    /* orbit camera (view matrix) */
    camera->view = lookAt(camera->position, camera->target, camera->up);
    camera->view = translate(camera->view, glm::vec3(0.f, 0.f, -camera->radius));
    camera->view = rotate(camera->view, glm::radians(camera->pitch), glm::vec3(1, 0, 0));
    camera->view = rotate(camera->view, glm::radians(camera->yaw), glm::vec3(0, 1, 0));

    render();
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

    glDepthMask(GL_FALSE);

    /* draw non-opaque stuff here */

    glDepthMask(GL_TRUE);
}

/**
 * draw imported .obj models that we're not planning on moving
 */
void drawStatic(std::stack<glm::mat4> &model, Shader *overrideShader)
{
    if (overrideShader != nullptr)
        for (auto &modelShader : staticWithLighting)
            modelShader.first->draw(overrideShader, camera, mainLight, model.top(), farPlane);
    else
        for (auto &modelShader : staticWithLighting)
            modelShader.first->draw(modelShader.second, camera, mainLight, model.top(), farPlane);
}

void drawTVs(std::stack<glm::mat4> &model, Shader *overrideShader)
{
    for (int i = 0; i < tvTransforms.first.size(); i++)
    {
        model.push(model.top());
        {
            model.top() = translate(model.top(), tvTransforms.first[i]);
            model.top() = rotate(model.top(), tvTransforms.second[i].x, glm::vec3(1, 0, 0));
            model.top() = rotate(model.top(), tvTransforms.second[i].y, glm::vec3(0, 1, 0));
            model.top() = rotate(model.top(), tvTransforms.second[i].z, glm::vec3(0, 0, 1));

            if (overrideShader != nullptr)
            {
                tvBodyModel->draw(overrideShader, camera, mainLight, model.top(), farPlane);
                tvScreenModel->draw(overrideShader, camera, mainLight, model.top(), farPlane);
            }
            else
            {
                tvBodyModel->draw(defaultShadowsShader, camera, mainLight, model.top(), farPlane);
                tvScreenModel->draw(emissiveShader, camera, mainLight, model.top(), farPlane);
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
    model.top() = translate(model.top(), glm::vec3(mainLight->position));
    model.top() = scale(model.top(), glm::vec3(0.1f, 0.1f, 0.1f));

    sphereModel->draw(emissiveShader, camera, mainLight, model.top(), farPlane);
}

/**
 * handle window resizing. The new window size is given, in pixels.
 */
void reshape(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    /* store aspect ratio to use for our perspective projection */
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
    glw->setLoopCallback(loop);
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

    /* delete misc pointers */
    delete glw;
    delete camera;
    delete mainLight;

    /* delete shader pointers */
    delete defaultShader;
    delete emissiveShader;
    /* delete texturedShader; */
    delete texturedNMapShader;
    delete depthShader;
    delete defaultShadowsShader;
    delete texturedNMapShadowsShader;

    /* delete model pointers */
    delete sphereModel;
    /* https://stackoverflow.com/a/27175070 */
    for (auto it = staticWithLighting.begin(); it != staticWithLighting.end();)
    {
        delete it->first;
        it = staticWithLighting.erase(it);
    }

    return 0;
}
