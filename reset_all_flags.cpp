#include "header.h"

void reset_all_flags(void)
{
    // function to reset all flags
    
    i_am_going_forward = false;
    i_am_going_backward = false;
    i_am_turning_left = false;
    i_am_turning_right = false;
    i_am_detecting_colour = false;
    i_am_detecting_red_colour = false;
    i_am_detecting_blue_colour = false;
}
