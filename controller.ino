#include "InternetButton/InternetButton.h"

InternetButton b = InternetButton();
Servo lights_servo;
Servo door_servo;

int on = 20;
int off = 0;

bool leds[11] = {false};

int door_power = A0;

int lights_on = 1;
int lights_off = 3;
int door_lock = 2;
int door_unlock = 4;

void setup() {
    b.begin();
    display_status();
    lights_servo.attach(D1);
    door_servo.attach(D2);
    b.playSong("C4,8,E4,8,G4,8,C5,8,G5,4");
    pinMode(A0, OUTPUT);
    bool success;
    success = Particle.publish("motion-detected");
    if (!success) {
      b.playNote("C1", 1);
    }
    Particle.publish("Event triggered");
    Particle.function("lights", lights);
    Particle.function("door", door);
}

void loop() {
    if(b.allButtonsOn()){
        b.rainbow(10);
        b.allLedsOff();
        return;
    }
    //ligths on
    if(b.buttonOn(lights_on)){
        lights("on");
        
        delay(100);
        display_status();
    }
    //lights off
    if(b.buttonOn(lights_off)){
        lights("off");
        delay(100);
        display_status();
    }
    //door unlock
    if(b.buttonOn(door_unlock)){ 
        door("unlock");
        for(int i = 6; i <= 11; i ++){
            b.ledOn(i, 0, 255, 0);
        }
        delay(100);
        display_status();
    }
    
    //door lock
    if(b.buttonOn(door_lock)){
        door("lock");
        // for(int i = 1; i <= 6; i ++){
        //     b.ledOn(i, 255, 0, 0);
        // }
        // delay(100);
        display_status();
    }
}

// Log message to cloud, message is a printf-formatted string
void debug(String message, int value) {
    char msg [50];
    sprintf(msg, message.c_str(), value);
    Particle.publish("DEBUG", msg);
}

int lights(String command){
    int off_pos = 2;
    int on_pos = 180;
    if(command == "on"){
        if (leds[1] == true) {
            return 1;
        }
        lights_servo.write(on_pos);
        led_loading("lights", command);
        leds[1] = true;
        leds[5] = true;
        leds[6] = true;
        leds[7] = true;
        leds[11] = true;
        b.allLedsOn(0, 0, 255);
        display_status();
        return 1;
    }
    if(command == "off" && leds[1] == true){
        lights_servo.write(off_pos); 
        led_loading("lights", command);
        leds[1] = false;
        leds[5] = false;
        leds[6] = false;
        leds[7] = false;
        leds[11] = false;
        display_status();
        return 1;
    }
}

int door(String command){
    int pos;
    if(command == "unlock"){
        pos = 100;
        leds[2] = false;
        leds[3] = false;
        leds[4] = false;
        leds[8] = true;
        leds[9] = true;
        leds[10] = true;
    }
    if(command == "lock"){
        pos = 0;
        leds[2] = true;
        leds[3] = true;
        leds[4] = true;
        leds[8] = false;
        leds[9] = false;
        leds[10] = false;
    }
    digitalWrite(door_power, HIGH);
    // delay(1000);
    door_servo.write(pos);
    led_loading("door", command);
    // delay(1000);
    digitalWrite(door_power, LOW);
    
    display_status();
    return 1;
}

void led_loading(String controller, String command) {
    int speed;
    int red = 0;
    int green = 0;
    int blue = 0;
    
    if(controller == "lights"){
        blue = 50;
        speed = 70;
    } else {
        speed = 100;
    }
    
    if(command == "on" || command == "unlock") {
        if(controller == "door"){
            red = 0;
            green = 50;
            blue = 0;
        }
        b.allLedsOff();
        for(int i = 1; i <= 11; i++){
            b.ledOn(i, red, green, blue);
            delay(speed);
        }
    }
    if(command == "off" || command == "lock"){
        int stop = 1;
        if(controller == "door"){
            red = 50;
            green = 0;
            blue = 0;
            stop = 5;
        }
        b.allLedsOn(red, green, blue);
        for(int i = 11; i >= stop; i--){
            b.ledOff(i);
            delay(speed);
        }
    }
    
}

void display_status(){
    for(int i = 1; i<=11; i++){
        int red = off;
        int green = off;
        int blue = off;
        if(leds[i] == true){
            if(i == 1 || i == 11 || (i >= 5 && i <= 7)) {
                red = on;
                green = on;
                blue = on;
            }
            
            if (i >= 2 && i <= 4){
                red = on;
                green = off;
                blue = off;
            }
            
            if (i >= 8 && i <= 10){
                red = off;
                green = on;
                blue = off;
            }
            b.ledOn(i, red, green, blue);
        }
        if (leds[i] == false) {
            b.ledOff(i);
        }
    }
}
    