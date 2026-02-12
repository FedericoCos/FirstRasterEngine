#include "Scene.hpp"

Scene::Scene() : GameEngine(), json_path("scene.json"){
    Scene::loadJson();
    initialBackgroundColor = {.0f, .085f, 1.f, 1.f};
    first = true;
    day = true;
    ambient_light = .4f;
    max_light = 1.0f;
    min_light = 0.1f;
    light_speed = 2.f;
    offset_change = 0.f;
    previous_camera_direction = false;
    switch_camera_pressed = false;

    index_prev = 0;
    prev_player_pos.reserve(5);
    prev_player_rot.reserve(5);
}
Scene::Scene(string& json_path) : GameEngine(), json_path(json_path){
    Scene::loadJson();
    initialBackgroundColor = {.0f, .085f, 1.f, 1.f};
    first = true;
    day = true;
    ambient_light = .4f;
    max_light = 1.0f;
    min_light = 0.1f;
    light_speed = 2.f;
    offset_change = 0.f;
    previous_camera_direction = false;
    switch_camera_pressed = false;

    index_prev = 0;
    prev_player_pos.reserve(5);
    prev_player_rot.reserve(5);
}
Scene::~Scene(){}

void Scene::loadJson(){
    ifstream file(json_path);
    nlohmann::json scene_json;
    file >> scene_json;
    file.close();

    int s = scene_json.size();
    //models.resize(s);

    for(size_t i = 0; i < s; i++){
        Model mod = Scene::jsonToModel(scene_json[i]);
        if(!scene_json[i]["isMenu"]){
            models.push_back(mod);
            if(models[i].isPlayer){
                player_index = i;
            }
        }
        else{
            menu_models.push_back(mod);
        }
    }
}

Model Scene::jsonToModel(const nlohmann::json& js){
    Model mod;
    mod.modelPath = js["model_path"];
    mod.texturePath = js["texture_path"];
    mod.name = js["name"];
    mod.vertex_shader = js["vertex_shader"];
    mod.fragment_shader = js["fragment_shader"];

    mod.scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(js["scale"][0], js["scale"][1], js["scale"][2]));
    mod.rotMatrix = glm::mat4(1);

    for(size_t i = 0; i < js["num_rotation"]; i++){
        mod.rotMatrix = Scene::jsonToRot(js["rotate"][i]) * mod.rotMatrix;
    }
    mod.transpMatrix = glm::translate(glm::mat4(1), glm::vec3(js["transpose"][0], js["transpose"][1], js["transpose"][2]));

    if(js["name"] == "Player"){
        mod.isPlayer = true;
        float rot = js["rotate"][0][2];
        rot = glm::radians(rot);
        player.initPlayer(mod.transpMatrix, rot, mod.scaleMatrix);

        glm::vec3 rhl = glm::vec3(js["r_h_l"][0], js["r_h_l"][1], js["r_h_l"][2]);
        glm::vec3 lhl = glm::vec3(js["l_h_l"][0], js["l_h_l"][1], js["l_h_l"][2]);
        glm::vec3 rbl = glm::vec3(js["r_b_l"][0], js["r_b_l"][1], js["r_b_l"][2]);
        glm::vec3 lbl = glm::vec3(js["l_b_l"][0], js["l_b_l"][1], js["l_b_l"][2]);
        player.setLights(rhl, lhl, rbl, lbl);
        mod.obj = &player;
    }
    else if(js["name"] == "Lamp"){
        mod.isPlayer = false;
        mod.modelMatrix = mod.transpMatrix * mod.rotMatrix * mod.scaleMatrix;
        mod.normalMatrix = glm::inverse(glm::transpose(mod.modelMatrix));
        glm::vec4 pos = mod.modelMatrix * glm::vec4(1.9, 4.8, 0.0, 1);
        glm::vec4 color = glm::vec4(1, 0.77, 0.56, 1);
        if(!js["isMenu"]){
            lamps.emplace_back(Lamp{pos, color});
            mod.obj = &lamps.back();
        }
        else{
            lamp_menu = Lamp{pos, color};
            mod.obj = &lamp_menu;
        }
    }
    else if(js["name"] == "Forest"){
        mod.isPlayer = false;
        mod.modelMatrix = mod.transpMatrix * mod.rotMatrix * mod.scaleMatrix;
        mod.normalMatrix = glm::inverse(glm::transpose(mod.modelMatrix));
        forest.emplace_back(Forest{});
        mod.obj = &forest.back();
    }
    else if(js["name"] == "Flag"){
        mod.isPlayer = false;
        mod.modelMatrix = mod.transpMatrix * mod.rotMatrix * mod.scaleMatrix;
        mod.normalMatrix = glm::inverse(glm::transpose(mod.modelMatrix));
        int index = js["num"];
        flags.emplace_back(Flag{index == 0, index});
        mod.obj = &flags.back();
    }
    else if(js["name"] == "Car"){
        mod.isPlayer = false;
        cars.emplace_back(Car{}); 
        mod.obj = &cars.back();
        mod.modelMatrix = mod.transpMatrix * mod.rotMatrix * mod.scaleMatrix;
        mod.normalMatrix = glm::inverse(glm::transpose(mod.modelMatrix));
        player_lights.emplace_back();
        player_lights[player_lights.size() - 1].push_back(glm::vec3(js["r_h_l"][0], js["r_h_l"][1], js["r_h_l"][2]));
        player_lights[player_lights.size() - 1].push_back(glm::vec3(js["l_h_l"][0], js["l_h_l"][1], js["l_h_l"][2]));
        player_lights[player_lights.size() - 1].push_back(glm::vec3(js["r_b_l"][0], js["r_b_l"][1], js["r_b_l"][2]));
        player_lights[player_lights.size() - 1].push_back(glm::vec3(js["l_b_l"][0], js["l_b_l"][1], js["l_b_l"][2]));
    }
    else{
        mod.isPlayer = false;
        objects.emplace_back(); 
        mod.obj = &objects.back();
        mod.modelMatrix = mod.transpMatrix * mod.rotMatrix * mod.scaleMatrix;
        mod.normalMatrix = glm::inverse(glm::transpose(mod.modelMatrix));
    }

    return mod;
}

