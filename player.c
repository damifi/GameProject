/* Einbinden der verwendeten Standard-Bibliotheken */
#include "game.h"

/*-------------------------------------------------------------------------------------*/

/* stellt fest ob man gewonnen hat bzw. verloren */
int spiel_status()
{
    int x, y, i, j, text_size, quit, read_highscore;
    Uint32 time;
    char *input_chars;
    char temp[50], temp1[50], temp2[50], temp3[50], text[50], space_chars[50];
    char *buffer[3];
    struct sDraw_element element;
    SDL_Color color;
    FILE *in, *out;

    x = move_elements_on_map[0]->x;
    y = move_elements_on_map[0]->y;

    /* gewonnen */
    if(num_move_elemnts == 1 && world_map_ids[y][x] == -1)
    {
        text_size = 0;
        quit = 0;
        text[0] = '\0';
        color.r = FONT_COLOR_RED;
        color.g = FONT_COLOR_GREEN;
        color.b = FONT_COLOR_BLUE;

        /* Errechne den Endpunktstand */
        time = SDL_GetTicks() - start_time;
        printf("time: %d\n", time);
        time /= 100;
        highscore += move_elements_on_map[0]->spec;

        if(time < 500)
            highscore += 500 - time;


        /* Laedt den alten highscore */
        in = fopen("./resources/highscore.txt", "r");
        if(in == NULL)
        {
            printf("kein highscore\n");
            fclose(in);

            /* highscore nicht gefunden */
            buffer[0] = "None     0\n";
            buffer[1] = "None     0\n";
            buffer[2] = "None     0\n";
        }
        else
        {
            printf("highscore\n");

            fgets(temp1, 50, in);
            buffer[0] = temp1;

            fgets(temp2, 50, in);
            buffer[1] = temp2;

            fgets(temp3, 50, in);
            buffer[2] = temp3;

            fclose(in);
        }

        /* prueft ob ein neuer highscore aufgestellt wurde */
        for(i = 0; i < 3; i++)
        {
            /* liest den highscore aus dieser zeile aus */
            sscanf(buffer[i],"%*[^0123456789]%d", &read_highscore);

            if(highscore <= read_highscore)
                continue;

            element.x = 0;
            element.y = 0;
            element.w = 500;
            element.h = 500;
            element.textur = all_textures[43];
            zeichne_sDraw_element(&element, 1, 0);

            sprintf(temp, "%d Points", highscore);
            element.textur = convert_string_to_surface(temp, font, &color, NULL, 0);
            element.x = 250 - (element.textur->w / 2);
            element.y = 150;
            zeichne_sDraw_element(&element, 0, 1);

            quit = 0;
            while(!quit)
            {
                while(SDL_PollEvent(&event))
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
                        input_chars = SDL_GetKeyName(event.key.keysym.sym);
                        if(strcmp(input_chars, "return") == 0)
                        {
                            quit = 1;
                            break;
                        }
                        else if(input_chars[0] > 96 && input_chars[0] < 123)
                        {
                            text[text_size++] = *input_chars;
                            text[text_size] = '\0';
                        }
                        if(text_size >= 8)
                        {
                            quit = 1;
                        }

                        /* zeichne den Screen neu */
                        element.x = 0;
                        element.y = 0;
                        element.w = 500;
                        element.h = 500;
                        element.textur = all_textures[43];
                        zeichne_sDraw_element(&element, 1, 0);

                        sprintf(temp, "%d Points", highscore);
                        element.textur = convert_string_to_surface(temp, font, &color, NULL, 0);
                        element.x = 250 - (element.textur->w / 2);
                        element.y = 150;
                        zeichne_sDraw_element(&element, 0, 0);

                        element.x = 80;
                        element.y = 400;
                        element.w = 500;
                        element.h = 500;
                        element.textur = convert_string_to_surface(text, font, &color, NULL, 0);
                        zeichne_sDraw_element(&element, 0, 1);
                    }
                }
            }
            for(j = 0; j < 9 - text_size ; j++)
            {
                space_chars[j] = ' ';
            }
            space_chars[j] = '\0';

            for(j = 2 ; j > i; j--)
            {
                buffer[j] = buffer[j - 1];
            }
            sprintf(temp, "%s%s%d\n", text, space_chars, highscore);
            buffer[i] = temp;


            /* Schreibt einen Neuen highscore */
            out = fopen("./resources/highscore.txt", "w");
            if(out == NULL)
            {
                fprintf(stderr, "\nFehler bei highscore Outputstream könnte nicht erstellt werden!\n");
                return 0;
            }

            fprintf(out, "%s", buffer[0]);
            fprintf(out, "%s", buffer[1]);
            fprintf(out, "%s", buffer[2]);
            fclose(out);

            return 0;
        }

        element.x = 0;
        element.y = 0;
        element.w = 500;
        element.h = 500;
        element.textur = all_textures[42];
        zeichne_sDraw_element(&element, 1, 0);

        sprintf(temp, "%d Points", highscore);
        element.textur = convert_string_to_surface(temp, font, &color, NULL, 0);
        element.x = 250 - (element.textur->w / 2);
        element.y = 150;
        zeichne_sDraw_element(&element, 0, 1);


        /* kein neuer highscore erstellt */
        while(SDL_WaitEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                /* SDL Fenster wird geschlossen, Spiel wird OHNE Speichern beendet */
                SDL_Quit();
                exit(1);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_1:
                        SDL_Quit();
                        exit(1);

                    case SDLK_2:
                        freeArrays();
                        return 0;
                    default:
                        break;
                }
            }
        }

        return 0;
    }
    /* verloren */
    else if(move_elements_on_map[0]->spec < 0)
    {
        /* spieler stirbt */

        /* laest den soieler "explodieren" */
        world_draw_elements[y][x][1]->textur = all_textures[5];
        zeichne_screen_all();
        SDL_Delay(1000);

        element.x = 0;
        element.y = 0;
        element.w = 500;
        element.h = 500;
        element.textur = all_textures[44];
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
                        TTF_Quit();
                        SDL_Quit();
                        exit(1);
                    case SDLK_2:
                        freeArrays();
                        return 0;;
                    default:
                        break;
                }
            }
        }
    }

    return 1;
}


