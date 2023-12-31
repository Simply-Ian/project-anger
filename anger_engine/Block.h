#ifndef PROJANGER_BLOCK
#define PROJANGER_BLOCK
#include <SFML/Graphics/Image.hpp>
#include <memory>

/// @file Block.h

namespace anger{
    struct Block{
        sf::Color color;
        enum Side {Top, Bottom, Right, Left};
        /// <div>
        /// Block::t_top, Block::t_bottom, Block::t_right, Block::t_left -- текстуры соответствующих
        /// граней куба.
        /// </div>
        std::shared_ptr<sf::Image> t_top = nullptr;
        std::shared_ptr<sf::Image> t_bottom = nullptr;
        std::shared_ptr<sf::Image> t_right = nullptr;
        std::shared_ptr<sf::Image> t_left = nullptr;
        /// @brief Матрица anger::Level::lvl_map, представляющая собой карту стен на уровне, в пустых клетках хранит
        /// "нулевые" блоки -- блоки с is_a_null_block=true
        bool is_a_null_block=false;
        /// @deprecated
        Block(sf::Color c, bool is_null) : color(c), is_a_null_block(is_null) {}
        Block() : color(sf::Color::White), is_a_null_block(true){}
        Block(std::shared_ptr<sf::Image> top, std::shared_ptr<sf::Image> bottom, 
                std::shared_ptr<sf::Image> right, std::shared_ptr<sf::Image> left, sf::Color c) : 
                t_top(top), t_bottom(bottom), t_right(right), t_left(left), is_a_null_block(false), color(c){}
    };
}

#endif