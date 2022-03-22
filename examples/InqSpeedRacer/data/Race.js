// Copyright (c) 2021 Dennis E. Cox. All rights reserved.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

window.onkeydown = (ev)=>{ if (event.keyCode == 13) newRace(); return true };

var tracks = null;      // Only used if there is more than one server.
var race = 0;           // Number of races posted.    
var finish = 0;         // First place finish time.

// The server spits out speed in meters/second.
// The default scaling is for HotWheels and miles/hour = 2.23693 * 64 = 143.163
var speed = 143.163;    
// Other common scaling factors
// HotWheels and km/hour = 3.6 * 64 = 230.4
// ?speed=230.4
// Pinewood Derby and miles/hour = 2.23693 * 24 = 53.6863
// ?speed=53.6863
// Pinewood Derby and km/hour = 3.6 * 24 = 
// ?speed=86.4

// =============================================================================
// InqPortal callback when communications are ready.
onConnected = function(ws)
{ 
    // If tracks = null, this is the first or only 
    // InqSpeedRacer server and ws is our primary.
    if (!tracks)
    {
        // Get our speed scaling factor.  Default converts
        // meters/sec from server to HotWheels scale miles/hour.
        let s = getparam('speed');
        if (s)
        {
            s = Number(s);
            if (s && (s > 0))
                scale = s;
        }        
        // Create connections to secondary servers if there are any.
        let ts = getparam('tracks');
        if (ts)
        {
            ts = ts.split(',');
            if (ts.length)
            {
                tracks = [];
                addTrack(ws);
                for(let i=0; i<ts.length; i++)
                    addTrack(new InqWS(ts[i]));
            }
        }
    }
    
    // Request the InqSpeedRacer Version
    ws.get(['V']);
    // Start the calibration process
    if (tracks)
        calibrate(ws);
    else
        newRace();
};

// =============================================================================
// InqPortal callback allowing us to intercept published
// variables.  Use them as needed, but return modified or original.
onModifyResult = function(p, v, ws)
{
    let track;
    switch (p)
    {
        case 'T':
            calibrate(ws, v);
            break;
            
        case 'S':
            track = getTrack(ws);
            if (track)
                track.row.cells[1].innerText = (v * speed).toFixed(1);
            break;
            
        case 'F':
            setBackground('CheckeredFlag.png');
            track = getTrack(ws);
            track.snd = v;  // Store in case we need to re-do it.
            let cell = track.row.cells[2];
            if (track)
            {
                v -= track.rel; 
                if (!finish)
                {
                    cell.innerText = 'First';
                    finish = v;
                }
                else if (v < finish)
                {
                    // This is very rare!  The first one that was received 
                    // was not the earliest across a line.  Have to recalc
                    // finish and then re-do all that came before.
                    finish = v;
                    for (let i=0; i<tracks.length; i++)
                    {
                        let t = tracks[i];
                        // Has it been changed.
                        if (t.row.cells[2].innerText.length)  
                            onModifyResult('F', t.snd, t.ws);   // Recurse.
                    }
                    cell.innerText = 'First';
                }
                else if (v - finish < 1)        // 1 ms
                    cell.innerText = 'First';   // Call it a tie!                 
                else
                    cell.innerText = '+' + ((v - finish) / 1000).toFixed(3);
            }
            break;
            
        case 'V':
            v = "InqSpeedRacer v" + v;
            break;
    }
    return v;
};

// =============================================================================

function calibrate(ws, T)
{
    // Calibration involves determining the relative time of all
    // servers with respect to this client.  That way when the finish
    // results come in, we can normalize them and determin a winner.
    // Also, noting that TCP has much variability, we don't want
    // to use some round trips that may exceed 100 ms, so we keep
    // asking till we get one we like.      
    let track = getTrack(ws);
    
    if (T)  // Can't process the initial call.
    {
        let n = performance.now();
        // Assume the server hack was half round trip time.
        let hrt = (n - track.snd) / 2;
        if (hrt < track.hrt)
        {
            // Getting a better hack.
            track.hrt = hrt;
            // Increase the hack time by half round trip to 
            // get current time now.  Subtract off the
            // relative time to when client started
            track.rel = T + hrt - n;
            console.log('ws=' + ws.url + '  hrt=' + 
                Math.round(hrt) + '  rel=' + Math.round(track.rel));
        }
    }
    if ((track.hrt > 1) &&                      // If half round trip > 1 ms
        (performance.now() - track.cal < 7000)) // Maximum of 7 seconds
    {
        // Keep sending the request out till we get a quick roundtrip.
        // We send out at random times so if we have a bunch of severs 
        // the results aren't all piling up here at the same time.
        setTimeout((tr)=>
        { 
            tr.snd = performance.now();
            tr.ws.get(['T']); 
        }, Math.random() * 100, track);
    }
    else
    {
        // This channel is calibrated and ready to go.
        console.log('ws=' + ws.url + '  hrt=' + track.hrt.toFixed(2) + 
            ' Calibrated');
        track.hrt = 0;
        for (let i=0; i<tracks.length; i++)
            if (tracks[i].hrt)
                return;     // Some are still calibrating.
        newRace();
    }        
};

// =============================================================================

function newRace()
{
    visible($('cal'), false);
    setBackground('GreenFlag.png');
    finish = 0; 
    let races = $('races');
    race++;
    let r = races.insertRow(1);
    r.innerHTML = "<th colspan='3' class='center'>Race " + race + "</th>";
    
    for (let i=0; i<tracks.length; i++)
    {
        r = races.insertRow(2 + i);
        let l = tracks[i].ws.url;
        r.insertCell(0).innerText = l.substring(5, l.length - 4);
        r.insertCell(1);
        r.insertCell(2);
        tracks[i].row = r;
    }
};

// =============================================================================

function setBackground(file)
{
    document.body.style.backgroundImage = "url('" + file + "')";
};

// =============================================================================

function getTrack(ws)
{
    for (let i=0; i<tracks.length; i++)
        if (tracks[i].ws.url == ws.url)
        {
            tracks[i].ws = ws;
            return tracks[i];
        }
    return null;
};

// =============================================================================

function addTrack(ws)
{
    var track = getTrack(ws);
    if (track)
        return; // Don't re-create one.
    track = {};
    track.ws = ws;                  // WebSocket
    track.cal = performance.now();  // Time calibration started
    track.snd = 0;                  // When a request for hack went out.
    track.hrt = 1E6;                // Half Round Trip Time
    track.rel = 0;                  // Our relative normalizer
    track.row = null;               // Current race row
    tracks.push(track);                
};

// =============================================================================