/** Der Spieler ist am Zug, er hat dafür nur eine gewisse Zeit,
  * nach ablauf wird nichts gemacht.
  */
void spieler_zug()
{
    int on_turn, x, y, old_time, new_time;
    struct sMove_element *element;

    on_turn = NUM_SUB_TURNS;

    /* Zeit des Spielerzugbeginns */
    old_time = SDL_GetTicks();

    x = move_elements_on_map[0]->x;
    y = move_elements_on_map[0]->y;

    while(on_turn)
    {
        new_time = SDL_GetTicks();
        if( (new_time - old_time) > DELAY_TURN)
        {
            /* Zugzeit abgelaufe */
            break;
        }

        SDL_PollEvent(&event);
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
                case SDLK_w:
                    /* nach oben gehen */
                    if(zug_moeglich(x, y - 1))
                    {
                        aktuallisiere_position(move_elements_on_map[0], x, y - 1, 0);
                        on_turn -= 1;

                        zeichne_screen_all();
                    }
                    break;
                case SDLK_d:
                    /* nach rechts gehen */
                    if(zug_moeglich(x + 1, y))
                    {
                        aktuallisiere_position(move_elements_on_map[0], x + 1, y, 1);
                        x = move_elements_on_map[0]->x;
                        y = move_elements_on_map[0]->y;

                        on_turn -= 1;
                        zeichne_screen_all();
                    }
                    break;
                case SDLK_s:
                    /* nach unten gehen */
                    if(zug_moeglich(x, y + 1))
                    {
                        aktuallisiere_position(move_elements_on_map[0], x, y + 1, 2);
                        x = move_elements_on_map[0]->x;
                        y = move_elements_on_map[0]->y;

                        on_turn -= 1;
                        zeichne_screen_all();
                    }
                    break;
                case SDLK_a:
                    /* nach links gehen */
                    if(zug_moeglich(x - 1, y))
                    {
                        aktuallisiere_position(move_elements_on_map[0], x - 1, y, 3);
                        x = move_elements_on_map[0]->x;
                        y = move_elements_on_map[0]->y;

                        on_turn -= 1;
                        zeichne_screen_all();
                    }
                    break;
                case SDLK_q:
                    /* angriff */
                    attack_animation(move_elements_on_map[0]);
                    if( (element = sMove_element_At(x, y - 1)) != NULL)
                    {
                        attack(element, 30);
                        on_turn -= 1;
                    }
                    else if( (element = sMove_element_At(x + 1, y)) != NULL)
                    {
                        attack(element, 30);
                        on_turn -= 1;
                    }
                    else if( (element = sMove_element_At(x, y + 1)) != NULL)
                    {
                        attack(element, 30);
                        on_turn -= 1;
                    }
                    else if( (element = sMove_element_At(x - 1, y)) != NULL)
                    {
                        attack(element, 30);
                        on_turn -= 1;
                    }
                    break;
                default:
                    break;
            }

        }
    }

    if((new_time - old_time) <= DELAY_TURN)
    {
        /* rest Zeit die am Runden ende gewartet wird */
        rest_turn_delay = DELAY_TURN - (new_time - old_time);
    }
}

/**
  * Sucht nach einem sMove_element was an den uebergebenen koordinaten steht
  * und gibt es zurueck,
  * es wird NULL zurueckgegeben wenn nichts gefunden wird.
  */
struct sMove_element* sMove_element_At(int x, int y)
{
    int i;
    if( ADVANCE_MAP && ((y  < 0) || (x  < 0) || (y > (world_draw_height - 1)) || (x > (world_draw_width - 1))) )
    {
        return NULL;
    }

    for(i = 0; i < num_move_elemnts; i++)
    {
        if(move_elements_on_map[i]->x == x && move_elements_on_map[i]->y == y)
            return move_elements_on_map[i];
    }
    return NULL;
}


