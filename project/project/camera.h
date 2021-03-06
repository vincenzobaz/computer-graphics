#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "bezier.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    PITCH_UP,
    PITCH_DOWN,
    YAW_LEFT,
    YAW_RIGHT
};

enum Camera_Mode {
    NORMAL,
    FIRST_PERSON,
    BEZIER
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat ZOOM = 45.0f;

int bezierAcceleration = 1;

class Camera {
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

    vector<glm::vec3> bezierPath;
    size_t cu = 0; // Position in path

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW, GLfloat pitch = PITCH)
        : front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVTY),
          zoom_(ZOOM), mode_(NORMAL) {
        this->position_ = position;
        this->world_up_ = up;
        this->yaw_ = yaw;
        this->pitch_ = pitch;
        this->updateCameraVectors();
        BezierCurve b;
        vector<glm::vec3> v = {glm::vec3(90, 10, 10), glm::vec3(79.1844, 4.429621, 71.1),
                               glm::vec3(74.4177, 5.88337, 52.0015), glm::vec3(73.4158, 1.0124063, 39.9451),
                               glm::vec3(78.3972, 9.39166, 26.2307), glm::vec3(82.7383, 4.53701, 16.2617),
                               glm::vec3(77.9093, 6.567705, 7.12833), glm::vec3(66.0981, 0.101448, -2.27427)};
        b.Init(3000, v);
        bezierPath = b.getPath();
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(this->position_, this->position_ + this->front_, this->up_);
    }

    glm::mat4 getReversedViewMatrix(float water_height) {
        glm::vec3 pos = glm::vec3(this->position_.x, 2.0f * water_height - this->position_.y, this->position_.z);
        glm::vec3 center = this->position_ + this->front_;
        center.y = 2.0f*water_height - center.y;

        return glm::lookAt(pos, center, this->up_);
    }

    glm::mat4 getProjectionMatrix(int window_width, int window_height) {
        return glm::perspective(this->zoom_, (float) window_width / (float) window_height, 0.1f, 100.0f);
    }

    void accelerate(int movement, GLfloat velocity, GLfloat terrain_height) {
        if (mode_ == BEZIER) return;
        switch (movement) {
        case 1:
            processKeyboard(FORWARD, velocity, terrain_height);
            break;
        case 2:
            processKeyboard(BACKWARD, velocity, terrain_height);
            break;
        case 3:
            processKeyboard(RIGHT, velocity, terrain_height);
            break;
        case 4:
            processKeyboard(LEFT, velocity, terrain_height);
            break;
        }
    }

    void update(GLfloat velocity, GLfloat terrain_height) {
        if (mode_ == BEZIER) {
            if (keys_[GLFW_KEY_W]) updateBezier(1);
            else if (keys_[GLFW_KEY_S]) updateBezier(-1);
            else updateBezier(0);
        }
        else {
            if (keys_[GLFW_KEY_W]) {
                processKeyboard(FORWARD, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_S]) {
                processKeyboard(BACKWARD, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_A]) {
                processKeyboard(LEFT, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_D]) {
                processKeyboard(RIGHT, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_Q]) {
                processKeyboard(DOWN, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_E]) {
                processKeyboard(UP, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_UP]) {
                processKeyboard(PITCH_UP, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_DOWN]) {
                processKeyboard(PITCH_DOWN, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_LEFT]) {
                processKeyboard(YAW_LEFT, velocity, terrain_height);
            }
            if (keys_[GLFW_KEY_RIGHT]) {
                processKeyboard(YAW_RIGHT, velocity, terrain_height);
            }
        }
    }

    void processMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrain_pitch = true) {
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

    void processMouseScroll(GLfloat yoffset) {
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

    void switchCameraMode() {
        switch (mode_) {
        case NORMAL:
            mode_ = FIRST_PERSON;
            break;
        case FIRST_PERSON:
            mode_ = BEZIER;
            break;
        case BEZIER:
            mode_ = NORMAL;
            break;
        }
    }

    glm::vec2 get2dCoords() {
        return glm::vec2(position_.x, position_.z);
    }

    void printCameraPosition() {
        cout << this->position_.x << ", " << this->position_.y << ", " << this->position_.z << endl;
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
        front.y = sin(glm::radians(this->pitch_));
        front.z = sin(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
        this->front_ = glm::normalize(front);
        this->right_ = glm::normalize(glm::cross(this->front_, this->world_up_));
        this->up_ = glm::normalize(glm::cross(this->right_, this->front_));
    }

    void processKeyboard(Camera_Movement direction, GLfloat velocity, GLfloat terrain_height) {
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
        case UP:
            this->position_ += this->up_ * velocity;
            break;
        case DOWN:
            this->position_ -= this->up_ * velocity;
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
            this->position_.y = terrain_height /*+ 0.15f*/;
            //printCameraPosition();
        }
    }

    void updateBezier(int acceleration = 0) {
      bezierAcceleration = glm::clamp(bezierAcceleration + acceleration, 1, 5);
      cu += bezierAcceleration;
      cu = cu >= bezierPath.size() ? 0 : cu;
      position_ = bezierPath[cu];
    }
};
