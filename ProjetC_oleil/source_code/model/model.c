#include "model.h"

// Default variable for easy [0;0] vector creation by copy.
const Vector2i vector2i_zero = {0, 0};
const Vector2f vector2f_zero = {0.f, 0.f};

// Global container for the app.
RenderWindow render_window = {NULL, NULL, NULL};
App app = {NULL, NULL, 0, 0, 0, 0, 0};

char *get_config_file_name(char **argv) {
    return (argv && argv[1]) ? argv[1] : "config.txt";
}

Config* load_config(const char *file_name) {
    app.config = calloc(1, sizeof(Config));

    assert(app.config);
    assert(file_name);

    if (file_name != NULL) {
        int line_index = 0;

        // Try to open file.
        FILE *file = NULL;
        const errno_t e = fopen_s(&file, file_name, FILE_MODE_READONLY);
        if (e || (!file)) // ERROR HANDLING - OPENING FAILED.
        {
            fprintf(stderr, "Error when opening config file [%s]: %s\n", file_name,
                    strerror(e));  // NOLINT(cert-err33-c) - Error Output
            abort();
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
                const int data_validated = validate_config_line(data, t);
                if (data_validated) {
                    // 5/ Process validated data.
                    process_data(data, t, file, &line_index);
                } else {
                    fprintf(stderr, "%s\n", "Not valid.");  // NOLINT(cert-err33-c) - Error Output
                }

                if (t != nb_solar_system)
                    ++line_index;

                free(data);
            }
            const int closed = fclose(file);
            if (closed != 0)
                fprintf(stderr, "%s\n", "Error closing config files.");  // NOLINT(cert-err33-c) - Error Output

            // Setup end location. From config to actual game data.
            app.entities->end->x = (int) app.config->goal_end.x;
            app.entities->end->y = (int) app.config->goal_end.y;
        }
    }

    return app.config;
}

int find_config_line_name(const char *line, char *out_config_name) {
    int char_index = 0;
    // Until we found the separator or goes out of range...
    while ((line[char_index] != SEPARATOR_SPACE) && (char_index < CONFIG_BUFFER_MAX_SIZE))
        ++char_index;

    if (char_index == (CONFIG_BUFFER_MAX_SIZE - 1))
    {
        fprintf(stderr, "%s\n", "Config name not valid.");  // NOLINT(cert-err33-c) - Error Output
        abort();
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
        fprintf(stderr, "%s: %s\n", line, "could not copy data from configs.");  // NOLINT(cert-err33-c) - Error Output
        abort();
        return 0;
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
    size_t i;
    // Check if first char is the sign
    if (data[0] == '-')
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
#if PRINT_CONFIG_CREATION
            printf("Window size set to %i:%i\n", app.config->window_size.x, app.config->window_size.y);
#endif
            break;
        case player_location:
            app.config->player_start = read_float_point(data);
#if PRINT_CONFIG_CREATION
            printf("Player start set to %f:%f\n", app.config->player_start.x, app.config->player_start.y);
#endif
            break;
        case goal_location:
            app.config->goal_end = read_float_point(data);
#if PRINT_CONFIG_CREATION
            printf("Goal set to %f:%f\n", app.config->goal_end.x, app.config->goal_end.y);
#endif
            break;
        case nb_solar_system:
            app.config->nb_solar_systems = read_int(data);
            app.entities->nb_solar_systems = app.config->nb_solar_systems;
            app.entities->solar_systems = malloc(app.config->nb_solar_systems * sizeof(SolarSystem));
            assert(app.entities->solar_systems != NULL);
#if PRINT_CONFIG_CREATION
            printf("Number of systems set to %i\n", app.config->nb_solar_systems);
#endif
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

Vector2i read_vector(const char *data) {
    // Find space separating x and y value
    int char_index = 0;
    while (data[char_index] != SEPARATOR_SPACE && char_index < CONFIG_BUFFER_MAX_SIZE)
        ++char_index;

    const int data_length = (int) SDL_strlen(data);

    // Get char until separator index is found.
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

    const int x_coords = SDL_atoi(x);
    const int y_coords = SDL_atoi(y);

    const Vector2i v = {x_coords, y_coords};
    return v;
}

SDL_FPoint read_float_point(const char *data) {
    // Find space separating x and y value
    int char_index = 0;
    while (data[char_index] != SEPARATOR_SPACE && char_index < CONFIG_BUFFER_MAX_SIZE)
        ++char_index;

    const int data_length = (int) SDL_strlen(data); // OK for our value size, but may be at risk of int overflow

    // Get char until separator index is found.
    char x[8];
    int i = 0;
    for (; i < char_index - 1; ++i)
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
                fprintf(stderr, "Could not read [%s] from config, expected an Int.",
                        data);  // NOLINT(cert-err33-c) - Error Output
                abort();
            }
        }
    }

    const int return_value = (data[0] == SEPARATOR_SUBSTRACT ? (SDL_atoi(&data[1]) * -1) : (SDL_atoi(&data[0])));
    // Multiply by -1 or 1, depending on first char.
    return return_value;
}