/* Prueft ob der Zug des Spieler, Ki moeglich ist, bezueglich neuer Position */
int zug_moeglich(int x, int y)
{
    if( ADVANCE_MAP &&  ((y  < 0) || (x  < 0) || (y > (world_draw_height - 1)) || (x > (world_draw_width - 1))) )
    {
        return 0;
    }

    if( world_map_ids[y][x] != -1 && (world_map_ids[y][x] < FIRST_FLOOR_TEXTUR || world_map_ids[y][x] > LAST_FLOOR_TEXTUR))
    {
        return 0;
    }

    if(sMove_element_At(x, y) != NULL)
    {
        return 0;
    }


    return 1;
}

/**
  * Aktuallisiert die Position des sMove_elements sowohl im move_elements_on_map
  * als auch world_draw_elements.
  */
void aktuallisiere_position(struct sMove_element *element, int new_x, int new_y, int direction)
{
    int x, y;
    struct sDraw_element *pointer;

    x = element->x;
    y = element->y;

    pointer =  world_draw_elements[y][x][1];
    pointer->x = new_x;
    pointer->y = new_y;

    world_draw_elements[y][x][1] = NULL;
    world_draw_elements[new_y][new_x][1] = pointer;

    element->x = new_x;
    element->y = new_y;
    switch(direction)
    {
        case 0: element->rev_direction = 2; break;
        case 1: element->rev_direction = 3; break;
        case 2: element->rev_direction = 0; break;
        case 3: element->rev_direction = 1; break;
        default: element->rev_direction = 0; break; /* wenn direction unbekannt, wird sie auf 0 gesetzt */
    }
    element->direction = direction;
}

/* Fuehrt eine Angriffs animation aus, unabhaengig davon ob etwas getroffen werden kann*/
void attack_animation(struct sMove_element *aggressor)
{
    int x, y;
    struct sDraw_element element;

    x = aggressor->x;
    y = aggressor->y;

    if( (y < (move_elements_on_map[0]->y - M/2)) || (y > (move_elements_on_map[0]->y + M/2))
       || (x < (move_elements_on_map[0]->x - N/2)) || (x > (move_elements_on_map[0]->x + N/2)) )
       return;

    element.x = (aggressor->x - (move_elements_on_map[0]->x - N/2)) * BLOCK_WIDTH;
    element.y = (aggressor->y - (move_elements_on_map[0]->y - M/2)) * BLOCK_HEIGHT;
    element.w = 50;
    element.h = 50;

    element.textur = all_textures[50];

    zeichne_sDraw_element(&element, 0, 1);

    world_draw_elements[y][x][2] = &element;

    zeichne_screen_all_in_z( (aggressor->x - (move_elements_on_map[0]->x - N/2)) * BLOCK_WIDTH, (aggressor->y - (move_elements_on_map[0]->y - M/2)) * BLOCK_HEIGHT, x, y, 1, 1);
    SDL_Delay(ATTACK_DELAY);

    world_draw_elements[y][x][2]  = NULL;
}

/**
  * Verusacht einen Angriff und verwaltet die Folgen:
  * Lebenspunkte werden reduziert und wenn ein Gegener oder der Spieler stirb
  */
void attack(struct sMove_element *defender, int damage)
{
    int x, y, i;

    if(defender == NULL)
        return;

    x = defender->x;
    y = defender->y;
    defender->spec -= damage;

    /* aendert die textur wenn der Spieler/Gegner schaden erleidtet */
    if(defender->id == 0)
    {
        if(defender->spec > 79)
            world_draw_elements[y][x][1]->textur = all_textures[0];
        else if(defender->spec > 59)
            world_draw_elements[y][x][1]->textur = all_textures[1];
        else if(defender->spec > 39)
            world_draw_elements[y][x][1]->textur = all_textures[2];
        else if(defender->spec > 19)
            world_draw_elements[y][x][1]->textur = all_textures[3];
        else if(defender->spec > 0)
            world_draw_elements[y][x][1]->textur = all_textures[4];
    }
    else if(defender->id == 1)
    {
        if(defender->spec > 25)
            world_draw_elements[y][x][1]->textur = all_textures[10];
        else if(defender->spec > 0)
            world_draw_elements[y][x][1]->textur = all_textures[11];
    }

    /* Spezialfall wenn Gegner oder der Spieler stirbt */
    if(defender->spec <= 0)
    {
        if( defender->id != 0)
        {
            /* gegner stirbt */
            world_draw_elements[y][x][1] = NULL;

            /* diese Element aus dem move_elements_on_map array entfernen*/
            for(i = 1; i < num_move_elemnts; i++)
            {
                if(move_elements_on_map[i] == defender)
                {
                    /* verschiebt die nachfolge element um luecken zu vermeiden */
                    for( ; i < num_move_elemnts; i++)
                    {
                        move_elements_on_map[i] = move_elements_on_map[i + 1];
                    }
                    move_elements_on_map[i] = NULL;
                    break;
                }
            }

            free(defender);
            num_move_elemnts--;
        }
    }
}