glm::mat4 Scene::jsonToRot(const nlohmann::json& js){
    glm::vec3 vec = glm::vec3(0.0f, 0.0f, 0.0f);
    float rot;
    for(int i = 0; i < 3; i++){
        if(js[i] != 0.0){
            vec[i] = 1;
            rot = js[i];
            break;
        }
    }
    return glm::rotate(glm::mat4(1.0f), glm::radians(rot), vec);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
    if(yoffset > 0){
        scene -> getCamera() -> modifyOffset(-1.0);
    }
    else{
        scene -> getCamera() -> modifyOffset(1.0);
    }
}

void Scene::selectCar(){
    models[player_index].texturePath = menu_models[active_car].texturePath;
    models[player_index].modelPath = menu_models[active_car].modelPath;
    models[player_index].scaleMatrix = menu_models[active_car].scaleMatrix;
    models[player_index].rotMatrix = menu_models[active_car].rotMatrix;
    models[player_index].transpMatrix = menu_models[active_car].transpMatrix;
    models[player_index].modelMatrix = models[player_index].transpMatrix * models[player_index].rotMatrix * models[player_index].scaleMatrix;

    player.initPlayer(models[player_index].transpMatrix, player.getRotation(), models[player_index].scaleMatrix);
    player.setLights(player_lights[active_car][0],player_lights[active_car][1],player_lights[active_car][2],player_lights[active_car][3]);
}

// To get input
void Scene::getSixAxis(float &deltaT, glm::vec3 &m, glm::vec3 &r, bool &fire) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static float lastTime = 0.0f;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>
                (currentTime - startTime).count();
    deltaT = time - lastTime;
    lastTime = time;

    
    if(glfwGetKey(window, GLFW_KEY_ENTER)) {
        switching=true;
        first = true;
        Scene::selectCar();
    }
    if(glfwGetKey(window, GLFW_KEY_A)) {
        m.x = -1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_D)) {
        m.x = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_S)) {
        m.z = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_W)) {
        m.z = -1.0f;
    }
    if(!reset_pressed && glfwGetKey(window, GLFW_KEY_R)) {
        reset_pressed = true;
        m.y = 1.0f;
    }
    else if(reset_pressed && !glfwGetKey(window, GLFW_KEY_R)){
        reset_pressed = false;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) && !switch_camera_pressed) {
        switch_camera_pressed = true;
        camera.switchCamera();
    }
    else if(!glfwGetKey(window, GLFW_KEY_Q)){
        switch_camera_pressed = false;
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)){
        player.startBoost();
    }
    else{
        player.stopBoost();
    }
    
    fire = glfwGetKey(window, GLFW_KEY_SPACE);

    if(!is_menu)
        glfwSetScrollCallback(window, scroll_callback);
}

