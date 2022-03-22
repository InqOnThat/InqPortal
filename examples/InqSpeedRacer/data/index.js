// Copyright (c) 2021 Dennis E. Cox. All rights reserved.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

var _conSpd = 2.23694;  // (m/s) to (mph)
var _unitsSpd = 'mph';  // Units to display for speed
var _conHgt = 39.3701;  // (m) to (in)
var _unitsHgt = 'in';   // Units to display for height
var _scale = 64.0;      // x64 scale speed of HotWheels

onConnected = function(ws)
{ 
    ws.get(['V']); 

    // Check our query string to configure
    var s = getparam('units');
    if (s == 'metric')
    {
        _conSpd = 3.6;      // (m/s) to (kph)
        _unitsSpd = 'kph';  // Units to display for speed
        _conHgt = 100;      // (m) to (cm)
        _unitsHgt = 'cm';   // Units to display for height
        $('HS').innerText = 'Speed(kph)';
        $('HSS').innerText = 'Scale(kph)';
        $('HH').innerText = 'Height(cm)';
    }
    _conSpd = getNumParam('conSpd', _conSpd);
    _conHgt = getNumParam('conHgt', _conHgt);
    _scale = getNumParam('scale', _scale);
    s = getparam('unitsSpd');
    if (s)
      _unitsSpd = s;
    s = getparam('unitsHgt');
    if (s)
      _unitsHgt = s;
};

function getNumParam(name, dflt, min, max)
{
    var s = getparam(name);
    if (!s)
        return dflt;
    s = parseFloat(s);
    if (isNaN(s))
        return dflt;
    return Math.max(0.01, Math.min(1000, s));
}

onModifyResult = function(p, v)
{
    if (p == 'S')
    {
        // Get start height
        var sh = v * v / 2 / 9.81 * _conHgt;

        // Converted Speed
        var s = v * _conSpd;
        
        // Scaled Speed
        var ss = s * _scale;
        
        addScore(s, ss, sh);
        $('UNITS').innerText = _unitsSpd;
    }
    else if (p == 'T')
    {
        var t = new Date(Number(v));
        v = t.getHours(t) % 12;
        v = v ? v : 12;
        v += ':' + t.getMinutes().pad(2);        
    }
    else if (p == 'V')
        v = "InqSpeedRacer v" + v;
    return v;
};

function addScore(s, ss, sh)
{
    $('S9').innerText = $('S8').innerText;
    $('S8').innerText = $('S7').innerText;
    $('S7').innerText = $('S6').innerText;
    $('S6').innerText = $('S5').innerText;
    $('S5').innerText = $('S4').innerText;
    $('S4').innerText = $('S3').innerText;
    $('S3').innerText = $('S2').innerText;
    $('S2').innerText = $('S1').innerText;
    $('S1').innerText = $('S0').innerText;
    $('S0').innerText =  Math.round(ss);
    
    var tl = $('log');
    if (!tl)
        return;
    var r = tl.insertRow();
    r.insertCell(0).innerHTML = s.toFixed(2);
    r.insertCell(1).innerHTML = ss.toFixed(2)
    r.insertCell(2).innerHTML = sh.toFixed(2)
    while (tl.rows.length > 11)
        tl.deleteRow(1);
};
    

