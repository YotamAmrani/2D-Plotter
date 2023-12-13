#ifndef SETTINGS_H
#define SETTINGS_H

// Instructions indices
#define X_AXIS 0
#define Y_AXIS 1
#define SERVO_ANGLE 2

#define N_AXIS 2
#define N_INSTRUCTIONS 3
#define bit_istrue(x, mask) ((x & mask) != 0)
#define bit_to_sign(x, mask) (((x & mask) != 0) ? -1 : 1)
#define DIRECTION_INVERT_MASK 3

// SERVO
#define PEN_DEBOUNCE_TIME 300

// STEPERS CONFIGURATIONS
#define X_STEP_PIN 5
#define Y_STEP_PIN 6
#define X_DIR_PIN 2
#define Y_DIR_PIN 3
#define EN_PIN 8

// LIMIT SWITCHES
#define X_LIMIT_SW_PIN 9
#define Y_LIMIT_SW_PIN 10

// ENCODERS PINS
#define ENCODER_A_BIT_1 (A0) // input IO for gray code bit 0 
#define ENCODER_A_BIT_0 (9) // input IO for gray code bit 1
#define ENCODER_B_BIT_1 (A1) // input IO for gray code bit 0
#define ENCODER_B_BIT_0 (12) // input IO for gray code bit 1
#define ENCODER_A_BUTTON (A2)
#define ENCODER_B_BUTTON (A3) 
#define ENCODER_READ_INTERVAL 200 //microseconds
#define STEPS_PER_CLICK 300
#define DIRECTION_CHANGE_DELAY 40
#define ENCODER_BEHAVIOR 1 // if 0: PEN_ON applyed as long as the user press, if 1: each press will toggle the pen state

// SERVO 
#define SERVO_COMMAND_PIN (13) 
#define PEN_ON 0
#define PEN_OFF 30


// SYSTEM CONFIGURATIONS
#define STEP_PULSE_LENGTH 20
#define STEPS_RATE 1500
#define X_STEPS_PER_MM (10)
#define Y_STEPS_PER_MM (7.6)

// SOFT LIMITS SETTINGS
#define ENABLE_SOFT_LIMIT 1 // uncomment to disable soft limits
#define AUTO_HOME_STEPS_RATE 1200

#define X_MM_RAIL_LENGTH 740
#define Y_MM_RAIL_LENGTH 600

#define X_MM_HOMING_OFFSET (-330)
#define Y_MM_HOMING_OFFSET (-330)

#define Y_MM_MIN_LIMIT (-290)
#define X_MM_MIN_LIMIT (-325)
#define Y_MM_MAX_LIMIT 290
#define X_MM_MAX_LIMIT 325

// AUTO PRINTING
#define PENDING_TIME (1000000 * 180)
#define PEN_PENDING_TIME (1000000 * 3)

#define steps_to_mm(steps, ratio) (steps / ratio)
#define mm_to_steps(mm, ratio) (mm * ratio)

template <typename T>
int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

enum State
{
  IDLE,
  MOVE,
  PRINT
};

struct sys_state
{
  State sys_mode;
  long unsigned last_move_time_stamp;
};

#endif
