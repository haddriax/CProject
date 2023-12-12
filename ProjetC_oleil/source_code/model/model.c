#include "model.h"

// Default variable for easy [0;0] vector creation by copy.
const Vector2f vector2f_zero = {0.f, 0.f};

// Global container for the app.
RenderWindow render_window = {NULL, NULL, NULL};
App app = {NULL, NULL, 0, 0, 0, 0, 0, NULL, 0,0 };

char *get_config_file_name(char **argv) {
    return (argv && argv[1]) ? argv[1] : "config.txt";
}

Config *load_config(const char *file_name) {
    app.config = calloc(1, sizeof(Config));

    assert(app.config);
    assert(file_name);

    if (file_name != NULL) {
        int line_index = 0; // @todo Should be remove, check for usages before.

        // Try to open file.
        FILE *file = NULL;
        // const int e = fopen_s(&file, file_name, FILE_MODE_READONLY); // C11 - DO NOT USE
        // @todo: check if errno as a global variable is ok with C99
        file = fopen(file_name, FILE_MODE_READONLY);
        if (!file) // ERROR HANDLING - OPENING FAILED.
        {
            char buffer[128];
            sprintf(buffer, "Error when opening config file [%s].\n", file_name);
            quit(Error, buffer);
        }

        // Read from file.
        if (file != NULL) {
            // Buffer for reading line from the file.
            char line[max_config_large_buffer_size];

            while (fgets(line, max_config_lines, file)) {

                // 0/ Prepare a buffer.
                char config_name[CONFIG_BUFFER_MAX_SIZE];

                // 1/ Get config name.
                const int name_length = find_config_line_name(line, config_name);

                // 2/ Identify the data type we want to load, using config name
                const config_type t = find_config_type(config_name);

                // 3/ Separate the data from the config name.
                char *data = get_data_from_line(line, name_length);

                // 4/ First data validation.
                if (!validate_config_line(data, t))
                {
                    quit(Config_Invalid, data);
                }

                process_data(data, t, file, &line_index);

                if (t != nb_solar_system)
                    ++line_index;

                free(data);
            }
            const int closed = fclose(file);
            if (closed != 0)
                fprintf(stderr, "%s\n", "Error closing config files.");  // NOLINT(cert-err33-c) - Error Output

            // Setup end location. From config to actual game data.
            app.entities->end->x = app.config->goal_end.x;
            app.entities->end->y = app.config->goal_end.y;
        }
    }
    else
    {
        quit(No_Conf_File, NULL);
    }

    return app.config;
}

int find_config_line_name(const char *line, char *out_config_name) {
    int char_index = 0;
    // Until we found the separator or go out of range...
    while ((line[char_index] != SEPARATOR_SPACE) && (char_index < CONFIG_BUFFER_MAX_SIZE))
        ++char_index;

    if (char_index == (CONFIG_BUFFER_MAX_SIZE - 1)) {
        char buffer[128];
        sprintf(buffer, "%s\n", "Config name not valid.");
        quit(Error, buffer);
    }

    // Once done, copy the data from beginning to separator into the argument char*
    SDL_strlcpy(out_config_name, line, ++char_index);

    return char_index;
}

config_type find_config_type(const char *config_name) {
    if (SDL_strcmp(config_name, CONFIG_NAME_PLANET_RADIUS) == 0)
        return planet_radius;
    else if (SDL_strcmp(config_name, CONFIG_NAME_PLANET_ORBIT) == 0)
        return planet_orbit;
    else if (SDL_strcmp(config_name, CONFIG_NAME_STAR_POS) == 0)
        return star_pos;
    else if (SDL_strcmp(config_name, CONFIG_NAME_STAR_RADIUS) == 0)
        return star_radius;
    else if (SDL_strcmp(config_name, CONFIG_NAME_NB_PLANET) == 0)
        return nb_planet;
    else if (SDL_strcmp(config_name, CONFIG_NAME_WINDOW_SIZE) == 0)
        return window_size;
    else if (SDL_strcmp(config_name, CONFIG_NAME_PLAYER_LOCATION) == 0)
        return player_location;
    else if (SDL_strcmp(config_name, CONFIG_NAME_GOAL_LOCATION) == 0)
        return goal_location;
    else if (SDL_strcmp(config_name, CONFIG_NAME_NUMBER_OF_SYSTEMS) == 0)
        return nb_solar_system;
    else {
        fprintf(stderr, "%s%s\n", config_name, ": Config not handled.");  // NOLINT(cert-err33-c) - Error Output
        return none;
    }
}

