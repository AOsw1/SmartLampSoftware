//Bilder in Dateisystem laden und aufrufen
// https://microcontrollerslab.com/upload-files-esp32-spiffs-vs-code-platformio-ide/

//slider
//https://randomnerdtutorials.com/esp32-esp8266-web-server-physical-button/

//https://www.favicon.cc/
//https://base64.guru/converter/encode/image/ico


const char index_html[] PROGMEM = R"rawliteral(
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
	<input type='color' id="color" onchange='myFunctionCOLOR()'><br>

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