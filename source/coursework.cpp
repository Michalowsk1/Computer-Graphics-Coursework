#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>
#include <common/animation.hpp>

float moveSpeed = 0.0f;
float height = 0.0f;
float doorXValue = 0.0f;
bool move = false;
bool inRange = false;
bool disco = false;

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

    // Load models
    Model cube("../assets/cube.obj");
    Model sphere("../assets/sphere.obj");


    // Load the textures
    cube.addTexture("../assets/crate.jpg", "diffuse");

    // Define cube object lighting properties
    cube.ka = 0.2f;
    cube.kd = 0.5f;
    cube.ks = 0.5f;
    cube.Ns = 20.0f;


    // Cube positions
    vec3 positions[] = {
        vec3(0.0f,  0.0f,  -5.0f),
        vec3(14.0f,  0.0f,  -5.0f),
        vec3(0.0f,  0.0f,  9.0f),
        vec3(14.0f,  0.0f,  9.0f),


    };

    // Add boxes to objects vector
    std::vector<Object> objects;
    std::vector<Object> Animobjects;
    Object object;
    Object Animobject;
    object.name = "cube";
    for (unsigned int i = 0; i < 4; i++)
    {
        object.position = positions[i];
        object.rotation = vec3(1.0f, 1.0f, 1.0f);
        object.scale = vec3(0.5f, 0.5f, 0.5f);
        object.angle = Maths::radians(0.0f);
        objects.push_back(object);
    }

    // FLOOR

    Model floor("../assets/plane.obj");
    floor.addTexture("../assets/stones_diffuse.png", "diffuse");
    floor.addTexture("../assets/stones_normal.png", "normal");
    floor.addTexture("../assets/stones_specular.png", "specular");

    // Define floor light properties
    floor.ka = 0.2f;
    floor.kd = 0.5f;
    floor.ks = 0.5f;
    floor.Ns = 20.0f;

    object.position = vec3(7.0f, -0.5f, 2.0f);
    object.scale = vec3(0.75f, 1.0f, 0.75f);
    object.rotation = vec3(0.0f, 1.0f, 0.0f);
    object.angle = 0.0f;
    object.name = "floor";
    objects.push_back(object);

    // Door

    Model door("../assets/cube.obj");
    door.addTexture("../assets/crate.jpg", "diffuse");

    door.ka = 0.2f;
    door.kd = 0.5f;
    door.ks = 0.5f;
    door.Ns = 20.0f;

    Animobject.name = "door";
    Animobject.position = vec3(7.0f, doorXValue, 2.0f);
    Animobject.scale = vec3(0.5f, 0.5f, 0.5f);
    Animobject.rotation = vec3(0.0f, 1.0f, 0.0f);
    Animobject.angle = 0.0f;
    Animobjects.push_back(Animobject);

    //WALL

    Model wall("../assets/plane.obj");
    wall.addTexture("../assets/bricks_diffuse.png", "diffuse");
    wall.addTexture("../assets/bricks_normal.png", "normal");
    wall.addTexture("../assets/bricks_specular.png", "specular");


    // Define wall light properties
    wall.ka = 0.2f;
    wall.kd = 0.5f;
    wall.ks = 0.5f;
    wall.Ns = 20.0f;

    //walls


    object.name = "wall";
    object.position = vec3(7.0f, -2.5f, -5.5f);
    object.scale = vec3(0.75f, 0.25f, 0.75f);
    object.rotation = vec3(1.0f, 0.0f, 0.0f);
    object.angle = Maths::radians(90.0f);
    objects.push_back(object);

    object.name = "wall";
    object.position = vec3(7.0f, -2.5f, 9.5f);
    object.scale = vec3(0.75f, 0.25f, 0.75f);
    object.rotation = vec3(-1.0f, 0.0f, 0.0f);
    object.angle = Maths::radians(90.0f);
    objects.push_back(object);

    Model wallRotate("../assets/plane.obj");
    wallRotate.addTexture("../assets/bricks_diffuse_Rotated.png", "diffuse");
    wallRotate.addTexture("../assets/bricks_normal_Rotated.png", "normal");
    wallRotate.addTexture("../assets/bricks_specular_Rotated.png", "specular");


    // Define wall light properties
    wallRotate.ka = 0.2f;
    wallRotate.kd = 0.5f;
    wallRotate.ks = 0.5f;
    wallRotate.Ns = 20.0f;

    object.name = "wallRotate";
    object.position = vec3(14.5f, -2.5f, 2.0f);
    object.scale = vec3(0.75f, 0.25f, 0.75f);
    object.rotation = vec3(0.0f, 0.0f, 1.0f);
    object.angle = Maths::radians(90.0f);
    objects.push_back(object);

    object.name = "wallRotate";
    object.position = vec3(-0.5f, -2.5f, 2.0f);
    object.scale = vec3(0.75f, 0.25f, 0.75f);
    object.rotation = vec3(0.0f, 0.0f, -1.0f);
    object.angle = Maths::radians(90.0f);
    objects.push_back(object);

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
        vec3(1.0f,4.0f,-4.0f),
        vec3(13.0f,4.0f,-4.0f),
        vec3(1.0f,4.0f,8.0f),
        vec3(13.0f,4.0f,8.0f),
        vec3(7.0f,4.0f,2.0f)
    };

    //point light
    light.position = vec3(2.0f, 4.0f, 2.0f);
    light.colour = vec3(1.0f, 1.0f, 1.0f);
    light.constant = 1.0f;
    light.linear = 0.2f;
    light.quadratic = 0.02f;
    light.type = 1;
    lightSources.push_back(light);

    // Add spotlight
    for (unsigned int i = 0; i < 5; i++)
    {

        light.position = lightPositions[i];
        light.direction = vec3(0.0f, -1.0f, 0.0f);
        light.colour = vec3(1.0f, 0.0f, 0.0f);
        light.cosPhi = std::cos(Maths::radians(45.0f));
        light.constant = 1.0f;
        light.linear = 0.2f;
        light.quadratic = 0.02f;
        light.type = 2;

        if (i == 4) { light.colour = vec3(1.0f, 1.0f, 1.0f); light.cosPhi = std::cos(Maths::radians(30.0f)); }

        else { light.colour = vec3(1.0f, 0.0f, 0.0f); light.cosPhi = std::cos(Maths::radians(45.0f)); }
        lightSources.push_back(light);
    }





    //camera starting position
    camera.eye.x = 7.5f;
    camera.eye.z = 8.0f;


    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        CheckingCollision();

        // Update timer
        float time = glfwGetTime();
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

        // Calculate view and projection matrices
        camera.target = camera.eye + camera.front;
        camera.quaternionCamera();


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
        glUniform1f(glGetUniformLocation(shaderID, "ka"), cube.ka);
        glUniform1f(glGetUniformLocation(shaderID, "kd"), cube.kd);
        glUniform1f(glGetUniformLocation(shaderID, "ks"), cube.ks);
        glUniform1f(glGetUniformLocation(shaderID, "Ns"), cube.Ns);

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
            if (objects[i].name == "cube")
                cube.draw(shaderID);

            if (objects[i].name == "floor")
                floor.draw(shaderID);

            if (objects[i].name == "wall")
                wall.draw(shaderID);

            if (objects[i].name == "wallRotate")
                wallRotate.draw(shaderID);

            if (objects[i].name == "ceiling")
                ceiling.draw(shaderID);
        }

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            move = true;
        }

        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            move = false;
        }

        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            inRange = true;
        }

        for (unsigned int i = 0; i < static_cast<unsigned int>(Animobjects.size()); i++)
        {
            if (inRange)
            {
                moveSpeed = -sin(glfwGetTime());
                // Animate door
                if (!move)
                {

                    float height = Maths::radians(moveSpeed);
                    glm::mat4 translate = Maths::translate(glm::vec3(7.0f, 4.0, 2.0f));
                    glm::mat4 scale = Maths::scale(glm::vec3(0.5f, 0.5f, 0.5f));
                    mat4 model = translate * scale;

                    // Send the MVP and MV matrices to the vertex shader
                    mat4 MV = camera.view * model;
                    mat4 MVP = camera.projection * MV;
                    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);


                }

                else if (move)
                {
                    float height = Maths::radians(moveSpeed);
                    glm::mat4 translate = Maths::translate(vec3(7.0f, 5.75f - (height * 100.0f), 2.0f));
                    glm::mat4 scale = Maths::scale(glm::vec3(0.5f, 0.5f, 0.5f));
                    mat4 model = translate * scale;

                    // Send the MVP and MV matrices to the vertex shader
                    mat4 MV = camera.view * model;
                    mat4 MVP = camera.projection * MV;
                    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

                    if (moveSpeed >= 0.99)
                    {
                        move = false;
                    }

                }

                // Draw the model
                if (Animobjects[i].name == "door")
                    cube.draw(shaderID);
                std::cout << moveSpeed << std::endl;
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


    std::cout << doorXValue << std::endl;

    // Cleanup
    cube.deleteBuffers();
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

    //if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    //{
    //    // Animate rectangle
    //    float angle = Maths::radians(glfwGetTime() * 360.0f / 3.0f);
    //    glm::mat4 translate = Maths::translate(glm::vec3(0.4f, doorXValue, 0.0f));
    //    glm::mat4 scale = Maths::scale(glm::vec3(0.4f, 0.3f, 0.0f));
    //    glm::mat4 rotate = Maths::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
    //    glm::mat4 transformation = rotate * translate * scale;
    //}

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




