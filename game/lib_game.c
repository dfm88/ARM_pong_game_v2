
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"
#include "../adc/adc.h"

/**
 * @brief  Function that initializes game
 */

void draw_score(int value, int player)
{
    int start;
    uint8_t is_reverse;
    char text[sizeof(value)];
    // start = player == 1 ? 7 : 200;
    start = 7;
    is_reverse = player == 1 ? 0 : 1;
    
    GUI_Text(start, 160, (unsigned char *)text, Black, Black, is_reverse);
    sprintf(text, "%d", value);
    GUI_Text(start, 160, (uint8_t *)text, White, Black, is_reverse);
}
void draw_record(int value)
{
    char text[sizeof(value)];
    sprintf(text, "%d", value);
    GUI_Text(200, 7, (uint8_t *)text, White, Black, 0);
}

void draw_ball(uint16_t x, uint16_t y, struct struct_ball *_ball)
{
    int i = 0;
    _ball->posX = x;
    _ball->posY = y;
    for (i = 0; i < 5; i++)
    {
        LCD_DrawLine(_ball->posX, _ball->posY + i, _ball->posX + 4, _ball->posY + i, Green);
    }
}

void initialize_ball(struct struct_ball *_ball)
{
    NVIC_DisableIRQ(EINT0_IRQn); /* enable Button interrupts			*/
    // _ball->posX = 230;
    // _ball->posY = 150;
    // _ball->h_direc = 1;
    // _ball->v_direc = 1;
    // _ball->h_speed = 2;
    // _ball->v_speed = 2;
    _ball->posX = 160;
    _ball->posY = 45;
    _ball->h_direc = 1;
    _ball->v_direc = -1;
    _ball->h_speed = 2;
    _ball->v_speed = 2;

    draw_ball(_ball->posX, _ball->posY, &ball);
}

void delete_ball(struct struct_ball *_ball)
{
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        LCD_DrawLine(_ball->posX, _ball->posY + i, _ball->posX + 4, _ball->posY + i, Black);
    }
}

void GAME_init(void)
{
    int i = 0;

    initialize_ball(&ball);

    // draw paddle1
    paddle1.posX = 110;
    paddle1.posY = 277;
    for (i = 0; i < 10; i++)
    {
        LCD_DrawLine(paddle1.posX, paddle1.posY + i, paddle1.posX + 49, paddle1.posY + i, Green);
    }
    // draw paddle2
    paddle2.posX = 130;
    paddle2.posY = 32;
    paddle2.h_direc = -1;
    for (i = 0; i < 10; i++)
    {
        LCD_DrawLine(paddle2.posX, paddle2.posY + i, paddle2.posX + 49, paddle2.posY + i, Green);
    }

    // draw walls
    for (i = 0; i < 5; i++)
    {
        // left wall
        LCD_DrawLine(i, 0, i, 319, Red);
        // right wall
        LCD_DrawLine(235 + i, 0, 235 + i, 319, Red);
    }

    draw_score(score1, 1);
    draw_score(score2, 2);
    GUI_Text(38, 305, (unsigned char *)"Press Key1 to start", White, Black, 0);
}

uint8_t increase_score(int player)
{
    // increase score
    if(player ==1) {
        score1++;
        draw_score(score1, 1);
        if(score1==5) {
            return 1;
        }
    }
    else if (player == 2) {
        score2++;
        draw_score(score2, 2);
        if(score2==5) {
            return 2;
        } 
    }

    pause_game();
    delete_ball(&ball);
    prepare_restart_game();
    return 0;
}

void game_over(int winner)
{
    uint8_t win_reverse;
    uint8_t lose_reverse;
    int lose_msg_high;
    int win_msg_high;

    NVIC_EnableIRQ(EINT0_IRQn); /* enable Button interrupts			*/
    score1 = 0;
    score2 = 0;
    // delete score p1
    GUI_Text(7, 160, (unsigned char *)"        ", Black, Black, 0);
    // delete score p2
    GUI_Text(7, 160, (unsigned char *)"        ", Black, Black, 1);
    draw_score(score1, 1);
    draw_score(score2, 2);

    if(winner == 1) 
    {
        lose_msg_high = 140;
        win_msg_high = 180;
        win_reverse = 0;
        lose_reverse = 1;
    }
    else if (winner ==2 )
    {
        lose_msg_high = 180;
        win_msg_high = 140;
        win_reverse = 1;
        lose_reverse = 0;
    }

    GUI_Text(90, lose_msg_high, (unsigned char *)"You lose", Red, Black, lose_reverse);
    GUI_Text(90, win_msg_high, (unsigned char *)"You win", Green, Black, win_reverse);
    GUI_Text(38, 210, (unsigned char *)"Press Int0 to continue", White, Black, 0);
    pause_game();
    delete_ball(&ball);
}

