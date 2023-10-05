#ifndef ANGER_GAMEOBJ
#define ANGER_GAMEOBJ
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class GameObject{
    protected:
        // В блоках/с
        double speed = 0;
        // sf::Vector2f dir;
        sf::Vector2f pos;
        sf::Vector2f dir;
        sf::FloatRect collide_box;

    public:
        sf::Vector2f getPos();
        /// @brief Устанавливает направление объекта.
        /// @param angle Угол в радианах
        void set_angle_rads(double angle);

        /// @brief Устанавливает направление объекта.
        /// @param angle Угол в градусах
        void set_angle_degs(double angle);
        /// @brief За один кадр перемещает объект вдоль вектора
        /// @param framelength длительность предыдущего кадра
        /// @param vector -- вектор, вдоль которого двигается объект. Параметр нужен, чтобы класс-наследник Player
        /// мог делать стрейфы
        void move(double framelength, sf::Vector2f vector);
        
        // virtual void update() const = 0;
};
#endif