SolarSystem *build_system(FILE *file, int *line_index, Vector2i spawn_location) {
    char line[64];
    static int creation_id = 0;
    // Create new system.
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
        validate_config_line(data, t);
        // 5/ Read and apply data. Here STAR_RADIUS
        s->radius = read_int(data);

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
        validate_config_line(data, t);

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
                validate_config_line(p_data, t);
                radius = read_int(p_data);
                free(p_data);
            }
            if (fgets(line, 64, file) != NULL) {
                const int p_name_length = find_config_line_name(line, config_name);
                t = find_config_type(config_name);
                char *p_data = get_data_from_line(line, p_name_length);
                validate_config_line(p_data, t);
                orbit = read_signed_int(p_data);
                free(p_data);
            }
            // Use read data to create planet.
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

Planet *build_planet(SolarSystem *parent_system, const int orbit, const int radius, const int index) {
    assert(parent_system);
    Planet *p = &(parent_system->planets[index]);
    if (p) {
        p->radius = radius;
        p->orbit = orbit;
        p->location.x = (float) parent_system->location.x + (float) orbit * cosf(0);
        p->location.y = (float) parent_system->location.y + (float) orbit * sinf(0);
        p->parent_system = parent_system;

#if PRINT_CONFIG_CREATION
        printf("    Create planet: radius[%i]|orbit[%i].\n", radius, orbit);
#endif

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
        p->location.y = (float) app.config->window_size.y - 20 -(float) player_width;

    // Down (y) boundary.
    if (p->location.y + (float) player_width > (float) app.config->window_size.y)
        p->location.y = 10;
}

void init_app(const int n_args, char **argv) {
    if (n_args < 1) {
        fprintf(stderr, "No config file argument detected.");  // NOLINT(cert-err33-c)
        abort();
    }

    Entities *entities_ptr = (Entities *) malloc(sizeof(Entities));
    if (entities_ptr != NULL) {
        app.entities = entities_ptr;
        entities_ptr->end = (SDL_Rect *) calloc(1, sizeof(SDL_Rect));
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
        app.entities->player = p_ptr;
    } else {
        printf("%s\n", " Error when allocating memory for Player struct");
        abort();
    }

    if (argv != NULL) {
        const char *config_file_name = get_config_file_name(argv);
        init_config(config_file_name);
        app.entities->player->location.x = (float) app.config->player_start.x;
        app.entities->player->location.y = (float) app.config->player_start.y;
    }

    //Create and initialize display.
    init_render_window(
            app.config->window_size.x,
            app.config->window_size.y,
            APP_DEFAULT_NAME
    );

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
        fprintf(stderr, " 1 SDL could not initialize! SDL_Error: %s\n",
                SDL_GetError());  // NOLINT(cert-err33-c) - Error Output
        abort();
    } else {
        SDL_Window *window = render_window.sdl_win =
                                     SDL_CreateWindow(
                                             name,
                                             SDL_WINDOWPOS_CENTERED,
                                             SDL_WINDOWPOS_CENTERED,
                                             width,
                                             height,
                                             SDL_WINDOW_SHOWN
                                     );

        if (window == NULL) {
            fprintf(stderr, "2 SDL could not initialize! SDL_CreateWindow returned NULL: %s\n",
                    SDL_GetError());  // NOLINT(cert-err33-c) - Error Output
            abort();
        }

        const SDL_Renderer *renderer = render_window.sdl_renderer = SDL_CreateRenderer(window, -1,
                                                                                       SDL_RENDERER_SOFTWARE);
        if (renderer == NULL) {
            fprintf(stderr, "3 SDL could not initialize! SDL_CreateRenderer returned NULL:%s\n",
                    SDL_GetError());  // NOLINT(cert-err33-c) - Error Output
            abort();
        }

        const SDL_Surface *surface = render_window.sdl_surface = SDL_GetWindowSurface(window);
        if (surface == NULL) {
            fprintf(stderr, "4 SDL could not initialize! SDL_GetWindowSurface returned NULL: %s\n",
                    SDL_GetError());  // NOLINT(cert-err33-c) - Error Output

            abort();
        }
    }
}

