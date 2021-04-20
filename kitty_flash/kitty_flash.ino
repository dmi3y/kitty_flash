/*
# This project is libre, and licenced under the terms of the
# DO WHAT THE FUCK YOU WANT TO PUBLIC LICENCE, version 3,

████████╗██╗  ██╗███████╗    ███████╗██╗     ██╗   ██╗███████╗██╗  ██╗
╚══██╔══╝██║  ██║██╔════╝    ██╔════╝██║     ██║   ██║██╔════╝██║  ██║
   ██║   ███████║█████╗      █████╗  ██║     ██║   ██║███████╗███████║
   ██║   ██╔══██║██╔══╝      ██╔══╝  ██║     ██║   ██║╚════██║██╔══██║
   ██║   ██║  ██║███████╗    ██║     ███████╗╚██████╔╝███████║██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═╝     ╚══════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝
                      |\__/,|   (`\
                    _.|o o  |_   ) )
                    -(((---(((--------
*/

#include "pitches.h"
#include <Servo.h>
#include "SR04.h"

#define TRIG_PIN 12
#define ECHO_PIN 11
#define BUZZ_PIN 3
#define SERVO_PIN 9

#define MEOW 1000ul
#define HOLD_MY_MYLK MEOW * 10
#define WAIT_FOR_IT MEOW * 1
#define MEOWS_TO_ADJUST 10
#define FLUSH_TIME 10
#define SOMETHING_GOING_ON 2
#define WISKERS_REACH 9
#define ANNOYED_AND_FURIOUS 30
#define ANNOYED_AND_BORED 60

#define IM_PURRING 0 // waiting
#define IM_PERKING 1 // target intersepted
#define IM_ANNOYED 2 // ready to flush
#define IM_FURIOUS 3 // flushing

long meowsFromFlush = 0;
long meowsFromAnnoyed = 0;
long meowsFromBored = 0;
long meowsFromInterception = 0;
long meowsInFlush = 0;
long measuredDistance = -1;
long etalonDistance = -1;
long distanceDiff = 0;
unsigned long timeNow = 0;
unsigned long timeNext = 0;
int flushState = IM_PURRING;

Servo myservo;
SR04 usonic = SR04(ECHO_PIN, TRIG_PIN);

void setup()
{
    pinMode(BUZZ_PIN, OUTPUT);
    myservo.attach(SERVO_PIN);
    myservo.write(0);
    Serial.begin(9600);
    timeNext = millis() + HOLD_MY_MYLK;
}

void loop()
{
    timeNow = millis();

    if (timeNow > timeNext)
    {
        measuredDistance = usonic.Distance();
        // Using both negative and positive diffs
        // as sometimes ultrasonic being mixed up
        // by kitty's fur and and outputs greater distances
        distanceDiff = labs(etalonDistance - measuredDistance);
        timeNext = timeNow + WAIT_FOR_IT;

        if (meowsFromFlush < MEOWS_TO_ADJUST)
        {
            // Give ultrasonic time to adjust
            etalonDistance += measuredDistance;
            // Everything meow reset!
            meowsFromInterception = 0;
            meowsFromAnnoyed = 0;
            meowsFromBored = 0;
            meowsInFlush = 0;
        }
        else
        {
            if (meowsFromFlush == MEOWS_TO_ADJUST)
            {
                // First MEOWS_TO_ADJUST cycles after the flush
                // Etalon distance to capture interceptions
                etalonDistance = etalonDistance / MEOWS_TO_ADJUST;
            }
            else
            {
                Serial.print("ETALON DISTANCE: ");
                Serial.println(etalonDistance);
                Serial.print("MEASURED DISTANCE: ");
                Serial.println(measuredDistance);
            }

            // Kitty Flash state machine starts here //
            if (distanceDiff < WISKERS_REACH && flushState < IM_ANNOYED)
            {
                meowsFromInterception = 0;
                flushState = IM_PURRING;
            }
            else if (distanceDiff > WISKERS_REACH && flushState < IM_ANNOYED)
            {
                flushState = IM_PERKING;
                meowsFromInterception += 1;

                if (meowsFromInterception > SOMETHING_GOING_ON)
                {
                    flushState = IM_ANNOYED;
                }
            }
            else if (flushState == IM_ANNOYED)
            {
                if (distanceDiff < WISKERS_REACH || meowsFromBored > ANNOYED_AND_BORED)
                {
                    meowsFromAnnoyed += 1;
                    Serial.print("MEOWS FROM ANNOYED: ");
                    Serial.println(meowsFromAnnoyed);
                    if (meowsFromAnnoyed > ANNOYED_AND_FURIOUS)
                    {
                        flushState = IM_FURIOUS;
                    }
                }
                else
                {
                    meowsFromAnnoyed = 0;
                    meowsFromBored += 1;
                }
            }
            else if (flushState == IM_FURIOUS)
            {
                if (meowsInFlush == 0)
                {
                    play(0);
                    Serial.println("flushing.... ");
                    myservo.write(180);
                }
                if (meowsInFlush > FLUSH_TIME)
                {
                    myservo.write(0);
                    play(1);
                    meowsFromFlush = 0;
                    flushState = IM_PURRING;
                }
                meowsInFlush += 1;
            }
        }
        // Kitty Flash state machine ends here //

        meowsFromFlush += 1;
        Serial.print("STATE: ");
        Serial.println(flushState);
    }
}

