#include <Arduino.h>
#include "StepperController.h"
#include "Planner.h"
#include "JoystickInterface.h"
#include "DrawingObjects.h"
#include "Settings.h"

// DEFINITIONS:
void print_current_position();

sys_state state = {IDLE, micros()};
Servo pen_controller;
StepperController stepper_c = StepperController(&pen_controller);
int current_steps_mask = 0;
int current_direction_mask = 0;
int target[N_INSTRUCTIONS] = {0, 0, 0};
const int *current_position = stepper_c.get_steps_count();
segment_plan seg_p = {0};
Planner pl = Planner(&stepper_c, &seg_p);
int current_drawing = 0;

// USER INTERFACE OBJECTS
Encoder encoder_a = Encoder(ENCODER_A_BIT_0, ENCODER_A_BIT_1, ENCODER_A_BUTTON,'A');
Encoder encoder_b = Encoder(ENCODER_B_BIT_0, ENCODER_B_BIT_1, ENCODER_B_BUTTON,'B');
unsigned long test_timer = 0;
int pen_state = PEN_OFF;


void state_handler(int current_steps_mask, int pen_state, StepperController *stepper_c)
{
    // if movement was deteced
    if (current_steps_mask ||  pen_state)
    {

        stepper_c->set_enable(true);
        if (state.sys_mode == IDLE)
        {
            state.sys_mode = MOVE;
            // toggle_led(true); // turn led on
        }
        else if (state.sys_mode == PRINT)
        {
            state.sys_mode = MOVE;
            // toggle_led(true); // turn led on
            // reset the current state
            // change to move state
        }
        state.last_move_time_stamp = micros();
    }
    else
    {
        if (state.sys_mode == MOVE && (micros() - state.last_move_time_stamp) > LED_DEBOUNCE_TIME)
        {
            state.sys_mode = IDLE;
            // stepper_c->set_enable(false);
            // toggle_led(false); // turn led off
        }
        else if (state.sys_mode == PRINT && pl.is_drawing_finished())
        {
            Serial.println("--LOG: Changing state to IDLE");
            state.sys_mode = IDLE;
            // toggle_led(false);
            state.last_move_time_stamp = micros();
        }
    }
}

void toggle_pen_state(StepperController *stepper_c,int pen_state)
{
  if(pen_state){
    int new_val = PEN_ON;
    if(stepper_c->get_servo_value() == PEN_ON){
      new_val = PEN_OFF;
    }
    stepper_c->set_servo_value(new_val);
    print_current_position();
  }

}

void auto_homing(StepperController *stepper_c)
{

    Serial.println("Auto homing! ");
    stepper_c->set_steps_rate(AUTO_HOME_STEPS_RATE);
    stepper_c->set_enable(true);
    stepper_c->set_servo_value(PEN_OFF);

    stepper_c->set_steps_count(mm_to_steps((X_MM_RAIL_LENGTH), X_STEPS_PER_MM), 0);

    // Move X to 0    
    while (stepper_c->get_steps_count()[X_AXIS] > 0)
    {
        stepper_c->move_step(1, 1);
    }
    Serial.println("Moved X axis to place.");

    // Move Y to 0 by using disable (gravity will drop the axis bottle to 0)
    const unsigned long current_time = millis();
    while(millis() - current_time < 1500 ){
      stepper_c->set_enable(false);
    }
    Serial.println("Moved Y axis to place.");
    stepper_c->set_steps_count(int(mm_to_steps(X_MM_HOMING_OFFSET, X_STEPS_PER_MM)), int(mm_to_steps(Y_MM_HOMING_OFFSET, Y_STEPS_PER_MM))); 
    
    // Move head to center of the board 
    stepper_c->set_enable(true);
    while (stepper_c->get_steps_count()[X_AXIS] < 0 )
    {
        stepper_c->move_step(1, 0);
    }
    while (stepper_c->get_steps_count()[Y_AXIS] < 0 )
    {
        stepper_c->move_step(2, 0);
    }

    stepper_c->set_steps_count(0, 0);
    stepper_c->set_limits(Y_MM_MAX_LIMIT,Y_MM_MAX_LIMIT,X_MM_MIN_LIMIT, Y_MM_MIN_LIMIT);

    stepper_c->set_steps_rate(STEPS_RATE);
    Serial.println("Auto homing completed successfully! ");
    print_current_position();
}

void test_draw(StepperController *stepper_c){
    stepper_c->set_enable(true);
    int counter = 0;
    for(int i = 0; i< 2500; i++){
      if(counter > 2500){
        stepper_c->step(3, 0);
        counter -= 2500;
      }
      else{
        stepper_c->step(1, 0);
      }
      counter += 1900;
      delayMicroseconds(1000);
    }
}

void print_current_position()
{
    Serial.println("Position: ");
    Serial.print(stepper_c.get_steps_count()[X_AXIS]);
    Serial.print(",");
    Serial.println(stepper_c.get_steps_count()[Y_AXIS]);

}

void initialize_auto_print(int *current_drawing)
{

    // running_time = micros();
    pl.reset_drawing();
    pl.load_drawing(&drawings[*current_drawing]);
    // toggle_led(true);
    stepper_c.set_enable(true);
    state.sys_mode = PRINT;
    Serial.println("--LOG: Changing state to PRINT");

    (*current_drawing) = (*current_drawing + 1) % NUMBER_OF_DRAWINGS;
}

void setup()
{

    Serial.begin(115200);
    /** Init Joystick input pins **/
    /** AUTO HOME**/
    pen_controller.attach(SERVO_COMMAND_PIN);

    auto_homing(&stepper_c);
    // test_draw(&stepper_c);
    state.sys_mode = IDLE;
}

void loop()
{
  test_timer = micros();
    /** GET INPUT MASK **/
    current_steps_mask = 0;
    current_direction_mask = 0;
    pen_state = 0;
    
    getMovementMask(&current_steps_mask,&current_direction_mask, &pen_state, encoder_a,encoder_b);
    
    state_handler(current_steps_mask, pen_state, &stepper_c);

    switch (state.sys_mode)
    {
    case MOVE:
        toggle_pen_state(&stepper_c, pen_state);
        stepper_c.move_step(current_steps_mask, current_direction_mask);
        break;
    case PRINT:
        pl.plot_drawing();
        break;
    case IDLE:
        if(micros() - state.last_move_time_stamp > PEN_PENDING_TIME && stepper_c.get_servo_value() == PEN_ON ){
            toggle_pen_state(&stepper_c, true);
        }
        if (micros() - state.last_move_time_stamp > PENDING_TIME)
        {
            initialize_auto_print(&current_drawing);
        }
        break;
    default:
        break;
    }
   test_timer = micros() - test_timer;
//   Serial.println(test_timer);
}
