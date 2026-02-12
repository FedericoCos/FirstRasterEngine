#include "GameObject.hpp"


using namespace std;

class Player : public GameObject {

private:
    glm::mat4 scale_matrix;
    glm::mat4 rot_matrix;

    glm::vec3 position;
    glm::vec3 initial_position;
    glm::vec3 current_velocity;

    glm::vec3 right_head_light;
    glm::vec3 left_head_light;

    glm::vec3 right_brake_light;
    glm::vec3 left_brake_light;
    glm::vec4 color_brake_light;

    bool braking;

    float max_speed;
    float speed;
    float turn_speed;
    float rotation_y;
    float inital_rotation;
    float acceleration;
    float friction;
    float brake_strength;
    float drift_factor;
    float speed_reduction = 1.f;

    float speed_y;
    float acceleration_y;

    bool is_boosting = false;
    float boost = 2;

    bool moving_forward;
    bool moving_backward;
    bool allow_rotation;

    bool is_falling;


    virtual void moveForward(float& delta_t);
    virtual void moveBackward(float& delta_t);
    virtual void turnLeft(float& delta_t);
    virtual void turnRight(float& delta_t);

    virtual void brake(float& delta_t);



public:
    Player();
    Player(int size, float max_speed, float turn_speed,
            float acceleration, float friction, float brake_strength, 
            float drift_factor, float acceleration_y);
    virtual ~Player();


    virtual glm::mat4 update(glm::vec3& m, glm::vec3& r, bool& fire, float& delta_t);
    virtual void fall(bool& on_floor, bool& on_ground, bool& on_lava);
    virtual glm::mat4 collision(glm::vec3& pos, float& rot);
    virtual void initPlayer(glm::mat4& trans, const float& rot, glm::mat4& scale);

    void setScaleMatrix(glm::mat4& mat){
        scale_matrix = mat;
    }

    bool isMovingBackward(){
        return moving_backward;
    }

    bool isMovingForward(){
        return moving_forward;
    }

    glm::mat4& getScaleMatrix(){
        return scale_matrix;
    }

    glm::mat4& getRotationalMatrix(){
        return rot_matrix;
    }

    glm::vec3 getPosition(){
        return position;
    }

    float getRotation(){
        return rotation_y;
    }

    glm::vec3 getRightHeadLight(){
        return right_head_light;
    }

    glm::vec3 getLeftHeadLight(){
        return left_head_light;
    }

    glm::vec3 getRightBrakeLight(){
        return right_brake_light;
    }

    glm::vec3 getLeftBrakeLight(){
        return left_brake_light;
    }

    glm::vec4 getBrakeColor(){
        color_brake_light = glm::vec4(1, 0, 0, Player::isBraking() ? 0.006 : 0.002);
        return color_brake_light;
    }

    bool isBraking(){
        return braking;
    }

    void startBoost(){
        is_boosting = true;
    }

    void stopBoost(){
        is_boosting = false;
    }

    void setLights(glm::vec3& rhl, glm::vec3& lhl, glm::vec3& rbl, glm::vec3& lbl){
        right_head_light = rhl;
        left_head_light = lhl;

        right_brake_light = rbl;
        left_brake_light = lbl;
    }

    void resetPosition(){
        position = initial_position;
        speed = 0.0f;
        is_falling = false;
        rotation_y = inital_rotation;
        speed_y = 0.0f;
    }

    void resetPosition(const glm::vec3& p, const float& r){
        position = p;
        rotation_y = r;
        speed = .0f;
        is_falling = false;
        speed_y = .0f;
    }


};