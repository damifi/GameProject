/* Einbinden der verwendeten Standard-Bibliotheken */
#define EXTERN
#include "game.h"

/* 'screen' will be used as a reference to the displayed window */
SDL_Surface *screen = NULL;

/* Hintergrung Farbe fürs neu zeichnen */
Uint32 bg_color;

/* event handling structure */
SDL_Event event;

/*  array welches alle texturen enthaelt*/
SDL_Surface *all_textures[NR_TEXTURES];


/* ist ein 3d array welches alle elemnte die zu zeichenen sind enthaelt */
struct sDraw_element ****world_draw_elements;
int world_draw_width;
int world_draw_height;

/* Zeiger auf die Schriftart */
TTF_Font *font;

/* world_map 2d array*/
int **world_map_ids;

/* array aus sMove_elemnt, welches den Spieler, Gegner usew. enthaelt*/
struct sMove_element **move_elements_on_map;
int num_move_elemnts;

/* int direction bedeut: 0 = norden, 1 = osten, 2 = sueden, 3 = westen */

/* die zeit die noch gewartet werden muss bis das runden ende erreicht ist */
int rest_turn_delay;

/* highscore des Spieler */
int highscore;
Uint32 start_time;

/*-----------------------------------------------------------------------------------*/

/* main */
int main(int argc, char *argv[])
{
    int i, j = 0;
    init();

    /* Spielschleife */
    while(1)
    {
        menue_oeffnen();
        if(argc == 2)
        {
            /* Laedt die Map im uebergeben Pfad */
            load_map(argv[1]);
        }
        else
        {
            /* Falls keine Map uebergeben ist, wird eine Standartadresse genohmen */
            load_map("./resources/maps/map_01.txt");
        }


        /* eigentliches spiel */
        while(spiel_status())
        {
            spieler_zug();

            /* wartet die rest zeit des Zuges ab*/
            SDL_Delay(rest_turn_delay);
            rest_turn_delay = 0;

            if(j == 0)
            {
                for(i = 1; i < num_move_elemnts; i++)
                {
                    ki_zug(move_elements_on_map[i]);
                }

                j = KI_TURN_DELAY;
            }
            else
            {
                j--;
            }

            zeichne_screen_all();
        }

    }
    return 0;
}

/* ----------------------------------------------------------------------------------- */

/* initialisiert alle wichtigen variablen des SDL*/
void init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError());
        exit(1);
    }
    if(TTF_Init() < 0)
    {
        fprintf(stderr, "SDL_TTF konnte nicht initialisiert werden:  %s\n", TTF_GetError());
        exit(2);
    }
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

    bg_color = SDL_MapRGB( screen->format, BACK_COLOR_RED, BACK_COLOR_GREEN, BACK_COLOR_BLUE);

    /* Laedt die zunutzende font */
    font = TTF_OpenFont("./resources/font.ttf", FONT_SIZE);
    if(font == NULL)
    {
        fprintf(stderr, "\nFehler: Font nicht gefunden!\n");
        exit(-1);
    }

    /* Laedt alle texturen und speichert sie im all_textures array */

    /* player texturen */
    all_textures[0] = load_img("./resources/player/player_01.bmp");
    all_textures[1] = load_img("./resources/player/player_02.bmp");
    all_textures[2] = load_img("./resources/player/player_03.bmp");
    all_textures[3] = load_img("./resources/player/player_04.bmp");
    all_textures[4] = load_img("./resources/player/player_05.bmp");
    all_textures[5] = load_img("./resources/player/player_06.bmp");

    /* gegner texturen */
    all_textures[10] = load_img("./resources/npc/monster_6.bmp");
    all_textures[11] = load_img("./resources/npc/monster_7.bmp");


    /* boden texturen */
    all_textures[20] = load_img("./resources/floor/floor_01.bmp");
    all_textures[21] = load_img("./resources/floor/floor_02.bmp");
    all_textures[22] = load_img("./resources/floor/floor_03.bmp");
    all_textures[23] = load_img("./resources/floor/floor_04.bmp");
    all_textures[24] = load_img("./resources/floor/floor_05.bmp");
    all_textures[25] = load_img("./resources/floor/floor_06.bmp");
    all_textures[26] = load_img("./resources/floor/floor_07.bmp");
    all_textures[27] = load_img("./resources/floor/floor_08.bmp");
    all_textures[28] = load_img("./resources/floor/floor_09.bmp");

    /* wand texturen */
    all_textures[30] = load_img("./resources/wall/wall_01.bmp");

    /* interface texturen */
    all_textures[40] = load_img("./resources/menue/menue_01.bmp");
    all_textures[41] = load_img("./resources/menue/highscore.bmp");
    all_textures[42] = load_img("./resources/menue/victory.bmp");
    all_textures[43] = load_img("./resources/menue/victory_2.bmp");
    all_textures[44] = load_img("./resources/menue/gameover.bmp");
    all_textures[45] = load_img("./resources/floor/end_portal.bmp");


    /* weapon texturen */
    all_textures[50] = load_img("./resources/weapon/sword_03.bmp");

    /* ermoeglich spater rand() fuer zufalls zahlen zunutzten */
    srand(time(NULL));
}

