// // delete ball
// int i =0;
// for(i=0; i<5; i++) {
//     LCD_DrawLine(ball.posX, ball.posY+i, ball.posX+4, ball.posY+i, Red);
// }
// ball.posX = ball.posX -3;
// ball.posY = ball.posY +1;
// // redraw ball
// for(i=0; i<5; i++) {
//     LCD_DrawLine(ball.posX, ball.posY+i, ball.posX+4, ball.posY+i, Blue);
// }

/**********************/
/*CASO 1*/
/**********************/
ball = {}
ball.posX = 8
ball.posY = 274
ball.h_speed = 3
ball.v_speed = 2
ball.h_direc = 1
ball.v_direc = 1


function is_colliding(ball, direction) {

    delta_h_position = ball.h_direc > 0 ? 4 : 0;
    delta_v_position = ball.v_direc > 0 ? 4 : 0;

    current_pos = direction == 'h' ? ball.posX+delta_h_position : ball.posY+delta_v_position

    v_limit = ball.v_direc > 0 ? 278 : 6; // (last or first valid pixel)
    h_limit = ball.h_direc > 0 ? 250 : 6; // (last or first valid pixel)
    
    switch( direction )
    {
        case 'h': 	 /* moving horizontally */
        return current_pos-h_limit==0 
        
        case 'v': 	 /* moving vertically */
        //if ball was moving up
        if (ball.v_direc<0) 
            return current_pos-v_limit==0  
        //if ball was moving down there is a collision only if paddle was under the ball
        /**@todo check paddle position */
        
        default:
            break; 
    }
        
}

    
function move_ball(ball) {

    is_game_over = false

    while (!is_game_over){
        
        //calcolo movimento
        // h_mov = ball.h_speed * ball.h_direc;
        // v_mov = ball.v_speed * ball.v_direc;
        // h_mov
        // v_mov
        previous_h_speed = ball.h_speed
        previous_v_speed = ball.v_speed

        
        longer_speed = ball.v_speed>=ball.h_speed ? ball.v_speed : ball.h_speed
        longer_speed
        // shorter_speed = ball.v_speed<=ball.h_speed ? ball.v_speed : ball.h_speed
        // shorter_speed
        //calcolo destinazione finale
        // delta_h_position = ball.h_direc > 0 ? 4 : 0;
        // delta_v_position = ball.v_direc > 0 ? 4 : 0;
        delta_h_position =4
        delta_v_position =4
    
        for(i=0; i<longer_speed; i++) {
            
            //if ball is moving HORIZONTALLY and Horizontal speed is not lapsed
            if (ball.h_direc && ball.h_speed){
                /**@todo HANDLE HORIZONTAL COLLISION WITH WALLS AND PADDLE**/
                isColliding = is_colliding(ball, 'h') 
                isColliding
                if(isColliding) {
                    ball.h_speed = previous_h_speed
                    ball.v_speed = previous_v_speed
                    ball.h_direc = -ball.h_direc
                    move_ball(ball)
                }
                if(ball.h_direc<0){
                    //MOVE horizontal left
                    console.log('MOVE horizontal left')
                    //delete horizontal right 
                    LCD_DrawLine(ball.posX+delta_h_position, ball.posY, ball.posX+delta_h_position, ball.posY+delta_v_position, 'Red');
                    //paint horizontal left
                    LCD_DrawLine(ball.posX+ball.h_direc, ball.posY, ball.posX+ball.h_direc, ball.posY+delta_v_position, 'Blue');
                } else {
                    //MOVE horizontal right
                    console.log('MOVE horizontal right')
                    //delete horizontal left 
                    LCD_DrawLine(ball.posX, ball.posY, ball.posX, ball.posY+delta_v_position, 'Red');
                    //paint horizontal right
                    LCD_DrawLine(ball.posX+ball.h_direc+delta_h_position, ball.posY, ball.posX+ball.h_direc+delta_h_position, ball.posY+delta_v_position, 'Blue');
                }
                //update x
                ball.posX = ball.posX+ball.h_direc
                //decrement horizontal speed
                ball.h_speed--
            }
            //if ball is moving VERTICALLY and Vertical speed is not Elapsed
            if (ball.v_direc && ball.v_speed){
                isColliding = is_colliding(ball, 'v') 
                isColliding
                if(isColliding) { 
                    ball.h_speed = previous_h_speed
                    ball.v_speed = previous_v_speed
                    ball.v_direc = -ball.v_direc
                    move_ball(ball)
                }
                if(ball.v_direc<0){
                    //MOVE vertical up
                    console.log('MOVE vertical up')
                    //delete vertical down 
                    LCD_DrawLine(ball.posX, ball.posY+delta_v_position, ball.posX+delta_h_position, ball.posY+delta_v_position, 'Red');
                    //paint vertical up
                    LCD_DrawLine(ball.posX, ball.posY+ball.v_direc, ball.posX+delta_v_position, ball.posY+ball.v_direc, 'Blue');
                } else {
                    //MOVE vertical down
                    console.log('MOVE vertical down')
                    //delete vertical up 
                    LCD_DrawLine(ball.posX, ball.posY, ball.posX+delta_h_position, ball.posY, 'Red');
                    //paint vertical down
                    LCD_DrawLine(ball.posX, ball.posY+ball.v_direc+delta_v_position, ball.posX+delta_h_position, ball.posY+ball.v_direc+delta_v_position, 'Blue');
                }
                //update y
                ball.posY = ball.posY+ball.v_direc

                is_game_over = ball.posY > 278
                //decrement vertical speed
                ball.v_speed--
            }

            //richiamo la funzione se il ciclo è finito senza collisioni
            if (longer_speed-1 == i) {
                ball.h_speed = previous_h_speed
                ball.v_speed = previous_v_speed
                move_ball(ball)
            }
        
        
        }


    }

}