char *get_data_from_line(const char *line, const int data_start) {
    const size_t line_length = SDL_strlen(line);
    const size_t data_length = line_length - data_start -
                               (line[line_length - 1] == '\n' ? 1 : 0); // Minus 1 to remove '\n' char at then end
    char *data = calloc(data_length + 1, sizeof(char)); // Allocate + 1 to add '\0' later
    if (data != NULL) {
        SDL_memcpy(data, &line[data_start], data_length); // Copy line data
        data[data_length] = '\0'; // Add the end of string character
    } else {
        char buffer[128];
        sprintf(buffer, "%s: %s\n", line, "could not copy data from configs.");
        quit(Error, buffer);
    }

    return data;
}

int validate_is_int(const char *data) {
    for (size_t i = 0; i < SDL_strlen(data); ++i) {
        if (SDL_isspace(data[i])) // Want no space
            return 0;
        if (SDL_isalpha(data[i])) // Want no alpha char
            return 0;
    }
    return 1;
}

int validate_is_signed_int(const char *data) {
    size_t i = 0;
    // Check if the first char is the sign
    if (data[0] == SEPARATOR_SUBSTRACT)
        i = 1;

    for (; i < SDL_strlen(data); ++i) {
        if (SDL_isspace(data[i])) // Want no space
            return 0;

        if (SDL_isalpha(data[i])) // Want no alpha char
            return 0;
    }
    return 1;
}

int validate_is_vector(const char *data) {
    int space_counter = 0;
    for (size_t i = 0; i < SDL_strlen(data); ++i) {
        if (SDL_isspace(data[i])) // Want exactly one space
        {
            ++space_counter;
            continue;
        }

        if (SDL_isalpha(data[i])) // Want no alpha char
            return 0;
    }
    return (space_counter == 1);
}

int validate_config_line(const char *data, const config_type t) {
    switch (t) {
        case window_size:
        case player_location:
        case goal_location:
        case star_pos:
            return validate_is_vector(data);
        case nb_solar_system:
        case star_radius:
        case nb_planet:
        case planet_radius:
            return validate_is_int(data);
        case planet_orbit:
            return validate_is_signed_int(data);
        case none:
            fprintf(stderr, "Can't validate: [%s].\n", data);  // NOLINT(cert-err33-c)
            return 0;
    }
    return 0;
}

void process_data(const char *data, const config_type type, FILE *file, int *line_index) {
    switch (type) {
        case window_size:
            app.config->window_size = read_vector(data);
            break;
        case player_location:
            app.config->player_start = read_float_point(data);
            break;
        case goal_location:
            app.config->goal_end = read_float_point(data);
            break;
        case nb_solar_system:
            app.config->nb_solar_systems = read_int(data);
            app.entities->nb_solar_systems = app.config->nb_solar_systems;
            app.entities->solar_systems = malloc(app.config->nb_solar_systems * sizeof(SolarSystem));
            app.list_forces = calloc(app.entities->nb_solar_systems, sizeof(Vector2f));
            app.nb_forces = app.entities->nb_solar_systems;
            assert(app.entities->solar_systems != NULL);
            break;
        case star_pos:
            build_system(file, line_index, read_vector(data));
            break;
        case star_radius:
        case nb_planet:
        case planet_radius:
        case planet_orbit:
        case none:
            break;
    }
}