/* oeffnet das menue */
void menue_oeffnen()
{
    struct sDraw_element element;

    element.x = 0;
    element.y = 0;
    element.w = 500;
    element.h = 500;
    element.textur = all_textures[40];
    zeichne_sDraw_element(&element, 1, 1);

    while(SDL_WaitEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            /* SDL Fenster wird geschlossen, Spiel wird OHNE Speichern beendet */
            TTF_Quit();
            SDL_Quit();
            exit(1);
        }
        else if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_1:
                    /* nicht tun */
                    return;

                case SDLK_2:
                    /* highscore oeffnen */
                    highscore_oeffnen();
                    zeichne_sDraw_element(&element, 1, 1);
                    break;

                case SDLK_3:
                    /* SDL Fenster wird geschlossen, Spiel wird OHNE Speichern beendet */
                    TTF_Quit();
                    SDL_Quit();
                    exit(1);
                    break;

                default:
                    break;
            }
        }
    }
}

/**
  * Laedt den Highscore und zeigt ihn an.
  */
void highscore_oeffnen()
{
    int i;
    FILE *in, *out;
    char buffer[50];
    struct sDraw_element element;
    SDL_Color color;

    color.r = FONT_COLOR_RED;
    color.g = FONT_COLOR_GREEN;
    color.b = FONT_COLOR_BLUE;

    element.x = 0;
    element.y = 0;
    element.w = 500;
    element.h = 500;
    element.textur = all_textures[41];
    zeichne_sDraw_element(&element, 1, 0);

    element.x = 100;
    element.y = 110;
    element.w = 200;
    element.h = 50;

    in = fopen("./resources/highscore.txt", "r");
    if(in == NULL)
    {
        fclose(in);
        fprintf(stderr, "\nFehler highscore nicht gefunden!\n");

        /* keine highscore gefunden, also wird ein neuer erzeugt */
        out = fopen("./resources/highscore.txt", "w");
        if(out == NULL)
        {
            fprintf(stderr, "\nFehler bei highscore Outputstream könnte nicht erstellt werden!\n");
            return;
        }

        fprintf(out, "None     0\n");
        fprintf(out, "None     0\n");
        fprintf(out, "None     0\n");

        element.textur = convert_string_to_surface("None    0", font, &color, NULL, 0);
        element.y = 110;
        zeichne_sDraw_element(&element, 0, 0);

        element.y = 190;
        zeichne_sDraw_element(&element, 0, 0);

        element.y = 270;
        zeichne_sDraw_element(&element, 0, 1);
        fclose(out);
    }
    /* liest den  highscore ein. */
    else
    {
        fgets(buffer, 50, in);
        i = 0;
        while(buffer[i++] != '\n');
        buffer[--i] = '\0';
        element.textur = convert_string_to_surface(buffer, font, &color, NULL, 0);
        element.y = 110;
        zeichne_sDraw_element(&element, 0, 1);

        fgets(buffer, 50, in);
        i = 0;
        while(buffer[i++] != '\n');
        buffer[--i] = '\0';
        element.textur = convert_string_to_surface(buffer, font, &color, NULL, 0);
        element.y = 190;
        zeichne_sDraw_element(&element, 0, 1);

        fgets(buffer, 50, in);
        i = 0;
        while(buffer[i++] != '\n');
        buffer[--i] = '\0';
        element.textur = convert_string_to_surface(buffer, font, &color, NULL, 0);
        element.y = 270;
        zeichne_sDraw_element(&element, 0, 1);

        fclose(in);
    }


    while(SDL_WaitEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            /* SDL Fenster wird geschlossen, Spiel wird OHNE Speichern beendet */
            TTF_Quit();
            SDL_Quit();
            exit(1);
        }
        else if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case SDLK_1:
                    return;
                default:
                    break;
            }
        }
    }
}


/**
  * Erstellt die map und positioniert spieler, gegener usw. zum Spiel beginn,
  * wird auch zum restart des Spiels genutzt.
  */
