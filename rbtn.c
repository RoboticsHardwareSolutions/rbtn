#include "rbtn.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"

#define LOW 0
#define HIGH 1
#define MAX(a, b) (a) >= (b) ? (a) : (b)

/**
 * Run the finite state machine (FSM) using the given level.
 */
static void fsm(rbtn* btn, bool active_level);

/**  Advance to a new state */
static void new_state(rbtn* btn, enum state_machine next_state);

void rbtn_init(rbtn* btn, const bool active_low, button_state state_func)
{
    memset(btn, 0, sizeof(rbtn));
    btn->state                   = OCS_INIT;
    btn->debounce_ms             = 50;    // number of msecs for debounce times.
    btn->click_ms                = 400;   // number of msecs before a click is detected.
    btn->press_ms                = 800;   // number of msecs before a long button press is detected
    btn->long_press_interval_ms  = 3000;  // number of msecs before a long button press is detected
    btn->debounced_pin_level     = -1;
    btn->last_debounce_pin_level = -1;  // used for pin debouncing
    btn->max_clicks        = 1;  // max number (1, 2, multi=3) of clicks of interest by registration of event functions.
    btn->button_state_func = state_func;

    if (active_low)
    {
        // the button connects the input pin to GND when pressed.
        btn->button_pressed = LOW;
    }
    else
    {
        // the button connects the input pin to VCC when pressed.
        btn->button_pressed = HIGH;
    }
}

void rbtn_set_debounce_ms(rbtn* btn, const unsigned int ms)
{
    btn->debounce_ms = ms;
}

void rbtn_set_press_ms(rbtn* btn, const unsigned int ms)
{
    btn->press_ms = ms;
}

void rbtn_attach_click(rbtn* btn, callback_function cb)
{
    btn->click_func = cb;
}

// save function for parameterized click event
void rbtn_attach_click_param(rbtn* btn, parameterized_callback_function cb, void* parameter)
{
    btn->param_click_func = cb;
    btn->click_func_param = parameter;
}

void rbtn_attach_double_click(rbtn* btn, callback_function cb)
{
    btn->double_click_func = cb;
    btn->max_clicks        = MAX(btn->max_clicks, 2);
}

void rbtn_attach_double_click_param(rbtn* btn, parameterized_callback_function cb, void* parameter)
{
    btn->param_double_click_func = cb;
    btn->double_click_func_param = parameter;
    btn->max_clicks              = MAX(btn->max_clicks, 2);
}

void rbtn_attach_multi_click(rbtn* btn, callback_function cb)
{
    btn->multi_click_func = cb;
    btn->max_clicks       = MAX(btn->max_clicks, 100);
}

void rbtn_attach_multi_click_param(rbtn* btn, parameterized_callback_function cb, void* parameter)
{
    btn->param_multi_click_func = cb;
    btn->multi_click_func_param = parameter;
    btn->max_clicks             = MAX(btn->max_clicks, 100);
}

void rbtn_attach_long_press_start(rbtn* btn, callback_function cb)
{
    btn->long_press_start_func = cb;
}

void rbtn_attach_long_press_start_param(rbtn* btn, parameterized_callback_function cb, void* parameter)
{
    btn->param_long_press_start_func = cb;
    btn->long_press_start_func_param = parameter;
}

void rbtn_attach_long_press_stop(rbtn* btn, callback_function cb)
{
    btn->long_press_stop_func = cb;
}

void rbtn_attach_long_press_stop_param(rbtn* btn, parameterized_callback_function cb, void* parameter)
{
    btn->param_long_press_stop_func = cb;
    btn->long_press_stop_func_param = parameter;
}

void rbtn_attach_during_long_press(rbtn* btn, callback_function cb)
{
    btn->during_long_press_func = cb;
}

void rbtn_attach_during_long_press_param(rbtn* btn, parameterized_callback_function cb, void* parameter)
{
    btn->param_during_long_press_func = cb;
    btn->during_long_press_func_param = parameter;
}

void rbtn_reset(rbtn* btn)
{
    btn->state      = OCS_INIT;
    btn->n_clicks   = 0;
    btn->start_time = 0;
}

int rbtn_get_number_clicks(rbtn* btn)
{
    return btn->n_clicks;
}

bool rbtn_is_idle(rbtn* btn)
{
    return btn->state == OCS_INIT;
}

bool rbtn_is_long_pressed(rbtn* btn)
{
    return btn->state == OCS_PRESS;
}

int rbtn_debounced_value(rbtn* btn)
{
    return btn->debounced_pin_level;
}

