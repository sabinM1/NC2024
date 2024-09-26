// Including the required Arduino libraries
#include "MD_MAX72xx.h"

// Initialization
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 5 //this example is using only 6
#define CS_PIN 26
#define DATA_PIN 27
#define CLK_PIN 22
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

//Call with "set_matrix()":
void set_matrix(){
 int a[5][8] = {
  {0, 0, 0, 11001000, 11011100, 1001001, 101010, 11111},
  {111100, 1111110, 111100, 11000, 11111111, 1011010, 10011001, 11111111},
  {0, 0, 0, 10011, 111011, 10010010, 1010100, 11111000},
  {111111, 111111, 11, 1111, 1111, 11, 111111, 10111111},
  {111, 1111, 11011, 11011, 1111, 11111, 11011, 1011011}
 };

 for (int i=0; i<5; i++){
   for (int j=0; j<8; j++){
    for (int k=8+8*i-1; k>8*i-1; k--){
     mx.setPoint(j, k, a[i][j]%10);
     a[i][j]/=10;
   }
  }
 }
}

 

void setup() {
  // Intialize the object
  mx.begin();
  // Set the intensity (brightness) of the display (0-15)
  mx.control(MD_MAX72XX::INTENSITY, 7);
  // Clear the display
  mx.clear();
  // Set the matrix to the patterns
  set_matrix();
}
void loop() {
  set_matrix();
}
