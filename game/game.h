#include "LPC17xx.h"

static int score1 = 3;
static int score2 = 0;
static uint8_t is_game_over;
struct struct_ball
{
	uint8_t h_speed;
	uint8_t v_speed;
	int8_t h_direc;
	int8_t v_direc;
	uint16_t posX;
	uint16_t posY;
};

struct struct_paddle
{
	int8_t h_direc;
	uint16_t posX;
	uint16_t posY;
};

extern struct struct_ball ball;
extern struct struct_paddle paddle1;
extern struct struct_paddle paddle2;

void GAME_init(void);
void handle_paddle_collsion(struct struct_ball *_ball, struct struct_paddle *_paddle);
void move_paddle(unsigned short, struct struct_paddle *_paddle, uint8_t);
void game_over(int);
uint8_t increase_score(int);
void pause_game(void);
void resume_game(void);
void restart_game(void);
void initialize_ball(struct struct_ball *_ball);
void draw_ball(uint16_t, uint16_t, struct struct_ball *_ball);
void delete_ball(struct struct_ball *_ball);
void start_game(void);
void prepare_restart_game(void);

void move_ball(struct struct_ball *_ball);

int is_colliding(struct struct_ball *_ball, int8_t direction);
