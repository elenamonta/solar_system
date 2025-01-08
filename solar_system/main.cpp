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
Camera camera(glm::vec3(0.0f, 4.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//input
bool trackballMode = false;
bool previousTrackballMode = trackballMode;
bool Clockwise = true; //senso orario


vector<string> planetNames;
vector<Mesh> scene;
vector<Model> sceneObj; 
int selected_obj = -1;

glm::mat4 projection;
glm::mat4 view;

//gestione texture
string imageDir = "Texture/";
vector<string> pathTexture; 
vector<int> texture; 

string modelDir = "Model/";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
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
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShader.glsl", "fragmentShader.glsl");
    Shader lightCubeShader("vertexShader.glsl", "fragmentShader_light.glsl");
    
    lightingShader.use(); 
    
    // lamp_object
    //__________________________________________
    
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
        -0.5f,  0.5f, -0.5f
    };


    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  10.0f),
        glm::vec3(0.0f, 0.0f, -7.0f)
    };

    
    // Configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO, lightCubeVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
        
    //_________________________________________________


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

    Mesh sfera(meshType::sfera, planetNames[0]);
    sfera.Model = mat4(1.0);
    sfera.Model = translate(sfera.Model, vec3(0.0, 0.0, 0.0));
    sfera.positions = vec3(0.0, 0.0, 0.0);
    sfera.Model = scale(sfera.Model, vec3(1.0, 1.0, 1.0));
    sfera.angle = 0.0f; 
    sfera.setTexture(texture[0]);
    scene.push_back(sfera);
 
    
    for (int i = 1; i < NR_PLANETS; i++) {
        Mesh sfera(meshType::sfera, planetNames[i]);
        sfera.Model = mat4(1.0);
        sfera.Model = translate(sfera.Model, vec3(i*2.0, 0.0, 0.0));
        sfera.positions = vec3(i * 2.0, 0.0, 0.0);
        sfera.angle = 0.0f; 
        sfera.Model = scale(sfera.Model, scaleValue[i]);
        sfera.setTexture(texture[i]);
        //sfera.setMaterial(Material::getMaterial(MaterialType::Emerald));
        scene.push_back(sfera);
    }
 

    //mesh obj
    auto path = modelDir + "ufo.obj";
    Model ufo(path.c_str());
    for (int i = 0; i < ufo.Model3D.size(); i++) {
        ufo.Model3D[i].INIT_VAO();
        ufo.Model3D[i].Model = mat4(1.0f);
        ufo.Model3D[i].Model = translate(ufo.Model3D[i].Model, vec3(0.0f, 3.0f, 0.0f));
        ufo.Model3D[i].Model = scale(ufo.Model3D[i].Model, vec3(0.5f, 0.5f, 0.5f));
        ufo.Model3D[i].name = "ufo";
        ufo.Model3D[i].ancora_obj = vec4(0.0, 0.0, 0.0, 1.0);
    }
    sceneObj.push_back(ufo);


    imgui.Initilize_IMGUI();


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        //imgui
        imgui.my_interface();

        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);


        //draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        
        for (unsigned int i = 0; i < 2; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightCubeShader.setMat4("model", model);
            glBindVertexArray(lightCubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        //orbital movement
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        if (deltaTime > 0.016f) {
            lastFrame = currentFrame;
            for (int i = 0; i < NR_PLANETS; i++) {
                float angle = i * 0.2f; 
                float radians = glm::radians(angle);          
                float x = scene[i].positions.x * cos(radians)- scene[i].positions.z * sin(radians); 
                float z = scene[i].positions.x * sin(radians) + scene[i].positions.z * cos(radians); 
                
                scene[i].Model = glm::mat4(1.0f); 
                scene[i].Model = translate(scene[i].Model, glm::vec3(x, scene[i].positions.y, z));
                scene[i].positions = vec3(x, scene[i].positions.y, z);
                scene[i].angle += angle; 
                scene[i].Model = glm::rotate(scene[i].Model, glm::radians(scene[i].angle), glm::vec3(0.0f, 1.0f, 0.0f));
                scene[i].Model = scale(scene[i].Model, scaleValue[i]);
            }
        }


        // set material from gui
        //sfera.setMaterial(Material::getMaterial(static_cast<MaterialType>(imgui.selectedMaterialType)));
        
        // draw meshes
        for(int i = 0; i < NR_PLANETS; i++)
            scene[i].draw(lightingShader, 1.0f);

        for (int i = 0; i < sceneObj.size(); i++)
            sceneObj[i].draw(lightingShader);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //de-allocate all resources
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &lightCubeVBO);

    imgui.close_GUI();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



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

    if (!previousTrackballMode && trackballMode) {
        camera.resetPosition(glm::vec3(0.0, 0.0, 3.0));
    }
    else if (previousTrackballMode && !trackballMode) {
        camera.resetPosition(glm::vec3(0.0, 0.0, 3.0));
    }

    camera.trackballMode = trackballMode;

    if (trackballMode) {
        camera.RotateAround(SCR_WIDTH, SCR_HEIGHT, xpos, ypos, lastX, lastY, Clockwise);
    }
    else {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    previousTrackballMode = trackballMode;
    lastX = xpos;
    lastY = ypos;

}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {
    //La funzione get_ray_from_mouse calcola il raggio che parte dalla posizione della telecamera
    //  e passa attraverso il punto sullo schermo corrispondente alla posizione del mouse.

    SCR_HEIGHT = h_up;
    SCR_WIDTH = w_up;
    mouse_y = SCR_HEIGHT - mouse_y;

    // mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]  
    float ndc_x = (2.0f * mouse_x) / SCR_WIDTH - 1.0;
    float ndc_y = (2.0f * mouse_y) / SCR_HEIGHT - 1.0;

    //Nello spazio di clippling z più piccola, oggetto più vicino all'osservatore, quindi si pone la z a - 1, 
    // posizionando il punto sul piano vicino del frustum.
    // Questo significa che il raggio che stiamo calcolando partirà dalla telecamera e si dirigerà 
    // verso il punto più vicino visibile sullo schermo.
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

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione p iù vicina.
*/

bool ray_sphere(vec3 O, vec3 d, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

    vec3 dist_sfera = O - sphere_centre_wor;
    float b = dot(dist_sfera, d);
    float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

    float delta = b * b - cc;

    if (delta < 0)  //Il raggio non interseca la sfera
        return false;
    //Calcolo i valori di t per cui il raggio interseca la sfera e restituisco il valore dell'intersezione 
    //più vicina all'osservatore (la t più piccola)
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
    //Caso in cui il raggio è tangente alla sfera: un interesezione con molteplicità doppia.
    if (delta == 0) {
        float t = -b + sqrt(delta);
        if (t < 0)
            return false;
        *intersection_distance = t;
        return true;
    }

    return false;
}


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
            if (selected_obj > -1)
                cout << "Oggetto selezionato " << scene[selected_obj].name.c_str() << endl;
        }

    default:
        break;
    }
}