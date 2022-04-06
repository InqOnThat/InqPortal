// * The Zambretti algorithm has 26 levels (A-Z).  Sunny/Clear weather is at the
//      beginnging while stormy weather at the end.
// * The JavaScript array "predict" at the bottom of this file contains the
//      strings used for these 26 levels. You can change text for localization
//      or more to your preferences.  Just be sure you keep 26 strings.
// * An image can be loaded on the ESP8266 server to show a background for 
//      each of these 26 levels.  eg (A.jpg, B.jpg, C.jpg... Z.jpg)
// * Unfortunately, the limited "disk" space may limit what images you can 
//      store.  Typical ESP8266 modules have 3MB available while a 
//      WeMos Pro has 15MB available. The algorthim for image selection can
//      compensate somewhat by using the closest (below) available.
//      e.g.  Say you've loaded A.jpg, F.jpg, M.jpg and T.jpg.  If the
//      Zambretti calculation returns E.jpg, the A.jpg will be shown.

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
            v = setBackground(v);
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

var last = -1;

function setBackground(v)
{
    // Get Zambretti letter code (A-Z) from the value (v = 0 to 25).
    let z = String.fromCharCode(v + 65);
    // Get return string for appropriate Zambretti conditions.
    let rtn = z + ' - ' + predict[v];
    // Look for the "closest" available file on the server.
    
    if (v != last)
    {
        // We don't want to keep searching every time we get an update
        // if we didn't find it the last time OR its the same image.
        last = v;   // (0 - 25)
        
        while (v >= 0)
        {
            z = String.fromCharCode(v + 65);    // (A - Z)
            let file = z + '.jpg';
            let http = new XMLHttpRequest();
            http.open('GET', file, false);
            http.send();
            if (http.status != 404)
            {
                // Get image for Zambretti code.
                document.body.style.backgroundImage = "url('" + file + "')";
                document.body.style.color = clrs[v];
                return rtn;
            }
            v--;
        }    
        document.body.style.backgroundImage = "url('back10.jpg')";
        document.body.style.color = clrs[v];
    }    
    return rtn;
};

function urlExists(url)
{
    var http = new XMLHttpRequest();
    http.open('HEAD', url, false);
    http.send();
    return http.status != 404;
};

var predict = new Array
(
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
    "Stormy, much rain"
); 

// These represent the colors to be assigned to the text
// of the 26 levels of the Zambretti algorithm.  
// Change color to match your chosen images as you like.
// Just make sure you keep 26 values.  The commented
// ones are ones that are included in the demo.  If you
// add more images, make sure you pick an appropriate
// color to go with it.
var clrs = new Array
(
  'navy',   // 0, A.jpg
  'black',
  'gold',   // 2, C.jpg
  'black',
  'black',
  'teal',   // 5, F.jpg
  'black',
  'black',
  'lime',   // 8, I.jpg
  'black',
  'black',
  'black',
  'black',
  'black',
  'black',
  'teal',   // 15, P.jpg
  'black',   
  'black',
  'black',
  'black',
  'black',
  'white',  // 21, V.jpg
  'orange', // 22, W.jpg
  'red',    // 23, X.jpg
  'black',
  'black'
);

