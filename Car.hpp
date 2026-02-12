#include "GameObject.hpp"

class Car : public GameObject{
public:
    glm::vec3 position;
    float speed = 3.f;

    void update(const glm::vec3& target, float& delta_t){
        position = glm::mix(position, target, speed * delta_t);
    }

    void setPosition(const glm::vec3& pos){
        position = pos;
    }

    glm::vec3 getPosition(){
        return position;
    }

private:


};