void Scene::updateUniformBufferMenu(uint32_t current_image){
    if(abs(ratio - swapChainExtent.width / (float) swapChainExtent.height) > 0.0001 || first){
        ratio = swapChainExtent.width / (float) swapChainExtent.height;
        camera.computeProjectionMatrix(ratio);
        if(first){
            initialBackgroundColor = {0.f/255.f, 70.f/255.f, 255.f/255.f, 1.f};
            float distance = 8.0f;
            float angle = glm::radians(30.f);
            glm::vec3 d = glm::vec3(cos(angle), 0.f, sin(angle)) * distance;
            d.y = 2.f;
            camera.menu(d, glm::vec3(0.f));
            glm::vec3 p = glm::vec3(1.5, -0.2, -0.0);
            for(Car& c : cars){
                c.setPosition(glm::vec3(.0f));
                car_positions.push_back(p);
                p.x = -2;
                p.z -= 4.5;
            }
        }
        first = false;
    } 

    float delta_t;
    glm::vec3 m = glm::vec3(.0f), r = glm::vec3(.0f);
    bool fire = false;
    getSixAxis(delta_t, m, r, fire);

    if(fire  && t_car <= 0){
        t_car = 0.7f;
        active_car++;
        if(active_car >= cars.size()){
            active_car = 0;
        }

        glm::vec3 temp = car_positions[car_positions.size() - 1];
        for(size_t i = car_positions.size() - 1; i > 0; i--){
            car_positions[i] = car_positions[i-1];
        }
        car_positions[0] = temp;

    }
    else if(!fire){
        t_car = 0;
    }
    t_car -= delta_t;

    int i = 0;
    for(Car& c : cars){
        c.update(car_positions[i], delta_t);
        menu_models[i].transpMatrix=glm::translate(glm::mat4(1.0f), c.getPosition());
        menu_models[i].rotMatrix=menu_models[i].rotMatrix*glm::rotate(glm::mat4(1.0f),0.6f * delta_t,glm::vec3(0.0f,1.0f,0.0f));
        menu_models[i].modelMatrix=menu_models[i].transpMatrix * menu_models[i].rotMatrix * menu_models[i].scaleMatrix;
        menu_models[i].normalMatrix=glm::transpose(glm::inverse(menu_models[i].modelMatrix));
        i++;
    }
    UniformBufferObject ubo{};
    ubo.view = camera.getViewMatrix();
    ubo.proj = camera.getProjectionMatrix();
    ubo.camera_pos = glm::vec4(camera.getPosition(), 1);
    ubo.ambient_light = glm::vec4(1, 1, 1, ambient_light);

    glm::vec4 sun_direction(0, 0, 1, 1);
    glm::vec4 sun_color(1, 1, 1, ambient_light);
    ubo.directional_light = DirectionalLight{sun_direction, sun_color};

    ubo.point_lights[0] = PointLight{lamp_menu.light_pos, lamp_menu.light_color};

    memcpy(uniformBuffersMapped[current_image], &ubo, sizeof(ubo));
}

