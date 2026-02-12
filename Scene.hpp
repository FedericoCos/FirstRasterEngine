#pragma once

#include "start.hpp"
#include "GameObject.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "Lamp.hpp"
#include "Forest.hpp"
#include "Flag.hpp"
#include "Lava.hpp"
#include "Car.hpp"
#include <list>

using namespace std;

class Scene : public GameEngine
{
private:
    string json_path;

    list<GameObject> objects;
    list<Lamp> lamps;
    Lamp lamp_menu;
    list<Forest> forest;
    list<Flag> flags;
    list<Car> cars;
    vector<glm::vec3> car_positions;
    float car_distance = 3.f;
    Lava lava;
    int lava_lights = 5;
    int active_car = 0;

    vector<glm::vec3> prev_player_pos;
    vector<float> prev_player_rot;
    int index_prev;

    Player player;
    vector<vector<glm::vec3>> player_lights;
    int player_index;
    std::vector<glm::vec3> player_vertices;
    Camera camera;
    bool previous_camera_direction;
    double offset_change;
    bool first;
    bool reset_pressed = false;

    bool day;
    float max_light;
    float min_light;
    float light_speed;
    float ambient_light;

    float time = 0.0;

    bool switch_camera_pressed;
    float ratio;

    float t_car = -RAND_MAX; 
    vector<glm::vec3> menu_light;
    int menu_light_size = 10;

    float min_time = 5.f;
    float max_time = 30.f;
    float current_time = max_time;
    float threshold = max_time;

    glm::vec3 checkpoint_pos;
    float checkpoint_rot;


    virtual void loadJson();
    virtual Model jsonToModel(const nlohmann::json& js);
    virtual glm::mat4 jsonToRot(const nlohmann::json& js);
    virtual void startScene();

public:
    Scene();
    Scene(string& json_path);
    virtual ~Scene();


    virtual void updateUniformBuffer(uint32_t currentImage) override;
    virtual void updateUniformBufferMenu(uint32_t currentImage) override;

    virtual void getSixAxis(float &deltaT, glm::vec3 &m, glm::vec3 &r, bool &fire) override;

    virtual void selectCar();

    void handleScroll(double& scroll){
        getCamera() -> modifyOffset(scroll);
    }

    Camera * getCamera(){
        return &camera;
    }

    void resetScene(){
        current_time = max_time;
        threshold = max_time;
        for(Flag& f : flags){
            f.setActive(f.getNum() == 0);
        }
        checkpoint_pos = player.getPosition();
        checkpoint_rot = player.getRotation();
    }


    
};