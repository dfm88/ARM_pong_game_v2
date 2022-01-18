
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

void draw_ball(uint16_t x, uint16_t y)
{
    int i = 0;
    ball.posX = x;
    ball.posY = y;
    for (i = 0; i < 5; i++)
    {
        LCD_DrawLine(ball.posX, ball.posY + i, ball.posX + 4, ball.posY + i, Green);
    }
}

void initialize_ball()
{
    NVIC_DisableIRQ(EINT0_IRQn); /* enable Button interrupts			*/
    ball.posX = 230;
    ball.posY = 160;
    ball.h_direc = 1;
    ball.v_direc = 1;
    ball.h_speed = 2;
    ball.v_speed = 2;

    draw_ball(ball.posX, ball.posY);
}

void delete_ball()
{
    int i = 0;
    for (i = 0; i < 5; i++)
    {
        LCD_DrawLine(ball.posX, ball.posY + i, ball.posX + 4, ball.posY + i, Black);
    }
}

void GAME_init(void)
{
    int i = 0;

    initialize_ball();

    // draw paddle
    paddle.posX = 110;
    paddle.posY = 277;
    for (i = 0; i < 10; i++)
    {
        LCD_DrawLine(paddle.posX, paddle.posY + i, paddle.posX + 49, paddle.posY + i, Green);
    }

    // draw walls
    for (i = 0; i < 5; i++)
    {
        // left wall
        LCD_DrawLine(i, 0, i, 319, Red);
        // right wall
        LCD_DrawLine(235 + i, 0, 235 + i, 319, Red);
    }

    draw_score(score, 1);
    draw_score(score, 2);
    GUI_Text(40, 140, (unsigned char *)"Press Key1 to start", White, Black, 1);
}

void increase_score()
{
    // increase score
    if (score >= 100)
        score += 10;
    else
        score += 5;

    draw_score(score, 1);
    // increase record
    if (score > record)
    {
        record = score;
        draw_record(record);
    }
}

void game_over()
{
    NVIC_EnableIRQ(EINT0_IRQn); /* enable Button interrupts			*/
    score = 0;
    // delete score
    GUI_Text(7, 160, (unsigned char *)"        ", Black, Black, 0);
    draw_score(score, 1);
    GUI_Text(80, 140, (unsigned char *)"You lose", White, Black, 0);
    GUI_Text(38, 210, (unsigned char *)"Press Int0 to continue", White, Black, 0);
    pause_game();
    delete_ball();
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
    GUI_Text(40, 140, (unsigned char *)"                   ", Black, Black, 0);
    ADC_init(); /* ADC Initialization	for paddle movement	*/
    enable_timer(0);
}

void prepare_restart_game()
{
    // delete "You lose"
    GUI_Text(80, 140, (unsigned char *)"        ", Black, Black, 0);
    // delete "Press Int0 to continue"
    GUI_Text(38, 210, (unsigned char *)"                      ", Black, Black, 0);
    initialize_ball();
    GUI_Text(40, 140, (unsigned char *)"Press Key1 to start", White, Black, 0);
}

void restart_game()
{
    // delete "Press Key1 to start"
    GUI_Text(40, 140, (unsigned char *)"                   ", White, Black, 0);
    resume_game();
}

void play_sound(uint16_t k)
{
    reset_timer(1);
    init_timer(1, 0, 0, 3, k);
    enable_timer(1);
}

