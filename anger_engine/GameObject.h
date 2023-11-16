#ifndef ANGER_GAMEOBJ
#define ANGER_GAMEOBJ
#include <memory>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>

/// @file GameObject.h
/// @brief В этом заголовочном файле определен класс GameObject

/// @brief Базовый класс для игрока, камеры, а в перспективе -- источников освещения, антуража и врагов.
class GameObject{
    protected:
        /// @brief Скорость движения в блоках/с
        double speed = 0;
        sf::Vector2f pos;
        /// @brief Вектор направления. -1 <= |x| <= 1; -1 <= |y| <= 1
        sf::Vector2f dir;
        sf::FloatRect collide_box;
        /// @brief Элементы перечисления обозначают, касается ли collide box стен и какой гранью, если да.
        enum Collision {NONE, TOP, RIGHT, BOTTOM, LEFT};

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

        double get_angle_degs();
        // virtual void update() const = 0;

        /// @brief Метод для определения столкновений
        /// @return Вектор элементов перечисления Collision: Collision::NONE, если объект не сталкивается ни с чем, 
        /// иначе элементы, соответствующие названиям граней collide box-а, на которых произошли столкновения
        virtual std::vector<Collision> get_collision() const = 0;
};
#endif