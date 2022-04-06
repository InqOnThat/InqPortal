var predict = new Array(
    "Settled fine",
    "Fine weather",
    "Becoming fine",
    "Fine, becoming less settled",
    "Fine, possible showers",
    "Fairly fine, improving", 
    "Fairly fine, possible showers early", 
    "Fairly fine, showery later", 
    "Showery early, improving", 
    "Changeable, mending", 
    "Fairly fine, showers likely", 
    "Rather unsettled clearing later",
    "Unsettled, probably improving",
    "Showery, bright intervals", 
    "Showery, becoming less settled",
    "Changeable, some rain", 
    "Unsettled, short fine intervals", 
    "Unsettled, rain later", 
    "Unsettled, some rain", 
    "Mostly very unsettled", 
    "Occasional rain, worsening", 
    "Rain at times, very unsettled", 
    "Rain at frequent intervals", 
    "Rain, very unsettled", 
    "Stormy, may improve", 
    "Stormy, much rain"); 

onModifyResult = function(p, v)
{
    switch (p)
    {
        case "T1":
        case "Th1":
        case "Tp":
        case "Th":  
            // Convert C to F
            v = (v * 9 / 5 + 32).toFixed(1);
            break;

        case "P1":
        case "P":
            // Convert mbar to in-Hg
            v *= 0.02953;            
        case "H1":
        case "H":
            v = v.toFixed(1);
            break;            
        
        case "Z":
            // Return string for appropriate conditions.
            v = String.fromCharCode(v + 65) + ' - ' + predict[v];
            break;
            
        case "V":
            v = "InqWeather v" + v;
            break;
            
        default:
            // For all others, leave as it is.
            break;
    }
    return v;
};