void load_map(const char *filename)
{
    FILE *in;
    int i, j, k, x, y, direction, rev_direction, id, spec;

    /* den FileInputstream */
    in = fopen(filename, "r");
    if(in == NULL)
    {
        fprintf(stderr, "\nFehler1 bei Map %s nicht gefunden!\n", filename);
        exit(-1);
    }

    /* liest die breite und hoehe der map aus und allociert dan den nötigen speicher */
    fscanf(in,"(%d, %d)\n", &world_draw_height, &world_draw_width);
    printf("Hohe %d Breite %d\n", world_draw_height, world_draw_width);

    world_map_ids = (int**) malloc(world_draw_height * sizeof(int*));
    if(world_map_ids == NULL)
    {
        fprintf(stderr, "\nFehler2 bei Speicherallokation!\n" );
        exit(-1);
    }
    for(i = 0; i < world_draw_height; i++)
    {
        world_map_ids[i] = (int*) malloc(world_draw_width * sizeof(int));
        if(world_map_ids[i] == NULL)
        {
            fprintf(stderr, "\nFehler3 bei Speicherallokation!\n" );
            exit(-1);
        }
    }

    /* liest den reste der map ein */
    for(i = 0; i < world_draw_height; i++)
    {
        for(j = 0; j < world_draw_width ; j++)
        {
            fscanf(in, "%d", &k);
            printf("%d\t", k);
            world_map_ids[i][j] = k;
        }
        printf("%d\n", k);
    }

    /* position vom Spieler usw.*/

    /* wie viele gegener */
    fscanf(in,"\nGegner: %d\n", &num_move_elemnts);
    printf("Gegner: %d\n", num_move_elemnts);
    num_move_elemnts++;

    move_elements_on_map = (struct sMove_element**) malloc(num_move_elemnts * sizeof(struct sMove_element*));
    if(move_elements_on_map == NULL)
    {
        fprintf(stderr, "\nFehler4 bei Speicherallokation!\n" );
        exit(-1);
    }

    /* das erste element ist IMMER der Spieler */
    for(i = 0; i < num_move_elemnts; i++)
    {
        struct sMove_element *element;

        element = (struct sMove_element*) malloc(sizeof(struct sMove_element));
        if(element == NULL)
        {
            fprintf(stderr, "\nFehler5 bei Speicherallokation!\n" );
            exit(-1);
        }

        fscanf(in, "(%d, %d, %d, %d, %d, %d)\n", &x, &y, &direction, &rev_direction, &id, &spec);
        printf("Daten sind: %d, %d, %d, %d, %d, %d\n", x, y, direction, rev_direction, id, spec);

        element->x = x;
        element->y = y;
        element->direction = direction;
        element->rev_direction = rev_direction;
        element->id = id;
        element->spec = spec;

        move_elements_on_map[i] = element;
    }

    /* schließt den lese Stream */
    fclose(in);


    /* kombiniert die texturen mit dem world_draw_elements array zum zeichnen */

    /* allociert den speicher für das 3d sDraw_element array */
    world_draw_elements = (struct sDraw_element****) malloc(world_draw_height * sizeof(struct sDraw_element***));
    if(world_draw_elements == NULL)
    {
        fprintf(stderr, "\nFehler6 bei Speicherallokation!\n" );
        exit(-1);
    }
    for(i = 0; i < world_draw_height; i++)
    {
        world_draw_elements[i] = (struct sDraw_element***) malloc(world_draw_width * sizeof(struct sDraw_element**));
        if(world_draw_elements[i] == NULL)
        {
            fprintf(stderr, "\nFehler7 bei Speicherallokation!\n" );
            exit(-1);
        }
        for(j = 0; j < world_draw_width; j++)
        {
            world_draw_elements[i][j] = (struct sDraw_element**) malloc(K * sizeof(struct sDraw_element*));
            if(world_draw_elements[i][j] == NULL)
            {
                fprintf(stderr, "\nFehler8 bei Speicherallokation!\n" );
                exit(-1);
            }
            /* saeubert das array */
            for(k = 0; k < K; k++)
            {
                world_draw_elements[i][j][k] = NULL;
            }
        }
    }

    /* kombniert die texturen des texturs array mit den ids der world_map */
    for(i = 0; i < world_draw_height; i++)
    {
        for(j = 0; j < world_draw_width; j++)
        {
            /* erzeugt ein sDraw_element und speichert es */
            struct sDraw_element *element;

            element = (struct sDraw_element*) malloc(sizeof(struct sDraw_element));
            if(element == NULL)
            {
                fprintf(stderr, "\nFehler9 bei Speicherallokation!\n" );
                exit(-1);
            }

            element->w = BLOCK_WIDTH;
            element->h = BLOCK_HEIGHT;


            k = world_map_ids[i][j];
            if(k == -1)
                 element->textur = all_textures[45];
            else if(k == 0)
                 element->textur = NULL;
            else
                 element->textur = all_textures[k];

            world_draw_elements[i][j][0] = element;
        }
    }

    /* fuegt den Spieler und die Gegner dem world_draw_elements array hinzu */
    for(i = 0; i < num_move_elemnts; i++)
    {
        struct sDraw_element *element;

        element = (struct sDraw_element*) malloc(sizeof(struct sDraw_element));
        if(element == NULL)
        {
            fprintf(stderr, "\nFehler10 bei Speicherallokation!\n" );
            exit(-1);
        }


        x = move_elements_on_map[i]->x;
        y = move_elements_on_map[i]->y;

        k = move_elements_on_map[i]->id;
        k =k * 10 + move_elements_on_map[i]->direction;


        element->x = BLOCK_WIDTH * x;
        element->y = BLOCK_HEIGHT * y;
        element->w = BLOCK_WIDTH;
        element->h = BLOCK_HEIGHT;
        element->textur = all_textures[k];

        world_draw_elements[y][x][1] = element;
    }

    /* Zeichnet einmal alles zum Start */
    zeichne_screen_all();


    /* speichert die start zeit des eigentlichen Spiels */
    start_time = SDL_GetTicks();

    /* setzt geb. falls den highscore zurück */
    highscore = 0;
}



