#include "GameObject.hpp"

struct Light{
    glm::vec4 position;
    glm::vec4 color;
};


class Lava : public GameObject {
public:
    vector<Light> lights;
    float radius = 0.1f;

    void addLight(const glm::vec4& position, const glm::vec4& color){
        lights.push_back(Light{position, color});
    }

    Light getLight(int& index){
        return lights[index];
    }

    vector<Light> getLights(){
        return lights;
    }

private:

};