Vector2f read_vector(const char *data) {
    // Find space separating x and y value
    int char_index = 0;
    while (data[char_index] != SEPARATOR_SPACE && char_index < CONFIG_BUFFER_MAX_SIZE)
        ++char_index;

    const int data_length = (int) SDL_strlen(data);

    // Get char until the separator index is found.
    char x[8];
    int i = 0;
    for (; i < char_index; ++i)
        x[i] = data[i];
    x[i + 1] = '\0';

    // Get char until end of string is found.
    char y[8];
    int j = 0;
    for (; j < (data_length - char_index + 1); ++j)
        y[j] = data[(char_index + 1) + j];
    y[j + 1] = '\0';

    const float x_coords = (float) SDL_atoi(x);
    const float y_coords = (float) SDL_atoi(y);

    const Vector2f v = {x_coords, y_coords};
    return v;
}

SDL_FPoint read_float_point(const char *data) {

    // Find space separating x and y value
    int char_index = 0;
    while (data[char_index] != SEPARATOR_SPACE && char_index < CONFIG_BUFFER_MAX_SIZE)
        ++char_index;

    const int data_length = (int) SDL_strlen(data); // OK for our value size, but may be at risk of int overflow

    // Get char until the separator index is found.
    char x[8];
    int i = 0;
    for (; i < char_index; ++i)
        x[i] = data[i];
    x[i + 1] = '\0';

    // Get char until end of string is found.
    char y[8];
    int j = 0;
    for (; j < (data_length - char_index + 1); ++j)
        y[j] = data[(char_index + 1) + j];
    y[j + 1] = '\0';

    // Convert to numeric value (long) and cast to int.
    const int x_coords = SDL_atoi(x);
    const int y_coords = SDL_atoi(y);

    const SDL_FPoint fp = {(float) x_coords, (float) y_coords};
    return fp;
}

int read_int(const char *data) {
    const int data_length = (int) SDL_strlen(data);
    for (int i = 0; i < (data_length - 1); ++i) {
        if (!SDL_isalnum((int) data[i])) {
            fprintf(stderr, "Could not read [%s] from config, expected an Int.",
                    data);  // NOLINT(cert-err33-c) - Error Output
            abort();
        }
    }

    return SDL_atoi(data);
}

int read_signed_int(const char *data) {
    const int data_length = (int) SDL_strlen(data);
    assert(data_length > 0);

    int i = 0;
    if (data[0] == SEPARATOR_SUBSTRACT)
        i = 1;
    for (; i < (data_length); ++i) {
        if (!SDL_isalnum((int) data[i])) {
            if (data[i] != SEPARATOR_SUBSTRACT) {

                char buffer[128];
                sprintf(buffer, "Could not read [%s] from config, expected an Int.", data);
                quit(Error, buffer);
            }
        }
    }

    // Multiply by -1 or 1, depending on first char.
    const int return_value = (data[0] == SEPARATOR_SUBSTRACT ? (SDL_atoi(&data[1]) * -1) : (SDL_atoi(&data[0])));
    return return_value;
}

