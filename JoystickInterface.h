#ifndef JOYSTICK_INTERFACE_H
#define JOYSTICK_INTERFACE_H

#include "Settings.h"
#include <Arduino.h>
#include <Servo.h>


class Encoder{
  private:
  const int bit_0_pin_;
  const int bit_1_pin_;
  const int push_button_pin_;
  
  const char encoder_char_;
  unsigned long time_stamp_;
  
  byte prev_encoder_read_;
  byte new_encoder_read_;
  // negative value for clockwise spin  
  
  
  public:
  static bool is_pressed_;
  int last_direction_;

  int counter_;
  int prev_counter_;
  int direction_;
  int steps_to_move_;
  Encoder(int bit_0_pin,int bit_1_pin,int push_button_pin, char encoder_char);
  
/**
  * read the encoders' current value. 
  * @retun the encoder current value (int)
  */
  int read_encoder();

  /**
  * Set the head current movement direction, based on the encoder.
  */
  void set_direction();

  /**
  * intialize the encoders' default values.
  */
  void initialize_encoder();
  
  /**
  * print the encoders' current counter value.
  */
  void print_current_counter();

  /**
  * returns the encoders' button state.
  * @retun true one of the encoders is currently pressed.
  */
  bool is_pressed();
  };



// Encoders interface

/**
* Based on the encoders state, initialze a new steps, and direction mask. 
* turning on and of the pen state.
*/
void getMovementMask(int *current_steps_mask, int *current_direction_mask,int *pen_state, Encoder &encoder_a, Encoder &encoder_b);

/**
* reads the ncodders buttons state.
*/
void read_encoders_buttons();

#endif
