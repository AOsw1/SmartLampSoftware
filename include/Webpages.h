//Bilder in Dateisystem laden und aufrufen
// https://microcontrollerslab.com/upload-files-esp32-spiffs-vs-code-platformio-ide/

//slider
//https://randomnerdtutorials.com/esp32-esp8266-web-server-physical-button/

//https://www.favicon.cc/
//https://base64.guru/converter/encode/image/ico



//==========================================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>SmartLamp</title>

<style>
:root {
    --bg: #E6E9ED;         /* Kühles Silber-Grau */
    --text: #222;
    --primary: #4CAF50;
    --danger: #e53e3e;
    --accent: #60a5fa;
    --neutral: #fef08a;
    --radius: 12px;
    --shadow: 0 3px 8px rgba(0,0,0,0.1);
    font-family: system-ui, Arial, sans-serif;
}

/* Body & Layout */
body {
    margin: 0;
    background: var(--bg);
    color: var(--text);
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 0px 25px 0px 5px; 
    max-width: 600px;
    margin-inline: auto;
}

/* Header */
h1 {
    font-size: 2rem;
    margin-bottom: 0.3em;
}

.subtitle {
    font-size: 1.2rem;
    margin-bottom: 0.5em;
    opacity: 0.7;
}

/* Buttons */
.btn {
    width: 160px;
    padding: 12px;
    margin: 6px 0;
    font-size: 1rem;
    border: none;
    border-radius: var(--radius);
    cursor: pointer;
    box-shadow: var(--shadow);
    transition: transform .15s, background .3s;
}
.btn:active { transform: scale(0.97); }
.btn-yellow { background: var(--neutral); }
.btn-accent { background: var(--accent); color: white; }
.btn-primary { background: var(--primary); color: white; }
.btn-danger { background: var(--danger); color: white; }

/* HR */
hr {
    width: 60%;
    margin: 0.5em 0;
    opacity: 0.4;
}

/* Color Picker */
.color-picker {
    margin: 0.5em 0; 
    text-align: center;
}

.color-picker input[type="color"] {
    display: block;
    margin: 0 auto;
    width: 60px;
    height: 60px;
    border: none;
    padding: 0;
    cursor: pointer;
    border-radius: 8px;
}

/* Switch */
.switch-container {
    margin-top: 0em;
    text-align: center;
}

.switch {
    position: relative;
    display: inline-block;
    width: 80px;
    height: 42px;
}

.switch input {
    opacity: 0;
    width: 0; height: 0;
}

.slider {
    position: absolute;
    cursor: pointer;
    top: 0; left: 0; right: 0; bottom: 0;
    background-color: #ccc;
    border-radius: 40px;
    transition: .3s;
}

.slider:before {
    content: "";
    position: absolute;
    height: 34px; width: 34px;
    left: 4px; bottom: 4px;
    background-color: white;
    border-radius: 50%;
    transition: .3s;
}

input:checked + .slider {
    background-color: var(--primary);
}

input:checked + .slider:before {
    transform: translateX(38px);
}

/* Mobile Optimierungen */
@media (max-width: 480px) {
    .btn { width: 140px; padding: 10px; font-size: 0.95rem; }
    .color-picker input[type="color"] { width: 80px; height: 80px; }
}
</style>

<script>
// ---------- Helper AJAX ----------
function sendCommand(path) {
    const xhr = new XMLHttpRequest();
    xhr.open("GET", path, true);
    xhr.send();
}

// ---------- Color picker ----------
function sendColor() {
    let c = document.getElementById("color").value.substring(1);
    sendCommand("/?rgb=%23" + c);
}

// ---------- Toggle Switch ----------
function togglePower(el) {
    if (el.checked) sendCommand("/?ledon=EIN");
    else            sendCommand("/?ledoff=AUS");
}

// ---------- LED Status Polling ----------
setInterval(() => {
    const xhr = new XMLHttpRequest();
    xhr.onload = function () {
        document.getElementById("powerSwitch").checked = (this.responseText === "EIN");
    };
    xhr.open("GET", "/ledStatus", true);
    xhr.send();
}, 5000);
</script>

</head>
<body>

<h1>Smart-Lamp</h1>
<div class="subtitle">HTL Leonding</div>

<button class="btn btn-yellow" onclick='sendCommand("/?save=SAVE")'>SAVE</button>
<button class="btn btn-yellow" onclick='sendCommand("/?recall=RECALL")'>RECALL</button>
<button class="btn btn-accent" onclick='sendCommand("/?heller=HELLER")'>HELLER</button>
<button class="btn btn-accent" onclick='sendCommand("/?dunkler=DUNKLER")'>DUNKLER</button>

<hr>

<div class="color-picker">
    <p>Lichtfarbe auswählen</p>
    <input type="color" id="color"  value="#FFFFFF" onchange="sendColor()">
</div>

<hr>

