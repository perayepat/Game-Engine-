#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include "model.h"
#include <learnopengl/shader_s.h>
#include <learnopengl/camera_m.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//New Stuff I Added
bool canCommand;
bool toggleCommandMode;
bool spawnObjectCheck = false;
int spawnObjectCount = 0;

bool showFPS_Check = false;
bool showFPS_Check_Pressed;
bool loadMapCheck = false;
bool mapLoaded_Check = false;
bool loadMapCheck_Pressed;

namespace fs = std::experimental::filesystem;
string modelPath = "resources/objects/Wolf_Level/wolf_lvl.obj";
glm::vec3 cubePos[16];
// positions all containers
glm::vec3 cubePositions[16] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
};
//Scale of objects to spawn
glm::vec3 floorScale = glm::vec3(20, 0.5, 20);
glm::vec3 wallScale = glm::vec3(4, 4, 1);



//Map Name
std::string newMapName;

//My Own Code to Read from Text File for new map
std::string _newXpos, _newYpos, _newZpos, _newrotY;
std::vector<float> _newWallPosX;
std::vector<float> _newWallPosY;
std::vector<float> _newWallPosZ;
std::vector<float> _newWallRoty;
//Number of lines
int lines = 0;



double lastTime = glfwGetTime();
int nbFrames = 0;
float vertCount = 0.0f;

//-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//
unsigned int DIFFUSE_NUMBER = 0;
//-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//
// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    //-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//
    //Textfile reading
    std::string diffuseNumber;
    std::ifstream MyReadFile("Dtext.txt");
    std::getline(MyReadFile, diffuseNumber);
    std::stringstream stringer(diffuseNumber);
    stringer >> DIFFUSE_NUMBER;
    //-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//  
      
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
    // build and compile our shader zprogram
    // ------------------------------------
  
