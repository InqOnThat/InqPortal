onModifyResult = function(p, v)
{
    switch (p)
    {
        case "T1":
        case "Th1":
        case "Tp":
        case "Th":
            v = v * 9 / 5 + 32;
            v = v.toFixed(1);
            break;
    
        case "H1":
        case "H":
        case "P1":
        case "P":
            v = v.toFixed(1);
            break;
        
        case "D3P":
            v = predict(v);
            break;
            
        case "V":
            v = "InqWeather v" + v;
            setBackground('RainComing.jpg', 'teal');
            break;
            
        default:
            // For all others, leave as it is.
            break;
    }
    return v;
};

function setBackground(file, color)
{
    document.body.style.backgroundImage = "url('" + file + "')";
    document.body.style.color = color;
};

function predict(delta)
{
    // https://sciencing.com/high-low-reading-barometric-pressure-5814364.html
    // Pressures and delta descriptions come from chart at the bottom of
    // https://www.artofmanliness.com/lifestyle/gear/fair-or-foul-how-to-use-a-barometer/
    // We don't have wind direction, so we'll "wing-it".

    const MID_HIGH = 0.18;
    const LOW_MID  = 0.04;
    const SLOW_MID = 0.01;
    const DFLT_CONDITION = "Same ole - Same ole";

    if (P < 29.8)
    {
        if (delta < -MID_HIGH)
        {
            setBackground('Gale.jpg', 'orchid');
			return "Severe northeast gales and heavy rain or snow,  followed in winter by cold wave.";
        }
        else if (delta > POS_MID_HIGH)
        {
            setBackground('ClearCold.jpg', 'navy');
			return "Clearing and colder.";
        }
        else
			return DFLT_CONDITION;
    }
    else if (P < 30)
    {
        if (delta < -MID_HIGH)
        {
            setBackground('RainWind.jpg', 'grey');
			return "Rain with high-wind, followed with 2 days by clearing, colder.";
        }
        else if (delta < -SLOW_MID)
        {
            setBackground('RainComing.jpg', 'teal');
			return "Rain within 18 hours that will continue for a day or two.";
        }
        else if (delta < SLOW_MID)
			return DFLT_CONDITION;
        else    // delta > SLOW_MID
        {
            setBackground('ClearCold.jpg', 'navy');
			return "Clearing and colder within 12 hours.";
        }
    }
    else if (P < 30.2)
    {
        if (delta < -MID_HIGH)
        {
            setBackground('WarmRain.jpg', 'lime');
			return "Warmer, and rain with 24 hours";
        }
        else if (delta < MID_HIGH)
        {
            setBackground('FareStable.jpg', 'white');
			return "Fair, with slight changes in temperature for 1 to 2 days.";
        }
        else    // delta > MID_HIGH
        {
            setBackground('FairRainComing.jpg', 'navy');
			return "Fair, followed within 2 days by warmer and rain.";
        }
    }
    else    // P >= 30.2
    {
        if (delta < -MID_HIGH)
        {
            setBackground('ClearCold.jpg', 'navy');
			return "Cold and clear, quickly followed by warmer and rain.";
        }
        else 
			return "No early change.";
    }
};

