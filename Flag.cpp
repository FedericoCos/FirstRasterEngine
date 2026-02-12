#include "Flag.hpp"


Flag::~Flag(){}

glm::mat4 Flag::update(const float& delta_t){
    if(position.y >= .7f){
        direction = false;
    }
    else if(position.y <= .0f){
        direction = true;
    }

    glm::vec3 velocity = glm::vec3(0.0, speed * (direction ? 1 : -1) * delta_t, 0.0);

    position += velocity;

    rotation += rotational_speed * delta_t;

    if(rotation > 2 * PI) rotation -= 2 * PI;
    if(rotation < -2 * PI) rotation += 2 * PI;

    rotational_matrix = glm::rotate(glm::mat4(1.f), rotation, glm::vec3(.0f, 1.0f, .0f)) * original_rotation;
    translation_matrix = glm::translate(glm::mat4(1.f), position); 


    return Flag::getModelMatrix();
}