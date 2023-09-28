#include "ResourceManager.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream> // До появления логгера
#include <sstream>
using namespace std;

void ResourceManager::load_settings_file(){
    ifstream FILE("settings");
    string cur_line;
    vector<string> raw_lines;
    while (getline(FILE, cur_line)){
        // Строка не пустая и не комментарий
        if (cur_line.size() > 0 && cur_line[0] != '#'){
            size_t colon_pos = cur_line.find(": ");
            if (colon_pos != string::npos)
                raw_lines.push_back(cur_line.replace(colon_pos, 2, ":"));
        }
    }
    for (string rl: raw_lines){
        string ident = rl.substr(0, rl.find(":"));
        string value = rl.substr(rl.find(":") + 1);
        if (ident == "FOV")
            FOV = atoi(value.c_str());
        else if (ident == "mini-map-size")
            mini_map_screensize = atoi(value.c_str());
        else if (ident == "text-color"){
            size_t comma_pos = value.find(", ");
            while (comma_pos != string::npos){
                value = value.replace(comma_pos, 2, ",");
                comma_pos = value.find(", ");
            }
            // Разбиваем строку по запятым
            stringstream line(value);
            vector<int> numbers;
            string num;
            while (getline(line, num, ','))
                numbers.push_back(atoi(num.c_str()));
            
            if(numbers.size() >= 3)
                text_color = {numbers[0], numbers[1], numbers[2], numbers.size() > 3? numbers[3] : 255};
            else{
                cout << "Некорректный файл настроек: не хватает значений для определения цвета" << endl;
                text_color = {255, 255, 255};
            }
        }
        else if (ident == "small-text-size")
            small_text_size = atoi(value.c_str()) ? atoi(value.c_str()) : 12;
        else if (ident == "player-speed")
            player_speed = atof(value.c_str());
    }
}

sf::Color color_from_hex(std::string hex){
    if (hex.size() >= 7){
        const char alphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        std::string raw_red = hex.substr(1, 2);
        std::string raw_green = hex.substr(3, 2);
        std::string raw_blue = hex.substr(5, 2);
        auto find = [&](char symbol) -> int {
            for (int i = 0; i < 16; i++)
                if (alphabet[i] == symbol) return i;
            return 15; // Все значения больше f интерпретируются как f
        };
        return sf::Color(find(raw_red[0]) * 16 + find(raw_red[1]),
                        find(raw_green[0]) * 16 + find(raw_green[1]),
                        find(raw_blue[0]) * 16 + find(raw_blue[1]));
    }
    else{
        cout << "Bad color string: " << hex << endl;
        return {0, 0, 0};
    }
}

void ResourceManager::load_level(std::string path){
    std::ifstream FILE(path);
    nlohmann::json j = nlohmann::json::parse(FILE);
    auto header = j["header"];
    lvl = Level{header["lvl_name"].template get<std::string>(), 
                header["width"].template get<int>(),
                header["height"].template get<int>(),
                color_from_hex(header["skycolor"].template get<std::string>()),
                header["player-x"].template get<double>(),
                header["player-y"].template get<double>()};

    auto blocks = j["walls"];
    for (auto iter = blocks.begin(); iter != blocks.end(); iter++){
        int x = iter->at("x").template get<int>();
        int y = iter->at("y").template get<int>();
        sf::Color color = color_from_hex(iter->at("color").template get<std::string>());
        lvl.set_block(Block{color, false}, x, y);
    }
}