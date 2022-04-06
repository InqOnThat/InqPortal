onModifyResult = function(p, v)
{
    switch(p)
    {
        case "T1":
            // Convert gauge data C to F
            document.gauges.get("gauge-temp").value = v * 9 / 5 + 32;
            break;
        
        case "P1":
            // Convert gauge data mbar to in-Hg
            document.gauges.get("gauge-press").value = v * 0.02953;
            break;
            
        case "H1":
            document.gauges.get("gauge-humid").value = v;
            break;
            
        case "V":
            v = "InqWeather v" + v;
            break;
            
        default:
            break;
    }
    return v;
};
