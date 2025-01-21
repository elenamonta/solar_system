#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float THETA = -90.0f;
const float PHI = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 WorldUp;
    glm::vec3 Target;
    // euler Angles
    float Theta;
    float Phi;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool trackballMode; 

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), float theta = THETA, float phi = PHI) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        Target = target;
        Direction = Target - Position;
        WorldUp = up;
        Theta = theta;
        Phi = phi;
    }

    bool setTrackballMode(bool active) {
        if (active != this->trackballMode) {
            this->trackballMode = active;
            return true;
        }
        return false;
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(glm::vec3(Position), glm::vec3(Target), glm::vec3(WorldUp));
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            Direction = Target - Position;
            Position += Direction * velocity;
            if (trackballMode) {
                Direction = glm::normalize(-Position);
            }
            Target = Position + Direction;
        }
        if (direction == BACKWARD) {
            Direction = Target - Position;
            Position -= Direction * velocity;
            if (trackballMode) {
                Direction = glm::normalize(-Position);
            }
            Target = Position + Direction;
        }
        if (!trackballMode)
        {
            if (direction == LEFT) {
                Direction = Target - Position;
                glm::vec3 slide_vec = glm::cross(Direction, glm::vec3(WorldUp)) * velocity;
                Position -= slide_vec;
                Target -= slide_vec;
            }
            if (direction == RIGHT) {
                Direction = Target - Position;
                glm::vec3 slide_vec = glm::cross(Direction, glm::vec3(WorldUp)) * velocity;
                Position += slide_vec;
                Target += slide_vec;
            }
        }
        else
        {
            if (direction == LEFT) {
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                Position = glm::vec3(rotationMatrix * glm::vec4(Position, 0.0f)); // Ruota la posizione
                Direction = glm::normalize(-Position); // Aggiorna la direzione verso il centro
                Target = glm::vec3(0.0f, 0.0f, 0.0f); // Assicura che il target sia il centro
            }
            if (direction == RIGHT) {
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                Position = glm::vec3(rotationMatrix * glm::vec4(Position, 0.0f)); // Ruota la posizione
                Direction = glm::normalize(-Position); // Aggiorna la direzione verso il centro
                Target = glm::vec3(0.0f, 0.0f, 0.0f); // Assicura che il target sia il centro
            }
        }
               

    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Theta += xoffset;
        Phi += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Phi > 89.0f)
                Phi = 89.0f;
            if (Phi < -89.0f)
                Phi = -89.0f;
        }

        glm::vec3 front;
        front.x = cos(glm::radians(Theta)) * cos(glm::radians(Phi));
        front.y = sin(glm::radians(Phi));
        front.z = sin(glm::radians(Theta)) * cos(glm::radians(Phi));
        Direction = glm::normalize(front);
        Target = Position + Direction;

    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void RotateAround(float width, float height, float xpos, float ypos, float last_mouse_pos_x, float last_mouse_pos_y)
    {
        float velocity = 100.0f; 
        glm::vec3 destination = getTrackBallPoint(xpos, ypos, width, height);
        glm::vec3 origin;
        if (firstMouse) {
            origin = getTrackBallPoint(width/2.0, height/2.0, width, height);
            firstMouse = false; 
        }
        else {
            origin = getTrackBallPoint(last_mouse_pos_x, last_mouse_pos_y, width, height);
        }
        float dx, dy, dz;
        dx = destination.x - origin.x;
        dy = destination.y - origin.y;
        dz = destination.z - origin.z;
        if (dx || dy || dz) {
            // rotation angle = acos( (v dot w) / (len(v) * len(w)) ) o approssimato da ||dest-orig||;
            float pi = glm::pi<float>();
            float angle = sqrt(dx * dx + dy * dy + dz * dz) * velocity;
            glm::vec3 rotationAxis = glm::cross(origin, destination);

            Direction = Position - Target;

            
            Position = Target + glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(-angle), rotationAxis) * glm::vec4(Direction, 0.0f));
           
            Target = glm::vec3(0.0f, 0.0f, 0.0f);
            Direction = glm::normalize(Target - Position);
        }
    }

    void resetPosition(glm::vec3 pos, glm::vec3 target=glm::vec3(0.0,0.0,0.0)) {
        Position = pos;
        Target = target; 
    }


private:
    bool firstMouse = true; 

    glm::vec3 getTrackBallPoint(float x, float y, float width, float height)
    {
        //Dalla posizione del mouse al punto proiettato sulla semisfera con centro l'origine e raggio 1
        float Delta, tmp;
        glm::vec3 point;
        //map to [-1;1]
        point.x = (2.0f * x - width) / width;
        point.y = (height - 2.0f * y) / height;

        //Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
        tmp = pow(point.x, 2.0) - pow(point.y, 2.0);
        Delta = 1.0f - tmp;
        if (Delta > 0.0f)
            point.z = sqrt(Delta);
        else
            point.z = 0;

        return normalize(point);
    }
};
#endif#pragma once
