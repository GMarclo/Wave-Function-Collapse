#pragma once
using namespace sf;

const int window_width = 1280, window_height = 704, tile_size = 16, scalex = 2, scaley = 2, tile_variants = 41, different_colors = 4, divisor = 99999, all_tiles_number = (window_width / tile_size / scalex) * (window_height / tile_size / scaley);

Font font;
RenderTexture render_texture;
Text number_text;
RectangleShape black_square;

struct New_tile {
	int posx;
	int posy;
	int index;
};

class Table_cell {
public:
	bool delated_tiles[tile_variants];
	int possible_tiles = tile_variants, actual_tile_index = -1, sum_of_chances=0;
	bool filled = false;
};

class Tile {
public:
	bool top, right, down, left;
	int color,chance;
};


std::priority_queue <std::pair<std::pair<int, int>,std::pair<int,int>>> pqueue;			//possible_tiles and position in grid
std::stack<Vector2<int>> tiles_to_redraw;
Table_cell renewed_tile;
Texture tile_textures[tile_variants];
Sprite tile_sprites[tile_variants];
Table_cell grid[window_width / scalex / tile_size][window_height / scaley / tile_size];
bool tiles_selected_to_renew[window_width / scalex / tile_size][window_height / scaley / tile_size];
Tile tile_properties[tile_variants];
New_tile new_tile;
int filled_tiles_number = 0, all_chances_sum=0;
std::ifstream file_with_probabilities("tile_probabilities.txt");
bool start = 0, renewer_on = 0;
std::string text_line;

void create_tile_sprites() {
	
	for (int i = 0; i < tile_variants; i++)
		tile_textures[i].loadFromFile("tiles.png", IntRect(i * tile_size, 0, tile_size, tile_size));
	for (int i = 0; i < tile_variants; i++) {
		tile_sprites[i].setTexture(tile_textures[i]);
		tile_sprites[i].setScale(scalex, scaley);
	}
}

int get_tile_variant_chance() {
	for (int i = text_line.size() - 1; i > 0; i--) {
		if (text_line[i] == ' ')
			return std::stoi(text_line.substr(i+1));
	}
}

void set_tile_variants_chances() {
	int textfile_iterator = 0;
	while (getline(file_with_probabilities, text_line)) {
		if (textfile_iterator < tile_variants) {
			tile_properties[textfile_iterator].chance = get_tile_variant_chance();
			textfile_iterator++;
		}
	}
	file_with_probabilities.close();
}

void init_tile_properties() {

	tile_properties[0] = { 0,1,0,0,0 };
	tile_properties[1] = { 0,0,0,1,0 };
	tile_properties[2] = { 1,0,0,0,0 };
	tile_properties[3] = { 0,0,1,0,0 };
	tile_properties[4] = { 0,1,0,1,0 };
	tile_properties[5] = { 1,0,1,0,0 };
	tile_properties[6] = { 1,0,0,1,0 };
	tile_properties[7] = { 1,1,0,0,0 };
	tile_properties[8] = { 0,0,1,1,0 };
	tile_properties[9] = { 0,1,1,0,0 };
	tile_properties[tile_variants-1] = { 0,0,0,0,-1 };
	for (int i = 1; i < different_colors; i++)
		for (int j = 0; j < (tile_variants - 1) / different_colors; j++) {
			tile_properties[i * (tile_variants - 1) / different_colors + j] = tile_properties[j];
			tile_properties[i * (tile_variants - 1) / different_colors + j].color = i;
		}
			
	pqueue.push({{ -tile_variants,rand() % divisor },{rand() % (window_width / scalex / tile_size),rand() % (window_height / scaley / tile_size)} });

	set_tile_variants_chances();

	for (int i = 0; i < tile_variants; i++)
		all_chances_sum += tile_properties[i].chance;

	for (int i = 0; i < window_width / scalex / tile_size; i++)
		for (int j = 0; j < window_height / scaley / tile_size; j++)
			grid[i][j].sum_of_chances = all_chances_sum;
}

void draw_black_square(int posx, int posy) {
	black_square.setSize(Vector2f(tile_size * scalex, tile_size * scaley));
	black_square.setPosition(posx * tile_size * scalex, posy * tile_size * scaley);
	render_texture.draw(black_square);
}

void draw_new_text(int posx, int posy, bool painted_before) {
	number_text.setString(std::to_string(grid[posx][posy].possible_tiles));
	number_text.setPosition(posx * tile_size * scalex + tile_size / 2 , posy * tile_size * scaley + tile_size / 2 );
	if (painted_before)
		draw_black_square(posx,posy);
	render_texture.draw(number_text);
}