SolarSystem *build_system(FILE *file, int *line_index, Vector2f spawn_location) {
    char line[64];
    static int creation_id = 0;
    // Create a new system.
    SolarSystem *s = calloc(1, sizeof(SolarSystem));
    app.entities->solar_systems[creation_id] = s;
    if (s != NULL) {
        s->location.x = spawn_location.x;
        s->location.y = spawn_location.y;
        // printf("STAR POS [%i:%i]\n", s->location.x, s->location.y);
    }

    // STAR RADIUS -> int
    if (fgets(line, 64, file) != NULL) {
        // 0/ Prepare buffer.
        char config_name[CONFIG_BUFFER_MAX_SIZE];
        // 1/ Get config name.
        const int name_length = find_config_line_name(line, config_name);
        // 2/ Identify the data type we want to load, using config name.
        const config_type t = find_config_type(config_name);
        // 3/ Separate the data from the config name.
        char *data = get_data_from_line(line, name_length);
        // 4/ First data validation.
        if (!validate_config_line(data, t))
        {
            quit(Config_Invalid, data);
        }
        // 5/ Read and apply data. Here STAR_RADIUS
        s->radius = (float) read_int(data);
        s->mass = s->radius;

        ++(*line_index);
        free(data);
    } else
        return NULL;

    // NB_PLANET -> int
    if (fgets(line, 64, file) != NULL) {
        // 0/ Prepare buffer.
        char config_name[CONFIG_BUFFER_MAX_SIZE];
        // 1/ Get config name.
        const int name_length = find_config_line_name(line, config_name);
        // 2/ Identify the data type we want to load, using config name.
        config_type t = find_config_type(config_name);
        // 3/ Separate the data from the config name.
        char *data = get_data_from_line(line, name_length);
        // 4/ First data validation.
        if (!validate_config_line(data, t))
        {
            quit(Config_Invalid, data);
        }

        // 5/ Read and apply data. Here NB_PLANET.
        s->nb_planets = read_int(data);

        // 6/ For each expected planet.
        s->planets = calloc(s->nb_planets, sizeof(Planet));
        for (int i = 0; i < s->nb_planets; ++i) {
            int orbit = 0, radius = 0;
            if (fgets(line, 64, file) != NULL) {
                const int p_name_length = find_config_line_name(line, config_name);
                t = find_config_type(config_name);
                char *p_data = get_data_from_line(line, p_name_length);
                if (!validate_config_line(data, t))
                {
                    quit(Config_Invalid, data);
                }
                radius = read_int(p_data);
                free(p_data);
            }
            if (fgets(line, 64, file) != NULL) {
                const int p_name_length = find_config_line_name(line, config_name);
                t = find_config_type(config_name);
                char *p_data = get_data_from_line(line, p_name_length);
                if (!validate_config_line(data, t))
                {
                    quit(Config_Invalid, data);
                }
                orbit = read_signed_int(p_data);
                free(p_data);
            }
            // Use read data to create a planet.
            build_planet(s, orbit, radius, i);
        }

        free(data);
    } else
        return NULL;

#if PRINT_CONFIG_CREATION
    printf("Created solar system nb %i of radius:%i, at [%i:%i].\n", creation_id, s->radius, s->location.x, s->location.y);
#endif

    return (app.entities->solar_systems[creation_id++]);
}

Planet *build_planet(SolarSystem *parent_system, const float orbit, const float radius, const int index) {
    assert(parent_system);
    Planet *p = &(parent_system->planets[index]);
    if (p) {
        p->radius = radius;
        p->orbit = orbit;
        p->location.x = (float) parent_system->location.x + (float) orbit * cosf(0);
        p->location.y = (float) parent_system->location.y + (float) orbit * sinf(0);
        p->parent_system = parent_system;

        return &(parent_system->planets[index]);
    }
    fprintf(stderr, "Error creating planet, memory was not allocated.\n");  // NOLINT(cert-err33-c)
    return NULL;
}

void keep_player_on_screen(void) {
    Player *p = app.entities->player;
    const int player_width = app.config->player_size;

    // Left (x) boundary.
    if (p->location.x < 10)
        p->location.x = (float) app.config->window_size.x - 20 - (float) player_width;

    // Right (x) boundary.
    if (p->location.x + (float) player_width > (float) app.config->window_size.x)
        p->location.x = 10;

    // Up (y) boundary.
    if (p->location.y < 0)
        p->location.y = (float) app.config->window_size.y - 20 - (float) player_width;

    // Down (y) boundary.
    if (p->location.y + (float) player_width > (float) app.config->window_size.y)
        p->location.y = 10;
}

