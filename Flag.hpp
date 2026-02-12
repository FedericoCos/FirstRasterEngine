#pragma once

#include "GameObject.hpp"


class Flag : public GameObject {
public:
    bool active;
    int num;
    bool direction = true;

    glm::vec3 position;
    float rotation = 0.f;

    float speed = 0.7f;
    float rotational_speed = 0.7f;

    glm::mat4 scale_matrix;
    glm::mat4 rotational_matrix;
    glm::mat4 original_rotation;
    glm::mat4 translation_matrix;
    

    Flag(): active(false), num(0) {};

    Flag(bool active, int num): active(active), num(num) {};

    virtual ~Flag();

    void switchActive(){
        active = !active;
    }

    bool isActive(){
        return active;
    }

    int getNum(){
        return num;
    }

    virtual glm::mat4 update(const float& delta_t);

    glm::mat4 getModelMatrix(){
        return translation_matrix * rotational_matrix * scale_matrix;
    }

    virtual void InitFlag(glm::mat4& scale, glm::mat4& rot, glm::mat4& trans){
        scale_matrix = scale;
        rotational_matrix = rot;
        original_rotation = rot;
        translation_matrix = trans;
        position = glm::vec3(trans * glm::vec4(position, 1.0f));
    }

    void setActive(const bool& b){
        active = b;
    }

private:

};