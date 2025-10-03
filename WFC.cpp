#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <queue>
#include <stack>
#include <fstream>
#include "wfc_header.h"
using namespace sf;


int main() {
    srand(time(0));
    RenderWindow window(VideoMode(window_width, window_height), "WFC");
    window.setFramerateLimit(3);
    int framelimit = 2;
    int framecounter = 0;
    
    render_texture.create(window_width, window_height);
    create_tile_sprites();
    font.loadFromFile("Sansation_bold.ttf");
    number_text.setFillColor(Color::Green);
    number_text.setCharacterSize(15);
    number_text.setFont(font);
    black_square.setFillColor(Color::Black);
    Sprite main_sprite;
    main_sprite.setTexture(render_texture.getTexture());
    init_tile_properties();
    draw_initial_possibilities();

    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape))
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (tiles_to_redraw.empty() && filled_tiles_number != all_tiles_number)
                    start = 1;
                else {
                    while (!tiles_to_redraw.empty()) {
                        update_stack(tiles_to_redraw.top());
                        tiles_to_redraw.pop();
                    }
                }
            }
                
            if (start == 0 && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                renewer_on = 1;
            }
                
            
            if (start == 0 && event.mouseButton.button == Mouse::Left && event.type == Event::MouseButtonReleased) {
                renewer_on = 0;
            }
    }
                
        


        if (renewer_on) {
            Vector2<int> tile_pos = { Mouse::getPosition(window).x / scalex / tile_size,Mouse::getPosition(window).y / scaley / tile_size };
            if (!tiles_selected_to_renew[tile_pos.x][tile_pos.y]) {
                filled_tiles_number--;
                grid[tile_pos.x][tile_pos.y] = renewed_tile;
                grid[tile_pos.x][tile_pos.y].sum_of_chances = all_chances_sum;
                draw_black_square(tile_pos.x, tile_pos.y);
                tiles_to_redraw.push(tile_pos);
                tiles_selected_to_renew[tile_pos.x][tile_pos.y] = 1;
            }
            
        }


        if (start && !pqueue.empty()) {
            framecounter++;
            if (framecounter == 10) {
                framecounter = 0;
                framelimit++;
                window.setFramerateLimit(std::min(framelimit, 90));
            }
            priority_queue_update();
            if (!grid[new_tile.posx][new_tile.posy].filled) {
                grid[new_tile.posx][new_tile.posy].filled = 1;
                grid[new_tile.posx][new_tile.posy].actual_tile_index = new_tile.index;
                tile_sprites[new_tile.index].setPosition(new_tile.posx * scalex * tile_size, new_tile.posy * scaley * tile_size);
                render_texture.draw(tile_sprites[new_tile.index]);
                filled_tiles_number++;
            }
            if (filled_tiles_number==all_tiles_number)
                start = 0;
        }

        

        render_texture.display();
        window.clear();
        window.draw(main_sprite);
        window.display();
    
    }
   
    
}