void init_app(const int n_args, char **argv) {
    if (n_args < 1) {
        char error_log[128];
        sprintf(error_log, "%s\n", "No config file argument detected."); // NOLINT(cert-err33-c) - Error Output
        quit(Error, error_log);
    }

    Entities *entities_ptr = (Entities *) malloc(sizeof(Entities));
    if (entities_ptr != NULL) {
        app.entities = entities_ptr;
        entities_ptr->end = (SDL_FRect *) calloc(1, sizeof(SDL_FRect));
        if (entities_ptr->end != NULL) {
            entities_ptr->end->x = 0;
            entities_ptr->end->y = 0;
            entities_ptr->end->h = 25;
            entities_ptr->end->w = 25;
            entities_ptr->solar_systems = NULL;
        }
    }

    // Player struct creation & assignation
    Player *p_ptr = (Player *) calloc(1, sizeof(Player));
    if (p_ptr != NULL) {
        p_ptr->location.x = 500;
        p_ptr->location.y = 450;
        p_ptr->draw_rect.h = PLAYER_SIZE;
        p_ptr->draw_rect.w = PLAYER_SIZE;
        p_ptr->velocity = vector2f_zero;
        p_ptr->mass = 2;
        app.entities->player = p_ptr;
    } else {
        printf("%s\n", " Error when allocating memory for Player struct");
        abort();
    }

    if (argv != NULL) {
        const char *config_file_name = get_config_file_name(argv);
        init_config(config_file_name);
        app.entities->player->location.x = app.config->player_start.x;
        app.entities->player->location.y = app.config->player_start.y;
    }

    int max_forces = 0;
    for (int i = 0; i < app.entities->nb_solar_systems; ++i)
    {
        max_forces += (app.entities->solar_systems[i]->nb_planets + 1);
    }

    app.list_forces = calloc(max_forces, sizeof(Vector2f));

    //Create and initialize display.
    init_render_window((int) app.config->window_size.x, (int) app.config->window_size.y, APP_DEFAULT_NAME);

    assert(app.config != NULL);
    assert(app.entities != NULL);
    assert(app.entities->player != NULL);

    player_update(); // update player location for the first render frame.
}

void init_config(const char *file_name) {
    if (file_name == NULL) {
        fprintf(stderr, "config_file_name in init_app was NULL");  // NOLINT(cert-err33-c) - Error Output
        abort();
    }
    load_config(file_name);
}

void init_render_window(const int width, const int height, const char *name) {
    // Set pointer to RenderWindow in Game;
    render_window.sdl_win = NULL;
    render_window.sdl_surface = NULL;
    render_window.sdl_renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        char buffer[128];
        sprintf(buffer, "1 SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        quit(Error, buffer);
    } else {
        SDL_Window *window = render_window.sdl_win = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
                                                                      SDL_WINDOWPOS_CENTERED, width, height,
                                                                      SDL_WINDOW_SHOWN);

        if (window == NULL) {
            char buffer[128];
            sprintf(buffer, "2 SDL could not initialize!SDL_CreateWindow returned NULL : %s\n", SDL_GetError());
            quit(Error, buffer);
        }

        const SDL_Renderer *renderer = render_window.sdl_renderer = SDL_CreateRenderer(window, -1,
                                                                                       SDL_RENDERER_SOFTWARE);
        if (renderer == NULL) {
            char buffer[128];
            sprintf(buffer, "3 SDL could not initialize! SDL_CreateRenderer returned NULL:%s\n", SDL_GetError());
            quit(Error, buffer);
        }

        const SDL_Surface *surface = render_window.sdl_surface = SDL_GetWindowSurface(window);
        if (surface == NULL) {
            char buffer[128];
            sprintf(buffer, "4 SDL could not initialize! SDL_GetWindowSurface returned NULL: %s\n", SDL_GetError());
            quit(Error, buffer);
        }
    }
}

Vector2f vectorf_add(const Vector2f *v1, const Vector2f *v2) {
    Vector2f vec;
    vec.x = (v1->x + v2->x);
    vec.y = (v1->y + v2->y);
    return vec;
}

void vector2f_rotate(Vector2f *v, float delta_rad) {
    const float d_cos = cosf(delta_rad);
    const float d_sin = sinf(delta_rad);
    v->x = v->x * d_cos - v->y * d_sin;;
    v->y = v->x * d_sin + v->y * d_cos;
}

int is_colliding_rect_circle(const SDL_FRect *rect, const SDL_FPoint *location, float radius) {
    float dist_x = fabsf(location->x - rect->x);
    float dist_y = fabsf(location->y - rect->y);
    if (dist_x > (rect->w / 2 + radius)) return 0;
    if (dist_y > (rect->h / 2 + radius)) return 0;
    if (dist_x <= (rect->w / 2)) return 1;
    if (dist_y <= (rect->h / 2)) return 1;
    // Corner collision
    float corner_dist_squared = powf(dist_x - rect->w / 2, 2) + powf(dist_y - rect->h / 2, 2);
    return (corner_dist_squared <= (radius * radius));
}

