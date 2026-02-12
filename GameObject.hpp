#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define PI 3.1415


using namespace std;

struct Simplex {
    std::vector<glm::vec3> points;
    void add(const glm::vec3& point) { points.push_back(point); }
    glm::vec3& operator[](size_t i) { return points[points.size() - 1 - i]; }
    size_t size() const { return points.size(); }
    void remove(size_t index) { points.erase(points.begin() + index); }
};


class GameObject {
protected:
    vector<vector<glm::vec3>> vertices_collection;


    virtual bool handleLine(Simplex& simplex, glm::vec3& direction);
    virtual bool handleTriangle(Simplex& simplex, glm::vec3& direction);
    virtual bool handleTetrahedron(Simplex& simplex, glm::vec3& direction);

public:

    virtual glm::vec3 support(const vector<glm::vec3>& vertices, const glm::vec3& direction);
    virtual glm::vec3 getSupport(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB, const glm::vec3& direction);


    bool sameDirection(const glm::vec3& direction, const glm::vec3& ao){
        return glm::dot(direction, ao) > 0;
    }

    virtual vector<glm::vec3> getBox(vector<glm::vec3>& vertices, const glm::vec3& tolerance);

    virtual bool handleSimplex(Simplex& simplex, glm::vec3& direction);

    virtual bool gjk(vector<glm::vec3>& vertices_a, vector<glm::vec3>& vertices_b);

    virtual bool intersect(GameObject& other);

    virtual void setVertices(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance, const int& index);

    virtual void setVertices(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance);
    virtual void setVerticesAll(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance);

    virtual void setVertices(vector<glm::vec3>& vertices, vector<uint32_t>& indices, glm::mat4& model, const glm::vec3& tolerance);

    virtual void setVerticesInChunk(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance, const int chunk_size);
};
