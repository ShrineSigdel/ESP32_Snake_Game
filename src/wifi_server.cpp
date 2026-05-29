#include "wifi_server.h"
#include "display.h"
#include "game.h"
#include <AsyncTCP.h>

#define HOME_SSID "YOUR_WIFI_SSID"
#define HOME_PASS "YOUR_WIFI_PASSWORD"

static AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Snake</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background: #111;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      font-family: monospace;
      color: #fff;
    }
    h2 { font-size: 1.4rem; letter-spacing: 3px; margin-bottom: 8px; }
    #status { font-size: 0.8rem; color: #555; margin-bottom: 8px; }
    #score  { font-size: 1rem; color: #aaa; margin-bottom: 32px; }
    .dpad {
      display: grid;
      grid-template-columns: repeat(3, 64px);
      grid-template-rows: repeat(3, 64px);
      gap: 6px;
    }
    .dpad button {
      background: #222;
      border: 1px solid #333;
      color: #fff;
      font-size: 1.4rem;
      border-radius: 10px;
      cursor: pointer;
    }
    .dpad button:active { background: #444; }
    .empty { visibility: hidden; }
    #restartBtn {
      display: none;
      margin-top: 28px;
      padding: 10px 28px;
      background: transparent;
      color: #fff;
      border: 1px solid #444;
      font-size: 0.9rem;
      font-family: monospace;
      cursor: pointer;
      letter-spacing: 2px;
    }
    #restartBtn:hover { border-color: #fff; }
    #hint { font-size: 0.75rem; color: #444; margin-top: 20px; }
  </style>
</head>
<body>
  <h2>SNAKE</h2>
  <div id="status">connecting...</div>
  <div id="score">score: 0</div>

  <div class="dpad">
    <div class="empty"></div>
    <button id="up">↑</button>
    <div class="empty"></div>
    <button id="left">←</button>
    <div class="empty"></div>
    <button id="right">→</button>
    <div class="empty"></div>
    <button id="down">↓</button>
    <div class="empty"></div>
  </div>

  <button id="restartBtn">RESTART</button>
  <div id="hint">or use arrow keys</div>

  <script>
    const statusEl  = document.getElementById('status');
    const scoreEl   = document.getElementById('score');
    const restartBtn = document.getElementById('restartBtn');
    let scoreVal = 0;
    let ws;

    function connect() {
      ws = new WebSocket('ws://' + location.hostname + '/ws');
      ws.onopen  = () => statusEl.textContent = 'connected';
      ws.onclose = () => {
        statusEl.textContent = 'disconnected — retrying...';
        setTimeout(connect, 2000);
      };
      ws.onmessage = (e) => {
        if (e.data === 'gameover') restartBtn.style.display = 'block';
        if (e.data === 'restart')  { restartBtn.style.display = 'none'; scoreVal = 0; scoreEl.textContent = 'score: 0'; }
        if (e.data === 'score')    { scoreVal++; scoreEl.textContent = 'score: ' + scoreVal; }
      };
    }
    connect();

    function send(msg) {
      if (ws && ws.readyState === WebSocket.OPEN) ws.send(msg);
    }

    // d-pad buttons
    document.getElementById('up').addEventListener('click',    () => send('up'));
    document.getElementById('down').addEventListener('click',  () => send('down'));
    document.getElementById('left').addEventListener('click',  () => send('left'));
    document.getElementById('right').addEventListener('click', () => send('right'));

    // touch support for d-pad
    ['up','down','left','right'].forEach(id => {
      document.getElementById(id).addEventListener('touchstart', (e) => {
        e.preventDefault();
        send(id);
      });
    });

    // keyboard
    document.addEventListener('keydown', (e) => {
      if (['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].includes(e.code)) {
        e.preventDefault();
      }
      if (restartBtn.style.display === 'block') {
        send('restart'); return;
      }
      const map = {
        ArrowUp:    'up',
        ArrowDown:  'down',
        ArrowLeft:  'left',
        ArrowRight: 'right'
      };
      if (map[e.code]) send(map[e.code]);
    });

    restartBtn.addEventListener('click', () => send('restart'));
  </script>
</body>
</html>
)rawliteral";

static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_DATA)
    {
        String msg = "";
        for (size_t i = 0; i < len; i++)
            msg += (char)data[i];

        if (msg == "up")
            game_set_direction(0, -1);
        if (msg == "down")
            game_set_direction(0, 1);
        if (msg == "left")
            game_set_direction(-1, 0);
        if (msg == "right")
            game_set_direction(1, 0);

        // any direction key starts the game
        if ((msg == "up" || msg == "down" ||
             msg == "left" || msg == "right") &&
            !game_is_over())
        {
            game_start();
        }

        if (msg == "restart" && game_is_over())
        {
            game_reset();
            ws_broadcast("restart");
        }
    }
}

void ws_broadcast(const char *msg)
{
    ws.textAll(msg);
}

void ws_cleanup()
{
    ws.cleanupClients();
}

void wifi_init()
{
    Serial.println("wifi_init start");
    WiFi.mode(WIFI_STA);
    Serial.println("mode set");
    WiFi.begin(HOME_SSID, HOME_PASS);
    Serial.println("wifi begin called");

    display_clear();
    display_draw_text(0, 20, "Connecting to");
    display_draw_text(0, 35, "WiFi...");
    display_render();

    while (WiFi.status() != WL_CONNECTED)
        delay(500);

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    display_show_ip(WiFi.localIP().toString().c_str());
    delay(3000);

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *req)
              { req->send(200, "text/html", INDEX_HTML); });
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *req)
              { req->send(204); });
    server.begin();
}