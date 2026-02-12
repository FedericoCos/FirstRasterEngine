#include "GameObject.hpp"

class Forest : public GameObject {
public:
    void setVertices(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance, const int& chunk){
        vector<glm::vec3> vs;
        int max_height = 2;

        for(auto& v : vertices){
            if(v.y <= max_height){
                vs.push_back(v);
            }
        }

        GameObject::setVerticesInChunk(vs, model, tolerance, chunk);
    }

private:

};