/* loads an image to an SDL surface */
SDL_Surface * load_img( const char *filename )
{
    /* image to be loaded */
    SDL_Surface* loadedImage = NULL;
    /* optimized image that will be used */
    SDL_Surface* optimizedImage = NULL;

    /* color key for transparent color */
    unsigned int colorkey = 0;

    /* loading the image */
    loadedImage = SDL_LoadBMP( filename );

    /* if image loaded */
    if( loadedImage != NULL )
    {
        /* create an optimized image */
        optimizedImage = SDL_DisplayFormat( loadedImage );
        /* and free the old image */
        SDL_FreeSurface( loadedImage );
    }

    /* was the image optimized */
    if( optimizedImage != NULL )
    {
        /*
        * map the color key:
        * call SDL_MapRGB() to take the red, green, and blue values
        * give us the pixel value back in the same format as the surface
        */
        colorkey = SDL_MapRGB( optimizedImage->format, COLOR_KEY_R, COLOR_KEY_G, COLOR_KEY_B );

        /*
        * set all pixels with the same color as _colorkey to be transparent:
        * - first argument is the surface we set the color key to
        * - second argument holds flag:
        *      SDL_SRCCOLORKEY flag makes sure that the color key
        *      is used when blitting this surface onto another
        * - third argument is the color that shall be set as the color key
        */
        SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
    }

    /* return the optimized image */
    return optimizedImage;
}  /* load_img */

/* Konvertiert einen String in ein SDL_Surface um */
SDL_Surface* convert_string_to_surface(char *text, TTF_Font *font, SDL_Color *text_color, SDL_Color *back_color, int render)
{
    int i, max_h, min_h, w, size, minx, maxx, miny, maxy, advance;
    int *h;
    SDL_Rect rect;
    SDL_Surface *surface, *element;

    /* wie lang ist der String */
    size = 0;
    while(text[size++] != '\0');
    size--;

    h = (int*) malloc(size * sizeof(int));
    if(h == NULL)
        return NULL;

    max_h = 0;
    min_h = 0;
    w = 0;
    for(i = 0; i < size; i++)
    {
        TTF_GlyphMetrics(font, text[i], &minx, &maxx, &miny, &maxy, &advance);
        h[i] = maxy;
        if(maxy > max_h)
            max_h = maxy;
        if(abs(miny) > min_h)
            min_h = abs(miny);
        w += advance;
    }

    rect.x = 0;
    rect.y = 0;
    rect.h = max_h;
    rect.w = w/size;
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, max_h + min_h, SCREEN_BPP, 0, 0, 0, 0);

    for(i = 0; i < size; i++)
    {
        switch(render)
        {
            case 1:
                element = TTF_RenderGlyph_Shaded(font, text[i], (*text_color), (*back_color));
                break;
            case 2:
                element = TTF_RenderGlyph_Blended(font, text[i], (*text_color));
                break;
            default:
                element = TTF_RenderGlyph_Solid(font, text[i], (*text_color));
                break;
        }

        if(h[i] < 0)
        {
            h[i] = 1;
        }
        rect.y = max_h - h[i];
        SDL_BlitSurface(element, NULL, surface, &rect);
        rect.x += w/size;
    }

    return surface;
}

