#include "Player.hpp"

Player::~Player(){}

Player::Player()
    : GameObject(),         
      max_speed(20.0f),      
      speed(0.0f),
      turn_speed(1.4f),
      rotation_y(0.0f),
      acceleration(6.0f),
      friction(1.5f),
      brake_strength(12.0f),
      drift_factor(0.9f),
      moving_forward(false),
      moving_backward(false),
      allow_rotation(true),
      is_falling(false)
{
    position = glm::vec3(.0f);
    current_velocity = glm::vec3(.0f);
    speed_y = 0.f;
    acceleration_y = 9.8f;
    right_head_light = glm::vec3(-0.45, 0.4, 0.35);
    left_head_light = glm::vec3(-0.45, 0.4, -0.35);

    right_brake_light = glm::vec3(1.6, 0.45, 0.45);
    left_brake_light = glm::vec3(1.6, 0.45, -0.45);
}

Player::Player(int size, float max_speed, float turn_speed,
               float acceleration, float friction, float brake_strength,
                float drift_factor, float acceleration_y)
    : GameObject(),        
      max_speed(max_speed),    
      speed(0.0f),
      turn_speed(turn_speed),
      rotation_y(0.0f),
      acceleration(acceleration),
      friction(friction),
      brake_strength(brake_strength),
      drift_factor(drift_factor),
      acceleration_y(acceleration_y),
      moving_forward(false),   
      moving_backward(false),
      allow_rotation(true),
      is_falling(false)
{
    position = glm::vec3(.0f);
    current_velocity = glm::vec3(.0f);
    speed_y = 0.f;
    right_head_light = glm::vec3(-0.45, 0.4, 0.35);
    left_head_light = glm::vec3(-0.45, 0.4, -0.35);

    right_brake_light = glm::vec3(1.6, 0.45, 0.45);
    left_brake_light = glm::vec3(1.6, 0.45, -0.45);
}

glm::mat4 Player::update(glm::vec3& m, glm::vec3& r, bool& fire, float& delta_t){
    allow_rotation = true;
    moving_forward = false;
    moving_backward = false;
    braking = false;
    
    if (m.z == -1){
        Player::moveForward(delta_t);
    }
    if(m.z == 1){
        Player::moveBackward(delta_t);
    }
    if(m.x == -1 && allow_rotation){
        if(speed > 0){
            Player::turnRight(delta_t);
        }
        else{
            Player::turnLeft(delta_t);
        }
    }
    if(m.x == 1 && allow_rotation){
        if(speed > 0){
            Player::turnLeft(delta_t);
        }
        else{
            Player::turnRight(delta_t);
        }
    }
    if(fire){
        Player::brake(delta_t);
    }

    if(abs(speed) > max_speed * (is_boosting ? boost : 1) * speed_reduction){
        speed = max_speed * (speed < 0 ? -1 : 1) * (is_boosting ? boost : 1) * speed_reduction;
    }

    if(!moving_forward && !moving_backward && abs(speed) < .5f){
        speed = .0f;
    }
    else if(!moving_forward && !moving_backward){
        speed -= friction * delta_t * (speed < 0 ? -1 : 1);
    }

    glm::vec3 velocity = speed * delta_t * glm::vec3(sin(rotation_y - PI / 2), .0f, cos(rotation_y - PI / 2));
    if(is_falling){
        speed_y -= acceleration_y * delta_t;
        velocity = glm::vec3(velocity.x, speed_y * delta_t, velocity.z);
    }

    current_velocity = glm::mix(current_velocity, velocity, drift_factor);
    position += current_velocity;

    if(rotation_y > 2 * PI) rotation_y -= 2 * PI;
    if(rotation_y < -2 * PI) rotation_y += 2 * PI;

    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.f), rotation_y, glm::vec3(.0f, 1.f, .0f));
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position);

    return translation_matrix * rotation_matrix * scale_matrix;
}

void Player::fall(bool& on_floor, bool& on_ground, bool& on_lava){
    if((on_floor || (on_ground && !on_lava) && !is_falling)){
        position.y = on_floor ? 0.f : -.2f;
        speed_y = 0.f;
        speed_reduction = on_floor ? 1.f : .3f;
    }
    else if(on_lava){
        is_falling = true;
    }

    if(position.y < -3.f){
        Player::resetPosition();
    }
}

glm::mat4 Player::collision(glm::vec3& pos, float& rot){
    position = glm::vec3(pos.x, position.y, pos.z);
    rotation_y = rot;
    speed = .0f;

    rot_matrix = glm::rotate(glm::mat4(1.0f), rotation_y, glm::vec3(.0f, 1.f, .0f));
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.f), position);

    return translation_matrix * rot_matrix * scale_matrix;
}

void Player::initPlayer(glm::mat4& trans, const float& rot, glm::mat4& scale){
    scale_matrix = scale;
    position = glm::vec3(trans * glm::vec4(position, 1.0f));
    initial_position = position;
    rotation_y = rot;
    inital_rotation = rot;
}

void Player::moveForward(float& delta_t){
    if(speed < 0){
        allow_rotation = false;
        Player::brake(delta_t);
    }
    else{
        moving_forward = true;
        speed += acceleration * speed_reduction * (is_boosting ? boost : 1) * delta_t * (1.f - (speed / (max_speed* (is_boosting ? boost : 1))));
    }
}

void Player::moveBackward(float& delta_t){
    if(speed > 0){
        allow_rotation = false;
        Player::brake(delta_t);
    }
    else{
        moving_backward = true;
        speed -= acceleration * speed_reduction * delta_t * (1.f - (-speed / max_speed));
    }
}

void Player::brake(float& delta_t){
    braking = true;
    if (speed > .0f){
        speed -= brake_strength * delta_t;
        if (speed < .0f) speed = .0f;
    }
    else if(speed < .0f){
        speed += brake_strength * delta_t;
        if(speed > .0f) speed = .0f;
    }
}

void Player::turnLeft(float& delta_t){
    if(abs(speed) > 0){
        float effective_turn_speed = turn_speed * (1.f - abs(speed) / (max_speed * 2));
        rotation_y -= effective_turn_speed * delta_t;
    }
}

void Player::turnRight(float& delta_t){
    if(abs(speed) > .0f){
        float effective_turn_speed = turn_speed * (1.f - abs(speed) / (max_speed * 2));
        rotation_y += effective_turn_speed * delta_t;
    }
}