void update_delated_tiles(int posx, int posy, int tileindex, Tile tiles_around_to_update_and_further_color,bool further_update ) {

	bool change_of_possible_tiles = 0;
	
	//for top tile
	if (tiles_around_to_update_and_further_color.top && posy > 0 && !grid[posx][posy - 1].filled) {
		for (int i = 0; i < tile_variants; i++) {
			if (!grid[posx][posy - 1].delated_tiles[i] && !further_update &&
				((tile_properties[tileindex].top != tile_properties[i].down) || (tile_properties[tileindex].top && tile_properties[i].down && tile_properties[tileindex].color != tile_properties[i].color))) {
				grid[posx][posy - 1].delated_tiles[i] = 1;
				grid[posx][posy - 1].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx][posy - 1].sum_of_chances -= tile_properties[i].chance;
			}
			else if (!grid[posx][posy - 1].delated_tiles[i] && further_update &&
				tile_properties[i].down && tile_properties[i].color != tiles_around_to_update_and_further_color.color) {
				grid[posx][posy - 1].delated_tiles[i] = 1;
				grid[posx][posy - 1].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx][posy - 1].sum_of_chances -= tile_properties[i].chance;
			}
		}
		if (change_of_possible_tiles)
		{
			change_of_possible_tiles = 0;
			pqueue.push({ { -grid[posx][posy - 1].possible_tiles,rand() % divisor},{posx,posy - 1} });
			draw_new_text(posx, posy - 1,true);
		}
		if (!further_update && tile_properties[tileindex].top)
			update_delated_tiles(posx, posy - 1, -1, { 1,1,0,1,tile_properties[tileindex].color }, true);
	}

	//for right tile
	if (tiles_around_to_update_and_further_color.right && posx < (window_width / scalex / tile_size)-1 && !grid[posx+1][posy].filled) {
		for (int i = 0; i < tile_variants; i++) {
			if (!grid[posx + 1][posy].delated_tiles[i] && !further_update &&
				((tile_properties[tileindex].right != tile_properties[i].left) || (tile_properties[tileindex].right && tile_properties[i].left && tile_properties[tileindex].color != tile_properties[i].color))) {
				grid[posx + 1][posy].delated_tiles[i] = 1;
				grid[posx + 1][posy].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx + 1][posy].sum_of_chances -= tile_properties[i].chance;
			}
			else if (!grid[posx + 1][posy].delated_tiles[i] && further_update &&
				tile_properties[i].left && tile_properties[i].color != tiles_around_to_update_and_further_color.color) {
				grid[posx + 1][posy].delated_tiles[i] = 1;
				grid[posx + 1][posy].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx + 1][posy].sum_of_chances -= tile_properties[i].chance;
			}
		}
		if (change_of_possible_tiles)
		{
			change_of_possible_tiles = 0;
			pqueue.push({ { -grid[posx + 1][posy].possible_tiles,rand() % divisor},{posx + 1,posy} });
			draw_new_text(posx + 1, posy, true);
		}
		if (!further_update && tile_properties[tileindex].right)
			update_delated_tiles(posx + 1, posy, -1, { 1,1,1,0,tile_properties[tileindex].color }, true);
	}

	//for down tiles
	if (tiles_around_to_update_and_further_color.down && posy < (window_height / scaley / tile_size)-1 && !grid[posx][posy + 1].filled) {
		for (int i = 0; i < tile_variants; i++) {
			if (!grid[posx][posy + 1].delated_tiles[i] && !further_update &&
				((tile_properties[tileindex].down != tile_properties[i].top) || (tile_properties[tileindex].down && tile_properties[i].top && tile_properties[tileindex].color != tile_properties[i].color))) {
				grid[posx][posy + 1].delated_tiles[i] = 1;
				grid[posx][posy + 1].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx][posy + 1].sum_of_chances -= tile_properties[i].chance;
			}
			else if (!grid[posx][posy + 1].delated_tiles[i] && further_update &&
				tile_properties[i].top && tile_properties[i].color != tiles_around_to_update_and_further_color.color) {
				grid[posx][posy + 1].delated_tiles[i] = 1;
				grid[posx][posy + 1].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx][posy + 1].sum_of_chances -= tile_properties[i].chance;
			}
		}
		if (change_of_possible_tiles)
		{
			change_of_possible_tiles = 0;
			pqueue.push({{ -grid[posx][posy + 1].possible_tiles,rand() % divisor },{posx,posy + 1} });
			draw_new_text(posx, posy + 1, true);
		}
		if (!further_update && tile_properties[tileindex].down)
			update_delated_tiles(posx, posy + 1, -1, { 0,1,1,1,tile_properties[tileindex].color }, true);
	}

	//for left tiles
	if (tiles_around_to_update_and_further_color.left && posx > 0 && !grid[posx - 1][posy].filled) {
		for (int i = 0; i < tile_variants; i++) {
			if (!grid[posx - 1][posy].delated_tiles[i] && !further_update &&
				((tile_properties[tileindex].left != tile_properties[i].right) || (tile_properties[tileindex].left && tile_properties[i].right && tile_properties[tileindex].color != tile_properties[i].color))) {
				grid[posx - 1][posy].delated_tiles[i] = 1;
				grid[posx - 1][posy].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx - 1][posy].sum_of_chances -= tile_properties[i].chance;
			}
			else if (!grid[posx - 1][posy].delated_tiles[i] && further_update &&
				tile_properties[i].right && tile_properties[i].color != tiles_around_to_update_and_further_color.color) {
				grid[posx - 1][posy].delated_tiles[i] = 1;
				grid[posx - 1][posy].possible_tiles--;
				change_of_possible_tiles = 1;
				grid[posx - 1][posy].sum_of_chances -= tile_properties[i].chance;
			}
		}
		if (change_of_possible_tiles)
		{
			change_of_possible_tiles = 0;
			pqueue.push({{ -grid[posx - 1][posy].possible_tiles,rand() % divisor },{posx - 1,posy} });
			draw_new_text(posx - 1, posy, true);
		}
		if (!further_update && tile_properties[tileindex].left)
			update_delated_tiles(posx - 1, posy, -1, { 1,0,1,1,tile_properties[tileindex].color }, true);
	}

	

}

