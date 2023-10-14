#include "MyRMan.h"
#include <fstream>
#include "os/get_default_font_filename.h"
#include <filesystem>

void MyLevel::load_textures(std::vector<std::string> textures_names){
    for(std::string name: textures_names){
       std::shared_ptr<sf::Image> image = std::make_shared<sf::Image>();
        image->loadFromFile(std::filesystem::path("assets") / "textures" / name);
        textures.insert({name, {image}});
    }
}

MyRMan::MyRMan(){
    font.loadFromFile(get_default_font_filename());
}

void MyRMan::load_walls(auto j){
    auto blocks = j["walls"];
    sf::Color color;
    anger::Block to_set;
    int x;
    int y;
    for (auto iter = blocks.begin(); iter != blocks.end(); iter++){
        x = iter->at("x").template get<int>();
        y = iter->at("y").template get<int>();
        color = anger::color_from_hex(iter->at("color").template get<std::string>());
        // Проверяем, заданы ли для данного блока текстуры.
        if (iter->contains("t_right"))
            to_set = anger::Block{
                lvl.textures.at(iter->at("t_top")), 
                lvl.textures.at(iter->at("t_bottom")),
                lvl.textures.at(iter->at("t_right")), 
                lvl.textures.at(iter->at("t_left")), 
                color};
        else to_set = anger::Block{color, false};
        lvl.set_block(to_set, x, y);
    }
}

void MyRMan::load_level(std::string path){
    std::ifstream FILE(path);
    nlohmann::json j = nlohmann::json::parse(FILE);
    auto header = j["header"];
    lvl = MyLevel{header["lvl_name"].template get<std::string>(), 
                header["width"].template get<int>(),
                header["height"].template get<int>(),
                header["light-brightness"].template get<double>(),
                header["light-decay-factor"].template get<double>(),
                header["player-x"].template get<double>(),
                header["player-y"].template get<double>()};

    lvl.load_textures(header["textures"]);
    load_walls(j);
    load_floor(j);
}

void MyRMan::load_floor(auto j){
    auto tiles = j["floor"];
    int x;
    int y;
    std::string cur_texture;
    for (auto iter = tiles.begin(); iter != tiles.end(); iter++){
        x = iter->at("x").template get<int>();
        y = iter->at("y").template get<int>();
        cur_texture = iter->at("t").template get<std::string>();
        if (lvl.textures.find(cur_texture) != lvl.textures.end()){ // Проверяем, что текстура с таким именем была загружена
            lvl.set_tile(lvl.textures[cur_texture], x, y);
        }
    }
}

void MyRMan::load_settings_file(std::string path){
    std::ifstream FILE(path);
    std::string cur_line;
    std::vector<std::string> raw_lines;
    while (std::getline(FILE, cur_line)){
        // Строка не пустая и не комментарий
        if (cur_line.size() > 0 && cur_line[0] != '#'){
            // Заменяем в прочитанных строках ": " на ":"
            size_t colon_pos = cur_line.find(": ");
            if (colon_pos != std::string::npos)
                raw_lines.push_back(cur_line.replace(colon_pos, 2, ":"));
        }
    }
    for (std::string rl: raw_lines){
        std::string ident = rl.substr(0, rl.find(":"));
        std::string value = rl.substr(rl.find(":") + 1);
        set_up_field(ident, value);
    }
}

void MyRMan::set_up_field(std::string ident, std::string value){
    if (ident == "FOV")
        FOV = atoi(value.c_str());
    else if (ident == "mini-map-size")
        mini_map_screensize = atoi(value.c_str());
    else if (ident == "text-color"){
        size_t comma_pos = value.find(", ");
        while (comma_pos != std::string::npos){
            value = value.replace(comma_pos, 2, ",");
            comma_pos = value.find(", ");
        }
        // Разбиваем строку по запятым
        std::stringstream line(value);
        std::vector<int> numbers;
        std::string num;
        while (getline(line, num, ','))
            numbers.push_back(atoi(num.c_str()));
        
        if(numbers.size() >= 3)
            text_color = {numbers[0], numbers[1], numbers[2], numbers.size() > 3? numbers[3] : 255};
        else{
            std::cout << "Некорректный файл настроек: не хватает значений для определения цвета" << std::endl;
            text_color = {255, 255, 255};
        }
    }
    else if (ident == "small-text-size")
        small_text_size = atoi(value.c_str()) ? atoi(value.c_str()) : 12;
    else if (ident == "player-speed")
        player_speed = atof(value.c_str());
    else if (ident == "camera-plane-width")
        camera_plane_width = atof(value.c_str());
}