int is_colliding_FRect_FRect(const SDL_FRect *r1, const SDL_FRect *r2) {
    if ((r1->x + r1->w < r2->x) || (r1->x > r2->x + r2->w)) {
        return 0;
    }

    if ((r1->y + r1->h < r2->y) || (r1->y > r2->y + r2->h)) {
        return 0;
    }
    return 1;
}

void player_update(void) {
    app.entities->player->location.x += app.entities->player->velocity.x * ((float) app.delta_time * VELOCITY_DELTA_T_WEIGHTING);
    app.entities->player->location.y += app.entities->player->velocity.y * ((float) app.delta_time * VELOCITY_DELTA_T_WEIGHTING);

    SDL_FRect *r = &app.entities->player->draw_rect;
    r->x = app.entities->player->location.x - (PLAYER_SIZE / 2.f);
    r->y = app.entities->player->location.y - (PLAYER_SIZE / 2.f);
}

void clamp_vector(Vector2f *v, float min, float max) {
    assert(min > 0 && max > min && v != NULL);
    float magnitude = sqrtf(v->x * v->x + v->y * v->y);

    if (magnitude < min) {
        v->x = v->x * min / magnitude;
        v->y = v->y * min / magnitude;
    } else if (magnitude > max) {
        v->x = v->x * max / magnitude;
        v->y = v->y * max / magnitude;
    }
}

void planet_revolution_update(void) {
    static float simulated_time = 0.0f;
    simulated_time += (float) app.delta_time / 1000;

    for (int i = 0; i < app.entities->nb_solar_systems; ++i) {
        for (int j = 0; j < app.entities->solar_systems[i]->nb_planets; ++j) {
            Planet *p = &(app.entities->solar_systems[i]->planets[j]);

            const float tr = p->radius;
            p->location.x = p->parent_system->location.x +
                            cosf(simulated_time * (float) ((2 * M_PI) / tr) * (p->orbit < 0 ? -1.f : 1.f)) * p->orbit;
            p->location.y = p->parent_system->location.y +
                            sinf(simulated_time * (float) ((2 * M_PI) / tr) * (p->orbit < 0 ? -1.f : 1.f)) * p->orbit;
        }
    }
}

int check_player_planets_collisions(void) {
    for (int i = 0; i < app.entities->nb_solar_systems; ++i) {
        const SolarSystem *s = app.entities->solar_systems[i];
        SDL_FPoint tmp = {(float) s->location.x, (float) s->location.y};
        if (is_colliding_rect_circle(&app.entities->player->draw_rect, &tmp, s->radius))
            return 1;
        for (int j = 0; j < s->nb_planets; ++j) {
            const Planet *p = &(s->planets[j]);
            if (is_colliding_rect_circle(&app.entities->player->draw_rect, &p->location, p->radius))
                return 1;
        }
    }
    return 0;
}

void handle_player_end_collision(void) {
    if (is_colliding_FRect_FRect(app.entities->end, &app.entities->player->draw_rect))
        quit(Victory, NULL);
}

void handle_player_planets_collisions(void) {
    if (check_player_planets_collisions())
        quit(PlayerDied, NULL);
}

Vector2f vector_divi(const Vector2f *v, float divisor) {
    assert(divisor > 0);
    const Vector2f res = {v->x / divisor, v->y / divisor};
    return res;
}

int vector_normalize(Vector2f *v) {
    const float x = v->x;
    const float y = v->y;
    const float norm_sq= x * x + y * y;

    if (fabsf(norm_sq) < 0.0000005f)
        return -1;

    assert(sqrtf(x * x + y * y) > 0);
    (*v) = vector_divi(v, sqrtf(x * x + y * y));
    return 0;
}

Vector2f vector_sub(const Vector2f *v1, const Vector2f *v2) {
    const Vector2f v = {v1->x - v2->x, v1->y - v2->y};
    return v;
}