<div class="switch-container">
    <p>An-/Aus-Schalter</p>
    <label class="switch">
        <input type="checkbox" id="powerSwitch" onchange="togglePower(this)">
        <span class="slider"></span>
    </label>
</div>

</body>
</html>

)rawliteral";



//===============
const char index_alt_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang='de'>
<HTML><HEAD>
<link href='data:image/x-icon;base64, AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAiDOsA9/X1AKEaEABMoRAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAzMyIiEREAADMzIiIREQAAMzMiIhERAAAzMyIiEREAADMzIiIREQAAMzMiIhERAAAzMyIiEREAADMzIiIREQAAMzMiIhERAAAzMyIiEREAADMzIiIREQAAMzMiIhERAAAzMyIiEREAADMzIiIREQAAMzMiIhERAAAzMyIiEREAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' rel='icon' type='image/x-icon' />
<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'>

  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
	
	.button1 {height:30px; width:120px; background-color: #00FF00; font-size:16px}
    .button2 {height:30px; width:120px; background-color: #FE0320; font-size:16px}
    .button4 {height:30px; width:120px; background-color: #87CEFA; font-size:16px}
    .button3 {height:30px; width:120px; background-color: #FFFFCC; font-size:16px}
	
    .switch {position: relative; display: inline-block; width: 96px; height: 56px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 40px; width: 40px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    
	.text .slider:after {
		/* Text vor dem FlipFlop-Schalter */
		
		position: absolute;
		content: "AUS";
		color: #ccc;
		font-weight: bold;
		height: 1.6em;
		left: -3.1em;
		bottom: 0.8em;
	}

	.text input:checked + .slider:after {
		/* Text hinter dem FlipFlop-Schalter */
		
		position: absolute;
		content: "AN";
		color: #b30000;
		left: 6.9em;
	}
	
	input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(40px); -ms-transform: translateX(40px); transform: translateX(40px)}
	
  </style> 
  
  <script>

    function myFunctionCOLOR() {
	    var myLink="";
       	var myCol=color.value;
		myCol=myCol.substring(1);
        var myLink="/?rgb=%23"+myCol;
        var xhr = new XMLHttpRequest();
        xhr.open("GET", myLink , true); 
        xhr.send();
    }

    function myFunction(myLink) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", myLink, true); 
        xhr.send();
    }

    function toggleCheckbox(element) {

        var xhr = new XMLHttpRequest();
        
        if(element.checked)
        { xhr.open("GET", "/?ledon=EIN", true); 
        }
        else 
        { xhr.open("GET", "/?ledoff=AUS", true); 
        }
        xhr.send();
    }

    setInterval(function ( ) {
        
		//LED Status abfragen und Ein/Aus Slider setzten
		var oReq = new XMLHttpRequest();
		oReq.addEventListener("load", reqListener);
		oReq.open("GET", "/ledStatus");
		oReq.send();
		
    }, 5000 ) ;

	function reqListener () {
		//alert("Listener "+ this.responseText);
		if (this.responseText=="EIN") {myDIV2.checked  = true;}
		if (this.responseText=="AUS") {myDIV2.checked  = false;}
	}

  </script>

  <title>SmartLamp</title>
</HEAD>

<BODY bgcolor='#FFFF99' text='#000000'>
	<CENTER>
	<FONT size='5' FACE='Verdana'><br><br>HTL-Leonding<BR>Smart-Lamp</FONT>
	<BR><BR><BR>

	<input class='button3' name='save' type='submit' value='SAVE' onclick='myFunction("/?save=SAVE")'><br>
	<input class='button3' name='recall' type='submit' value='RECALL' onclick='myFunction("/?recall=RECALL")'><br>
	<input class='button4' name='heller' type='submit' value='HELLER' onclick='myFunction("/?heller=HELLER")'><br>
	<input class='button4' name='dunkler' type='submit' value='DUNKLER' onclick='myFunction("/?dunkler=DUNKLER")'><br>

	<br><HR width=50%>
	
	<FONT size='2' FACE='Verdana'>Lichtfarbe ausw&auml;hlen</font>
	<br><br>
	<input type='color' id="color"  value="#FFFFFF" onchange='myFunctionCOLOR()'><br>

	<HR width=50%><BR>
	
	<div class="switch text">
		<label> An-/Aus-Schalter
			<input type='checkbox' onchange='toggleCheckbox(this)' id='myDIV2' > <span class="slider"></span> </label>
	</div>

	</CENTER>
	<BR>
	
</body>
</HTML>

)rawliteral";




//==============================================================================================================================================
//==============================================================================================================================================

const char SETUP_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP SmartLamp %espBeschreibung% SETUP</title>
<style>
  body {
    font-family: system-ui, sans-serif;
    background: #f4f7fa;
    color: #222;
    margin: 0;
    padding: 1.5em;
    text-align: center;
  }

  h2, h3 {
    color: #5b2b72ff;
    margin-top: 1em;
  }

  form {
    background: white;
    border-radius: 12px;
    padding: 1.5em;
    max-width: 500px;
    margin: 1em auto;
    box-shadow: 0 4px 8px rgba(0,0,0,0.1);
    text-align: left;
  }

  label {
    display: block;
    font-weight: 600;
    margin-top: 1em;
  }

  input[type="text"], input[type="password"], select {
    width: 100%%;
    padding: 0.7em;
    margin-top: 0.3em;
    font-size: 1em;
    border: 1px solid #8370caff;
    border-radius: 8px;
    box-sizing: border-box;
  }

  input[readonly] {
    background: #eee;
    color: #666;
  }

  input[type="submit"] {
    width: 100%%;
    margin-top: 1.5em;
    padding: 0.9em;
    font-size: 1.1em;
    border: none;
    border-radius: 8px;
    background-color: #b30000;
    color: white;
    cursor: pointer;
    transition: background 0.2s;
  }

  input[type="submit"]:hover {
    background-color: #900000;
  }

  a {
    display: inline-block;
    margin-top: 1.5em;
    color: #b30000;
    text-decoration: none;
  }

  a:hover {
    text-decoration: underline;
  }

  /* Responsive kleinere Bildschirme */
  @media (max-width: 400px) {
    h2, h3 { font-size: 1.2em; }
    input, select { font-size: 0.95em; }
  }
</style>
</head>
<body>

<h2>ESP SmartLamp</h2>

<hr>
<h3>SmartLamp Beschreibung</h3>
<form action="/setup" method="post">
  <label>Beschreibung, Titeltext der Webseiten:</label>
  <input type="text" name="espBeschreibung" value="%espBeschreibung%">
  <input type="submit" value="Speichern">
</form>

<hr>
<h3>MQTT Einstellungen</h3>
<form action="/setup" method="post">
  <label>Host:</label>
  <input type="text" name="mqttLink" value="%mqttLink%">

  <label>User:</label>
  <input type="text" name="mqttUser" value="%mqttUser%">

  <label>KW:</label>
  <input type="text" name="mqttKW" value="%mqttKW%">

  <label>Topic:</label>
  <input type="text" name="mqttTopic" value="%mqttTopic%">

  <label>Sendeintervall min:</label>
  <input type="text" name="mqttIntervall" value="%mqttIntervall%">

  <small><br>Achtung: Sendeintervall darf keine Kommastellen enthalten! <br>0 = keine MQTT-Daten</small><br>
  Farben per mqtt setzen mit den Topics: setR, setG und setB
  <input type="submit" value="Speichern">
</form>


<hr>
<a href="\">Startseite</a>
<br><br>
<small>Webcounter %COUNTER%</small>

</body>
</html>

)rawliteral";






//==============================================================================================================================================
//==============================================================================================================================================
const char SETUP_old_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP SmartLamp %espBeschreibung% SETUP</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 96px; height: 56px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 40px; width: 40px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(40px); -ms-transform: translateX(40px); transform: translateX(40px)}
  </style>
