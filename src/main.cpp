#include <WiFi.h>
#include <WebServer.h>

// ── Pin Definitions ──────────────────────
#define PIR_PIN    13
#define RELAY_PIN  12
#define LDR_PIN    34
#define BTN_PIN    35

// ── Timing ───────────────────────────────
#define TIMEOUT_MS   300000UL   // 5 min auto-off
#define OVERRIDE_MS 1800000UL   // 30 min manual override

// ── WiFi ─────────────────────────────────
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

WebServer server(80);

// ── State ─────────────────────────────────
unsigned long lastMotionTime = 0;
unsigned long overrideUntil  = 0;
bool   relayState  = false;
int    motionCount = 0;
String lastEvent   = "System booted";

// ─────────────────────────────────────────
void setCORS() {
  server.sendHeader("Access-Control-Allow-Origin",  "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void setRelay(bool state, String reason) {
  if (relayState != state) {
    relayState = state;
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
    lastEvent = reason;
    Serial.println((state ? "[ON ] " : "[OFF] ") + reason);
  }
}

// ── Route: GET / ─────────────────────────
void handleRoot() {
  setCORS();
  server.send(200, "application/json",
    "{\"status\":\"Smart Hostel API running\","
    "\"routes\":[\"/data\",\"/relay?state=on\",\"/relay?state=off\"]}");
}

// ── Route: GET /data ─────────────────────
void handleData() {
  setCORS();
  int  ldr    = analogRead(LDR_PIN);
  bool isDark = ldr < 2000;
  bool motion = digitalRead(PIR_PIN);

  String json = "{";
  json += "\"relay\":"        + String(relayState ? "true" : "false")  + ",";
  json += "\"motion\":"       + String(motion     ? "true" : "false")  + ",";
  json += "\"ldr\":"          + String(ldr)                            + ",";
  json += "\"isDark\":"       + String(isDark      ? "true" : "false") + ",";
  json += "\"motionCount\":"  + String(motionCount)                    + ",";
  json += "\"override\":"     + String(millis() < overrideUntil ? "true" : "false") + ",";
  json += "\"lastEvent\":\""  + lastEvent + "\"";
  json += "}";

  server.send(200, "application/json", json);
}

// ── Route: POST /relay?state=on|off ──────
void handleRelay() {
  setCORS();
  String state = server.arg("state");

  if (state == "on") {
    overrideUntil = millis() + OVERRIDE_MS;
    setRelay(true, "Dashboard forced ON");
    server.send(200, "application/json", "{\"ok\":true,\"relay\":true}");
  }
  else if (state == "off") {
    overrideUntil = 0;
    setRelay(false, "Dashboard forced OFF");
    server.send(200, "application/json", "{\"ok\":true,\"relay\":false}");
  }
  else {
    server.send(400, "application/json",
      "{\"error\":\"use ?state=on or ?state=off\"}");
  }
}

// ── Route: OPTIONS (preflight CORS) ──────
void handleOptions() {
  setCORS();
  server.send(204);
}

// ── Route: anything else ──────────────────
void handleNotFound() {
  setCORS();
  Serial.print("[404] ");
  Serial.print(server.method() == HTTP_GET ? "GET " : "POST ");
  Serial.println(server.uri());
  server.send(404, "application/json", "{\"error\":\"not found\"}");
}

// ─────────────────────────────────────────
void setup() {
  pinMode(PIR_PIN,   INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BTN_PIN,   INPUT_PULLDOWN);
  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(115200);
  Serial.println("\n=============================");
  Serial.println(" Smart Hostel Energy Saver");
  Serial.println("=============================");

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[OK] WiFi connected!");
  Serial.print("[OK] IP Address : http://");
  Serial.println(WiFi.localIP());
  Serial.println("[OK] Wokwi URL : http://localhost:8180");
  Serial.println("-----------------------------");
  Serial.println("Routes:");
  Serial.println("  GET  /          -> API info");
  Serial.println("  GET  /data      -> sensor JSON");
  Serial.println("  POST /relay     -> ?state=on|off");
  Serial.println("=============================\n");

  // Register routes
  server.on("/",            HTTP_GET,     handleRoot);
  server.on("/data",        HTTP_GET,     handleData);
  server.on("/relay",       HTTP_POST,    handleRelay);
  server.on("/relay",       HTTP_OPTIONS, handleOptions);
  server.on("/favicon.ico", HTTP_GET,     []() { server.send(204); });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("[OK] Web server started");
}

// ─────────────────────────────────────────
void loop() {
  server.handleClient();

  int  ldr    = analogRead(LDR_PIN);
  bool isDark = ldr > 2000;
  bool motion = digitalRead(PIR_PIN);
  bool btn    = digitalRead(BTN_PIN);
  unsigned long now = millis();

  // Physical button override
  if (btn) {
    overrideUntil = now + OVERRIDE_MS;
    setRelay(true, "Physical button override");
  }

  // Skip auto logic during override
  if (now < overrideUntil) {
    delay(100);
    return;
  }

  // PIR + dark → turn ON
  if (motion && isDark) {
    lastMotionTime = now;
    motionCount++;
    setRelay(true, "Motion + dark detected");
  }

  // Timeout → turn OFF
  if (relayState && (now - lastMotionTime > TIMEOUT_MS)) {
    setRelay(false, "No motion for 5 min");
  }

  delay(100);
}