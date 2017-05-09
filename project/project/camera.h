#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    PITCH_UP,
    PITCH_DOWN,
    YAW_LEFT,
    YAW_RIGHT
};

enum Camera_Mode {
    NORMAL,
    FIRST_PERSON
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat ZOOM = 45.0f;

class Camera
{
public:
    // Camera Attributes
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;
    // Eular Angles
    GLfloat yaw_;
    GLfloat pitch_;
    // Camera options
    GLfloat movement_speed_;
    GLfloat mouse_sensitivity_;
    GLfloat zoom_;
    // Mode
    Camera_Mode mode_;
    bool keys_[1024];

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW, GLfloat pitch = PITCH)
        : front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVTY),
          zoom_(ZOOM), mode_(NORMAL)
    {
        this->position_ = position;
        this->world_up_ = up;
        this->yaw_ = yaw;
        this->pitch_ = pitch;
        this->updateCameraVectors();
    }

    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(this->position_, this->position_ + this->front_, this->up_);
    }

    glm::mat4 getReversedViewMatrix(float water_height)
    {
        glm::vec3 pos = glm::vec3(this->position_.x, 2.0f * water_height - this->position_.y, this->position_.z);
        glm::vec3 center = this->position_ + this->front_;
        center.y = 2.0f*water_height - center.y;
        //pos.z = 2.0f * water_height - pos.z;
        return glm::lookAt(pos, center, this->up_);
    }

    void update(GLfloat delta_time, GLfloat terrain_height)
    {
        if (keys_[GLFW_KEY_W]) {
            processKeyboard(FORWARD, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_S]) {
            processKeyboard(BACKWARD, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_A]) {
            processKeyboard(LEFT, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_D]) {
            processKeyboard(RIGHT, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_Q] || keys_[GLFW_KEY_UP]) {
            processKeyboard(PITCH_UP, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_E] || keys_[GLFW_KEY_DOWN]) {
            processKeyboard(PITCH_DOWN, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_LEFT]) {
            processKeyboard(YAW_LEFT, delta_time, terrain_height);
        }
        if (keys_[GLFW_KEY_RIGHT]) {
            processKeyboard(YAW_RIGHT, delta_time, terrain_height);
        }
    }

    void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrain_pitch = true)
    {
        xoffset *= this->mouse_sensitivity_;
        yoffset *= this->mouse_sensitivity_;

        this->yaw_ += xoffset;
        this->pitch_ += yoffset;

        if (constrain_pitch) {
            if (this->pitch_ > 89.0f) {
                this->pitch_ = 89.0f;
            }
            if (this->pitch_ < -89.0f) {
                this->pitch_ = -89.0f;
            }
        }

        this->updateCameraVectors();
    }

    void processMouseScroll(GLfloat yoffset)
    {
        if (this->zoom_ >= 1.0f && this->zoom_ <= 45.0f) {
            this->zoom_ -= yoffset;
        }
        if (this->zoom_ <= 1.0f) {
            this->zoom_ = 1.0f;
        }
        if (this->zoom_ >= 45.0f) {
            this->zoom_ = 45.0f;
        }
    }

    void switchCameraMode()
    {
        mode_ = mode_ == NORMAL ? FIRST_PERSON : NORMAL;
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
        front.y = sin(glm::radians(this->pitch_));
        front.z = sin(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
        this->front_ = glm::normalize(front);
        this->right_ = glm::normalize(glm::cross(this->front_, this->world_up_));
        this->up_ = glm::normalize(glm::cross(this->right_, this->front_));
    }

    void processKeyboard(Camera_Movement direction, GLfloat delta_time, GLfloat terrain_height)
    {
        GLfloat velocity = this->movement_speed_ * delta_time;

        switch (direction) {
        case FORWARD:
            this->position_ += this->front_ * velocity;
            break;
        case BACKWARD:
            this->position_ -= this->front_ * velocity;
            break;
        case LEFT:
            this->position_ -= this->right_ * velocity;
            break;
        case RIGHT:
            this->position_ += this->right_ * velocity;
            break;
        case PITCH_UP:
            processMouseMovement(0.0f, 10.0f);
            break;
        case PITCH_DOWN:
            processMouseMovement(0.0f, -10.0f);
            break;
        case YAW_LEFT:
            processMouseMovement(-10.0f, 0.0f);
            break;
        case YAW_RIGHT:
            processMouseMovement(10.0f, 0.0f);
            break;
        }

        if (mode_ == FIRST_PERSON) {
            this->position_.y = terrain_height /*+ 2.0f*/; // TODO: replace by height of terrain + offset for pos of head
        }
    }
};