void pause_game()
{
    disable_timer(0);
    ADC_paused();
}

void resume_game()
{
    is_game_over = 0;
    enable_timer(0);
    ADC_resumed();
}

void start_game()
{
    // delete 'Press key1 to start"
    GUI_Text(38, 305, (unsigned char *)"                   ", Black, Black, 0);
    ADC_init(); /* ADC Initialization	for paddle movement	*/
    enable_timer(0);
}

void prepare_restart_game()
{
    // delete "You lose/ you win"
    GUI_Text(80, 140, (unsigned char *)"           ", Black, Black, 1);
    // delete "You lose/ you win"
    GUI_Text(80, 180, (unsigned char *)"           ", Black, Black, 0);
    // delete "Press Int0 to continue"
    GUI_Text(38, 210, (unsigned char *)"                      ", Black, Black, 0);
    initialize_ball(&ball);
    GUI_Text(38, 305, (unsigned char *)"Press Key1 to start", White, Black, 0);
}

void restart_game()
{
    // delete "Press Key1 to start"
    GUI_Text(38, 305, (unsigned char *)"                   ", White, Black, 0);
    resume_game();
}

void play_sound(uint16_t k)
{
    reset_timer(1);
    init_timer(1, 0, 0, 3, k);
    enable_timer(1);
}

int is_colliding(struct struct_ball *_ball, int8_t direction)
{

    uint8_t delta_h, delta_v;
    int v_limit, h_limit, current_pos;

    delta_h = _ball->h_direc > 0 ? 4 : 0;
    delta_v = _ball->v_direc > 0 ? 4 : 0;

    // 0x68 = h    0x76 = v
    current_pos = direction == 'h' ? _ball->posX + delta_h : _ball->posY + delta_v;

    v_limit = _ball->v_direc > 0 ? 276 : 42; // (last or first valid pixel)
    h_limit = _ball->h_direc > 0 ? 234 : 5; // (last or first valid pixel)

    switch (direction)
    {
    case 'h': /* moving horizontally */
        return current_pos - h_limit == 0;

    case 'v': /* moving vertically */
        // if _ball->was moving up
        if (_ball->v_direc < 0) 
        {
            // is colliding with paddle high && paddle is upon the ball
            if (current_pos - v_limit == 0)
            {
                if ((paddle2.posX - 4) <= _ball->posX)
                {
                    if (_ball->posX <= (paddle2.posX + 49))
                    {
                        return 1;
                    }
                }
            }
            else
            {
                return 0;
            }
        }
        // if _ball->was moving down, check paddle position
        else
        {
            // is colliding with paddle high && paddle is under the ball
            if (current_pos - v_limit == 0)
            {
                if ((paddle1.posX - 4) <= _ball->posX)
                {
                    if (_ball->posX <= (paddle1.posX + 49))
                    {
                        return 1;
                    }
                }
            }
            else
            {
                return 0;
            }
        }

    default:
        return 0;
    }
}

