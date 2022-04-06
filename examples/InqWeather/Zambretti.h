// Near enough Zambretti Algorhithm 
// http://www.beteljuice.co.uk/zambretti/forecast.html 
// June 2008 - v1.0
// tweak added so decision # can be output

// Negretti and Zambras 'slide rule' is supposed to be better than 90% accurate 
// for a local forecast upto 12 hrs, it is most accurate in the temperate zones
// and about 09:00  hrs local solar time.
// I hope I have been able to 'tweak it' a little better ;-)	

// This code is free to use and redistribute as long as NO CHARGE is EVER made
// for its use or output

// Converted from JavaScript at above link to INO/C++
// (DEC) Dennis E. Cox, April 2022

// ---- 'environment' variables ------------

// P is Sea Level Adjusted (Relative) barometer in hPa or mB
// z_month is current month as a number between 1 to 12
// dirWind = degrees (0° = North to 350°)
// NB. if calm a 'nonsense' value should be sent as dirWind (direction) eg. 1 
//      or calm !
// z_trend is barometer trend: 0 = no change, 1= rise, 2 = fall
// hemi - OPTIONAL for posting with form
// [0] a short forecast text is returned
// [1] zambretti severity number (0 - 25) is returned ie. betel_cast() returns 
//      a two deep array

// (DEC) z_forecast is moved to the client side.  Server only generates ID.
//      0-25 => (A-Z) and sends to client.  Text can easily be localized as
//      desired.    
// var z_forecast = new Array("Settled fine", "Fine weather", "Becoming fine", 
//      "Fine, becoming less settled", "Fine, possible showers", 
//      "Fairly fine, improving", "Fairly fine, possible showers early", 
//      "Fairly fine, showery later", "Showery early, improving", 
//      "Changeable, mending", "Fairly fine, showers likely", 
//      "Rather unsettled clearing later", "Unsettled, probably improving",
//       "Showery, bright intervals", 
//      "Showery, becoming less settled", "Changeable, some rain", 
//      "Unsettled, short fine intervals", "Unsettled, rain later", 
//      "Unsettled, some rain", "Mostly very unsettled", 
//      "Occasional rain, worsening", "Rain at times, very unsettled", 
//      "Rain at frequent intervals", "Rain, very unsettled", 
//      "Stormy, may improve", "Stormy, much rain"); 

// * zMinP, zMaxP = miniumum and maximum pressures seen by sensors.  Values are
//      persisted when range is expanded.
// * zNorth = northern hemisphere else southern hemisphere.  Configured by
//      client. Persisted.
// month49 = Is April through September inclusive.

#include <Arduino.h>

// Run time calculated
bool month49 = true;
// 3 hour pressure change -1 falling, 0 steady (+/- 1.6 mbar/3hr), 1 rising

#define NO_WIND 1E6
#define STEADY 1.0  // between +/- STEADY is not rising/falling.

// P = sea-level adjusted barometric pressure (mbar)
// d3P = change in barometric pressure over past 3 hours.
// dLP = difference in minimum and maxium pressure experienced by sensor ever.
// month = Month of the year (1 - 12)
// dirWind = degrees, North = 0, East = 90, etc.

u8 zambretti(float P, float d3P, float minP, float maxP, bool north,
    float dirWind = NO_WIND)
{        
    float dLP = maxP - minP;
    if (!dLP)
        return 0; 

    if (dirWind < 1000)     // NO_WIND = 1E6
    {
        if (!north)
            dirWind += 180;
        // This mess, just converts angle in degrees to 16 windrose cardinal 
        // 0 = N to 15 = NNW
        u8 w = (u8)((dirWind + 11.25) / 22.5) % 16;

        const float wind_dir[] 
            {6,5,5,2,-0.5,-2,-5,-8.5,-12,-10,-6,-4.5,-3,-0.5,1.5,3};
        P += dLP * wind_dir[w] / 100.0;
    }
    
    d3P = d3P > STEADY ? 1.0 : (d3P < -STEADY ? -1.0 : 0);
    if (month49 == north)   // Summer in each hemisphere
        P += d3P * 7.0 / 100.0 * dLP;
    u8 index = (u8)((P - minP) / dLP * 21.99F);
    
    // equivalents of Zambretti 'dial window' letters A - Z
    const u8 rise_options[] 
        {25,25,25,24,24,19,16,12,11,9,8,6,5,2,1,1,0,0,0,0,0,0}; 
    const u8 steady_options[]
        {25,25,25,25,25,25,23,23,22,18,15,13,10,4,1,1,0,0,0,0,0,0}; 
    const u8 fall_options[] 
        {25,25,25,25,25,25,25,25,23,23,21,20,17,14,7,3,1,1,1,0,0,0}; 
    
	if (d3P > 0)
        return rise_options[index];
	else if (d3P < 0)
		return fall_options[index];
    return steady_options[index];
}
		
// =============================================================================

float doPressureChange(float p)
{
    // Create a min-by-min circular buffer to store 3 hours of pressure
    // readings.
    #define STORE 3             // Hours
    #define CNT 60 * STORE      // One reading for each minute for 3 hours.
    static u16 next = 0;        // Where to place the next reading.
    static bool filled = false; // Flag that we've filled array at least once.
    static float* press = NULL; // The array!
    
    if (!press)
    {
        press = (float*)os_malloc(CNT * sizeof(float));
        os_memset(press, 0, sizeof(CNT * sizeof(float)));
    }
    press[next] = p;
    next++;
    if (next == CNT)    // We've filled the array, start overwriting 
    {
        filled = true;
        next = 0;       // from the beginning again.
    }
    
    // Calculate and publish 3 hour pressure differential.
    if (filled)
        return (float)(p - press[next]);
    return (p - press[0]);
}

// =============================================================================