/**
  * Zeichnet den screen mit allen elementen neu,
  * dabei wird die Position des Spielers beachtet.
  */
void zeichne_screen_all()
{
    int i,j,k, x_correction, y_correction;

    /* sorgt dafuer das der Spieler immer im Mittelpunkt des screens ist */
    x_correction = move_elements_on_map[0]->x - N/2;
    y_correction = move_elements_on_map[0]->y - M/2;

    /* clean old image */
    SDL_FillRect(screen, NULL,  bg_color);

    for(i = 0; i < M; i++)
    {
        for(j = 0; j < N; j++)
        {
            for(k = 0; k < K; k++)
            {
                if( ADVANCE_MAP &&  ((i + y_correction < 0) || (j + x_correction < 0)
                    || (i + y_correction > (world_draw_height - 1)) || (j + x_correction > (world_draw_width - 1))) )
                {
                    break;
                }
                else
                {
                    if(world_draw_elements[i + y_correction][j + x_correction][k] == NULL)
                        break;

                    world_draw_elements[i + y_correction][j + x_correction][k]->x = BLOCK_WIDTH * j;
                    world_draw_elements[i + y_correction][j + x_correction][k]->y = BLOCK_WIDTH * i;
                    zeichne_sDraw_element(world_draw_elements[i + y_correction][j + x_correction][k], 0, 0);
                }
            }
        }
    }

    /* use 'SDL_FLIP' to actually print updated screen */
    SDL_Flip(screen);
}

/**
  * Zeichnet den angegebenen bereich mit allen Elementen in der z-achse neu,
  * nur für block texturen im standart Formart verwendbar.
  */
void zeichne_screen_all_in_z(int x_screen, int y_screen, int x_array, int y_array, int draw_black, int flip)
{
    int k;
    SDL_Rect screen_draw_rect;

    if(draw_black)
    {
        screen_draw_rect.x = x_screen;
        screen_draw_rect.y = y_screen;
        screen_draw_rect.w = BLOCK_WIDTH;
        screen_draw_rect.h = BLOCK_HEIGHT;

        SDL_FillRect(screen, &screen_draw_rect, bg_color);
    }

    for(k = 0; k < K; k++)
    {
        if(world_draw_elements[y_array][x_array][k] == NULL)
            break;

        world_draw_elements[y_array][x_array][k]->x = x_screen;
        world_draw_elements[y_array][x_array][k]->y = y_screen;
        zeichne_sDraw_element(world_draw_elements[y_array][x_array][k], 0, 0);
    }

    if(flip)
        SDL_Flip(screen);
}

/**
  * Zeichnet das uebergebene element auf den screen,
  * draw_black true uebermalt die Stelle zuerst mit der backround colour,
  * flip true zeichnet den screen neu.
  */
void zeichne_sDraw_element(struct sDraw_element *element, int draw_black, int flip)
{
    SDL_Rect screen_draw_rect;

    screen_draw_rect.x = element->x;
    screen_draw_rect.y = element->y;
    screen_draw_rect.w = element->w;
    screen_draw_rect.h = element->h;

    if(draw_black)
        SDL_FillRect(screen, &screen_draw_rect, bg_color);
    if(element->textur != NULL)
        SDL_BlitSurface(element->textur, NULL, screen, &screen_draw_rect);
    if(flip)
        SDL_Flip(screen);
}


/* gibt alle Elemente des world_map_ids, world_draw_elements und move_elements_on_map frei */
void freeArrays()
{
    int i, j, k;

    /* world_map_ids */
    for(i = 0; i < world_draw_height; i++)
    {
        free(world_map_ids[i]);
    }
    free(world_map_ids);

    /* world_draw_elements */
    for(i = 0; i < world_draw_height; i++)
    {
        for(j = 0; j < world_draw_width; j++)
        {
            for(k = 0; k < K; k++)
            {
                free(world_draw_elements[i][j][k]);
            }
            free(world_draw_elements[i][j]);
        }
        free(world_draw_elements[i]);
    }
    free(world_draw_elements);


    /* move_elements_on_map */
    for(i = 0; i < num_move_elemnts; i++)
    {
        free(move_elements_on_map[i]);
    }
    free(move_elements_on_map);
}