Vector2f direction_from_FPoint(const SDL_FPoint *p1, const SDL_FPoint *p2) {
    Vector2f v = {p1->x - p2->x, p1->y - p2->y};
    vector_normalize(&v);
    return v;
}

float calculate_distance(const SolarSystem *s) {
    const SDL_FPoint p_loc = app.entities->player->location;
    const float x1 = p_loc.x;
    const float y1 = p_loc.y;

    const float x2 = s->location.x;
    const float y2 = s->location.y;

    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float calculate_distance_planet(const Planet *p) {
    const SDL_FPoint p_loc = app.entities->player->location;
    const float x1 = p_loc.x;
    const float y1 = p_loc.y;

    const float x2 = p->location.x;
    const float y2 = p->location.y;

    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

Vector2f grav_force(const SolarSystem *s) {
    assert(GRAV_DISTANCE_WEIGHTING != 0);
    Vector2f dir = direction_from_FPoint(&s->location, &app.entities->player->location);
    const float mass_player = app.entities->player->mass;
    const float mass_star = s->mass;
    const float dist = calculate_distance(s);
    const float F = GRAVITY_CONST * (mass_player * mass_star) / (dist * dist * GRAV_DISTANCE_WEIGHTING);
    const Vector2f v = {F * dir.x, F * dir.y};
    return v;
}

Vector2f grav_force_planet(const Planet *p) {
    assert(GRAV_DISTANCE_WEIGHTING != 0);
    Vector2f dir = direction_from_FPoint(&p->location, &app.entities->player->location);
    const float mass_player = app.entities->player->mass;
    const float mass_planet = p->radius;
    const float dist = calculate_distance_planet(p);
    const float F = GRAVITY_CONST * (mass_player * mass_planet) / (dist * dist * GRAV_DISTANCE_WEIGHTING);
    const Vector2f v = {F * dir.x, F * dir.y};
    return v;
}

Vector2f sum_forces(const Vector2f *vector_list, const int length) {
    Vector2f res = vector2f_zero;
    for (int i = 0; i < length; ++i)
        res = vectorf_add(&res, &vector_list[i]);
    return res;
}

void start_simulation(void) {
    // Create random seed based on current time.
    srand((unsigned int) time(NULL));  // NOLINT(cert-msc51-cpp) // Doesn't mind if this is predictable.

    // Create a random value in [0.00f;1.00f]
    const float s = ((float) (rand() % 2) / 10) + ((float) (rand() % 100) / 100);  // NOLINT(concurrency-mt-unsafe)

    // const float initial_speed = 2.f + (float) (rand() % 6); // For a random initial speed // NOLINT(concurrency-mt-unsafe)

    // Create initial velocity, using <s> to get 2 float which added gives 1.0.
    // Then reverse direction by multiplying it by 1 or -1, depending on a random value.
    Vector2f initial_velocity;
    initial_velocity.x = s * INITIAL_SPEED * (rand() % 2 == 0 ? -1.f : 1.f);  // NOLINT(concurrency-mt-unsafe)
    initial_velocity.y = (1 - s) * INITIAL_SPEED * (rand() % 2 == 0 ? -1.f : 1.f);  // NOLINT(concurrency-mt-unsafe)

    // Apply our velocity to the Player.
    app.entities->player->velocity = initial_velocity;

    const float initial_speed = sqrtf(
            (initial_velocity.x * initial_velocity.x) + (initial_velocity.y * initial_velocity.y));

    app.entities->player->speed = initial_speed;

    vector_normalize(&initial_velocity);
    app.entities->player->direction = initial_velocity; // Velocity normalized here.

    // Start simulation. I.e., Player will now be affected by Gravity and by its own velocity.
    app.simulation_started = 1;
}

void physic_update(void) {
    planet_revolution_update();

    if (app.simulation_started) {
        handle_player_end_collision();
        handle_player_planets_collisions();

        apply_forces();
        keep_player_on_screen();
        player_update();
    }
}

void apply_forces(void) {
    Player *player = app.entities->player;
    SolarSystem **systems = app.entities->solar_systems;
    Vector2f *list_forces = app.list_forces;

    // Get a list of attraction forces on player from all systems.

    int offset = 0;
    for (int i = 0; i < app.entities->nb_solar_systems; i++)
    {
        list_forces[i + offset] = grav_force(systems[i]);
        for (int j = 0; j < systems[i]->nb_planets; j++)
        {
            list_forces[i + j + offset + 1] = grav_force_planet(&systems[i]->planets[j]);
        }
        offset += systems[i]->nb_planets;
    }

    // Compute the sum of this list.
    Vector2f _sum_forces = sum_forces(list_forces, app.nb_forces);

    // Add the player thrust to this list.
    _sum_forces = vectorf_add(&_sum_forces, &player->thrust);
    _sum_forces.x *= GRAV_GENERAL_WEIGHTING;
    _sum_forces.y *= GRAV_GENERAL_WEIGHTING;
   //  const Vector2f velocity = {(player->direction.x * player->new_speed), (player->direction.y * player->new_speed)};
   // Vector2f result_sum_forces = vectorf_add(&_sum_forces, &velocity);
    Vector2f result_sum_forces = vectorf_add(&_sum_forces, &player->velocity);

    const float new_speed = sqrtf(
            (result_sum_forces.x * result_sum_forces.x) + (result_sum_forces.y * result_sum_forces.y));
    player->speed = new_speed;

    vector_normalize(&result_sum_forces);
    player->velocity.x += (result_sum_forces.x * new_speed);
    player->velocity.y += (result_sum_forces.y * new_speed);

    Vector2f velocity_to_normalize = player->velocity;
    vector_normalize(&velocity_to_normalize);
    player->direction = velocity_to_normalize;

    clamp_vector(&player->velocity, MIN_SPEED_VALUE, MAX_SPEED_VALUE);
}

void game_loop(float delta_time) {
    handle_move_input();
    physic_update();
}

void quit(const quit_code code, const char *message) {
    switch (code) {
        case Error:
            if (message != NULL)
                fprintf(stderr, "%s", message);
            break;
        case Exit:
            if (message != NULL)
                fprintf(stdout, "%s", message);
            break;
        case PlayerDied:
            fprintf(stdout, "%s", "Game Over");
            break;
        case Victory:
            fprintf(stdout, "%s", "Victory");
            break;
        case No_Conf_File:
            fprintf(stderr, "%s", "No config file found.\n");
            break;
        case Config_Invalid:fprintf(stderr, "%s: %s", "is not a valid data.\n", message);
            break;
    }

    SDL_DestroyWindow(render_window.sdl_win);
    SDL_DestroyRenderer(render_window.sdl_renderer);
    SDL_Quit();

    // Memory cleaning
    if (app.entities->end) free(app.entities->end);
    if (app.entities->player) free(app.entities->player);
    for (int i = 0; i < app.entities->nb_solar_systems; ++i) {
        SolarSystem *s = app.entities->solar_systems[i];
        if (s) free(s);
    }
    if (app.entities) free(app.entities);
    if (app.config) free(app.config);

    exit(1);  // NOLINT(concurrency-mt-unsafe)
}

void handle_move_input() {
    app.entities->player->thrust = vector2f_zero;
    if (key_flags.left == 1)
        on_left_arrow();
    else if (key_flags.right == 1)
        on_right_arrow();
}

void on_left_arrow(void) {
    Player *p = app.entities->player;
    p->thrust.x = p->direction.y * THRUST_SPEED_WEIGHTING;
    p->thrust.y = -p->direction.x * THRUST_SPEED_WEIGHTING;

    p->thrust.x = -1 * THRUST_SPEED_WEIGHTING;

    if (app.simulation_started)
        ++app.score; // Inc score for each frame where we "consume fuel"
}

void on_right_arrow(void) {
    Player *p = app.entities->player;

    p->thrust.x = 1 * THRUST_SPEED_WEIGHTING;

    if (app.simulation_started)
        ++app.score; // Inc score for each frame where we "consume fuel"
}