void handle_paddle_collsion(struct struct_ball *_ball, struct struct_paddle *_paddle)
{
    // Dichotomic search
    /**
     *            Speed change after paddle hit (v vertical, h horizontal, r right, l left))
     *  --------------------------------------------------------------------------------------------------------------
     * | hl=4,v=1 | hl=3,v=2 | hl=2,v=3 | hl=1,v=4 | hl=1,v=5 || hr=1,v=5 | hr=1,v=4 | hr=2,v=3 | hr=3,v=2 | hr=4,v=1 |
     *
     * |   1-5    |   6-10   |   7-15   |   16-20  |   21-25  ||   25-30  |   31-35  |   36-40  |   41-45  |   46-50  |
     *  --------------------------------------------------------------------------------------------------------------
     *                                      Paddle lenght
     *
     *  Speed change will be handle by the portion of the pad hit by at least sup(ball_lenght/2)=3 ball pixels
     */

    /** 1-23 **/
    if (_ball->posX <= (_paddle->posX + 22))
    {
        _ball->h_direc = -1;
        // 1-13
        if (_ball->posX <= (_paddle->posX + 12))
        {
            // 1-8
            if (_ball->posX <= (_paddle->posX + 7))
            {
                // 1-3
                if (_ball->posX <= _paddle->posX + 2)
                {
                    _ball->v_speed = 1;
                    _ball->h_speed = 4;
                    return;
                }
                // 4-8
                else
                {
                    _ball->v_speed = 2;
                    _ball->h_speed = 3;
                    return;
                }
            }
            // 9-13
            else
            {
                _ball->v_speed = 3;
                _ball->h_speed = 2;
                return;
            }
        }
        // 14-23
        else
        {
            // 14-18
            if (_ball->posX <= _paddle->posX + 17)
            {
                _ball->v_speed = 4;
                _ball->h_speed = 1;
                return;
            }
            // 19-23
            else
            {
                _ball->v_speed = 5;
                _ball->h_speed = 1;
                return;
            }
        }
    }
    /** 24-50 **/
    else
    {
        _ball->h_direc = 1;
        // 39-50
        if (_ball->posX >= _paddle->posX + 38)
        {
            // 44-50
            if (_ball->posX >= _paddle->posX + 43)
            {
                _ball->v_speed = 1;
                _ball->h_speed = 4;
                return;
            }
            // 39-43
            else
            {
                _ball->v_speed = 2;
                _ball->h_speed = 3;
                return;
            }
        }
        // 24-38
        else
        {
            // 29-38
            if (_ball->posX >= _paddle->posX + 28)
            {
                // 34-38
                if (_ball->posX >= _paddle->posX + 33)
                {
                    _ball->v_speed = 3;
                    _ball->h_speed = 2;
                    return;
                }
                // 29-33
                else
                {
                    _ball->v_speed = 4;
                    _ball->h_speed = 1;
                    return;
                }
            }
            // 24-28
            else
            {
                _ball->v_speed = 5;
                _ball->h_speed = 1;
                return;
            }
        }
    }
}

void move_paddle(unsigned short mov, struct struct_paddle *_paddle, uint8_t ai)
{
    uint8_t increment = 0;
    int8_t dir = 0;
    uint8_t i = 0;
    uint8_t drawing_dir;
    uint8_t delta_del;
    uint8_t delta_draw;
    // mov == 0-3 move left
    // mov == 4   hold
    // mov == 5-8 move right

    // if AI is true, direction of paddle is evaluated from oaddle h_direc attribute,
    // else it is evaluated from mov value
    dir = ai ? _paddle->h_direc : mov > 4 ? 1 : -1;        // -1
    drawing_dir =1;
    // drawing_dir = ai ? -1 : 1;
    delta_del = dir > 0 ? 0 : 49;  // 49
    delta_draw = dir > 0 ? 49 : 0; // 0

    switch (mov)
    {
    case 0:
    case 8:
        increment = 10;
        break;
    case 1:
    case 7:
        increment = 8;
        break;
    case 2:
    case 6:
        increment = 6;
        break;
    case 3:
    case 5:
        increment = 4;
        break;
    case 4:
        increment = 0;
        dir = 0;
        return;
    }

    for (i = 0; i < increment; i++)
    {
        if ((dir > 0 && _paddle->posX < 183) || (dir < 0 && _paddle->posX > 5))
        {
            // delete line
            LCD_DrawLine(_paddle->posX + (delta_del /* + i*dir*/), _paddle->posY, _paddle->posX + (delta_del /* + i*dir*/), _paddle->posY + (9*drawing_dir), Black);
            // change _paddle->position
            _paddle->posX = _paddle->posX + dir;
            // draw line
            LCD_DrawLine(_paddle->posX + (delta_draw /* + i*dir*/), _paddle->posY, _paddle->posX + (delta_draw /* + i*dir*/), _paddle->posY + (9*drawing_dir), Green);
        }
        else
        {
            // if _paddle pointer is the paddle2 (AI)
            if(ai)
            {
                _paddle->h_direc = -_paddle->h_direc;
            }
            return;
        }
    }
    return;
}

