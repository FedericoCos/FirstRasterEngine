#include "GameObject.hpp"

class Lamp : public GameObject {
public:
    glm::vec4 light_pos;
    glm::vec4 light_color;

    Lamp() : light_pos(glm::vec4(1)), light_color(glm::vec4(1)) {}

    Lamp(glm::vec4& light_pos, glm::vec4& light_color) : 
        light_pos(light_pos),
        light_color(light_color){}

    void setVertices(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance){
        vertices_collection.resize(1);
        vector<glm::vec3> vs;
        int max_height = 4;

        for(auto& v : vertices){
            if(v.y < max_height){
                vs.push_back(v);
            }
        }

        GameObject::setVertices(vs, model, tolerance, 0);
    }

private:

};