int is_colliding(struct struct_ball ball, int8_t direction)
{

    uint8_t delta_h, delta_v;
    int v_limit, h_limit, current_pos;

    delta_h = ball.h_direc > 0 ? 4 : 0;
    delta_v = ball.v_direc > 0 ? 4 : 0;

    // 0x68 = h    0x76 = v
    current_pos = direction == 'h' ? ball.posX + delta_h : ball.posY + delta_v;

    v_limit = ball.v_direc > 0 ? 276 : 5; // (last or first valid pixel)
    h_limit = ball.h_direc > 0 ? 234 : 5; // (last or first valid pixel)

    switch (direction)
    {
    case 'h': /* moving horizontally */
        return current_pos - h_limit == 0;

    case 'v': /* moving vertically */
        // if ball was moving up
        if (ball.v_direc < 0)
            return current_pos - v_limit == 0;
        // if ball was moving down, check paddle position
        else
        {
            // is colliding with paddle high && paddle is under the ball
            if (current_pos - v_limit == 0)
            {
                if ((paddle.posX - 4) <= ball.posX)
                {
                    if (ball.posX <= (paddle.posX + 49))
                    {
                        increase_score();
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

void handle_paddle_collsion()
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
    if (ball.posX <= (paddle.posX + 22))
    {
        ball.h_direc = -1;
        // 1-13
        if (ball.posX <= (paddle.posX + 12))
        {
            // 1-8
            if (ball.posX <= (paddle.posX + 7))
            {
                // 1-3
                if (ball.posX <= paddle.posX + 2)
                {
                    ball.v_speed = 1;
                    ball.h_speed = 4;
                    return;
                }
                // 4-8
                else
                {
                    ball.v_speed = 2;
                    ball.h_speed = 3;
                    return;
                }
            }
            // 9-13
            else
            {
                ball.v_speed = 3;
                ball.h_speed = 2;
                return;
            }
        }
        // 14-23
        else
        {
            // 14-18
            if (ball.posX <= paddle.posX + 17)
            {
                ball.v_speed = 4;
                ball.h_speed = 1;
                return;
            }
            // 19-23
            else
            {
                ball.v_speed = 5;
                ball.h_speed = 1;
                return;
            }
        }
    }
    /** 24-50 **/
    else
    {
        ball.h_direc = 1;
        // 39-50
        if (ball.posX >= paddle.posX + 38)
        {
            // 44-50
            if (ball.posX >= paddle.posX + 43)
            {
                ball.v_speed = 1;
                ball.h_speed = 4;
                return;
            }
            // 39-43
            else
            {
                ball.v_speed = 2;
                ball.h_speed = 3;
                return;
            }
        }
        // 24-38
        else
        {
            // 29-38
            if (ball.posX >= paddle.posX + 28)
            {
                // 34-38
                if (ball.posX >= paddle.posX + 33)
                {
                    ball.v_speed = 3;
                    ball.h_speed = 2;
                    return;
                }
                // 29-33
                else
                {
                    ball.v_speed = 4;
                    ball.h_speed = 1;
                    return;
                }
            }
            // 24-28
            else
            {
                ball.v_speed = 5;
                ball.h_speed = 1;
                return;
            }
        }
    }
}

void move_paddle(unsigned short mov)
{
    uint8_t increment = 0;
    int8_t dir = 0;
    uint8_t i = 0;
    uint8_t delta_del;
    uint8_t delta_draw;
    // mov == 0-3 move left
    // mov == 4   hold
    // mov == 5-8 move right

    dir = mov > 4 ? 1 : -1;        // -1
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
        if ((dir > 0 && paddle.posX < 188) || (dir < 0 && paddle.posX > 0))
        {
            // delete line
            LCD_DrawLine(paddle.posX + (delta_del /* + i*dir*/), paddle.posY, paddle.posX + (delta_del /* + i*dir*/), paddle.posY + 9, Black);
            // change paddle position
            paddle.posX = paddle.posX + dir;
            // draw line
            LCD_DrawLine(paddle.posX + (delta_draw /* + i*dir*/), paddle.posY, paddle.posX + (delta_draw /* + i*dir*/), paddle.posY + 9, Green);
        }
        else
        {
            return;
        }
    }
    return;
}

void move_ball()
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
        previous_h_speed = ball.h_speed;
        previous_v_speed = ball.v_speed;

        longer_speed = ball.v_speed >= ball.h_speed ? ball.v_speed : ball.h_speed;

        for (i = 0; i < longer_speed; i++)
        {

            // if ball is moving HORIZONTALLY and Horizontal speed is not lapsed
            if (ball.h_direc && ball.h_speed)
            {
                isColliding = is_colliding(ball, 'h');

                if (isColliding)
                {
                    play_sound(1890);
                    ball.h_speed = previous_h_speed;
                    ball.v_speed = previous_v_speed;
                    ball.h_direc = -ball.h_direc;
                    draw_score(score, 1);   // just to partially handle score erasing on ball moving hover
                    draw_score(score, 2);   // just to partially handle score erasing on ball moving hover
                    return;
                }
                if (ball.h_direc < 0)
                {
                    // MOVE horizontal left
                    // delete horizontal right
                    LCD_DrawLine(ball.posX + delta, ball.posY, ball.posX + delta, ball.posY + delta, Black);
                    // paint horizontal left
                    LCD_DrawLine(ball.posX + ball.h_direc, ball.posY, ball.posX + ball.h_direc, ball.posY + delta, Green);
                }
                else
                {
                    // MOVE horizontal right
                    // delete horizontal left
                    LCD_DrawLine(ball.posX, ball.posY, ball.posX, ball.posY + delta, Black);
                    // paint horizontal right
                    LCD_DrawLine(ball.posX + ball.h_direc + delta, ball.posY, ball.posX + ball.h_direc + delta, ball.posY + delta, Green);
                }
                // update x
                ball.posX = ball.posX + ball.h_direc;
                // decrement horizontal speed
                ball.h_speed--;
            }
            // if ball is moving VERTICALLY and Vertical speed is not Elapsed
            if (ball.v_direc && ball.v_speed)
            {
                isColliding = is_colliding(ball, 'v');
                if (isColliding)
                {
                    // handle vertical collission..
                    // ..opposite reflection angle if hits roof (v_direc < 0)
                    if (ball.v_direc < 0)
                    {
                        play_sound(1890);
                        ball.h_speed = previous_h_speed;
                        ball.v_speed = previous_v_speed;
                        ball.v_direc = -ball.v_direc;
                    }
                    // ..handle reflection angle if hits paddle (v_direc > 0)
                    else
                    {
                        play_sound(1125);
                        handle_paddle_collsion();
                        ball.v_direc = -ball.v_direc;
                    }
                    draw_score(score, 1);   // just to partially handle score erasing on ball moving hover
                    draw_score(score, 2);   // just to partially handle score erasing on ball moving hover
                    return;
                }
                if (ball.v_direc < 0)
                {
                    // MOVE vertical up
                    // delete vertical down
                    LCD_DrawLine(ball.posX, ball.posY + delta, ball.posX + delta, ball.posY + delta, Black);
                    // paint vertical up
                    LCD_DrawLine(ball.posX, ball.posY + ball.v_direc, ball.posX + delta, ball.posY + ball.v_direc, Green);
                }
                else
                {
                    // MOVE vertical down
                    // delete vertical up
                    LCD_DrawLine(ball.posX, ball.posY, ball.posX + delta, ball.posY, Black);
                    // paint vertical down
                    LCD_DrawLine(ball.posX, ball.posY + ball.v_direc + delta, ball.posX + delta, ball.posY + ball.v_direc + delta, Green);
                }
                // update y
                ball.posY = ball.posY + ball.v_direc;

                is_game_over = ball.posY > 278;
                if (is_game_over)
                {
                    game_over();
                    break;
                }
                // decrement vertical speed
                ball.v_speed--;
            }
        }
        // restart if cycle ended up with no collisions
        ball.h_speed = previous_h_speed;
        ball.v_speed = previous_v_speed;
        return;
    }
}
