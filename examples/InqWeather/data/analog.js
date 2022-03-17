onModifyResult = function(p, v)
{
    switch(p)
    {
        case "T1":
            document.gauges.get("gauge-temp").value = v * 9 / 5 + 32;
            break;
        
        case "P1":
            document.gauges.get("gauge-press").value = v;
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
