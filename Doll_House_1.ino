// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

//#include <IRremote.h>
#include <IRLib.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//***************IR Receiver**************
IRrecv My_Receiver(11);
IRdecode My_Decoder;
IRTYPES codeType=RC5;
//decode_results results;
int RECV_PIN = 7;
//int BUTTON_PIN = 12;
int STATUS_PIN = 12;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

//****************NEO-PIXELS****************
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

#define MAXPULSE 65000

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_BRG + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_BRG + NEO_KHZ800);
int delayval = 50; // delay for half a second

int rainbowDelay = 100;

int pixel =0;        //pixel to light.
int red;
int green;
int blue;
//***************-FLAGS-******************
String messReceived = "";
boolean repeat = false;


void setup() {
  //***********************--NEO-PIXELS--**********************
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // pixels.begin(); // This initializes the NeoPixel library.
  //***********************--IR-RECEIVER--**********************  
  My_Receiver.enableIRIn(); // Start the receiver
  //pinMode(BUTTON_PIN, INPUT);
  pinMode(RECV_PIN, INPUT);
  pinMode(STATUS_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  // rainbowCycle(1);
  // if(irrecv.decode(&results)){

  receiveIRCodes();
  //delay(5);

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  //  int myCircle[]={0,1,2,3,4,5,6,7};
  for(j=0; j<255; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< (strip.numPixels()-4); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void receiveIRCodes(){

  //When IR code is received store it into string messReceived until \r (carriage return is received)
  //Flash LED when receiving characters
  //Blink (1 second) LED when carriage return is received

  if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();
    if(My_Decoder.value == 13){
      Serial.println(messReceived);//Extract instructions from message transmitted
      extractInstruction(messReceived);


      messReceived="";
      //blinkLED(1000);
    }
    else{
      messReceived += char(My_Decoder.value);
      blinkLED(1);
    }
    //Serial.print(char(results.value));
    My_Receiver.resume(); // Receive the next value    
  }
}

void blinkLED(int blinkLength){
  digitalWrite(STATUS_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(blinkLength);               // wait for a second
  digitalWrite(STATUS_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(blinkLength);
}      

void extractInstruction(String messReceived){
  //read input String until markers are found
  String buffer = "";

  for(int i=0; i <= messReceived.length(); i++){
    //blinkLED(500);
    if (messReceived.substring(i,i+3)=="W: "){
      buffer=messReceived.substring(i+3,i+6);
      //Serial.println(buffer);
      //blinkLED(500);
      if(buffer=="ALL"){
        pixel = 20;
        //blinkLED(500);
        buffer = "";
      }
      else{
        pixel=buffer.toInt();
        //Serial.print(" Pixel: ");
        //Serial.println(pixel);
        buffer = "";
        //i=i+1;
      }
    }
    else if (messReceived.substring(i,i+3)=="R: "){
      red = messReceived.substring(i+3,i+6).toInt();
      //i=i+1;
      // blinkLED(2000);
    }
    else if (messReceived.substring(i,i+3)=="G: "){
      green = messReceived.substring(i+3,i+6).toInt();
      //i=i+1;
    }
    else if (messReceived.substring(i,i+3)=="B: "){
      blue = messReceived.substring(i+3,i+6).toInt();
    }
    else if (messReceived.substring(i,i+3)=="A: "){
      buffer=messReceived.substring(i+3,i+6);

      //blinkLED(500);
      if(buffer=="ALL"){
        pixel = 20;
        //Serial.println(buffer);
        //blinkLED(500);
        buffer = "";
      }
      else if(buffer=="PLU"){
        rainbowDelay = rainbowDelay - 10;
        rainbowDelay = constrain(rainbowDelay,10,1000);
      }
      else if(buffer=="MIN"){
        rainbowDelay = rainbowDelay + 100;
        rainbowDelay = constrain(rainbowDelay,10,1000);
      }
      else if(buffer=="OFF"){
        red=0;
        green=0;
        blue=0;
      }
      else if(buffer=="RAI"){
        Serial.println("Start Rainbow ...");
        rainbowCycle(rainbowDelay);
        buffer = "";
      }
      //i=i+1;
      // blinkLED(2000);
    }
  }
  lightItUp(pixel,red,green,blue);
}

void lightItUp(int pixel, int red, int green, int blue){
  if (pixel == 20){
    for(int i=0;i<NUMPIXELS;i++){
      strip.setPixelColor(i, red, green , blue);
    }
    /*    
     Serial.print("R: ");
     Serial.print(red);
     Serial.print(" G: ");
     Serial.print(green);
     Serial.print(" B: ");
     */
     Serial.println("set ALL pixels");
     
  }
  else {
    strip.setPixelColor(pixel,red,green,blue);
    //    Serial.print(" Pixel: ");
    //    Serial.println(pixel);
  }
  strip.show();
}