move_ball(ball)



// final_x_dest = ball.posX + h_mov;
// final_y_dest = ball.posY + v_mov;
// final_x_dest
// final_y_dest
// delta_h_position
// delta_v_position
// //collision a DX se h_collision >= 0
// //collision a SX se h_collision =< 0
// h_collision = final_x_dest + delta_h_position - h_limit
// h_collision
// //collision a SUD se v_collision >= 0
// //collision a NORD se v_collision =< 0
// v_collision = final_y_dest + delta_v_position - v_limit
// v_collision

// //check horizonatal collision 
// if(ball.h_direc) { //if is moving horizontally
//     if(ball.h_direc<0) { //if direction is west
//         ball.h_speed = 
//     }
// }

//check vertical collision


// /**********************/
// /*CASO 2*/
// /**********************/
// ball = {}
// ball.posX = 2
// ball.posY = 3
// ball.h_speed = 3
// ball.v_speed = 1
// ball.h_direc = -1
// ball.v_direc = 1

// v_limit = ball.v_direc > 0 ? 278 : 1; //1
// h_limit = ball.h_direc > 0 ? 255 : 1; //278
// h_limit
// v_limit

// //calcolo movimento
// h_mov = ball.h_speed * ball.h_direc;
// v_mov = ball.v_speed * ball.v_direc;
// h_mov
// v_mov
// //calcolo destinazione finale
// delta_h_position = ball.h_direc > 0 ? 4 : 0;
// delta_v_position = ball.v_direc > 0 ? 4 : 0;
// final_x_dest = ball.posX + h_mov;
// final_y_dest = ball.posY + v_mov;
// final_x_dest
// final_y_dest
// delta_h_position
// delta_v_position
// //collision a DX se h_collision >= 0
// //collision a SX se h_collision =< 0
// h_collision = final_x_dest + delta_h_position - h_limit
// h_collision
// //collision a SUD se v_collision >= 0
// //collision a NORD se v_collision =< 0
// v_collision = final_y_dest + delta_v_position - v_limit
// v_collision


// /**********************/
// /*CASO 3*/
// /**********************/
// ball = {}
// ball.posX = 250
// ball.posY = 3
// ball.h_speed = 3
// ball.v_speed = 1
// ball.h_direc = 1
// ball.v_direc = 1

// v_limit = ball.v_direc > 0 ? 278 : 1; //1
// h_limit = ball.h_direc > 0 ? 255 : 1; //278
// h_limit
// v_limit

