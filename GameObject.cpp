#include "GameObject.hpp"

glm::vec3 GameObject::support(const vector<glm::vec3>& vertices, const glm::vec3& direction){
    float current_max = -FLT_MAX;
    glm::vec3 best;

    for(auto& v: vertices){
        float prod = glm::dot(v, direction);
        if(prod > current_max){
            current_max = prod;
            best = v;
        }
    }

    return best;
}

glm::vec3 GameObject::getSupport(const std::vector<glm::vec3>& verticesA, const std::vector<glm::vec3>& verticesB, const glm::vec3& direction) {
    return GameObject::support(verticesA, direction) - GameObject::support(verticesB, -direction);
}

bool GameObject::handleSimplex(Simplex& simplex, glm::vec3& direction) {

    switch (simplex.size()) {
        case 2: return handleLine(simplex, direction);
        case 3: return handleTriangle(simplex, direction);
        case 4: return handleTetrahedron(simplex, direction);
        default: return false;
    }
}

bool GameObject::handleLine(Simplex& simplex, glm::vec3& direction) {
    const float EPSILON = 1e-6f;

    const glm::vec3& a = simplex[0];
    const glm::vec3& b = simplex[1];
    const glm::vec3 ab = b - a;
    const glm::vec3 ao = -a;

    if (glm::dot(ab, ao) > 0.0f) {
        direction = glm::cross(glm::cross(ab, ao), ab);
        if (glm::length(direction) < EPSILON) {
            direction = glm::cross(ab, glm::vec3(1, 0, 0));
            if (glm::length(direction) < EPSILON) {
                direction = glm::cross(ab, glm::vec3(0, 1, 0));
            }
        }
    } else {
        simplex.remove(1);
        direction = ao;
    }
    return false;
}

bool GameObject::handleTriangle(Simplex& simplex, glm::vec3& direction) {
    const glm::vec3& a = simplex[0];
    const glm::vec3& b = simplex[1];
    const glm::vec3& c = simplex[2];
    const glm::vec3 ab = b - a;
    const glm::vec3 ac = c - a;
    const glm::vec3 ao = -a;
    const glm::vec3 abc = glm::cross(ab, ac);

    if (glm::dot(glm::cross(abc, ac), ao) > 0.0f) {
        if (glm::dot(ac, ao) > 0.0f) {
            simplex.remove(1);
            direction = glm::cross(glm::cross(ac, ao), ac);
        } else if (glm::dot(ab, ao) > 0.0f) {
            simplex.remove(2);
            direction = glm::cross(glm::cross(ab, ao), ab);
        } else {
            simplex.remove(1);
            simplex.remove(1);
            direction = ao;
        }
    } else if (glm::dot(glm::cross(ab, abc), ao) > 0.0f) {
        if (glm::dot(ab, ao) > 0.0f) {
            simplex.remove(2);
            direction = glm::cross(glm::cross(ab, ao), ab);
        } else {
            simplex.remove(1);
            simplex.remove(1);
            direction = ao;
        }
    } else {
        if (glm::dot(abc, ao) > 0.0f) {
            direction = abc;
        } else {
            std::swap(simplex[1], simplex[2]);
            direction = -abc;
        }
    }
    return false;
}

bool GameObject::handleTetrahedron(Simplex& simplex, glm::vec3& direction) {
    const glm::vec3& a = simplex[0];
    const glm::vec3& b = simplex[1];
    const glm::vec3& c = simplex[2];
    const glm::vec3& d = simplex[3];
    const glm::vec3 ab = b - a;
    const glm::vec3 ac = c - a;
    const glm::vec3 ad = d - a;
    const glm::vec3 ao = -a;

    const glm::vec3 abc = glm::cross(ab, ac);
    const glm::vec3 acd = glm::cross(ac, ad);
    const glm::vec3 adb = glm::cross(ad, ab);

    if (glm::dot(abc, ao) > 0.0f) {
        simplex.remove(3);
        return handleTriangle(simplex, direction);
    }
    if (glm::dot(acd, ao) > 0.0f) {
        simplex.remove(1);
        return handleTriangle(simplex, direction);
    }
    if (glm::dot(adb, ao) > 0.0f) {
        simplex.remove(2);
        return handleTriangle(simplex, direction);
    }

    return true; // Origin is inside the tetrahedron
}

