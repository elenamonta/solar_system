#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "shader_maker.h"
#include "gui.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "Model.h"
#include "utils.h"
#include <random>
#include <iostream>

#define NR_PLANETS 9
#define PI 3.141592653589793

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int btn, int action, int mods);

GLFWwindow* window;

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;
float w_up = SCR_WIDTH, h_up = SCR_HEIGHT;

// camera
Camera camera(glm::vec3(0.0f, 4.0f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool trackBall = false, lastTrackball = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

vector<string> planetNames;
vector<Mesh> scene;
vector<Model> sceneObj;
int selected_obj = -1;

glm::mat4 projection;
glm::mat4 view;

string modelDir = "Model/";
string SkyboxDir = "SkyBoxes/";
string imageDir = "Texture/";
vector<string> pathTexture;
vector<int> texture;


int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _APPLE_
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    Gui imgui(window);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // load cubemap texture
    vector<std::string> faces
    {
        /*"right.jpg",
            "left.jpg",
            "top.jpg",
            "bottom.jpg",
            "front.jpg",
            "back.jpg"*/
            /*SkyboxDir + "posx.jpg",
            SkyboxDir + "negx.jpg",
            SkyboxDir + "posy.jpg",
            SkyboxDir + "negy.jpg",
            SkyboxDir + "posz.jpg",
            SkyboxDir + "negz.jpg"*/
            SkyboxDir + "/toDelete/1.png",
            SkyboxDir + "/toDelete/2.png",
            SkyboxDir + "/toDelete/3.png",
            SkyboxDir + "/toDelete/6.png",
            SkyboxDir + "/toDelete/4.png",
            SkyboxDir + "/toDelete/5.png"

    };
    unsigned int cubemapTexture = Texture().loadCubemap(faces, 0);


    // load mesh texture 
    pathTexture.push_back(imageDir + "2k_sun.jpg");
    pathTexture.push_back(imageDir + "2k_mercury.jpg");
    pathTexture.push_back(imageDir + "2k_venus.jpg");
    pathTexture.push_back(imageDir + "earth2k.jpg");
    pathTexture.push_back(imageDir + "2k_mars.jpg");
    pathTexture.push_back(imageDir + "2k_jupiter.jpg");
    pathTexture.push_back(imageDir + "2k_saturn.jpg");
    pathTexture.push_back(imageDir + "2k_uranus.jpg");
    pathTexture.push_back(imageDir + "2k_neptune.jpg");

    for (int i = 0; i < pathTexture.size(); i++) {
        texture.push_back(Texture::loadTexture(pathTexture[i].c_str(), 0).ID);
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shader program
    Shader lightingShader("vertexShader.glsl", "fragmentShader.glsl");
    Shader skyboxShader("vertexShader_CubeMap.glsl", "fragmentShader_CubeMap.glsl");
    Shader BBShader("vertexShaderBB.glsl", "fragmentShaderBB.glsl");

    // build the skybox as if it were a mesh
    Mesh sky(meshType::cubo, "", vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0f, 0.0f, 0.0f), 0.0f);


    // build all planets
    planetNames = { "Sole", "Mercurio", "Venere", "Terra", "Marte", "Giove", "Saturno", "Urano", "Nettuno" };

    vector<vec3> scaleValue = {
        {1.0, 1.0, 1.0}, //Sole
        {0.3, 0.3, 0.3}, //Mercurio
        {0.5, 0.5, 0.5}, //Venere
        {0.6, 0.6, 0.6}, //Terra
        {0.5, 0.5, 0.5}, //Marte
        {0.9, 0.9, 0.9}, //Giove
        {0.75, 0.75, 0.75}, //Saturno
        {0.6, 0.6, 0.6}, //Urano
        {0.3, 0.3, 0.3} //Nettuno

    };

    // build the sun as the ONLY customizable object within the system
    Mesh sfera(meshType::sfera, planetNames[0], vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0f, 0.0f, 0.0f), 0.0f);
    sfera.setMaterial(Material::getMaterial(MaterialType::RedPlastic));
    scene.push_back(sfera);

    // then build the other planets with their own texture
    for (int i = 1; i < NR_PLANETS; i++) {
        Mesh sfera(meshType::sfera, planetNames[i], vec3(i * 2.0, 0.0, 0.0), scaleValue[i], vec3(1.0f, 0.0f, 0.0f), 0.0f);
        sfera.setTexture(texture[i]);
        scene.push_back(sfera);
    }


    // build meshes obj and push them in sceneObj vector
    Model ufo((modelDir + "ufo.obj").c_str(), vec3(2.0f, 2.0f, 5.0f), vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), 340.0f);
    Model spaceship((modelDir + "Low Poly Spaceship.obj").c_str(), vec3(-2.0, 3.0, 2.0), vec3(0.5f, 0.5f, 0.5f), vec3(0.0, 1.0, 0.0), 35.f);

    sceneObj.push_back(ufo);
    sceneObj.push_back(spaceship);



    imgui.Initilize_IMGUI();


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        // update frequency control
        if (deltaTime > 0.016f) {
            lastFrame = currentFrame;

            //imgui
            imgui.my_interface();
            imgui.set_selected_id(selected_obj);

            // input
            processInput(window);

            // render
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            lightingShader.use();
            lightingShader.setVec3("viewPos", camera.Position);

            // set uniforms point lights
            lightingShader.setVec3("pointLights[0].position", imgui.lightPosition1);
            lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
            lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
            lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

            lightingShader.setVec3("pointLights[1].position", imgui.lightPosition2);
            lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
            lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
            lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);


            // view/projection transformations
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = camera.GetViewMatrix();
            lightingShader.setMat4("projection", projection);
            lightingShader.setMat4("view", view);

            // Skybox uniform
            glDepthMask(GL_FALSE);
            skyboxShader.use();
            skyboxShader.setMat4("projection", projection);
            skyboxShader.setMat4("view", view);
            glBindVertexArray(sky.VAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawElements(GL_TRIANGLES, sky.indices.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);


            // orbital movement
            for (int i = 0; i < NR_PLANETS; i++) {
                float angle = i * 0.2f;
                float radians = glm::radians(angle);
                float x = scene[i].positions.x * cos(radians) - scene[i].positions.z * sin(radians);
                float z = scene[i].positions.x * sin(radians) + scene[i].positions.z * cos(radians);

                scene[i].Model = glm::mat4(1.0f);
                scene[i].Model = translate(scene[i].Model, glm::vec3(x, scene[i].positions.y, z));
                scene[i].positions = vec3(x, scene[i].positions.y, z);
                scene[i].angle += angle;
                scene[i].Model = glm::rotate(scene[i].Model, glm::radians(scene[i].angle), glm::vec3(0.0f, 1.0f, 0.0f));
                scene[i].Model = scale(scene[i].Model, scaleValue[i]);
            }


            // set material from gui
            scene[0].setMaterial(Material::getMaterial(static_cast<MaterialType>(imgui.selectedMaterialType)));

            // draw all meshes
            scene[0].setShader(static_cast<shaderOpt>(imgui.selectedShader));
            scene[0].draw(lightingShader, 0.0f);

            for (int i = 1; i < NR_PLANETS; i++)
                scene[i].draw(lightingShader, 1.0f);

            for (int i = 0; i < sceneObj.size(); i++)
                sceneObj[i].draw(lightingShader, static_cast<shaderOpt>(imgui.selectedShader), BBShader);

            system("cls");

            // collision detections
            for (int i = 0; i < scene.size(); i++) {
                if (Utils().isColliding(camera.Position, scene[i])) {
                    camera.Position += glm::normalize(camera.Position - scene[i].positions);
                    camera.Direction = glm::normalize(camera.Target - camera.Position);
                    camera.Target = camera.Position + camera.Direction;
                }
            }

            for (int i = 0; i < sceneObj.size(); i++) {
                if (Utils().isCollidingObj(camera.Position, sceneObj[i]) && Utils().rayBoxIntersection(sceneObj[i].min_BB, sceneObj[i].max_BB, sceneObj[i].positionVec, camera.Direction)) {
                    camera.Position += glm::normalize(camera.Position - sceneObj[i].positionVec);
                    camera.Direction = glm::normalize(camera.Target - camera.Position);
                    camera.Target = camera.Position + camera.Direction;
                    
                    /*cout << sceneObj[i].min_BB.x << " " << sceneObj[i].min_BB.y << " " << sceneObj[i].min_BB.z << endl;
                    cout << sceneObj[i].max_BB.x << " " << sceneObj[i].max_BB.y << " " << sceneObj[i].max_BB.z << endl;

                    cout << i << endl;*/
                }
            }
            
            // check gui trackball flag 
            camera.setTrackballMode(imgui.get_trackball_mode());
            trackBall = imgui.get_trackball_mode();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    // de-allocate all resources
    for (int i = 0; i < scene.size(); i++)
        scene[i].DESTROY_VAO();

    for (int i = 0; i < sceneObj.size(); i++)
        sceneObj[i].clear_objModel();


    imgui.close_GUI();

    glfwTerminate();
    return 0;
}

