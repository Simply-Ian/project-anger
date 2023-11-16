#ifndef ANGER_CAMERA
#define ANGER_CAMERA
#include "GameObject.h"
#include <memory>
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>

/// @file anger_engine/Camera.h

namespace anger{
    /// @brief Информация о точке касания лучом стены
    struct touchdownData{
        sf::Vector2f pos;
        anger::Block::Side side;
        anger::Block block;
        /// @brief Является ли позиция касания лучом стены допустимой: если луч почти параллелен одной из осей координат,
        /// точка касания может находиться далеко за пределами уровня. Такая точка помечается как некорректная и не учитывается 
        /// в дальнейшем
        bool valid = false;
    };

    /// @brief Класс камеры. На основе данных об уровне создает видимое пользователю изображение. Поддерживает многопоточный 
    /// рендеринг
    class Camera: public GameObject{
        double plane_width = 1;
        double plane_height = 1;
        /// @brief Расстояние от точки, из которой испускаются лучи, до плоскости камеры
        double distance_to_plane = 1.5;
        sf::Vector2u screen_res;
        /// @brief Пул потоков, используемых для отрисовки изображения
        std::vector<std::thread> render_thread_pool;

        /// @subsection threads-sync
        /// <div>
        /// <h3> Как работает синхронизация потоков: </h3>
        /// Каждый поток, закончивший работу, увеличивает счетчик Camera::ready_threads_num (защищен мьютексом) и оповещает 
        /// главный поток. Тот просыпается и проверяет количество. Отработавший поток отрисовки ожидает пробуждения через 
        /// Camera::wake_threads. Если все потоки отрисовки закончили свою работу, главный поток отрисовывает изображение на 
        /// экране, очищает буфер, оповещает потоки отрисовки через Camera::wake_threads и ожидает пробуждения через 
        /// Camera::ready_threads.
        /// </div>

        /// @brief Условная переменная, пробуждающая потоки отрисовки.
        std::condition_variable ready_threads;
        /// @brief Счетчик потоков, закончивших отрисовку своей части данного кадра. Обнуляется сразу после очистки экрана
        int ready_threads_num = 0;
        std::mutex ready_threads_num_mutex;
        std::condition_variable wake_threads;

        /// @brief Условие пробуждения потоков отрисовки. При переходе к отрисовке нового кадра главный поток ИНВЕРТИРУЕТ 
        /// эту переменную. Потоки отрисовки сохраняют ее значение себе и сравнивают ее значение с сохраненным при каждом 
        /// пробуждении.
        bool threads_awoke = false;
        /// @brief Мьютекс, привязанный к Camera::wake_threads.
        std::mutex wake_threads_mutex;

        // Должна изменяться только главным потоком. Если false, потоки отрисовки прекращают работу.
        bool app_alive = true;

        /// @brief Создает потоки отрисовки. Вызывается один раз, в конструкторе камеры.
        void init_render_threads();

        /// @brief Запускается в каждом потоке отрисовки. Обертка над Camera::drawImagePart()
        /// @param start_offset Смещение полосы изображения относительно левого края изображения
        /// @param width ширина полосы
        void render_thread_inst(int start_offset, int width);

        /// @brief Коэффициенты затемнения разных граней кубов для имитации глобального освещения
        double glob_lighting_factors[4];

        /// @brief  Текстура, на которую отрисовывается текущий кадр в getImage(). 
        sf::RenderTexture cur_image;

        /// @brief Графический буфер, куда попиксельно отрисовываются изображения.
        sf::Image buffer;

        /// @brief Рассчитывает параметры для отрисовки полоски стены. Ничего не возвращает, изменяет значения по ссылкам.
        void calc_wall_strip(sf::Vector2f start_dot, sf::Vector2f ray_coords, sf::Vector2f plane_vect, std::shared_ptr<sf::Image>& skin,
                    int& raw_strip_height, double& brightness, int& texture_offset);