// -----------
    Shader ourShader("model_loading.vert", "model_loading.frag");
    // Sky and Cube Box
    Shader shader("cubemaps.vert", "cubemaps.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Model ourModel(modelPath);
    Model crateModel("resources/objects/WolfBoy/wolfBoi.obj");
    Model wolfModel("resources/objects/WolfCrate/crate.obj");
  
    /*if (modelPath != "resources/objects/backpack/backpack.obj")
    {
        std::cout << "Failed to initialize Model corret Model Name" << std::endl;
    }*/

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // texture Coords
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
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    vector<std::string> faces
    {
          "resources/Skybox/right.jpg",
          "resources/Skybox/left.jpg",
      
                 "resources/Skybox/bottom.jpg",
         "resources/Skybox/top.jpg",
           
                       "resources/Skybox/front.jpg",
                "resources/Skybox/back.jpg",

         
    };  

    unsigned int cubemapTexture = loadCubemap(faces);

    //------------------------------------------------------------------My changes-----------------------------
    std::string newTitle = "LearnOpenGL - CommandMode (Disabled) :";
    glfwSetWindowTitle(window, newTitle.c_str());

    //My Own Code to Read from Text File
    std::string Xpos, Ypos, Zpos, rotY;
    std::vector<float> wallPosX;
    std::vector<float> wallPosY;
    std::vector<float> wallPosZ;
    std::vector<float> wallRoty;

    ////Scale of objects to spawn
    //glm::vec3 floorScale = glm::vec3(20, 0.5, 20);
    //glm::vec3 wallScale = glm::vec3(4, 4, 1);


    //File Name
    std::string file = "Wall_List.txt";

    ////Number of lines
    //int lines = 0;

    std::ifstream coeff(file); //Opening file
    if (coeff.is_open()) //If its open
    {
        while (!coeff.eof())
        {
            std::getline(coeff, Xpos, ',');
            wallPosX.push_back(std::stof(Xpos));
            std::getline(coeff, Ypos, ',');
            wallPosY.push_back(std::stof(Ypos));
            std::getline(coeff, Zpos, ',');
            wallPosZ.push_back(std::stof(Zpos));
            std::getline(coeff, rotY, '\n'); //New line
            wallRoty.push_back(std::stof(rotY));


            lines += 1; //To Count the amount of entries (Walls)
        }
        coeff.close(); // Close File
        std::cout << "Num of Walls in Text File: " << lines << std::endl;
    }
    else
    {
        std::cout << "Unable to Open File";
    }


    //glm::vec3 cubePos[16];

    for (int w = 0; w < lines; w++)
    {
        cubePos[w].x = wallPosX[w];
        cubePos[w].y = wallPosY[w];
        cubePos[w].z = wallPosZ[w];
    }
    for (int v = 0; v < lines; v++)
    {
        cubePositions[v] = cubePos[v];
    }
    //-----------------------------------------------------------------------My Changes---------------------------------------


    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    //Point light  colors
    glm::vec3 pointLightColors[] = {
    glm::vec3(1.0f, 0.6f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 1.0, 0.0),
    glm::vec3(0.2f, 0.2f, 1.0f)
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------//-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//
    unsigned int diffuseMap[10] = { loadTexture("resources/container2.png") ,loadTexture("resources/mLava.jpg"),loadTexture("resources/BelguimBlock.png"),
                                     loadTexture("resources/mBLava.png"), loadTexture("resources/mBlueGlaze.png"), loadTexture("resources/mCLava.png"), 
                                      loadTexture("resources/mDarkMarble.png"),loadTexture("resources/mGlass.jpg"),loadTexture("resources/mGreyMarble.jpg"),
                                        loadTexture("resources/mGreyMarble2.png") };
    unsigned int specularMap = loadTexture("resources/container2_specular.png");
    //-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//

    // shader configuration
    // --------------------
   // lightingShader.use();
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);



    //Count Verts
    vertCount = GL_ARRAY_BUFFER / 3;
    vertCount = (lines + 4) * 12;
    std::cout << vertCount << std::endl;



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //----------------------------------------------------------------My Changes-----------------------------------------

        //FPS Counter's
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
            // printf and reset timer
            float ch = 1000.0 / double(nbFrames);

            if (canCommand && showFPS_Check) 
            {
                printf("%f ms/frame\n", 1000.0 / double(nbFrames));
                std::cout << 1000 / ch << " FPS" << std::endl;

                std::string newTitle = "LearnOpenGL - CommandMode (Active) : " + std::to_string(1000 / ch) + "fps / " + std::to_string(1000.0 / double(nbFrames)) + "ms";
                glfwSetWindowTitle(window, newTitle.c_str());
            }

            nbFrames = 0;
            lastTime += 1.0;
        }

        //----------------------------------------------------------------My Changes-----------------------------------------


        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
         // don't forget to enable shader before setting uniforms
        ourShader.use();
      
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
        ourShader.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


        // point light 1
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        ourShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
        ourShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
        ourShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09);
        ourShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        ourShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
        ourShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
        ourShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09);
        ourShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        ourShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
        ourShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
        ourShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09);
        ourShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        ourShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
        ourShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
        ourShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09);
        ourShader.setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09);
        ourShader.setFloat("spotLight.quadratic", 0.032);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        //Draw The imported Models
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.gammaCorrection = true;
        ourModel.Draw(ourShader);
        // Crate
  
        model = glm::translate(model, glm::vec3(2.0f, 2.0f, 2.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("crate", model);
        crateModel.Draw(ourShader);

        model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("wolf", model);
        wolfModel.Draw(ourShader);

      




       //// world transformation
       // glm::mat4 model = glm::mat4(1.0f);
       // lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        //-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//
        glBindTexture(GL_TEXTURE_2D, diffuseMap[DIFFUSE_NUMBER]);  //Use the text file to assign the number here 
        //-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------////-------------HIGHLIGHT------------------//
        // bind specular map0z
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
     

        // render containers
        glBindVertexArray(cubeVAO);
        //---------------------------------------------------------------My Changes---------------------------------------------
        //Floor first
        glm::mat4 floor = glm::mat4();
        floor = glm::translate(floor, glm::vec3(0.0f, -2.0f, 0.0f));
        floor = glm::scale(floor, floorScale);
        ourShader.setMat4("model", floor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //Walls
        for (unsigned int i = 0; i < lines; i++)
        {
            if (i < cubePositions->length()) 
            {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f);
                float angle = wallRoty[i];
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, cubePositions[i]);
                model = glm::scale(model, wallScale);
                ourShader.setMat4("model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        



        //----------------------------------------------------------------My Changes-----------------------------------------

        //Spawn Object Via Command Mode
        for (unsigned int i = 0; i < spawnObjectCount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f + i, 5.0f, 0.0f)); //Spawn every new object pos based on i
            model = glm::scale(model, glm::vec3(0.8f)); //So that the objects are easier to see
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //----------------------------------------------------------------My Changes-----------------------------------------


        // also draw the lamp object(s)
      //  lightCubeShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !canCommand)
        camera.ProcessKeyboard(FORWARD, deltaTime*3);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !canCommand)
        camera.ProcessKeyboard(BACKWARD, deltaTime * 3);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !canCommand)
        camera.ProcessKeyboard(LEFT, deltaTime * 3);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !canCommand)
        camera.ProcessKeyboard(RIGHT, deltaTime * 3);


    //----------------------------------------------------------------My Changes-----------------------------------------


    //NewKeyBinds for console commands (Toggle)
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) 
    {
        if (toggleCommandMode == false && canCommand == false) 
        {
            canCommand = true;
            toggleCommandMode = true;

            std::string newTitle = "LearnOpenGL - CommandMode (Active) :";
            glfwSetWindowTitle(window, newTitle.c_str());
        }
        else if (canCommand == true && toggleCommandMode == false)
        {
            canCommand = false;
            toggleCommandMode = true;
            std::string newTitle = "LearnOpenGL - CommandMode (Disabled) :";
            glfwSetWindowTitle(window, newTitle.c_str());
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
    {
        toggleCommandMode = false;
    }


    //Spawn Object
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && canCommand == true && spawnObjectCheck == false && loadMapCheck == false)
    {
        spawnObjectCheck = true;
        spawnObjectCount++;

        //Put Any code here to spawn in a object 
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        spawnObjectCheck = false;
    }


    //Show FPS
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && canCommand == true && loadMapCheck == false && showFPS_Check_Pressed == false)
    {
        showFPS_Check_Pressed = true;

        if (showFPS_Check == false)
        {
            showFPS_Check = true;
        }
        else
        {
            showFPS_Check = false;
        }
    }
    else if(glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        showFPS_Check_Pressed = false;
    }


    //Load Map
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && canCommand == true)
    {
        showFPS_Check = false;
        std::string holder;

        if (loadMapCheck == false)
        {
            loadMapCheck = true;
        }
        else
        {
            loadMapCheck = false;

        }



        if (loadMapCheck == true && mapLoaded_Check == false) 
        {
            std::cout << "Load Map - Enter Map Name: " << std::endl;
            std::cin >> holder;

            std::cout << "Map name - " + holder << std::endl;
            lines = 0;

            if (holder != "" && mapLoaded_Check == false) 
            {
                std::ifstream coeff(holder); //Opening file
                if (coeff.is_open()) //If its open
                {
                    std::cout << "Map Loaded" << std::endl;
                    mapLoaded_Check = true;

                    while (!coeff.eof())
                    {
                        std::getline(coeff, _newXpos, ',');
                        _newWallPosX.push_back(std::stof(_newXpos));
                        std::getline(coeff, _newYpos, ',');
                        _newWallPosY.push_back(std::stof(_newYpos));
                        std::getline(coeff, _newZpos, ',');
                        _newWallPosZ.push_back(std::stof(_newZpos));
                        std::getline(coeff, _newrotY, '\n'); //New line
                        _newWallRoty.push_back(std::stof(_newrotY));

                        lines += 1; //To Count the amount of entries (Walls)
                    }
                    coeff.close(); // Close File
                    std::cout << "Num of Walls in Text File: " << lines << std::endl;

                    for (int w = 0; w < lines; w++)
                    {
                        cubePos[w].x = _newWallPosX[w];
                        cubePos[w].y = _newWallPosY[w];
                        cubePos[w].z = _newWallPosZ[w];
                    }

                    ////Assigns all retrieved values into CubePositions
                    for (int v = 0; v < lines; v++)
                    {
                        cubePositions[v] = cubePos[v];
                    }
                }
                else
                {
                    std::cout << "Unable to Open File";
                }
            }
            else 
            {
                std::cout << "Invalid Name";
            }
        }   
    }

    //----------------------------------------------------------------My Changes-----------------------------------------
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;

}
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}