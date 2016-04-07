/*
    Copyright 2015-2016 Clément Gallet <clement.gallet@ens-lyon.org>

    This file is part of libTAS.

    libTAS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libTAS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libTAS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sdlgamecontroller.h"
#include "inputs.h"
#include "../logging.h"
#include "../hook.h"
#include "../../shared/AllInputs.h"
#include "../../shared/tasflags.h"
#include <stdlib.h>

/* Do we have to generate controller events? */
bool sdl_controller_events = true;

SDL_JoystickID joyid[4] = {-1, -1, -1, -1};
const char joy_name[] = "XInput Controller";

/* Override */ SDL_bool SDL_IsGameController(int joystick_index)
{
    debuglog(LCF_SDL | LCF_JOYSTICK, __func__, " call with id ", joystick_index);
    if (joystick_index >= 0 && joystick_index < tasflags.numControllers)
        return SDL_TRUE;
    return SDL_FALSE;

}

/* Override */ SDL_GameController *SDL_GameControllerOpen(int joystick_index)
{
    debuglog(LCF_SDL | LCF_JOYSTICK, __func__, " call with id ", joystick_index);
    SDL_GameController* gc_id = new SDL_GameController;
    *gc_id = joystick_index;

    /* Save the opening of the game controller */
    joyid[joystick_index] = joystick_index;
    return gc_id;
}

/* Override */ const char *SDL_GameControllerNameForIndex(int joystick_index)
{
    debuglog(LCF_SDL | LCF_JOYSTICK, __func__, " call with id ", joystick_index);
    return joy_name;
}

/* Override */ const char *SDL_GameControllerName(SDL_GameController *gamecontroller)
{
    debuglog(LCF_SDL | LCF_JOYSTICK, __func__, " call with id ", *gamecontroller);
    return joy_name;
}

/* Override */ SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController* gamecontroller)
{
    debuglog(LCF_SDL | LCF_JOYSTICK, __func__, " call with id ", *gamecontroller);
    /* We simply return the same id */
    return (SDL_Joystick*) gamecontroller;
}

/* Override */ SDL_GameController* SDL_GameControllerFromInstanceID(SDL_JoystickID joy)
{
    debuglog(LCF_SDL | LCF_JOYSTICK | LCF_TODO, __func__, " call with id ", joy);
    SDL_GameController* gc_id = new SDL_GameController;
    *gc_id = 0;
	return gc_id;
}

/* Override */ SDL_bool SDL_GameControllerGetAttached(SDL_GameController *gamecontroller)
{
    debuglog(LCF_SDL | LCF_JOYSTICK | LCF_FRAME, __func__, " call with id ", *gamecontroller);
    if (joyid[*gamecontroller] != -1)
        return SDL_TRUE;
    return SDL_FALSE;
}

/* Override */ int SDL_GameControllerEventState(int state)
{
    debuglog(LCF_SDL | LCF_JOYSTICK | LCF_TODO, __func__, " call with state ", state);
    switch (state) {
        case 1:
            sdl_controller_events = true;
            return 1;
        case 0:
            sdl_controller_events = false;
            return 0;
        case -1:
            return sdl_controller_events;
        default:
            return state;
    }
}

/* Override */ void SDL_GameControllerUpdate(void)
{
    DEBUGLOGCALL(LCF_SDL | LCF_JOYSTICK | LCF_TODO);
}

/* Override */ Sint16 SDL_GameControllerGetAxis(SDL_GameController *gamecontroller,
                                          SDL_GameControllerAxis axis)
{
    debuglog(LCF_SDL | LCF_JOYSTICK | LCF_FRAME, __func__, " call with id ", *gamecontroller, " and axis ", axis);

    /* Check if controller is available */
    if (joyid[*gamecontroller] == -1)
        return 0;

    /* Check if axis is valid */
    if ((axis < 0) || (axis >= SDL_CONTROLLER_AXIS_MAX ))
        return 0;

    /* Return axis value */
    return ai.controller_axes[joyid[*gamecontroller]][axis];

}

/* Override */ Uint8 SDL_GameControllerGetButton(SDL_GameController *gamecontroller,
                                                 SDL_GameControllerButton button)
{
    debuglog(LCF_SDL | LCF_JOYSTICK | LCF_FRAME, __func__, " call with id ", *gamecontroller, " and button ", button);

    /* Check if controller is available */
    if (joyid[*gamecontroller] == -1)
        return 0;

    /* Check if button is valid */
    if ((button < 0) || (button >= SDL_CONTROLLER_BUTTON_MAX ))
        return 0;

    /* Return button value */
    return (ai.controller_buttons[joyid[*gamecontroller]] >> button) & 0x1;

}

/* Override */ void SDL_GameControllerClose(SDL_GameController *gamecontroller)
{
    debuglog(LCF_SDL | LCF_JOYSTICK | LCF_FRAME, __func__, " call with id ", *gamecontroller);

    joyid[*gamecontroller] = -1;
}