</head>
<body>
  <h2>ESP SmartLamp</h2>
  
  <hr><h3>Einstellungen</h3>
  <form action="/setup"  method="post">

  <h3>SmartLamp Beschreibung</h3>
  Diese Beschreibung wird in den &Uuml;berschriften und Titelzeilen der Webseiten angezeigt.
  <form action="/setup"  method="post">
    <table align=center border=0>
        <td align=left>Beschreibung : <br><br><td><input type="text" name="espBeschreibung" value="%espBeschreibung%"><br><br>
    </table> 
    <br><input type="submit" value="Submit">
  </form><br><hr>

  <h3>mqtt Einstellungen</h3>
  <form action="/setup"  method="post">

  <table align=center border=0>
  <tr>
      <td align=left>Host : <br><br><td><input type="text" name="mqttLink" value="%mqttLink%"><br><br> 
  <tr>
      <td align=left>User : <br><br><td><input type="text" name="mqttUser" value="%mqttUser%"><br><br>
  <tr>
      <td align=left>KW : <br><br><td><input type="text" name="mqttKW" value="%mqttKW%"><br><br>
  <tr>
      <td align=left>Topic : <br><br><td><input type="text" name="mqttTopic" value="%mqttTopic%"><br><br>
  <tr>
      <td align=left>Sendeintervall min : <br><br><td><input type="text" name="mqttIntervall" value="%mqttIntervall%"><br><br>    
  </table> 
  Achtung: Sendeintervall darf keine Kommastellen enthalten! <br>0=keine Mqtt-Daten versenden
  <br><br><input type="submit" value="Submit">

  </form><br><hr>

  <br> <a href="\">SmartLamp Startseite</a><br>
  <br><br> Webcounter %COUNTER%
</body>
</html>
)rawliteral";