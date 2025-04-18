#include <iostream>
#include <cmath>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>
#include <common/animation.hpp>

//Disco Ball variables
float XmoveSpeed = 0.0f;
float YmoveSpeed = 0.0f;
float ZmoveSpeed = 0.0f;
float Xtimer = 0.0f;
float Ytimer = 0.0f;
float Ztimer = 0.0f;
bool YMove = false;
bool XMove = false;
bool ZMove = false;

//pressurePlates
vec3 YpressurePlateLocation = vec3(7.0f, -0.55f, 2.0f);
vec3 XpressurePlateLocation = vec3(2.0f, -0.5f, -3.0f);
vec3 ZpressurePlateLocation = vec3(12.0f, -0.5f, 7.0f);

//Jump
bool jumped = false;
float YCamPos = 1.0f;
float heightPower = 0.0f;
float jumpTime = 0.0f;

//third-person camera
bool firstPerson = true;
float XLook = 0.0f;
float YLook = 0.0f;

// Function prototypes
void keyboardInput(GLFWwindow* window);
void mouseInput(GLFWwindow* window);

// Frame timers
float previousTime = 0.0f;  // time of previous iteration of the loop
float deltaTime = 0.0f;  // time elapsed since the previous frame

// Create camera object
Camera camera(vec3(0.0f, 0.0f, 4.0f),vec3(0.0f, 0.0f, 0.0f));

//Animation animation;

// Object struct
struct Object
{
    vec3 position = vec3(0.0f, 0.0f, 0.0f);
    vec3 rotation =vec3(0.0f, 1.0f, 0.0f);
    vec3 scale =vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
    std::string name;
};

//light structure
struct Light
{
    vec3 position;
    vec3 colour;
    vec3 direction;
    float cosPhi;
    float constant;
    float linear;
    float quadratic;
    unsigned int type;
};

//Create vector of light sources
std::vector<Light> lightSources;

void CheckingCollision()
{
        if (camera.eye.x >= 14)
        {
            camera.eye.x = 14;
        }

        else if (camera.eye.x <= 0)
        {
            camera.eye.x = 0;
        }

        if (camera.eye.z >= 9)
        {
            camera.eye.z = 9;
        }

        else if (camera.eye.z <= -5)
        {
            camera.eye.z = -5;
        }
}

void pressurePlateCheckers()

{
    if (Maths::MathsLength(camera.eye, YpressurePlateLocation) < 1.75f)
    {
        YMove = true;
    }
    else YMove = false;


    if (YMove)
    {
        Ytimer = Ytimer + 0.005f;
        XmoveSpeed = sin(Ytimer);
        float height = Maths::radians(ZmoveSpeed);
    }

    if (Maths::MathsLength(camera.eye, XpressurePlateLocation) < 1.75f)
    {
        XMove = true;
    }
    else XMove = false;


    if (XMove)
    {
        Xtimer = Xtimer + 0.005f;
        YmoveSpeed = sin(Xtimer);
        float height = Maths::radians(ZmoveSpeed);
    }

    if (Maths::MathsLength(camera.eye, ZpressurePlateLocation) < 1.75f)
    {
        ZMove = true;
    }
    else ZMove = false;


    if (ZMove)
    {
        Ztimer = Ztimer + 0.005f;
        ZmoveSpeed = sin(Ztimer);
        float height = Maths::radians(ZmoveSpeed);
    }
}

void ClampCamera()
{
    if (camera.pitch >= 1)
        camera.pitch = 1;
    if (camera.pitch < -1)
        camera.pitch = -1;
}


