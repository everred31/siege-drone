const char* header = R"===(
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Connection: close
)===";

const char* page = R"HTML(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <title>vroom shi</title>

    <style>
        body{
            user-select: none;
            text-align: center;
        }
        .drive{
            user-select: none;
            height: 100px;
            width: 300px;
            touch-action: manipulation;
            -webkit-touch-callout: none; 
            -webkit-user-select: none;
            border-radius: 8px;  
            user-select: none;         

        }
        .header{
            display: flex;
            align-items: center;
            text-align: center;
            justify-content: space-between;
            padding:10px;
        }
        .light{
            border-radius: 8px;
            user-select: none;
            height: 50px;
            width: 50px;
            -webkit-user-select: none;
            user-select: none;
        }
        .control-top{
            display: flex;
            align-items: center;
            text-align: center;
            justify-content: center;
        }
        .main{
            height: 80%;
        }
        .rotate{
            rotate: -90deg;
        }
        .stream{ /*div for video - 320x240px*/ 
            margin-top: 100px;
            height: 360px;
            width: 480px;
            
                }
        .video{ /*class for video*/
            height: 100%;
            width: 100%;
        }
        .control{
            position: fixed; 
            bottom: 10pt; 
            width:96%;
            text-align: center;
            margin: auto;
        }
    </style>
  </head>
  <body>
    <div>
        <div class="header">
            <h1>drive or sum</h1>
            <button class = "light" id="light" onclick="toggleLight()">L</button>
        </div>


        <div class="main">
            <div class="rotate">
                <div class="stream">
                    <img src="" id="camStream" class ="video">
                </div>
            </div>
            
            <div class ="control">
                <div class = "control-top">
                    <button class="drive" id="left" onmousedown="sendCommand('left')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('left')" ontouchend="sendCommand('stop')"> l </button>
                    <button class="drive" id="for" onmousedown="sendCommand('forward')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('forward')" ontouchend="sendCommand('stop')"> f </button>
                    <button class="drive" id="right" onmousedown="sendCommand('right')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('right')" ontouchend="sendCommand('stop')"> r </button>
                </div>

                <div class = "control-bot">
                    <button class="drive" id="back" onmousedown="sendCommand('backward')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('backward')" ontouchend="sendCommand('stop')"> b </button>
                </div>
            </div>

            
        </div>
        
    </div>

  </body>

  <script>
    // stream server lives on port 81, control server on port 80 (this page's own port)
    document.getElementById('camStream').src =
        "http://" + window.location.hostname + ":81/stream";

    function sendCommand(action){
        fetch("/" + action).catch(err => console.log(err));
    }
    let toggle = false;
    function toggleLight(){
        if (toggle == false) {
            toggle = true;
            fetch("/lightOn").catch(err => console.log(err));
        }
        else if (toggle == true){
            toggle = false;
            fetch("/lightOff").catch(err => console.log(err));
        }
    }
  </script>
</html>
)HTML";