Vector2i vector_add(const Vector2i *v1, const Vector2i *v2) {
    Vector2i vec;
    vec.x = (v1->x + v2->x);
    vec.y = (v1->y + v2->y);
    return vec;
}

Vector2f vectorf_add(const Vector2f *v1, const Vector2f *v2) {
    Vector2f vec;
    vec.x = (v1->x + v2->x);
    vec.y = (v1->y + v2->y);
    return vec;
}

Vector2i vector_sub(const Vector2i *v1, const Vector2i *v2) {
    Vector2i vec;
    vec.x = (v1->x - v2->x);
    vec.y = (v1->y - v2->y);
    return vec;
}

float dot_product(const Vector2i *v1, const Vector2i *v2) {
    return (float) ((v1->x * v2->x) + (v1->y * v2->y));
}

void player_update(void) {
    SDL_FRect *r = &app.entities->player->draw_rect;

    r->x = app.entities->player->location.x - (PLAYER_SIZE / 2.f);
    r->y = app.entities->player->location.y - (PLAYER_SIZE / 2.f);
}

void apply_player_velocity(void) {
    // @todo: Normalized direction * speed for proper movement.
    apply_velocity_to_fpoint( &app.entities->player->location, &app.entities->player->velocity);
}

void planet_revolution_update(void) {
    static float simulated_time_D = 0.0f;
    simulated_time_D += (float)app.delta_time / 1000;

    for (int i = 0; i < app.entities->nb_solar_systems; ++i) {
        for (int j = 0; j < app.entities->solar_systems[i]->nb_planets; ++j) {
            Planet *p = &(app.entities->solar_systems[i]->planets[j]);

            const int tr = p->radius;
            p->location.x =
                    (float)p->parent_system->location.x
                    + cosf(simulated_time_D * (float)(tr / (2*M_PI)))
                      * (float)p->orbit;
            p->location.y =
                    (float) p->parent_system->location.y
                    + sinf(simulated_time_D * (float)(tr / (2*M_PI)))
                      * (float)p->orbit;
        }
    }
}

void apply_velocity_to_fpoint(SDL_FPoint* target_point, const Vector2f* v) {
    target_point->x += v->x;
    target_point->y += v->y;
}

Vector2f vector_divi(const Vector2f *v, float divisor) {
    assert(divisor > 0);
    Vector2f res = {v->x / divisor, v->y / divisor};
    return res;
}

/*
void vector_normalize(Vector2f* v)
{
	int x = v->x;
	int y = (*v).y;
	return vector_divi(v, sqrt(x * x + y * y));
}
*/

struct Vector2f grav(int d, int mv, int mp, const int G, const Vector2f *distance) {
    float F = G * (mv * mp) / (d * d);
    Vector2f v = {F * distance->x, F * distance->y};
    return v;

}

struct Vector2f somme_forces() {
}

void game_loop(void) {
    physic_update();
}

void physic_update(void) {

    planet_revolution_update();

    if (app.simulation_started) {
        app.entities->player->velocity.x = 1.7f;
        app.entities->player->velocity.y = 1.00f;
        apply_player_velocity();
        keep_player_on_screen();
        player_update();
    }
}