void Scene::updateUniformBuffer(uint32_t current_image){
    if(first){
        Scene::startScene();
        glfwSetWindowUserPointer(window, this);
        for(int i = 0; i < 5; i++){
            prev_player_pos[i] = player.getPosition();
            prev_player_rot[i] = player.getRotation();
        }

        checkpoint_pos = player.getPosition();
        checkpoint_rot = player.getRotation();

        first = false;
    }

    if(abs(ratio - swapChainExtent.width / (float) swapChainExtent.height) > 0.0001){
        ratio = swapChainExtent.width / (float) swapChainExtent.height;
        camera.computeProjectionMatrix(ratio);
    } 

    float delta_t;
    glm::vec3 m = glm::vec3(.0f), r = glm::vec3(.0f);
    bool fire = false;
    offset_change = 0.0;
    Scene::getSixAxis(delta_t, m, r, fire);

    current_time -= delta_t;
    if(current_time <= 0){
        player.resetPosition();
        Scene::resetScene();
    }

    if(m.y == 1){
        player.resetPosition(checkpoint_pos, checkpoint_rot);
        current_time = threshold;
    }


    ambient_light += (day ? 1 : -1) * delta_t * light_speed / 100;
    if(ambient_light >= max_light || ambient_light <= min_light){
        day = !day;
        ambient_light = min(ambient_light, max_light);
        ambient_light = max(ambient_light, min_light);
    }
    initialBackgroundColor = {.0f, .085f * (ambient_light / (2 * max_light)), 1.f, 1.f};
    

    UniformBufferObject ubo_player{};
    prev_player_pos[index_prev] = player.getPosition();
    prev_player_rot[index_prev] = player.getRotation();
    index_prev = (index_prev + 1) % 5;
    glm::mat4 player_model = player.update(m, r, fire, delta_t);
    float tollerance = .15f;
    player.setVertices(player_vertices, player_model, glm::vec3(0, 0, 0.1));
    bool on_road = false;
    bool on_ground = false;
    bool on_lava = false;
    for(size_t i = 0; i < models.size(); i++){
        if(models[i].name == "Flag" && dynamic_cast<Flag *> (models[i].obj) -> isActive()){
            glm::mat4 mat = dynamic_cast<Flag *> (models[i].obj) -> update(delta_t);
            models[i].modelMatrix = mat;
            models[i].normalMatrix = glm::transpose(glm::inverse(mat));
        }


        if(!models[i].isPlayer && player.intersect(* models[i].obj)){
            if(models[i].name == "Field"){
                on_ground = true;
            }
            else if(models[i].name == "Road"){
                on_road = true;
            }
            else if(models[i].name == "Flag"){
                Flag * f = dynamic_cast<Flag *> (models[i].obj);
                if(f -> isActive()){
                    threshold -= 2.f;
                    threshold = max(threshold, min_time);
                    current_time = threshold;
                    checkpoint_pos = player.getPosition();
                    checkpoint_rot = player.getRotation();
                    f -> switchActive();
                    int ind = (f -> getNum() + 1) % 8;
                    for(Flag& flag: flags){
                        if(flag.getNum() == ind){
                            flag.switchActive();
                            break;
                        }
                    }
                }
            }           
            else if(models[i].name != "Lava"){
                player_model = player.collision(prev_player_pos[(index_prev + 1) % 5], prev_player_rot[(index_prev + 1) % 5]);
            }
            if(models[i].name == "Lava"){
                on_lava = true;
            }
        }
    }
    player.fall(on_road, on_ground, on_lava);
    glm::vec3 player_pos = player.getPosition();
    models[player_index].modelMatrix = player_model;
    models[player_index].normalMatrix = glm::transpose(glm::inverse(player_model));
    if(player.isMovingBackward() || player.isMovingForward()){
        previous_camera_direction = player.isMovingBackward();
    }

    camera.update(player_pos, player.getRotation(), m, delta_t, previous_camera_direction);

    UniformBufferObject ubo{};
    ubo.view = camera.getViewMatrix();
    ubo.proj = camera.getProjectionMatrix();
    ubo.camera_pos = glm::vec4(camera.getPosition(), 1);


    // Ambient light
    ubo.ambient_light = glm::vec4(1, 1, 1, ambient_light);

    // Setting up all the point lights -> brake lights and lamp lights
    int i = 0;
    ubo.point_lights[i++] = PointLight{player_model * glm::vec4(player.getLeftBrakeLight(), 1), player.getBrakeColor()};
    ubo.point_lights[i++] = PointLight{player_model * glm::vec4(player.getRightBrakeLight(), 1), player.getBrakeColor()};
    for(auto& lamp : lamps){
        ubo.point_lights[i] = PointLight{lamp.light_pos, lamp.light_color};
        i++;
    }
    for(auto& light : lava.getLights()){
        ubo.point_lights[i] = PointLight{light.position, light.color};
        i++;
    }

    // setting up the head lights
    glm::vec4 color(1, 1, 1, ambient_light > 0.7 ? 1.5: 3.5);
    glm::vec4 direction = glm::rotate(glm::mat4(1), player.getRotation(), glm::vec3(0, 1, 0)) * glm::vec4(-1, -0.1, 0, 1);
    float cutOff = glm::cos(glm::radians(18.f));
    float outerCutOff = glm::cos(glm::radians(30.f));

    ubo.spot_lights[0] = SpotLight{player_model * glm::vec4(player.getLeftHeadLight(), 1), color, direction, 
                                    cutOff, outerCutOff};
    
    ubo.spot_lights[1] = SpotLight{player_model * glm::vec4(player.getRightHeadLight(), 1), color, direction, 
                                    cutOff, outerCutOff};


    // setting up the sun light
    float angle = (PI / 2) + ((max_light - max_light * (ambient_light - min_light) / (max_light - min_light)) * (day ? 1 : -1) * (-PI));
    glm::vec4 sun_direction(cos(angle), sin(angle), 0, 1);
    glm::vec4 sun_color(1, 1, 1, ambient_light);
    ubo.directional_light = DirectionalLight{sun_direction, sun_color};

    time += delta_t;
    ubo.time = time;


    memcpy(uniformBuffersMapped[current_image], &ubo, sizeof(ubo));
}

