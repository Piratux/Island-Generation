#include <algorithm>
#include <iostream>

// returns random int in range [min, max] (inclusive)
int rand_range(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

void generateMap(int seed) {
    int size_x = 60;
    int size_y = 20;
    int num_islands = 6;
    int spread_percentage_chance = 6;
    int sick_duration = 10;

    char map[2][20][60];
    int sick_map[2][20][60];
    int curr_map_idx = 0;
    int next_map_idx = 1;

    int total_sick_tiles = 0;
    int total_new_tiles = 0;
    int min_new_tiles_required = 5; // prevent islands that are made from just few tiles.

    srand(seed);

    int neighbor_offsets[4][2] = { {0, -1},{0, 1},{-1, 0},{1, 0} };

    // must keep 1 sea tile around all islands
    // must keep 1+ beach tiles around island
    int boundary_offset = 2;
    int min_pos_x = boundary_offset;
    int max_pos_x = size_x - boundary_offset - 1;
    int min_pos_y = boundary_offset;
    int max_pos_y = size_y - boundary_offset - 1;

    auto get_tile = [&](int pos_x, int pos_y) {
        return map[curr_map_idx][pos_y][pos_x];
    };

    auto set_tile = [&](int pos_x, int pos_y, char symbol, bool force_place) {
        if (force_place || (pos_x == std::clamp(pos_x, min_pos_x, max_pos_x) && pos_y == std::clamp(pos_y, min_pos_y, max_pos_y))) {
            map[next_map_idx][pos_y][pos_x] = symbol;
        }
    };

    auto get_sick_map_tile = [&](int pos_x, int pos_y) {
        return sick_map[curr_map_idx][pos_y][pos_x];
    };

    auto set_sick_map_tile = [&](int pos_x, int pos_y, int sick_value, bool force_place) {
        if (force_place || (pos_x == std::clamp(pos_x, min_pos_x, max_pos_x) && pos_y == std::clamp(pos_y, min_pos_y, max_pos_y))) {
            sick_map[next_map_idx][pos_y][pos_x] = sick_value;
        }
    };

    

    auto neighbor_is_sick = [&](int pos_x, int pos_y) {
        for (int i = 0; i < 4; i++) {
            if (get_tile(pos_x + neighbor_offsets[i][0], pos_y + neighbor_offsets[i][1]) == 'S') {
                return true;
            }
        }

        return false;
    };

    auto switch_maps = [&]() {
        curr_map_idx = 1 - curr_map_idx;
        next_map_idx = 1 - next_map_idx;

        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                set_tile(x, y, get_tile(x, y), true);
                set_sick_map_tile(x, y, get_sick_map_tile(x, y), true);
            }
        }
    };

    auto print_map = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                printf("%c", get_tile(x, y));
            }
            printf("\n");
        }
    };

    auto generate_map = [&]() {
        for (int y = min_pos_y; y <= max_pos_y; y++) {
            for (int x = min_pos_x; x <= max_pos_x; x++) {
                if (get_tile(x, y) != '~') {
                    continue;
                }

                if (neighbor_is_sick(x, y) && rand_range(1, 100) <= spread_percentage_chance) {
                    set_tile(x, y, 'S', false);
                    set_sick_map_tile(x, y, sick_duration, false);
                    total_sick_tiles++;
                    total_new_tiles++;
                }
            }
        }
    };

    auto update_sick_tile_map = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                if (get_sick_map_tile(x,y) == 1) {
                    set_tile(x, y, 'O', false);
                    total_sick_tiles--;
                }
                set_sick_map_tile(x, y, get_sick_map_tile(x, y) - 1, true);
            }
        }
    };

    // Initialize maps
    for (int y = 0; y < size_y; y++) {
        for (int x = 0; x < size_x; x++) {
            set_tile(x, y, '~', true);
        }
    }

    // select start location.
    int start_pos_x = rand_range(min_pos_x, max_pos_x);
    int start_pos_y = rand_range(min_pos_y, max_pos_y);

    set_tile(start_pos_x, start_pos_y, 'S', false);
    set_sick_map_tile(start_pos_x, start_pos_y, sick_duration, false);
    total_sick_tiles++;
    total_new_tiles++;

    switch_maps();

    // cellular automata based generation
    // rules:
    // - 

    while (total_sick_tiles > 0) {
        //std::cout << total_sick_tiles << std::endl;
        generate_map();

        switch_maps();

        if (total_new_tiles >= min_new_tiles_required) {
            update_sick_tile_map();
        }

        //print_map();
        //update_sick_tile_map();
        //system("pause");
        //system("cls");
    }

    // Need to copy last updates
    switch_maps();

    // todo: 
    // - blurring
    // - more islands
    // - beach

    print_map();
}

int main() {
    int seed = 0;
    while (true) {
        generateMap(seed);
        seed++;
        system("pause");
        system("cls");
    }
}