// navigating scenes using the WASD key
// when a collision occurs, the camera is nudged slightly in the opposite direction of the key press
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT);
    }


}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// navigating scenes using mouse, depending on the mode specified by imgui
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    // block camera movement if i'm over imgui window 
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        return;
    }

    // when switch camera mode, reset camera position 
    if (trackBall != lastTrackball) {
        camera.resetPosition(glm::vec3(0.0f, 4.0f, 15.0f));
    }

    if (trackBall) {
        camera.RotateAround(SCR_WIDTH, SCR_HEIGHT, xpos, ypos, lastX, lastY);
    }
    else {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    lastTrackball = trackBall;
    lastX = xpos;
    lastY = ypos;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// this function is used in mouse_button_callback function
vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {
    //La funzione get_ray_from_mouse calcola il raggio che parte dalla posizione della telecamera
    //  e passa attraverso il punto sullo schermo corrispondente alla posizione del mouse.

    SCR_HEIGHT = h_up;
    SCR_WIDTH = w_up;
    mouse_y = SCR_HEIGHT - mouse_y;

    // mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]  
    float ndc_x = (2.0f * mouse_x) / SCR_WIDTH - 1.0;
    float ndc_y = (2.0f * mouse_y) / SCR_HEIGHT - 1.0;

    //Nello spazio di clippling z pi  piccola, oggetto pi  vicino all'osservatore, quindi si pone la z a - 1, 
    // posizionando il punto sul piano vicino del frustum.
    // Questo significa che il raggio che stiamo calcolando partir  dalla telecamera e si diriger  
    // verso il punto pi  vicino visibile sullo schermo.
    float ndc_z = -1.0f;

    // Coordinate nel clip space 
    vec4 P_clip = vec4(ndc_x, ndc_y, ndc_z, 1.0);

    // Le coordinate nell' eye space si ottengono premoltiplicando per l'inversa della matrice Projection.
    vec4 ViewModelp = glm::inverse(projection) * P_clip;

    // le coordinate nel world space: si ottengono premoltiplicando per l'inversa della matrice di Vista 

    ViewModelp.w = 1;
    vec4 Pw = inverse(view) * ViewModelp;

    //Il vettore del raggio viene calcolato sottraendo la posizione della telecamera SetupTelecamera.position dal punto Pw nel world space.
    vec3 ray_wor = normalize(vec3(Pw) - vec3(camera.Position));

    return ray_wor;
}

// this function is used in mouse_button_callback function. 
// return true if ray, determinated by center and a direction, cross a sphere
bool ray_sphere(vec3 O, vec3 d, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

    vec3 dist_sfera = O - sphere_centre_wor;
    float b = dot(dist_sfera, d);
    float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

    float delta = b * b - cc;

    if (delta < 0)  //Il raggio non interseca la sfera
        return false;
    //Calcolo i valori di t per cui il raggio interseca la sfera e restituisco il valore dell'intersezione 
    //pi  vicina all'osservatore (la t pi  piccola)
    if (delta > 0.0f) {
        //calcola le due intersezioni
        float t_a = -b + sqrt(delta);
        float t_b = -b - sqrt(delta);
        *intersection_distance = t_b;

        //Caso di intersezioni dietro l'osservatore
        if (t_a < 0.0) {
            if (t_b < 0)
                return false;
        }

        return true;
    }
    //Caso in cui il raggio   tangente alla sfera: un interesezione con molteplicit  doppia.
    if (delta == 0) {
        float t = -b + sqrt(delta);
        if (t < 0)
            return false;
        *intersection_distance = t;
        return true;
    }

    return false;
}


// select objects individually via mouse click
void mouse_button_callback(GLFWwindow* window, int btn, int action, int mods)
{
    double xpos, ypos;
    switch (btn)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        if (action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &xpos, &ypos);
            float xmouse = xpos;
            float ymouse = ypos;
            vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

            selected_obj = -1;
            float closest_intersection = 0.0f;
            for (int i = 0; i < scene.size(); i++)
            {

                float t_dist = 0.0f;

                if (ray_sphere(camera.Position, ray_wor, scene[i].ancora_world, 1.0f, &t_dist))
                {
                    if (selected_obj == -1 || t_dist < closest_intersection)
                    {
                        selected_obj = i;
                        closest_intersection = t_dist;
                    }
                }
            }
        }

    default:
        break;
    }
}