#ifndef ANGER_CAMERA
#define ANGER_CAMERA
#include "GameObject.h"
#include <memory>
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace anger{
    /// @brief Информация о точке касания лучом стены
    struct touchdownData{
        sf::Vector2f pos;
        anger::Block::Side side;
        anger::Block block;
        bool valid = false;
    };

    class Camera: public GameObject{
        double plane_width = 1;
        double plane_height = 1;
        double distance_to_plane = 1.5;
        const anger::Level& level;
        sf::Vector2u screen_res;

        /// @brief Коэффициенты затемнения разных граней кубов для имитации глобального освещения
        double glob_lighting_factors[4];


        /// @brief  Текстура, на которую отрисовывается текущий кадр в getImage(). 
        sf::RenderTexture cur_image;

        /// @brief Графический буфер, куда попиксельно отрисовываются изображения.
        sf::Image buffer;

        /// @brief Рассчитывает параметры для отрисовки полоски стены. Ничего не возвращает, изменяет значения по ссылкам.
        void calc_wall_strip(double start_x, sf::Vector2f ray_coords, sf::Vector2f plane_vect, std::shared_ptr<sf::Image>& skin,
                    int& raw_strip_height, double& brightness, int& texture_offset);

        /// @brief Рисует в буфер столбец пикселей -- полоску стены
        /// @param strip_pos позиция полоски в буфере (на экране)
        /// @param x_offset Смещение столбца пикселов в исходном изображении (текстуре) относительно левого края изображения
        /// @param h Высота столбца. double, чтобы избежать явного приведения в коде
        /// @param brightness Коэффициент яркости
        /// @param skin Указатель на текстуру
        void draw_wall_strip(sf::Vector2i strip_pos, int x_offset, double h, double brightness, std::shared_ptr<sf::Image> skin);

        /// @brief Рисует попиксельно вертикальную полоску пола.
        /// @param strip_pos позиция верхнего конца полоски в буфере (на экране)
        /// @param proj_coords -- координаты 2D-луча зрения
        void draw_floor_strip(double start_x, sf::Vector2i strip_pos, sf::Vector2f proj_coords);

        /// @brief Умножает все компоненты заданного цвета на заданный коэффициент. Используется для затемнения
        /// @param source Исходный цвет
        /// @param factor Коэффициент
        /// @return Затемненный цвет
        static sf::Color multiply_color(sf::Color source, double factor);

        /// @brief Забивает буфер пикселами (0, 0, 0, 0)
        void clear_buffer();

        /// @brief Возвращает глобальные координаты точки на плоскости камеры, из которой исходит зрительный луч.
        /// @param start_x Смещение относительно левого края плоскости камеры (в долях ширины плоскости)
        sf::Vector2f get_start_point(double start_x);

        public:
            /// @brief Текстура, хранящая текущий кадр. Если нужно получить изображение с данной камеры,
            /// необходимо обратиться к этой текстуре
            sf::Texture shot;

            Camera(const anger::Level& lvl, double x, double y, sf::Vector2u s_r);
            Camera(const anger::Level& lvl, double x, double y, double pw, double d_t_p, sf::Vector2u s_r);

            /// @brief Метод для рендеринга изображения, видимого камерой
            /// @param single_height экранная высота стены высотой 1
            void takeImage();

            // Публичная только на время отладки
            /// @brief Возвращает координаты первого столкновения луча со стеной
            /// @param start_x -- смещение по зрительной плоскости относительно ее левого конца (в долях от 1)
            anger::touchdownData get_touchdown_coords(double start_x, sf::Vector2f ray_coords, sf::Vector2f plane);

            /// @brief Возвращает координаты пересечения луча зрения и прямой с y = const (горизонтальной линией сетки)
            // start_x, start_y -- глобальные координаты точки, из которой выходит луч
            // line_y -- координата линии сетки, с которой ищется пересечение
            // ray_coords -- координаты вектора, вдоль которого лежит луч зрения
            double hor_intersect(int line_y, double start_x, double start_y, sf::Vector2f ray_coords) const;

            /// @brief Возвращает координаты пересечения луча зрения и прямой с x = const (вертикальной линией сетки)
            // start_x, start_y -- глобальные координаты точки, из которой выходит луч
            // line_y -- координата линии сетки, с которой ищется пересечение
            // ray_coords -- координаты вектора, вдоль которого лежит луч зрения
            double vert_intersect(int line_x, double start_x, double start_y, sf::Vector2f ray_coords) const;

            /// @brief Метод, ищущий точку пересечения заданного луча с вертикальной стенкой некоторого блока
            /// @return объект touchdownData с данными о позиции касания и увиденном блоке
            anger::touchdownData get_point_on_vert_line(sf::Vector2f ray_coords, double dot_x, double dot_y);

            /// @brief Метод, ищущий точку пересечения заданного луча с горизонтальной стенкой некоторого блока
            /// @return объект touchdownData с данными о позиции касания и увиденном блоке
            anger::touchdownData get_point_on_hor_line(sf::Vector2f ray_coords, double dot_x, double dot_y);
    };
}
#endif