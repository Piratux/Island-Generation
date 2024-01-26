#include <algorithm>
#include <cstdio>

// returns random int in range [min, max] (inclusive)
int rand_range(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

void my_assert(bool condition, const char* text) {
    if (condition) {
        printf("%s\n", text);
        exit(-1);
    }
}

void generate_map(int seed) {
    srand(seed);

    char TILE_WATER = '~';
    char TILE_ISLAND = 'O';
    char TILE_BEACH = '+';
    char TILE_SICK = 'S';
    char TILE_NONE = 'x';

    // Parameters that can be fiddled around with
    constexpr int size_x = 60;
    constexpr int size_y = 20;
    constexpr int total_islands = 6;

    // Control these parameters to affect island sizes and growth rate
    int spread_percentage_chance = 6; // Chance for island to grow
    int sick_duration = 10; // Iteration count where tile is able to "spread" towards directly adjacent neighboring tiles
    int min_island_size = 5; // Minimum island size measured by tiles (not counting beaches). Controls when sick_duration timer starts counting for "sick" island tiles

    // Controls upper limit on island growing iterations.
    int max_generation_iterations = 1000;

    int starting_positions_x[total_islands];
    int starting_positions_y[total_islands];

    char map[2][size_y][size_x];
    char sick_map[size_y][size_x];
    int curr_map_idx = 0;
    int next_map_idx = 1;

    // ideally, we'd wanna avoid bias of which island gets to expand, but
    // then we need to solve issue of 2 islands expanding towards each other during same iteration.
    char island_id_map[size_y][size_x];

    // minimum water distance between 2 islands (required for beaches of opposite islands not to touch)
    int min_island_distance = 3; 

    int total_sick_tiles = 0;
    int island_total_tiles[total_islands] = { 0 };

    int neighbor_offsets[4][2] = { {0, -1},{0, 1},{-1, 0},{1, 0} };
    int extended_neighbor_offsets[8][2] = { {0, -1},{0, 1},{-1, 0},{1, 0},{1, 1},{1, -1},{-1, -1},{-1, 1} };

    // must keep 1 sea tile around all islands
    // must keep 1+ beach tiles around all islands
    int boundary_offset = 2;
    int min_pos_x = boundary_offset;
    int min_pos_y = boundary_offset;
    int max_pos_x = size_x - boundary_offset - 1;
    int max_pos_y = size_y - boundary_offset - 1;

    auto get_island_total_tiles = [&](int island_id) {
        return island_total_tiles[island_id - 1];
    };

    auto set_island_total_tiles = [&](int island_id, int new_value) {
        island_total_tiles[island_id - 1] = new_value;
    };

    auto get_island_id = [&](int pos_x, int pos_y) {
        if (pos_x == std::clamp(pos_x, 0, size_x - 1) && pos_y == std::clamp(pos_y, 0, size_y - 1)) {
            return island_id_map[pos_y][pos_x];
        }
        return (char)0;
    };

    auto set_island_id = [&](int pos_x, int pos_y, char island_id) {
        if (pos_x == std::clamp(pos_x, 0, size_x - 1) && pos_y == std::clamp(pos_y, 0, size_y - 1)) {
            island_id_map[pos_y][pos_x] = island_id;
        }
    };

    // Checks if there is island nearby, which has different island_id
    auto nearby_island_exists = [&](int pos_x, int pos_y, int island_id) {
        for (int y = -min_island_distance; y <= min_island_distance; y++) {
            for (int x = -min_island_distance; x <= min_island_distance; x++) {
                if (std::abs(x) == min_island_distance || std::abs(y) == min_island_distance) {
                    int sampled_island_id = get_island_id(pos_x + x, pos_y + y);
                    if (sampled_island_id != 0 && sampled_island_id != island_id) {
                        return true;
                    }
                }
            }
        }

        return false;
    };

    auto get_tile = [&](int pos_x, int pos_y) {
        if (pos_x == std::clamp(pos_x, 0, size_x - 1) && pos_y == std::clamp(pos_y, 0, size_y - 1)) {
            return map[curr_map_idx][pos_y][pos_x];
        }
        return TILE_NONE;
    };

    auto set_tile = [&](int pos_x, int pos_y, char symbol, bool force_place, int island_id = 0) {
        if (force_place || (pos_x == std::clamp(pos_x, min_pos_x, max_pos_x) && pos_y == std::clamp(pos_y, min_pos_y, max_pos_y))) {
            if (pos_x == std::clamp(pos_x, 0, size_x - 1) && pos_y == std::clamp(pos_y, 0, size_y - 1)) {
                if (force_place || nearby_island_exists(pos_x, pos_y, island_id) == false) {
                    if (island_id != 0) {
                        set_island_id(pos_x, pos_y, island_id);
                    }
                    map[next_map_idx][pos_y][pos_x] = symbol;
                    return true;
                }
            }
        }
        return false;
    };

    auto get_sick_map_tile = [&](int pos_x, int pos_y) {
        if (pos_x == std::clamp(pos_x, 0, size_x - 1) && pos_y == std::clamp(pos_y, 0, size_y - 1)) {
            return sick_map[pos_y][pos_x];
        }
        return (char)0;
    };

    auto set_sick_map_tile = [&](int pos_x, int pos_y, int sick_value, bool force_place) {
        if (force_place || (pos_x == std::clamp(pos_x, min_pos_x, max_pos_x) && pos_y == std::clamp(pos_y, min_pos_y, max_pos_y))) {
            if (pos_x == std::clamp(pos_x, 0, size_x - 1) && pos_y == std::clamp(pos_y, 0, size_y - 1)) {
                sick_map[pos_y][pos_x] = sick_value;
            }
        }
    };

    auto sick_neighbor_count = [&](int pos_x, int pos_y) {
        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (get_tile(pos_x + neighbor_offsets[i][0], pos_y + neighbor_offsets[i][1]) == TILE_SICK) {
                count++;
            }
        }

        return count;
    };

    auto neighbor_island_id = [&](int pos_x, int pos_y) {
        for (int i = 0; i < 4; i++) {
            int sampled_island_id = get_island_id(pos_x + neighbor_offsets[i][0], pos_y + neighbor_offsets[i][1]);
            if (sampled_island_id != 0) {
                return sampled_island_id;
            }
        }

        return 0;
    };

    auto extended_neighbor_is_tile = [&](int pos_x, int pos_y, char tile) {
        for (int i = 0; i < 8; i++) {
            if (get_tile(pos_x + extended_neighbor_offsets[i][0], pos_y + extended_neighbor_offsets[i][1]) == tile) {
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
            }
        }
    };

    auto debug_print_new_tiles_for_islands = [&]() {
        for (int i = 1; i <= total_islands; i++) {
            printf("island_id: %d; new_tiles: %d\n", i, get_island_total_tiles(i));
        }
        printf("\n");
    };

    auto debug_print_sick_map = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                printf("%c", get_sick_map_tile(x, y) + '0');
            }
            printf("\n");
        }
        printf("\n");
    };

    auto debug_print_island_id_map = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                printf("%c", get_island_id(x, y) + '0');
            }
            printf("\n");
        }
        printf("\n");
    };

    auto print_map = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                printf("%c", get_tile(x, y));
            }
            printf("\n");
        }
    };    

    auto new_sick_tile_created = [&](int pos_x, int pos_y, int island_id) {
        set_sick_map_tile(pos_x, pos_y, sick_duration, false);
        total_sick_tiles++;
        set_island_total_tiles(island_id, get_island_total_tiles(island_id) + 1);
    };

    auto generate_map = [&]() {
        for (int y = min_pos_y; y <= max_pos_y; y++) {
            for (int x = min_pos_x; x <= max_pos_x; x++) {
                if (get_tile(x, y) != TILE_WATER) {
                    continue;
                }

                int count = sick_neighbor_count(x, y);
                for (int i = 0; i < count; i++) {
                    if (rand_range(1, 100) <= spread_percentage_chance) {
                        int island_id = neighbor_island_id(x, y);
                        if (set_tile(x, y, TILE_SICK, false, island_id)) {
                            new_sick_tile_created(x, y, island_id);
                        }
                        break;
                    }
                }
            }
        }
    };

    auto update_sick_tile_map = [&](bool force_update = false) {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                int sick_map_tile_value = get_sick_map_tile(x, y);
                if (sick_map_tile_value > 0) {
                    // avoid small islands
                    if (force_update || get_island_total_tiles(get_island_id(x, y)) >= min_island_size) {
                        sick_map_tile_value--;
                        set_sick_map_tile(x, y, sick_map_tile_value, true);

                        if (sick_map_tile_value == 0) {
                            set_tile(x, y, TILE_ISLAND, true);
                            total_sick_tiles--;
                        }
                    }
                }
            }
        }
    };

    auto init_maps = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                set_tile(x, y, TILE_WATER, true);
                set_sick_map_tile(x, y, 0, true);
                set_island_id(x, y, 0);
            }
        }
    };

    auto fill_beach = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                if (get_tile(x, y) == TILE_WATER && extended_neighbor_is_tile(x, y, TILE_ISLAND)) {
                    set_tile(x, y, TILE_BEACH, true);
                }
            }
        }
    };

    auto swap_beaches_to_islands = [&]() {
        for (int y = 0; y < size_y; y++) {
            for (int x = 0; x < size_x; x++) {
                if (get_tile(x, y) == TILE_BEACH && extended_neighbor_is_tile(x, y, TILE_WATER) == false) {
                    set_tile(x, y, TILE_ISLAND, true);
                }
            }
        }
    };

    auto calculate_sector_sizes = [&](int& sector_parts_x, int& sector_parts_y, int& sector_size_x, int& sector_size_y) {
        // There must be a simpler way to calculate this...
        sector_parts_x = 1;
        sector_parts_y = 1;
        sector_size_x = size_x / sector_parts_x;
        sector_size_y = size_y / sector_parts_y;

        while (total_islands > sector_parts_x * sector_parts_y) {
            if (sector_size_x > sector_size_y) {
                sector_parts_x++;
                sector_size_x = size_x / sector_parts_x;
            }
            else if (sector_size_x < sector_size_y) {
                sector_parts_y++;
                sector_size_y = size_y / sector_parts_y;
            }
            else if (sector_size_x == sector_size_y) {
                if (sector_parts_x > sector_parts_y) {
                    sector_parts_x++;
                    sector_size_x = size_x / sector_parts_x;
                }
                else if (sector_parts_x <= sector_parts_y) {
                    sector_parts_y++;
                    sector_size_y = size_y / sector_parts_y;
                }
            }
        }

        while (total_islands <= (sector_parts_x - 1) * sector_parts_y) {
            sector_parts_x--;
        }

        while (total_islands <= sector_parts_x * (sector_parts_y - 1)) {
            sector_parts_y--;
        }

        sector_size_x = size_x / sector_parts_x;
        sector_size_y = size_y / sector_parts_y;
    };

    auto create_initial_islands = [&]() {
        int sector_parts_x;
        int sector_parts_y;
        int sector_size_x;
        int sector_size_y;
        calculate_sector_sizes(sector_parts_x, sector_parts_y, sector_size_x, sector_size_y);

        for (int island_id = 0; island_id < total_islands; island_id++) {
            // NOTE: if min_island_distance changes, sector offset formula needs to change too
            my_assert(min_island_distance != 3, "min_island_distance doesn't work with values other than 3");

            int sector_start_x = sector_size_x * (island_id % sector_parts_x) + boundary_offset;
            int sector_start_y = sector_size_y * (island_id / sector_parts_x) + boundary_offset;

            int sector_end_x = sector_start_x + sector_size_x - 2 * boundary_offset - 1;
            int sector_end_y = sector_start_y + sector_size_y - 2 * boundary_offset - 1;

            my_assert(sector_start_x >= sector_end_x, "Map size too small. sector_start_x >= sector_end_x");
            my_assert(sector_start_y >= sector_end_y, "Map size too small. sector_start_y >= sector_end_y");

            int start_pos_x = rand_range(sector_start_x, sector_end_x);
            int start_pos_y = rand_range(sector_start_y, sector_end_y);

            set_tile(start_pos_x, start_pos_y, TILE_SICK, false, island_id + 1);
            new_sick_tile_created(start_pos_x, start_pos_y, island_id + 1);

            starting_positions_x[island_id] = start_pos_x;
            starting_positions_y[island_id] = start_pos_y;
        }
    };

    auto mark_starting_positions = [&]() {
        for (int island_id = 0; island_id < total_islands; island_id++) {
            // There isn't a proper way to mark islands with single char, so might as well wrap around
            char island_num = '1' + island_id % 9;

            set_tile(starting_positions_x[island_id], starting_positions_y[island_id], island_num, true, island_id+1);
        }
    };

    init_maps();
    switch_maps();

    init_maps();

    create_initial_islands();
    switch_maps();

    for (int i = 0; 0 < total_sick_tiles && i < max_generation_iterations; i++) {
        generate_map();
        switch_maps();

        update_sick_tile_map();
    }

    // If we abruptly stop generation by reaching max_generation_iterations, there may still be sick tiles
    for (int i = 0; i < sick_duration; i++) {
        update_sick_tile_map(true);
    }
    switch_maps();

    fill_beach();
    switch_maps();

    swap_beaches_to_islands();
    switch_maps();

    mark_starting_positions();
    switch_maps();

    //debug_print_island_id_map();
    //debug_print_sick_map();
    print_map();
}

int main() {
    int seed = 0;
    while (true) {
        generate_map(seed);
        seed++;
        system("pause");
        system("cls");
    }
}