/*
████████╗██╗  ██╗███████╗    ███████╗ ██████╗ ███╗   ██╗ ██████╗ 
╚══██╔══╝██║  ██║██╔════╝    ██╔════╝██╔═══██╗████╗  ██║██╔════╝ 
   ██║   ███████║█████╗      ███████╗██║   ██║██╔██╗ ██║██║  ███╗
   ██║   ██╔══██║██╔══╝      ╚════██║██║   ██║██║╚██╗██║██║   ██║
   ██║   ██║  ██║███████╗    ███████║╚██████╔╝██║ ╚████║╚██████╔╝
   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚══════╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ 
                        |\      _,,,---,,_
                ZZZzz /,`.-'`'    -.    ;-;;,_
                     |,4-  ) )-,_. ,\ (    `'-'
                    '---''(_/--'  `-'\_)
*/

int main_theme[] = {
    NOTE_E7, NOTE_E7, 0, NOTE_E7,
    0, NOTE_C7, NOTE_E7, 0,
    NOTE_G7, 0, 0, 0,
    NOTE_G6, 0, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0,

    NOTE_C7, 0, 0, NOTE_G6,
    0, 0, NOTE_E6, 0,
    0, NOTE_A6, 0, NOTE_B6,
    0, NOTE_AS6, NOTE_A6, 0,

    NOTE_G6, NOTE_E7, NOTE_G7,
    NOTE_A7, 0, NOTE_F7, NOTE_G7,
    0, NOTE_E7, 0, NOTE_C7,
    NOTE_D7, NOTE_B6, 0, 0};

int main_theme_tempo[] = {
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12,

    9, 9, 9,
    12, 12, 12, 12,
    12, 12, 12, 12,
    12, 12, 12, 12};

int underworld_melody[] = {
    NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
    NOTE_AS3, NOTE_AS4, 0,
    0,
    NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
    NOTE_AS3, NOTE_AS4, 0,
    0,
    NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
    NOTE_DS3, NOTE_DS4, 0,
    0,
    NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
    NOTE_DS3, NOTE_DS4, 0,
    0, NOTE_DS4, NOTE_CS4, NOTE_D4,
    NOTE_CS4, NOTE_DS4,
    NOTE_DS4, NOTE_GS3,
    NOTE_G3, NOTE_CS4,
    NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
    NOTE_GS4, NOTE_DS4, NOTE_B3,
    NOTE_AS3, NOTE_A3, NOTE_GS3,
    0, 0, 0};

int underworld_melody_tempo[] = {
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    3,
    12, 12, 12, 12,
    12, 12, 6,
    6, 18, 18, 18,
    6, 6,
    6, 6,
    6, 6,
    18, 18, 18, 18, 18, 18,
    10, 10, 10,
    10, 10, 10,
    3, 3, 3};

void play(int s)
{
    Serial.println("sinning the song.... ");
    if (s == 0)
    {
        int size = sizeof(main_theme) / sizeof(int);
        for (int thisNote = 0; thisNote < size; thisNote++)
        {
            // to calculate the note duration, take one second
            // divided by the note type.
            //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
            int noteDuration = 1000 / main_theme_tempo[thisNote];

            buzz(main_theme[thisNote], noteDuration);

            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 30% seems to work well:
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);

            // stop the tone playing:
            buzz(0, noteDuration);
        }
    }
    else
    {
        int size = sizeof(underworld_melody) / sizeof(int);
        for (int thisNote = 0; thisNote < size; thisNote++)
        {
            // to calculate the note duration, take one second
            // divided by the note type.
            //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
            int noteDuration = 1000 / underworld_melody_tempo[thisNote];

            buzz(underworld_melody[thisNote], noteDuration);

            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 30% seems to work well:
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);

            // stop the tone playing:
            buzz(0, noteDuration);
        }
    }
}

void buzz(long frequency, long length)
{
    long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
    //// 1 second's worth of microseconds, divided by the frequency, then split in half since
    //// there are two phases to each cycle
    long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
    //// multiply frequency, which is really cycles per second, by the number of seconds to
    //// get the total number of cycles to produce
    for (long i = 0; i < numCycles; i++)
    {                                  // for the calculated length of time...
        digitalWrite(BUZZ_PIN, HIGH);  // write the buzzer pin high to push out the diaphram
        delayMicroseconds(delayValue); // wait for the calculated delay value
        digitalWrite(BUZZ_PIN, LOW);   // write the buzzer pin low to pull back the diaphram
        delayMicroseconds(delayValue); // wait again or the calculated delay value
    }
}

/*
████████╗██╗  ██╗███████╗    ██╗  ██╗██╗████████╗████████╗██╗   ██╗
╚══██╔══╝██║  ██║██╔════╝    ██║ ██╔╝██║╚══██╔══╝╚══██╔══╝╚██╗ ██╔╝
   ██║   ███████║█████╗      █████╔╝ ██║   ██║      ██║    ╚████╔╝ 
   ██║   ██╔══██║██╔══╝      ██╔═██╗ ██║   ██║      ██║     ╚██╔╝  
   ██║   ██║  ██║███████╗    ██║  ██╗██║   ██║      ██║      ██║   
   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═╝  ╚═╝╚═╝   ╚═╝      ╚═╝      ╚═╝   
                         _._     _,-'""`-._
                        (,-.`._,'(       |\`-/|
                            `-.-' \ )-`( , o o)
                                   `-    \`_`"'-
*/