void priority_queue_update() {
	
	if (pqueue.empty())
		return;

	int pos_x = pqueue.top().second.first;
	int pos_y = pqueue.top().second.second;
	pqueue.pop();

	if (grid[pos_x][pos_y].filled) {
		priority_queue_update();
		return;
	}

	
	
	int choice = rand() % grid[pos_x][pos_y].sum_of_chances, chosen_tile = -1;
	for (int i = 0; i < tile_variants; i++) {
		if (!grid[pos_x][pos_y].delated_tiles[i] && tile_properties[i].chance <= choice)
			choice -= tile_properties[i].chance;
		else if (!grid[pos_x][pos_y].delated_tiles[i] && tile_properties[i].chance > choice) {
			chosen_tile = i;
			break;
		}
	}

	update_delated_tiles(pos_x, pos_y, chosen_tile, {1,1,1,1,-1},false);
	

	new_tile.posx = pos_x;
	new_tile.posy = pos_y;
	new_tile.index = chosen_tile;
}

void draw_initial_possibilities() {
	for (int i = 0; i < (window_width / tile_size / scalex); i++)
		for (int j = 0; j < (window_height / tile_size / scaley); j++)
			draw_new_text(i, j, false);
}

void update_stack(Vector2<int>& tile_pos) {

	int posx = tile_pos.x, posy = tile_pos.y, filled_tileindex;

	tiles_selected_to_renew[tile_pos.x][tile_pos.y] = 0;

	//for top tile
	if (posy > 0 && grid[posx][posy - 1].filled) {
		filled_tileindex = grid[posx][posy - 1].actual_tile_index;
		update_delated_tiles(posx, posy - 1, filled_tileindex, { 1,1,1,1,-1 }, false);
	}

	//for right tile
	if (posx < (window_width / scalex / tile_size) - 1 && grid[posx + 1][posy].filled) {
		filled_tileindex = grid[posx + 1][posy].actual_tile_index;
		update_delated_tiles(posx + 1, posy, filled_tileindex, { 1,1,1,1,-1 }, false);
	}

	//for down tiles
	if (posy < (window_height / scaley / tile_size) - 1 && grid[posx][posy + 1].filled) {
		filled_tileindex = grid[posx][posy + 1].actual_tile_index;
		update_delated_tiles(posx, posy + 1, filled_tileindex, { 1,1,1,1,-1 }, false);
	}

	//for left tiles
	if (posx > 0 && grid[posx - 1][posy].filled) {
		filled_tileindex = grid[posx - 1][posy].actual_tile_index;
		update_delated_tiles(posx - 1, posy, filled_tileindex, { 1,1,1,1,-1 }, false);
	}

	draw_new_text(posx, posy, true);
	pqueue.push({{ -grid[posx][posy].possible_tiles,rand()%divisor}, {posx,posy}
});
}