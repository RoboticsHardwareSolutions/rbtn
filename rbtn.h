#pragma once

#include "stdbool.h"
#include "rbtn_def.h"

void rbtn_init(rbtn* btn, const bool active_low, button_state state_func);

/** set the number of millisec that have to pass by before a click is assumed stable */
void rbtn_set_debounce_ms(rbtn* btn, const unsigned int ms);

/** set # millisec after press is assumed */
void rbtn_set_press_ms(rbtn* btn, const unsigned int ms);

/**
 * set interval in msecs between calls of the DuringLongPress event.
 * 0 ms is the fastest events calls.
 */
void rbtn_set_long_press_interval(rbtn* btn, const unsigned int ms);

/**
 * Attach an event to be called when a single click is detected.
 * @param cb This function will be called when the event has been detected.
 */
void rbtn_attach_click(rbtn* btn, callback_function cb);
void rbtn_attach_click_param(rbtn* btn, parameterized_callback_function cb, void* parameter);

/**
 * Attach an event to be called after a double click is detected.
 * @param cb This function will be called when the event has been detected.
 */
void rbtn_attach_double_click(rbtn* btn, callback_function cb);
void rbtn_attach_double_click_param(rbtn* btn, parameterized_callback_function cb, void* parameter);

/**
 * Attach an event to be called after a multi click is detected.
 * @param cb This function will be called when the event has been detected.
 */
void rbtn_attach_multi_click(rbtn* btn, callback_function cb);
void rbtn_attach_multi_click_param(rbtn* btn, parameterized_callback_function cb, void* parameter);

/**
 * Attach an event to fire when the button is pressed and held down.
 * @param cb
 */
void rbtn_attach_long_press_start(rbtn* btn, callback_function cb);
void rbtn_attach_long_press_start_param(rbtn* btn, parameterized_callback_function cb, void* parameter);

/**
 * Attach an event to fire as soon as the button is released after a long press.
 * @param cb
 */
void rbtn_attach_long_press_stop(rbtn* btn, callback_function cb);
void rbtn_attach_long_press_stop_param(rbtn* btn, parameterized_callback_function cb, void* parameter);

/**
 * Attach an event to fire periodically while the button is held down.
 * The period of calls is set by rbtn_set_long_press_interval(ms).
 * @param cb
 */
void rbtn_attach_during_long_press(rbtn* btn, callback_function cb);
void rbtn_attach_during_long_press_param(rbtn* btn, parameterized_callback_function cb, void* parameter);

/**
 * @brief Call this function every time the input level has changed.
 * Using this function no digital input pin is checked because the current
 * level is given by the parameter.
 * Run the finite state machine (FSM) using the given level.
 */
void rbtn_tick(rbtn* btn);

/**
 * Reset the button state machine.
 */
void rbtn_reset(rbtn* btn);

/**
 * return number of clicks in any case: single or multiple clicks
 */
int rbtn_get_number_clicks(rbtn* btn);

/**
 * @return true if we are currently handling button press flow
 * (This allows power sensitive applications to know when it is safe to power down the main CPU)
 */
bool rbtn_is_idle(rbtn* btn);

/**
 * @return true when a long press is detected
 */
bool rbtn_is_long_pressed(rbtn* btn);

/**
 * @return debounced pin level
 */

int rbtn_debounced_value(rbtn* btn);

/**
 * @brief Use this function in the during_long_press and long_press_stop events to get the time since the button was
 * pressed.
 * @return milliseconds from the start of the button press.
 */
unsigned long rbtn_get_pressed_ms(rbtn* btn);