unsigned long rbtn_get_pressed_ms(rbtn* btn)
{
    return (xTaskGetTickCount() * (1000 / configTICK_RATE_HZ) - btn->start_time);
}

void rbtn_set_long_press_interval(rbtn* btn, const unsigned int ms)
{
    btn->long_press_interval_ms = ms;
}

/**
 * @brief Debounce input pin level for use in SpesialInput.
 */
static int debounce(rbtn* btn, const int value)
{
    btn->now = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);  // current (relative) time in msecs.
    if (btn->last_debounce_pin_level == value)
    {
        if (btn->now - btn->last_debounce_time >= btn->debounce_ms)
            btn->debounced_pin_level = value;
    }
    else
    {
        btn->last_debounce_time      = btn->now;
        btn->last_debounce_pin_level = value;
    }
    return btn->debounced_pin_level;
}

void rbtn_tick(rbtn* btn)
{
    if (!btn->button_state_func)
        return;
    int  actual_value = (int) btn->button_state_func();
    bool value        = debounce(btn, actual_value) == btn->button_pressed;
    fsm(btn, value);
}

/**
 *  @brief Advance to a new state and save the last one to come back in cas of bouncing detection.
 */
void new_state(rbtn* btn, enum state_machine next_state)
{
    btn->state = next_state;
}

/**
 * @brief Run the finite state machine (FSM) using the given level.
 */
void fsm(rbtn* btn, bool active_level)
{
    unsigned long wait_time = (btn->now - btn->start_time);

    // Implementation of the state machine
    switch (btn->state)
    {
    case OCS_INIT:
        // waiting for level to become active.
        if (active_level)
        {
            new_state(btn, OCS_DOWN);
            btn->start_time = btn->now;  // remember starting time
            btn->n_clicks   = 0;
        }  // if
        break;

    case OCS_DOWN:
        // waiting for level to become inactive.

        if (!active_level)
        {
            new_state(btn, OCS_UP);
            btn->start_time = btn->now;  // remember starting time
        }
        else if (wait_time > btn->press_ms)
        {
            if (btn->long_press_start_func)
                btn->long_press_start_func();
            if (btn->param_long_press_start_func)
                btn->param_long_press_start_func(btn->long_press_start_func_param);
            new_state(btn, OCS_PRESS);
        }  // if
        break;

    case OCS_UP:
        // level is inactive

        // count as a short button down
        btn->n_clicks++;
        new_state(btn, OCS_COUNT);
        break;

    case OCS_COUNT:
        // debounce time is over, count clicks

        if (active_level)
        {
            // button is down again
            new_state(btn, OCS_DOWN);
            btn->start_time = btn->now;  // remember starting time
        }
        else if ((wait_time >= btn->click_ms) || (btn->n_clicks == btn->max_clicks))
        {
            // now we know how many clicks have been made.

            if (btn->n_clicks == 1)
            {
                // this was 1 click only.
                if (btn->click_func)
                    btn->click_func();
                if (btn->param_click_func)
                    btn->param_click_func(btn->click_func_param);
            }
            else if (btn->n_clicks == 2)
            {
                // this was a 2 click sequence.
                if (btn->double_click_func)
                    btn->double_click_func();
                if (btn->param_double_click_func)
                    btn->param_double_click_func(btn->double_click_func_param);
            }
            else
            {
                // this was a multi click sequence.
                if (btn->multi_click_func)
                    btn->multi_click_func();
                if (btn->param_multi_click_func)
                    btn->param_multi_click_func(btn->multi_click_func_param);
            }  // if

            rbtn_reset(btn);
        }  // if
        break;

    case OCS_PRESS:
        // waiting for pin being release after long press.

        if (!active_level)
        {
            new_state(btn, OCS_PRESSEND);
        }
        else
        {
            // still the button is pressed
            if ((btn->now - btn->last_during_long_press_time) >= btn->long_press_interval_ms)
            {
                if (btn->during_long_press_func)
                    btn->during_long_press_func();
                if (btn->param_during_long_press_func)
                    btn->param_during_long_press_func(btn->during_long_press_func_param);
                btn->last_during_long_press_time = btn->now;
            }
        }  // if
        break;

    case OCS_PRESSEND:
        // button was released.

        if (btn->long_press_stop_func)
            btn->long_press_stop_func();
        if (btn->param_long_press_stop_func)
            btn->param_long_press_stop_func(btn->long_press_stop_func_param);
        rbtn_reset(btn);
        break;

    default:
        // unknown state detected -> reset state machine
        new_state(btn, OCS_INIT);
        break;
    }  // if
}