bool GameObject::gjk(vector<glm::vec3>& vertices_a, vector<glm::vec3>& vertices_b){
    glm::vec3 direction = glm::normalize(vertices_b[0] - vertices_a[0]);
    Simplex simplex;
    glm::vec3 support = getSupport(vertices_a, vertices_b, direction);
    simplex.add(support);
    direction = -support;

    const int MAX_ITERATIONS = 300;
    const float EPSILON = 1e-6f;

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        support = getSupport(vertices_a, vertices_b, direction);
        
        if (glm::dot(support, direction) <= 0.0f) {
            return false; // No collision
        }

        simplex.add(support);

        if (handleSimplex(simplex, direction)) {
            return true; // Collision detected
        }

        if (glm::length(direction) < EPSILON * EPSILON) {
            return false; // No clear direction to proceed, assume no collision
        }
    }

    return false; 
}


bool GameObject::intersect(GameObject& other){
    for(size_t i = 0; i < vertices_collection.size(); i++){
        for(size_t j = 0; j < other.vertices_collection.size(); j++){
            if(GameObject::gjk(vertices_collection[i], other.vertices_collection[j])){
                return true;
            }
        }
    }

    return false;
}

void GameObject::setVertices(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance, const int& index){
    vertices_collection[index].clear();

    vector<glm::vec3> vs = GameObject::getBox(vertices, tolerance);
    for(auto& v : vs){
        vertices_collection[index].push_back(glm::vec3(model * glm::vec4(v, 1.f)));
    }
}

void GameObject::setVertices(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance){
    vertices_collection.resize(1);

    GameObject::setVertices(vertices, model, tolerance, 0);
}

void GameObject::setVertices(vector<glm::vec3>& vertices, vector<uint32_t>& indices, glm::mat4& model, const glm::vec3& tolerance){
    vector<vector<glm::vec3>> connections;
    connections.resize(vertices.size());
    for(size_t i = 0; i < indices.size(); i += 3){
        for(size_t j = 0; j < 3; j++){
            connections[indices[i + j]].push_back(vertices[indices[i + (j + 1) % 3]]);
            connections[indices[i + j]].push_back(vertices[indices[i + (j + 2) % 3]]);
        }
    }

    vertices_collection.resize(vertices.size());
    for(int i = 0; i < vertices.size(); i++){
        vertices_collection[i].clear();
        for(auto& v : connections[i]){
            vertices_collection[i].push_back(glm::vec3(model * glm::vec4(v, 1.f)));
        }
    }
}

void GameObject::setVerticesAll(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance){
    vertices_collection.resize(1);
    vertices_collection[0].clear();
    for(int i = 0; i < vertices.size(); i++){
        vertices_collection[0].push_back(glm::vec3(model * glm::vec4(vertices[i] + tolerance, 1.f)));
        vertices_collection[0].push_back(glm::vec3(model * glm::vec4(vertices[i] - tolerance, 1.f)));

    }
}

void GameObject::setVerticesInChunk(vector<glm::vec3>& vertices, glm::mat4& model, const glm::vec3& tolerance, const int chunk_size){
    vertices_collection.resize(vertices.size() / chunk_size);

    for(size_t i = 0; i < vertices.size(); i += chunk_size){
        vector<glm::vec3> vs;
        vs.resize(chunk_size);
        for(size_t j = 0; j < chunk_size; j++){
            vs[j] = vertices[i + j];
        }

        GameObject::setVertices(vs, model, tolerance, i / chunk_size);
    }
}

vector<glm::vec3> GameObject::getBox(vector<glm::vec3>& vertices, const glm::vec3& tolerance){
    float min_x, min_y, min_z;
    float max_x, max_y, max_z;

    glm::vec3 res = vertices[0];

    min_x = res.x;
    min_y = res.y;
    min_z = res.z;

    max_x = res.x;
    max_y = res.y;
    max_z = res.z;

    for(size_t i = 1; i < vertices.size(); i++){
        res = vertices[i];
        min_x = min(min_x, res.x);
        max_x = max(max_x, res.x);

        min_y = min(min_y, res.y);
        max_y = max(max_y, res.y);

        min_z = min(min_z, res.z);
        max_z = max(max_z, res.z);
    }

    return {
        {min_x + tolerance.x, min_y + tolerance.y, min_z + tolerance.z},
        {min_x + tolerance.x, min_y + tolerance.y, max_z - tolerance.z},
        {min_x + tolerance.x, max_y - tolerance.y, min_z + tolerance.z},
        {min_x + tolerance.x, max_y - tolerance.y, max_z - tolerance.z},
        {max_x - tolerance.x, min_y + tolerance.y, min_z + tolerance.z},
        {max_x - tolerance.x, min_y + tolerance.y, max_z - tolerance.z},
        {max_x - tolerance.x, max_y - tolerance.y, min_z + tolerance.z},
        {max_x - tolerance.x, max_y - tolerance.y, max_z - tolerance.z},
    };

}

