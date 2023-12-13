#include "JoystickInterface.h"

Encoder::Encoder(int bit_0_pin,int bit_1_pin,int push_button_pin, char encoder_char)
:bit_0_pin_(bit_0_pin),bit_1_pin_(bit_1_pin),push_button_pin_(push_button_pin), encoder_char_(encoder_char),time_stamp_(0),
prev_encoder_read_(0), new_encoder_read_(0),counter_(0),prev_counter_(0),direction_(0), steps_to_move_(0)
{
  initialize_encoder();
 }

void Encoder::initialize_encoder(){
  pinMode(bit_0_pin_, INPUT_PULLUP);  
  pinMode(bit_1_pin_, INPUT_PULLUP);  
  pinMode(push_button_pin_, INPUT_PULLUP);
  time_stamp_ = millis();
  Serial.print("Initialized encoder ");
  Serial.println(encoder_char_);
  }

bool Encoder::is_pressed_ = 0;
  
int Encoder::read_encoder(){
   if(micros() - time_stamp_ > ENCODER_READ_INTERVAL){
     time_stamp_ = micros();
//     delay(3);
     prev_encoder_read_ = new_encoder_read_ ;
     new_encoder_read_ = (((digitalRead(bit_1_pin_)) << 1) + (digitalRead(bit_0_pin_))) ;
     byte check_direction  = (prev_encoder_read_ << 2) + new_encoder_read_  ; // x4 = 2 rotate left 
     switch (check_direction)
     {
      case 1: case 7: case 8: case 14:
      counter_++;
      return 1 ;
      case 2: case 4: case 11: case 13:
      counter_--;
      return -1 ;
      case 0: case 5: case 10: case 15:
      return 0 ;
      default: // need to avoide delay in return 
      return 0 ; // 
    }
  }
    
   return 0;
}

void Encoder::set_direction(){
    direction_ = sgn(counter_ - prev_counter_);
    if(direction_ && direction_ != last_direction_  ){
      // delay(DIRECTION_CHANGE_DELAY); // delay added for direction changes
      last_direction_ = direction_;
    }
  }

void Encoder::print_current_counter(){
  Serial.print("Encoder ");
  Serial.print(encoder_char_);
//  Serial.print(", time:");
//  Serial.print(millis()-time_stamp_);
//  time_stamp_ = micros();
  Serial.print(", is pressed:");
  Serial.print(!digitalRead(push_button_pin_));
  Serial.print(", counter: ");
  Serial.println(counter_);
  }

bool Encoder::is_pressed(){
  return !digitalRead(push_button_pin_);
  }
  
 void read_encoder_toggle_press(Encoder &encoder_a, Encoder &encoder_b, int *pen_state){
    if(!Encoder::is_pressed_ && (encoder_a.is_pressed() || encoder_b.is_pressed())){
      Encoder::is_pressed_ = true;
   }
   else if(Encoder::is_pressed_ && (!encoder_a.is_pressed() && !encoder_b.is_pressed())){
      Encoder::is_pressed_ = false;
      Serial.println("pressed! ");
      *pen_state = 1;
   }
 }

  
void read_encoder_long_press(Encoder &encoder_a, Encoder &encoder_b, int *pen_state){
  if(!Encoder::is_pressed_ && (encoder_a.is_pressed() || encoder_b.is_pressed())){
      Encoder::is_pressed_ = true;
      *pen_state = 1;
   }
   else if(Encoder::is_pressed_ && (encoder_a.is_pressed() || encoder_b.is_pressed())){
      // Serial.println("pressed! ");
      *pen_state = 0;
      
   }
    else if(Encoder::is_pressed_ && (!encoder_a.is_pressed() && !encoder_b.is_pressed())){
      Encoder::is_pressed_ = false;
      Serial.println("pressed! ");
      *pen_state = 1;
   }
   

}

void getMovementMask(int *current_steps_mask, int *current_direction_mask,int *pen_state, Encoder &encoder_a, Encoder &encoder_b)
{
  // read and update the encoders cunter at each iteration  
  encoder_a.read_encoder();
  encoder_b.read_encoder();


  if(ENCODER_BEHAVIOR){
    read_encoder_toggle_press(encoder_a,encoder_b, pen_state); //  case 1: each press toggle the servo state
  }
  else{
    read_encoder_long_press(encoder_a,encoder_b, pen_state); // case 2: we want that pen_on will be applyed only when pressing
  }
  
  
  
  
  *current_steps_mask = 0;
  *current_direction_mask = 0;

  int x_input = 0;
  int y_input = 0;
  
  if(encoder_a.steps_to_move_== 0){
    encoder_a.set_direction();
    encoder_a.prev_counter_ = encoder_a.counter_;
    if(encoder_a.direction_){
       encoder_a.steps_to_move_ = STEPS_PER_CLICK;
      }
    }
  else{
    x_input = encoder_a.direction_;
    encoder_a.steps_to_move_--;
    }
  if(encoder_b.steps_to_move_== 0){
    encoder_b.set_direction();
    encoder_b.prev_counter_ = encoder_b.counter_;
    if(encoder_b.direction_){
       encoder_b.steps_to_move_ = STEPS_PER_CLICK;
      }
    }
  else{
    y_input = encoder_b.direction_;
    encoder_b.steps_to_move_--;
    }
    
  unsigned long test_timer = micros();
  test_timer = micros() - test_timer;
//  Serial.println(test_timer);

//
//  /* Move X AXIS */
  if (x_input > 0)
  {
    *current_steps_mask = *current_steps_mask | (1 << X_AXIS);
    *current_direction_mask = *current_direction_mask | (1 << X_AXIS);
  }
  else if (x_input < 0)
  {
    *current_steps_mask = *current_steps_mask | (1 << X_AXIS);
  }

  /* Move Y AXIS */
  if (y_input > 0)
  {
    *current_steps_mask = *current_steps_mask | (1 << Y_AXIS);
    *current_direction_mask = *current_direction_mask | (1 << Y_AXIS);
  }
  else if (y_input < 0)
  {
    *current_steps_mask = *current_steps_mask | (1 << Y_AXIS);
  }
}
