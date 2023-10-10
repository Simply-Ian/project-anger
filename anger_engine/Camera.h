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
        double distance_to_plane = 1.5;
        const anger::Level& level;
        sf::Vector2u screen_res;

        /// @brief  Текстура, на которую отрисовывается текущий кадр в getImage(). 
        sf::RenderTexture cur_image;

        /// @brief Спрайт, представляющий собой столбец пикселов шириной в 1 px. При отрисовке текстурированных стен этому спрайту
        /// последовательно присваивается текстура блока (с уже рассчитанной яркостью пикселов), после чего спрайт сжимается и
        /// отрисовывается.
        sf::Sprite strip;

        /// @brief Изображение 1x1080. В это изображение копируются незатемненные пиксели текстур стен, после чего к ним применяется
        /// затемнение.
        sf::Image strip_px_buffer;

        /// @brief Текстура, которая назначается спрайту strip
        sf::Texture strip_texture;

        /// @brief Готовит к отрисовке спрайт strip, задавая ему необходимые высоту, текстуру, высоту, яркость.
        /// @param x_offset Смещение столбца пикселов в исходном изображении относительно левого края изображения
        /// @param h Высота столбца
        /// @param brightness Коэффициент яркости
        /// @param skin Указатель на текстуру
        void bake_strip(int x_offset, int h, double brightness, std::shared_ptr<sf::Image> skin);

        /// @brief Умножает все компоненты заданного цвета на заданный коэффициент. Используется для затемнения
        /// @param source Исходный цвет
        /// @param factor Коэффициент
        /// @return Затемненный цвет
        static sf::Color multiply_color(sf::Color source, double factor);

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

            /// @brief Рассчитывает расстояние от заданной точки до ПРЯМОЙ (не отрезка!), представляющей плоскость камеры
            /// @param point_pos глобальные координаты точки
            /// @param plane_vect координаты вектора, задабщего плоскость камеры (повернутый dir, длина всегда равна 1)
            /// @return расстояние от точки до прямой
            double calculate_dist_plane_to_point(sf::Vector2f point_pos, sf::Vector2f plane_vect);
    };
}
#endif