void Scene::startScene(){
    ratio = swapChainExtent.width / (float) swapChainExtent.height;
    camera.computeProjectionMatrix(ratio);

    for(Model model : models){
        if(model.obj != nullptr){

            if(model.isPlayer){
                player_vertices.resize(model.vertices.size());
            }
            std::vector<glm::vec3> vertices;
            vertices.resize(model.vertices.size());

            for(size_t i = 0; i < vertices.size(); i++){
                vertices[i] = model.vertices[i].pos;
                if(model.isPlayer){
                    player_vertices[i] = model.vertices[i].pos;
                }
            }

            glm::mat4 model_mat = model.transpMatrix * model.rotMatrix * model.scaleMatrix;
            if(model.name == "Road"){
                model.obj -> setVertices(vertices, model.indices, model_mat, glm::vec3(0, 0, 0));
            }
            else if(model.name == "Forest"){
                dynamic_cast<Forest *> (model.obj) -> setVertices(vertices, model_mat, glm::vec3(0), 50);
            }
            else if(model.name == "Lamp"){
                dynamic_cast<Lamp *> (model.obj) -> setVertices(vertices, model_mat, glm::vec3(0, 0, 0));
            }
            else if(model.name == "House"){
                model.obj -> setVertices(vertices, model_mat, glm::vec3(0));
            }
            else if(model.name == "Box"){
                model.obj -> setVertices(vertices, model_mat, glm::vec3(0));
            }
            else if(model.name == "Player"){
                model.obj -> setVertices(vertices, model_mat, glm::vec3(0, 0, 0.1));
            }
            else if(model.name == "Flag"){
                dynamic_cast<Flag *> (model.obj) -> InitFlag(model.scaleMatrix, model.rotMatrix, model.transpMatrix);
                model.obj -> setVertices(vertices, model_mat, glm::vec3(0));
            }
            else if(model.name == "Field"){
                model.obj -> setVertices(vertices, model_mat, glm::vec3(0, -0.3, 0));
            }
            else if(model.name == "Lava"){
                model.obj -> setVerticesAll(vertices, model_mat, glm::vec3(0, 4.0, 0));

                for(size_t i = 0; i < lava_lights; i++){
                    float d = ((float) rand() / RAND_MAX) * 0.8 + 0.2;
                    glm::vec4 dir = glm::vec4(glm::normalize(glm::vec3((float) rand() / RAND_MAX - 0.5, 0.0f, (float) rand() / RAND_MAX - 0.5)) * d, 1.f);
                    lava.addLight(model_mat * dir, glm::vec4(0.81, 0.6, 0.13, 5));
                }
            }
        }
    }
}

int main() {
    Scene app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
