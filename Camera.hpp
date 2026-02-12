#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <iostream>

#define PI 3.1415

using namespace std;

class Camera{
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 past_offset;
    float offset;
    float offset_step;
    float min_offset;
    float max_offset;
    float height;
    float min_height;
    float max_height;
    glm::vec3 target_position;
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;

    float speed;
    float fov;
    float near_plane;
    float far_plane;
    float aspect_ratio;
    bool is_look_in;

    virtual void updateViewMatrix();



public:
    Camera();
    virtual ~Camera();
    Camera(glm::vec3 target_position, float offset, float height, float speed,
            float fov, float near_plane, float far_plane);
    
    virtual void update(glm::vec3& t_position, float t_rotation, glm::vec3 &m, float delta_t, const bool& back);

    glm::mat4 getViewMatrix(){
        return view_matrix;
    }

    glm::mat4 getProjectionMatrix(){
        return projection_matrix;
    }

    virtual void computeProjectionMatrix(float& ratio);

    virtual void updateViewMatrixLookIn(float& t_rotation);

    glm::vec3 getPosition(){
        return position;
    }

    void modifyOffset(double scroll){
        offset += offset_step * scroll;
        offset = max(offset, min_offset);
        offset = min(offset, max_offset);

        height = min_height + (max_height - min_height) * ((offset - min_offset) / (max_offset - min_offset));
    }

    bool isLookIn(){
        return is_look_in;
    }

    void switchCamera(){
        is_look_in = !is_look_in;
    }

    virtual void menu(const glm::vec3& pos, const glm::vec3& t);

};