void move_ball( struct struct_ball *_ball)
{
    // Vertical movement  : 1 = Sud,
    //					   -1 = North
    // Horizontal movement: 1 = East,
    //					   -1 = West

    uint8_t previous_h_speed, previous_v_speed, longer_speed, isColliding;
    uint8_t delta = 4;
    uint8_t i = 0;

    while (!is_game_over)
    {
        previous_h_speed = _ball->h_speed;
        previous_v_speed = _ball->v_speed;

        longer_speed = _ball->v_speed >= _ball->h_speed ? _ball->v_speed : _ball->h_speed;

        for (i = 0; i < longer_speed; i++)
        {

            // if ball is moving HORIZONTALLY and Horizontal speed is not lapsed
            if (_ball->h_direc && _ball->h_speed)
            {
                isColliding = is_colliding(&ball, 'h');

                if (isColliding)
                {
                    // play_sound(1890);
                    _ball->h_speed = previous_h_speed;
                    _ball->v_speed = previous_v_speed;
                    _ball->h_direc = -_ball->h_direc;
                    draw_score(score1, 1);   // just to partially handle score erasing on ball moving hover
                    draw_score(score2, 2);   // just to partially handle score erasing on ball moving hover
                    return;
                }
                if (_ball->h_direc < 0)
                {
                    // MOVE horizontal left
                    // delete horizontal right
                    LCD_DrawLine(_ball->posX + delta, _ball->posY, _ball->posX + delta, _ball->posY + delta, Black);
                    // paint horizontal left
                    LCD_DrawLine(_ball->posX + _ball->h_direc, _ball->posY, _ball->posX + _ball->h_direc, _ball->posY + delta, Green);
                }
                else
                {
                    // MOVE horizontal right
                    // delete horizontal left
                    LCD_DrawLine(_ball->posX, _ball->posY, _ball->posX, _ball->posY + delta, Black);
                    // paint horizontal right
                    LCD_DrawLine(_ball->posX + _ball->h_direc + delta, _ball->posY, _ball->posX + _ball->h_direc + delta, _ball->posY + delta, Green);
                }
                // update x
                _ball->posX = _ball->posX + _ball->h_direc;
                // decrement horizontal speed
                _ball->h_speed--;
            }
            // if ball is moving VERTICALLY and Vertical speed is not Elapsed
            if (_ball->v_direc && _ball->v_speed)
            {
                isColliding = is_colliding(&ball, 'v');
                if (isColliding)
                {
                    // handle vertical collission..
                    // ..opposite reflection angle if hits roof (v_direc < 0)
                    if (_ball->v_direc < 0)
                    {
                        // play_sound(1890);
                        handle_paddle_collsion(&ball, &paddle2);
                        _ball->v_direc = -_ball->v_direc;
                    }
                    // ..handle reflection angle if hits paddle (v_direc > 0)
                    else
                    {
                        // play_sound(1125);
                        handle_paddle_collsion(&ball, &paddle1);
                        _ball->v_direc = -_ball->v_direc;
                    }
                    draw_score(score1, 1);   // just to partially handle score erasing on ball moving hover
                    draw_score(score2, 2);   // just to partially handle score erasing on ball moving hover
                    return;
                }
                if (_ball->v_direc < 0)
                {
                    // MOVE vertical up
                    // delete vertical down
                    LCD_DrawLine(_ball->posX, _ball->posY + delta, _ball->posX + delta, _ball->posY + delta, Black);
                    // paint vertical up
                    LCD_DrawLine(_ball->posX, _ball->posY + _ball->v_direc, _ball->posX + delta, _ball->posY + _ball->v_direc, Green);
                }
                else
                {
                    // MOVE vertical down
                    // delete vertical up
                    LCD_DrawLine(_ball->posX, _ball->posY, _ball->posX + delta, _ball->posY, Black);
                    // paint vertical down
                    LCD_DrawLine(_ball->posX, _ball->posY + _ball->v_direc + delta, _ball->posX + delta, _ball->posY + _ball->v_direc + delta, Green);
                }
                // update y
                _ball->posY = _ball->posY + _ball->v_direc;

                // player 2 scores
                if (_ball->posY > 278)
                {
                    is_game_over = increase_score(2);
                } // player 2 scores
                else if (_ball->posY < 40)
                {
                    is_game_over = increase_score(1);
                }
                if (is_game_over)
                {
                    game_over(is_game_over);
                    break;
                }
                _ball->v_speed--;
            }
        }
        // restart if cycle ended up with no collisions
        _ball->h_speed = previous_h_speed;
        _ball->v_speed = previous_v_speed;
        return;
    }
}