        /// @brief Рисует в буфер столбец пикселей -- полоску стены
        /// @param strip_pos позиция полоски в буфере (на экране)
        /// @param x_offset Смещение столбца пикселов в исходном изображении (текстуре) относительно левого края изображения
        /// @param h Высота столбца. double, чтобы избежать явного приведения в коде
        /// @param brightness Коэффициент яркости
        /// @param skin Указатель на текстуру
        void draw_wall_strip(sf::Vector2i strip_pos, int x_offset, double h, double brightness, std::shared_ptr<sf::Image> skin);

        /// @brief Рисует попиксельно вертикальную полоску пола.
        /// @param start_dot глобальные координаты (2D) точки на плоскости камеры, через которую проходит луч зрения
        /// @param strip_pos позиция верхнего конца полоски в буфере (на экране)
        /// @param proj_coords -- координаты 2D-луча зрения
        void draw_floor_strip(sf::Vector2f start_dot, sf::Vector2i strip_pos, sf::Vector2f proj_coords);

        /// @brief Рисует попиксельно вертикальную полоску неба.
        /// @param strip_scr_x X-координата полоски на экране
        /// @param strip_h Высота полоски (px)
        /// @param proj_coords -- координаты 2D-луча зрения
        void draw_sky_strip(int strip_scr_x, int strip_h, sf::Vector2f proj_coords);

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

        /// @brief Возвращает координаты пересечения луча зрения и прямой с y = const (горизонтальной линией сетки)
        /// @param start_x, start_y -- глобальные координаты точки, из которой выходит луч
        /// @param  line_y -- координата линии сетки, с которой ищется пересечение
        /// @param  ray_coords -- координаты вектора, вдоль которого лежит луч зрения
        double hor_intersect(int line_y, double start_x, double start_y, sf::Vector2f ray_coords) const;

        /// @brief Возвращает координаты пересечения луча зрения и прямой с x = const (вертикальной линией сетки)
        /// @param start_x, start_y -- глобальные координаты точки, из которой выходит луч
        /// @param line_y -- координата линии сетки, с которой ищется пересечение
        /// @param ray_coords -- координаты вектора, вдоль которого лежит луч зрения
        double vert_intersect(int line_x, double start_x, double start_y, sf::Vector2f ray_coords) const;

        /// @brief Метод, ищущий точку пересечения заданного луча с вертикальной стенкой некоторого блока
        /// @return объект touchdownData с данными о позиции касания и увиденном блоке
        anger::touchdownData get_point_on_vert_line(sf::Vector2f ray_coords, double dot_x, double dot_y);

        /// @brief Метод, ищущий точку пересечения заданного луча с горизонтальной стенкой некоторого блока
        /// @return объект touchdownData с данными о позиции касания и увиденном блоке
        anger::touchdownData get_point_on_hor_line(sf::Vector2f ray_coords, double dot_x, double dot_y);

        /// @brief Возвращает координаты первого столкновения луча со стеной
        /// @param start_x -- смещение по зрительной плоскости относительно ее левого конца (в долях от 1)
        anger::touchdownData get_touchdown_coords(sf::Vector2f start_dot, sf::Vector2f ray_coords, sf::Vector2f plane);

        /// @brief Используется при отрисовке пола для рисования теней.
        /// @param glob_x Глобальные координаты текущего пикселя пола
        /// @param glob_y Глобальные координаты текущего пикселя пола
        /// @returns true, если данный пиксель находится в тени.
        bool is_px_shadowed(double glob_x, double glob_y);

        /// @brief Отрисовывает вертикальную полосу изображения. Должна запускаться в отдельном потоке.
        /// @param start_offset Смещение полосы относительно левого края изображения
        /// @param width ширина полосы
        void drawImagePart(int start_offset, int width);

        protected:
            const anger::Level& level;

        public:
            /// @brief Текстура, хранящая текущий кадр. Если нужно получить изображение с данной камеры,
            /// необходимо обратиться к этой текстуре
            sf::Texture shot;

            Camera(const anger::Level& lvl, double x, double y, sf::Vector2u s_r);
            Camera(const anger::Level& lvl, double x, double y, double pw, double d_t_p, sf::Vector2u s_r);

            /// @brief Метод для рендеринга изображения, видимого камерой.
            /// @return Ничего не возвращает, обновляет изображение на текстуре Camera::shot
            void takeImage();
            ~Camera();
    };
}
#endif