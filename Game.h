#ifndef PROJANGER_GAME
#define PROJANGER_GAME
#include "MyRMan.h"
#include "anger_engine/WinManager.h"
#include "Player.h"
#include <memory>
#include <string>
#include "miniMap.h"
#include <SFML/Graphics/Text.hpp>

class Game{
    std::shared_ptr<MyRMan> R{std::make_shared<MyRMan>()};

    // Храним не сами объекты, а указатели, чтобы проинициализировать их в конструкторе: сначала нужно загрузить уровень и настройки,
    // (а в конструкторе MyRMan-а мы это не пропишем, т. к. нужно знать имена файлов), а уже потом создавать оконный менеджер и игрока
    std::unique_ptr<anger::WinManager> wm;
    std::unique_ptr<Player> player;
    
    miniMap mini_map{R, 30};

    bool is_paused=false;
    void handleMouse();
    void handleKeyboard(double fl);

    /// @brief Выполняется WinManager-ом в каждой итерации главного цикла. 
    /// @warning Не вызывать вручную, только назначать как callback менеджеру окна
    /// @param framelength длительность предыдущего кадра (в мс)
    void update(int framelength);

    sf::Text FPS_label;
    sf::Text player_pos_label;
    sf::Text player_angle_label;
    /// @brief Подготавливает элементы GUI (в данном случае -- только текст с отладочной инфой). Вызывается в конструкторе.
    void prepare_gui();

    public:
        Game(std::string level_path);

        /// @brief Запускает бой
        void start();
};

#endif