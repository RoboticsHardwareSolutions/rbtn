#pragma once
enum state_machine
{
    OCS_INIT     = 0,
    OCS_DOWN     = 1,  // button is down
    OCS_UP       = 2,  // button is up
    OCS_COUNT    = 3,
    OCS_PRESS    = 6,  // button is hold down
    OCS_PRESSEND = 7,
};

typedef bool (*button_state)(void);
typedef void (*callback_function)(void);
typedef void (*parameterized_callback_function)(void*);

typedef struct
{
    enum state_machine state;
    button_state       button_state_func;
    unsigned int       debounce_ms;     // number of msecs for debounce times.
    unsigned int       click_ms;        // number of msecs before a click is detected.
    unsigned int       press_ms;        // number of msecs before a long button press is detected
    int                button_pressed;  // this is the level of the input pin when the button is pressed.
                                        // LOW if the button connects the input pin to GND when pressed.
                                        // HIGH if the button connects the input pin to VCC when pressed.
    int           debounced_pin_level;
    int           last_debounce_pin_level;  // used for pin debouncing
    unsigned long last_debounce_time;       // millis()
    unsigned long now;                      // millis()
    unsigned long start_time;               // start time of current activeLevel change
    int           n_clicks;                 // count the number of clicks with this variable
    int           max_clicks;  // max number (1, 2, multi=3) of clicks of interest by registration of event functions.
    unsigned int  long_press_interval_ms;       // interval in msecs between calls of the DuringLongPress event
    unsigned long last_during_long_press_time;  // used to produce the DuringLongPress interval

    // These variables will hold functions acting as event source.
    callback_function               click_func;
    parameterized_callback_function param_click_func;
    void*                           click_func_param;

    callback_function               double_click_func;
    parameterized_callback_function param_double_click_func;
    void*                           double_click_func_param;

    callback_function               multi_click_func;
    parameterized_callback_function param_multi_click_func;
    void*                           multi_click_func_param;

    callback_function               long_press_start_func;
    parameterized_callback_function param_long_press_start_func;
    void*                           long_press_start_func_param;

    callback_function               long_press_stop_func;
    parameterized_callback_function param_long_press_stop_func;
    void*                           long_press_stop_func_param;

    callback_function               during_long_press_func;
    parameterized_callback_function param_during_long_press_func;
    void*                           during_long_press_func_param;
} rbtn;