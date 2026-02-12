#include "Camera.hpp"

Camera::~Camera() {}

Camera::Camera() : target_position(glm::vec3(.0f)),
                     offset(5.5f),
                     speed(5.0f),
                     fov(glm::radians(60.0f)),
                     near_plane(.1f),
                     far_plane(200.f),
                     is_look_in(false)
    {
        past_offset = glm::vec3(offset, height, 0);

        min_offset = 2.0f;
        max_offset = 10.5f;
        offset_step = 0.5f;
        
        min_height = 1.f;
        max_height = 5.5f;

        height = min_height + (max_height - min_height) * ((offset - min_offset) / (max_offset - min_offset));

        is_look_in = false;
    }

Camera::Camera(glm::vec3 target_position, float offset, float height, float speed,
            float fov, float near_plane, float far_plane) :
                    target_position(target_position),
                     offset(offset),
                     height(height),
                     speed(speed),
                     fov(glm::radians(fov)),
                     near_plane(near_plane),
                     far_plane(far_plane), 
                     min_height(min_height),
                     max_height(max_height)
    {
        past_offset = glm::vec3(offset, height, 0);

        min_offset = 2.5f;
        max_offset = 10.5f;
        offset_step = 0.5f;

        height = min_height + (max_height - min_height) * ((offset - min_offset) / (max_offset - min_offset));

        is_look_in = false;
    }


void Camera::updateViewMatrix(){
    view_matrix = glm::lookAt(position, target_position, glm::vec3(.0f, 1.f, .0f));
}

void Camera::update(glm::vec3& t_position, float t_rotation, glm::vec3 &m, float delta_t, const bool& back){


    target_position = t_position;

    if(!is_look_in){
        glm::vec3 offset_vec = offset * glm::vec3(
            sin(t_rotation + PI / 2),
            0,
            cos(t_rotation + PI / 2)
        );
        if(back){
            offset_vec *= -1;
        }

        offset_vec = offset_vec + glm::vec3(.0f, height, .0f);

        past_offset = glm::mix(past_offset, offset_vec, speed * delta_t);

        glm::vec3 desired_position = target_position + past_offset;

        position = glm::mix(position, desired_position, speed * delta_t);
        
        Camera::updateViewMatrix();
    }
    else{
        t_rotation = t_rotation + PI / 2;
        position = target_position;
        position.y++;
        Camera::updateViewMatrixLookIn(t_rotation);
    }
}

void Camera::computeProjectionMatrix(float& ratio){
    aspect_ratio = ratio;
    projection_matrix = glm::perspective(fov, aspect_ratio, near_plane, far_plane);
    projection_matrix[1][1] *= -1;
}

void Camera::updateViewMatrixLookIn(float &t_rotation){
    view_matrix = glm::translate(glm::mat4(1), -position);
    view_matrix = glm::rotate(glm::mat4(1), -t_rotation, glm::vec3(0, 1, 0)) * view_matrix;
}

void Camera::menu(const glm::vec3& pos, const glm::vec3& t){
    position = pos;
    target_position = t;

    Camera::updateViewMatrix();
}