// //calcolo movimento
// h_mov = ball.h_speed * ball.h_direc;
// v_mov = ball.v_speed * ball.v_direc;
// h_mov
// v_mov
// //calcolo destinazione finale
// delta_h_position = ball.h_direc > 0 ? 4 : 0;
// delta_v_position = ball.v_direc > 0 ? 4 : 0;
// final_x_dest = ball.posX + h_mov;
// final_y_dest = ball.posY + v_mov;
// final_x_dest
// final_y_dest
// delta_h_position
// delta_v_position

// //collision a DX se h_collision >= 0
// //collision a SX se h_collision =< 0
// h_collision = final_x_dest + delta_h_position - h_limit
// h_collision
// //collision a SUD se v_collision >= 0
// //collision a NORD se v_collision =< 0
// v_collision = final_y_dest + delta_v_position - v_limit
// v_collision



// /**********************/
// /*CASO 4*/
// /**********************/
// ball = {}
// ball.posX = 4
// ball.posY = 320 - 48 //272
// ball.h_speed = 1
// ball.v_speed = 3
// ball.h_direc = -1
// ball.v_direc = 1

// v_limit = ball.v_direc > 0 ? 278 : 1; //1
// h_limit = ball.h_direc > 0 ? 255 : 1; //278
// h_limit
// v_limit

// //calcolo movimento
// h_mov = ball.h_speed * ball.h_direc;
// v_mov = ball.v_speed * ball.v_direc;
// h_mov
// v_mov
// //calcolo destinazione finale
// delta_h_position = ball.h_direc > 0 ? 4 : 0;
// delta_v_position = ball.v_direc > 0 ? 4 : 0;
// final_x_dest = ball.posX + h_mov;
// final_y_dest = ball.posY + v_mov;
// final_x_dest
// final_y_dest
// delta_h_position
// delta_v_position

// //collision a DX se h_collision >= 0
// //collision a SX se h_collision =< 0
// h_collision = final_x_dest + delta_h_position - h_limit
// h_collision
// //collision a SUD se v_collision >= 0
// //collision a NORD se v_collision =< 0
// v_collision = final_y_dest + delta_v_position - v_limit
// v_collision



// /**********************/
// /*CASO 5*/
// /**********************/
// ball = {}
// ball.posX = 4
// ball.posY = 3
// ball.h_speed = 3
// ball.v_speed = 0
// ball.h_direc = 0
// ball.v_direc = -1

// v_limit = ball.v_direc == 0 ? 0 : ball.v_direc > 0 ? 278 : 1; //1
// h_limit = ball.h_direc == 0 ? 0 : ball.h_direc > 0 ? 255 : 1; //278
// h_limit
// v_limit

// //calcolo movimento
// h_mov = ball.h_speed * ball.h_direc;
// v_mov = ball.v_speed * ball.v_direc;
// h_mov
// v_mov
// //calcolo destinazione finale
// delta_h_position = ball.h_direc > 0 ? 4 : 0;
// delta_v_position = ball.v_direc > 0 ? 4 : 0;
// final_x_dest = ball.posX + h_mov;
// final_y_dest = ball.posY + v_mov;
// final_x_dest
// final_y_dest
// delta_h_position
// delta_v_position

// //collision a DX se h_collision >= 0
// //collision a SX se h_collision =< 0
// h_collision = final_x_dest + delta_h_position - h_limit
// h_collision //collision a SUD se v_collision >= 0
// //collision a NORD se v_collision =< 0
// v_collision = final_y_dest + delta_v_position - v_limit
// v_collision


function LCD_DrawLine(x0, y0, x1, y1, color) {
    action = color=='Blue' ? "Painting" : "Deleting"
    console.log(`${action} - the line from ${x0}-${y0} to ${x1}-${y1}`)
}

// longer_speed = ball.v_speed>=ball.h_speed ? ball.v_speed : ball.h_speed
// shorter_speed = ball.v_speed<=ball.h_speed ? ball.v_speed : ball.h_speed
// longer_speed
// shorter_speed
// for(i=0; i<longer_speed; i++){
//     i
//     LCD_DrawLine(ball.posX, ball.posY+i, ball.posX, ball.posY, 'Blue');
//     for(j=0; j<shorter_speed; j++) {
//         j
//         LCD_DrawLine(ball.posX, ball.posY+j, ball.posX, ball.posY, 'Red');
//     }
// }