int main(void)
{
    // =========================================================================
    // Window creation - you shouldn't need to change this code
    // -------------------------------------------------------------------------
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "Coursework", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    // -------------------------------------------------------------------------
    // End of window creation
    // =========================================================================

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Use back face culling
    glEnable(GL_CULL_FACE);

    // Ensure we can capture keyboard inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Capture mouse inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Compile shader program
    unsigned int shaderID, lightShaderID;
    shaderID = LoadShaders("vertexShader.glsl", "multipleLightsFragmentShader.glsl");
    lightShaderID = LoadShaders("lightVertexShader.glsl", "lightFragmentShader.glsl");

    // Activate shader
    glUseProgram(shaderID);

    // Add boxes to objects vector
    std::vector<Object> objects;
    std::vector<Object> Animobjects;
    Object object;
    Object Animobject;

    Model sphere("../assets/sphere.obj");

    // FLOOR

    Model floor("../assets/cube.obj");
    floor.addTexture("../assets/stones_diffuse.png", "diffuse");
    floor.addTexture("../assets/stones_normal.png", "normal");
    floor.addTexture("../assets/stones_specular.png", "specular");

    // Define floor light properties
    floor.ka = 0.2f;
    floor.kd = 0.5f;
    floor.ks = 0.5f;
    floor.Ns = 20.0f;

    object.position = vec3(7.0f, -10.5f, 2.0f);
    object.scale = vec3(7.5f, 10.0f, 7.5f);
    object.rotation = vec3(0.0f, 1.0f, 0.0f);
    object.angle = 0.0f;
    object.name = "floor";
    objects.push_back(object);

    // PressurePlate

    vec3 pressurePlateLocations[] =
    {
        XpressurePlateLocation,
        YpressurePlateLocation,
        ZpressurePlateLocation,
    };



    Model pressurePlate("../assets/pressurePlate.obj");
    pressurePlate.addTexture("../assets/white.bmp", "diffuse");

    // Define floor light properties
    pressurePlate.ka = 0.2f;
    pressurePlate.kd = 0.5f;
    pressurePlate.ks = 0.5f;
    pressurePlate.Ns = 20.0f;
    object.name = "pressurePlate";   
    for (unsigned int i = 0; i < 5; i++)
    {

        object.position = pressurePlateLocations[i];
        object.rotation = vec3(1.0f, 1.0f, 1.0f);
        object.scale = vec3(0.75f, 0.75f, 0.75f);
        object.angle = Maths::radians(0.0f);

        objects.push_back(object);
    }
    
    // discoball
    Model discoBall("../assets/discoBall.obj");
    discoBall.addTexture("../assets/white.bmp", "diffuse");
    

    discoBall.ka = 0.2f;
    discoBall.kd = 0.5f;
    discoBall.ks = 0.5f;
    discoBall.Ns = 20.0f;

    Animobject.name = "discoBall";
    Animobject.position = vec3(7.0f, 0.0f, 2.0f);
    Animobject.scale = vec3(0.5f, 0.5f, 0.5f);
    Animobject.rotation = vec3(0.0f, 1.0f, 0.0f);
    Animobject.angle = 0.0f;
    Animobjects.push_back(Animobject);

    //WALL

    vec3 WallLocations[] =
    {
        vec3(7.0, 2.0f, -6.0),
        vec3(7.0, 2.0f, 10.0),
        vec3(-1.0, 2.0f, 2.0),
        vec3(15.0, 2.0f, 2.0),
    };

    Model wall("../assets/cube.obj");
    wall.addTexture("../assets/bricks_diffuse_Rotated.png", "diffuse");
    wall.addTexture("../assets/bricks_normal_Rotated.png", "normal");
    wall.addTexture("../assets/bricks_specular_Rotateed.png", "specular");


    // Define wall light properties
    wall.ka = 0.2f;
    wall.kd = 0.5f;
    wall.ks = 0.5f;
    wall.Ns = 20.0f;

    for (unsigned int i = 0; i < 4; i++)
    {
        object.name = "wall";
        object.position = WallLocations[i];
        object.scale = vec3(9.0f, 3.0f, 0.5f);

        if (i == 2 || i == 3)
        {
            object.scale = vec3(0.5f, 3.0f, 9.0);
        }
        objects.push_back(object);
    }

    //CEILING

    Model ceiling("../assets/cube.obj");
    ceiling.addTexture("../assets/stones_normal.png", "normal");


    // Define wall light properties
    ceiling.ka = 0.2f;
    ceiling.kd = 0.5f;
    ceiling.ks = 0.5f;
    ceiling.Ns = 20.0f;

    object.name = "ceiling";
    object.position = vec3(7.0f, 5.0f, 2.0f);
    object.scale = vec3(8.0f, 0.1f, 8.0f);
    object.rotation = vec3(0.0f, -1.0f, 0.0f);
    objects.push_back(object);


    Model Monkey("../assets/monkey.obj");


    // Define wall light properties
    Monkey.ka = 0.2f;
    Monkey.kd = 0.5f;
    Monkey.ks = 0.5f;
    Monkey.Ns = 20.0f;

    Animobject.name = "monkey";
    Animobject.position = camera.eye;
    Animobject.scale = vec3(0.75f, 0.75f, 0.75f);
    Animobject.rotation = vec3(camera.yaw * camera.pitch);
    Animobjects.push_back(Animobject);


    //Define light source properties
    vec3 lightPosition = vec3(2.0f, 2.0f, 2.0f);
    vec3 lightColour = vec3(1.0f, 1.0f, 1.0f);
    float constant = 1.0f;
    float linear = 0.1f;
    float quadratic = 0.02f;

    glUniform1f(glGetUniformLocation(shaderID, "constant"), constant);
    glUniform1f(glGetUniformLocation(shaderID, "linear"), linear);
    glUniform1f(glGetUniformLocation(shaderID, "quadratic"), quadratic);

    // Add first point light source
    Light light;
    vec3 lightPositions[] =
    {
        vec3(2.0f,4.0f,-3.0f),
        vec3(12.0f,4.0f,-3.0f),
        vec3(2.0f,4.0f,7.0f),
        vec3(12.0f,4.0f,7.0f),
        vec3(7.0f,4.0f,2.0f)
    };

    //point light
    light.position = vec3(7.0f, 4.0f, 2.0f);
    light.colour = vec3(1.0f, 1.0f, 1.0f);
    light.constant = 1.0f;
    light.linear = 0.1f;
    light.quadratic = 0.02f;
    light.type = 1;
    lightSources.push_back(light);

    // Add spotlight
    for (unsigned int i = 0; i < 5; i++)
    {
        light.position = lightPositions[i];
        light.direction = vec3(0.0f, -1.0f, 0.0f);
        light.colour = vec3(1.0f, 0.0f, 0.0f);
        light.cosPhi = std::cos(Maths::radians(30.0f));
        light.constant = 1.0f;
        light.linear = 0.2f;
        light.quadratic = 0.02f;
        light.type = 2;

        if (i == 4) 
        { 
            light.colour = vec3(1.0f, 1.0f, 1.0f);
            light.cosPhi = std::cos(Maths::radians(20.0f));
            lightSources.push_back(light);
        }
        lightSources.push_back(light);

    }




    //camera starting position
    camera.eye.x = 7.0f;
    camera.eye.z = 8.0f;


    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        //own methods
        CheckingCollision();
        pressurePlateCheckers();
        ClampCamera();

        // Update timer
        float time = glfwGetTime();
        float offset = glfwGetTime();
        deltaTime = time - previousTime;
        previousTime = time;


        // Get inputs
        keyboardInput(window);
        mouseInput(window);

        // Clear the window
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        glUseProgram(shaderID);

        // Calculate view and projection matrices + 3RD PERSON CAMERA
        if (firstPerson)
        {
            camera.target = camera.eye + camera.front;
            camera.quaternionCamera();

            std::cout << camera.eye << std::endl;
        }

        else
        {
            camera.target = camera.eye + camera.front;
            camera.ThirdPersonCamera();

            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) //adjusts camera
                camera.varBackOffset += 0.01;

            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
                camera.varBackOffset -= 0.01;

            std::cout << camera.view << std::endl;
        }

        // Send multiple light source properties to the shader
        for (unsigned int i = 0; i < static_cast<unsigned int>(lightSources.size()); i++)
        {
            vec3 viewSpaceLightPosition = vec3(camera.view * vec4(lightSources[i].position, 1.0f));
            std::string idx = std::to_string(i);
            glUniform3fv(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].colour").c_str()), 1, &lightSources[i].colour[0]);
            glUniform3fv(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].position").c_str()), 1, &viewSpaceLightPosition[0]);
            glUniform1f(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].constant").c_str()), lightSources[i].constant);
            glUniform1f(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].linear").c_str()), lightSources[i].linear);
            glUniform1f(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].quadratic").c_str()), lightSources[i].quadratic);
            glUniform1i(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].type").c_str()), lightSources[i].type);

            vec3 viewSpaceLightDirection = vec3(camera.view * vec4(lightSources[i].direction, 0.0f));
            glUniform3fv(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].direction").c_str()), 1, &viewSpaceLightDirection[0]);
            glUniform1f(glGetUniformLocation(shaderID, ("lightSources[" + idx + "].cosPhi").c_str()), lightSources[i].cosPhi);
        }

        // Send object lighting properties to the fragment shader
        glUniform1f(glGetUniformLocation(shaderID, "ka"), discoBall.ka);
        glUniform1f(glGetUniformLocation(shaderID, "kd"), discoBall.kd);
        glUniform1f(glGetUniformLocation(shaderID, "ks"), discoBall.ks);
        glUniform1f(glGetUniformLocation(shaderID, "Ns"), discoBall.Ns);

        // Loop through objects
        for (unsigned int i = 0; i < static_cast<unsigned int>(objects.size()); i++)
        {
            // Calculate model matrix
            mat4 translate = Maths::translate(objects[i].position);
            mat4 scale = Maths::scale(objects[i].scale);
            mat4 rotate = Maths::rotate(objects[i].angle, objects[i].rotation);
            mat4 model = translate * rotate * scale;

            // Send the MVP and MV matrices to the vertex shader
            mat4 MV = camera.view * model;
            mat4 MVP = camera.projection * MV;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

            // Draw the model

            if (objects[i].name == "floor")
                floor.draw(shaderID);

            if (objects[i].name == "wall")
                wall.draw(shaderID);

            if (objects[i].name == "ceiling")
                ceiling.draw(shaderID);

            if (objects[i].name == "pressurePlate")
                pressurePlate.draw(shaderID);
                
        }

            //DiscoBall & Monkey
        for (unsigned int i = 0; i < static_cast<unsigned int>(Animobjects.size()); i++)
        {
            //MONKEY

            float YPos = 4.0f + sin(XmoveSpeed) * 0.5;
            float XPos = 7.0f + sin(YmoveSpeed) * 8;
            float ZPos = 2.0f + sin(ZmoveSpeed) * 8;

            XLook = camera.pitch * 100;
            YLook = -camera.yaw * 57;
            
            if (Animobjects[i].name == "monkey") //lets monkey move
            {
                float angleY = Maths::radians(YLook - (0.3 * camera.yaw)); // removes slight offset with horizontal camera movement
                float angleX = Maths::radians(XLook);
                glm::mat4 translate = Maths::translate(vec3(camera.eye.x, camera.eye.y - 0.75f, camera.eye.z)); //moves monkey down so player can see monkey and the scene
                glm::mat4 scale = Maths::scale(glm::vec3(0.25f, 0.25f, 0.25f));
                glm::mat4 rotateY = Maths::rotate(angleY, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 rotateX = Maths::rotate(angleX, glm::vec3(1.0f, 0.0f, 0.0f));
                mat4 model = translate * rotateY * rotateX * scale;

                mat4 MV = camera.view * model;
                mat4 MVP = camera.projection * MV;
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

            }

            //DISCOBALL
            else //lets disco ball move
            {
                glm::mat4 translate = Maths::translate(vec3(XPos, YPos, ZPos));
                glm::mat4 scale = Maths::scale(glm::vec3(0.5f, 0.5f, 0.5f));
                mat4 model = translate * scale;

                mat4 MV = camera.view * model;
                mat4 MVP = camera.projection * MV;
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);
            }

            


            // Draw the model
            if (Animobjects[i].name == "discoBall")
                discoBall.draw(shaderID);

            if (!firstPerson)
            {
                if (Animobjects[i].name == "monkey")
                {
                    Monkey.draw(shaderID);
                }
            }
        }

        glUseProgram(lightShaderID);

        for (unsigned int i = 0; i < static_cast<unsigned int>(lightSources.size()); i++)
        {
            // Calculate model matrix
            mat4 translate = Maths::translate(lightSources[i].position);
            mat4 scale = Maths::scale(vec3(0.1f));
            mat4 model = translate * scale;

            // Send the MVP and MV matrices to the vertex shader
            mat4 MVP = camera.projection * camera.view * model;
            glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);

            // Send model, view, projection matrices and light colour to light shader
            glUniform3fv(glGetUniformLocation(lightShaderID, "lightColour"), 1, &lightSources[i].colour[0]);

            // Draw light source
            sphere.draw(lightShaderID);
        }
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // Cleanup
    pressurePlate.deleteBuffers();
    wall.deleteBuffers();
    ceiling.deleteBuffers();
    floor.deleteBuffers();
    glDeleteProgram(shaderID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

void keyboardInput(GLFWwindow* window)
{
    int cameraMoveSpeed;

    //prevents user from flying around
    camera.eye.y = 1.0f;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraMoveSpeed = 10.0f;
    else
        cameraMoveSpeed = 5.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Move the camera using WSAD keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.eye += cameraMoveSpeed * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.eye -= cameraMoveSpeed * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.eye -= cameraMoveSpeed * deltaTime * camera.right;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.eye += cameraMoveSpeed * deltaTime * camera.right;

    //Changing Camera Perspective
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        firstPerson = false;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        firstPerson = true;


    //JUMP BUTTON
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        jumped = true;

    if (jumped)
    {
        jumpTime = jumpTime + 0.025f;

        heightPower = sin(jumpTime);

        camera.eye.y = 1 + heightPower;

        if (camera.eye.y <= 1)
        {
            jumped = false;
            jumpTime = 0;
        }
    }
    

}

void mouseInput(GLFWwindow* window)
{
    // Get mouse cursor position and reset to centre
    double xPos, yPos;

    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Update yaw and pitch angles
    camera.yaw += 0.005f * float(xPos - 1024 / 2);
    camera.pitch += 0.005f * float(768 / 2 - yPos);


    // Calculate camera vectors from the yaw and pitch angles
    camera.calculateCameraVectors();

    
}





