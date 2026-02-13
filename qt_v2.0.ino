#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

// ==================== HARDWARE ====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SDA_PIN 6
#define SCL_PIN 7
#define LED_PIN 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);
Preferences prefs;

// ==================== AP CONFIG ====================
const char* ap_ssid = "QT-Robot";
const char* ap_password = "qt@mintfire";

// ==================== WIFI STA ====================
String sta_ssid = "";
String sta_pass = "";
bool staConnected = false;

// ==================== WEATHER ====================
String apiKey = "";
String city = "";
unsigned long weatherInterval = 600000;
unsigned long lastWeatherFetch = 0;
float wTemp = 0; float wHumid = 0; int wAqi = 0;
String wDesc = "";
float wLat = 0, wLon = 0;
bool weatherValid = false;

// ==================== TIME ====================
bool timeSynced = false;

// ==================== TEXT BANNER ====================
String bannerText = "Hello World!";
int animType = 0; // 0=static,1=scrollL,2=scrollR,3=bounce,4=typewriter,5=blink

// ==================== SCREEN ROTATION ====================
bool screenOn[4] = {true, true, true, true};
unsigned long rotInterval = 10000;
int curScreen = 0;
unsigned long lastRotation = 0;

// ==================== ANIMATION STATE ====================
int scrollX = SCREEN_WIDTH;
int bounceX = 0, bounceDir = 1;
int twPos = 0;
bool blinkOn = true;
unsigned long lastAnim = 0;
unsigned long lastStaticDraw = 0;

// ==================== HACKER ANIM ====================
int hackerFrame = 0;
uint8_t rainDrops[22];
unsigned long lastHacker = 0;

// ==================== WIFI RECONNECT ====================
unsigned long lastWifiCheck = 0;
const unsigned long wifiCheckInterval = 30000; // check every 30s

// ==================== HTML HELPERS ====================
String pageHead(String title, int active) {
  String h = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>QT Robot - " + title + "</title><style>"
    "*{margin:0;padding:0;box-sizing:border-box}"
    "body{font-family:'Segoe UI',Arial,sans-serif;background:#0d1117;color:#c9d1d9;min-height:100vh}"
    ".nav{display:flex;background:#161b22;border-bottom:1px solid #30363d;padding:0 8px}"
    ".nav a{color:#8b949e;text-decoration:none;padding:12px 14px;font-size:13px;font-weight:600;border-bottom:2px solid transparent;transition:.2s}"
    ".nav a:hover{color:#c9d1d9}.nav a.ac{color:#58a6ff;border-bottom-color:#58a6ff}"
    ".wrap{max-width:500px;margin:0 auto;padding:16px}"
    ".card{background:#161b22;border:1px solid #30363d;border-radius:12px;padding:20px;margin-bottom:14px}"
    ".card h2{color:#58a6ff;font-size:16px;margin-bottom:12px}"
    ".row{display:flex;gap:8px;margin-bottom:8px}"
    ".box{flex:1;background:#0d1117;border:1px solid #30363d;border-radius:8px;padding:10px;text-align:center}"
    ".box .lbl{font-size:10px;color:#8b949e;text-transform:uppercase;letter-spacing:1px}"
    ".box .val{font-size:14px;font-weight:700;color:#58a6ff;margin-top:2px}"
    "label{display:block;font-size:13px;color:#8b949e;margin:12px 0 4px;font-weight:600}"
    "input[type=text],input[type=password],select,textarea{width:100%;padding:10px;background:#0d1117;border:1px solid #30363d;border-radius:8px;color:#c9d1d9;font-size:14px;outline:none}"
    "input:focus,select:focus,textarea:focus{border-color:#58a6ff}"
    "textarea{min-height:70px;resize:vertical;font-family:monospace}"
    ".btn{display:block;width:100%;padding:12px;background:#238636;color:#fff;border:none;border-radius:8px;font-size:14px;font-weight:600;cursor:pointer;text-align:center;margin-top:14px;transition:.2s}"
    ".btn:hover{background:#2ea043}.btn-blue{background:#1f6feb}.btn-blue:hover{background:#388bfd}"
    ".tgl{display:flex;align-items:center;justify-content:space-between;padding:10px 0;border-bottom:1px solid #21262d}"
    ".tgl:last-child{border:none}"
    ".sw{position:relative;width:42px;height:24px;flex-shrink:0}"
    ".sw input{opacity:0;width:0;height:0}"
    ".sl{position:absolute;cursor:pointer;inset:0;background:#30363d;border-radius:24px;transition:.3s}"
    ".sl:before{position:absolute;content:'';height:18px;width:18px;left:3px;bottom:3px;background:#fff;border-radius:50%;transition:.3s}"
    "input:checked+.sl{background:#238636}input:checked+.sl:before{transform:translateX(18px)}"
    ".net{padding:8px 12px;margin:4px 0;background:#0d1117;border:1px solid #30363d;border-radius:8px;cursor:pointer;display:flex;justify-content:space-between;transition:.2s}"
    ".net:hover{border-color:#58a6ff}"
    ".tag{display:inline-block;padding:2px 8px;border-radius:20px;font-size:11px;font-weight:600}"
    ".tg{background:#0d3d1e;color:#3fb950}.tr{background:#3d0d0d;color:#f85149}"
    ".sub{font-size:13px;color:#8b949e;margin-bottom:12px}"
    "</style></head><body><div class='nav'>";
  String p[] = {"Home","WiFi","API","Display"};
  String u[] = {"/","/wifi","/api","/display"};
  for(int i=0;i<4;i++) h+="<a href='"+u[i]+"'"+(i==active?" class='ac'":"")+">"+p[i]+"</a>";
  h += "</div><div class='wrap'>";
  return h;
}

String pageFoot() { return "</div></body></html>"; }

// ==================== WIFI ====================
void startAP() {
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  WiFi.softAPConfig(IPAddress(192,168,4,1),IPAddress(192,168,4,1),IPAddress(255,255,255,0));
  WiFi.softAP(ap_ssid, ap_password, 1, 0, 4);
  delay(500);
  Serial.println("AP: " + String(ap_ssid) + " IP: " + WiFi.softAPIP().toString());
}

void connectSTA() {
  if (sta_ssid.length()==0) return;
  Serial.println("Connecting to: " + sta_ssid);
  
  // Show connection status on OLED
  display.clearDisplay();
  display.setTextSize(1);display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,15);display.print("Connecting WiFi...");
  display.setCursor(10,30);display.print(sta_ssid.substring(0,16));
  display.display();
  
  WiFi.begin(sta_ssid.c_str(), sta_pass.c_str());
  int tries=0;
  while(WiFi.status()!=WL_CONNECTED && tries<20){
    delay(500); tries++;
    display.setCursor(10,45);display.print(String(20-tries)+"s  ");
    display.display();
  }
  if(WiFi.status()==WL_CONNECTED){
    staConnected=true;
    Serial.println("WiFi connected: "+WiFi.localIP().toString());
    // Sync NTP time
    configTime(19800,0,"pool.ntp.org","time.nist.gov");
    delay(2000);
    struct tm t;
    if(getLocalTime(&t,5000)){
      timeSynced=true;
      Serial.println("NTP time synced");
    }
    display.clearDisplay();
    display.setCursor(10,15);display.print("WiFi Connected!");
    display.setCursor(10,30);display.print("IP: "+WiFi.localIP().toString());
    display.setCursor(10,45);display.print("AP still active");
    display.display();delay(1500);
  } else {
    staConnected=false;
    Serial.println("WiFi connection failed");
    display.clearDisplay();
    display.setCursor(10,20);display.print("WiFi Failed");
    display.setCursor(10,35);display.print("AP: "+String(ap_ssid));
    display.setCursor(10,50);display.print(WiFi.softAPIP().toString());
    display.display();delay(1500);
  }
}

void checkWifiReconnect() {
  if(sta_ssid.length()==0) return;
  if(millis()-lastWifiCheck < wifiCheckInterval) return;
  lastWifiCheck=millis();
  
  if(WiFi.status()!=WL_CONNECTED){
    staConnected=false;
    Serial.println("WiFi lost, reconnecting...");
    WiFi.disconnect();
    delay(100);
    WiFi.begin(sta_ssid.c_str(), sta_pass.c_str());
    int tries=0;
    while(WiFi.status()!=WL_CONNECTED && tries<10){delay(500);tries++;}
    if(WiFi.status()==WL_CONNECTED){
      staConnected=true;
      Serial.println("WiFi reconnected: "+WiFi.localIP().toString());
      if(!timeSynced){
        configTime(19800,0,"pool.ntp.org","time.nist.gov");
        struct tm t;
        if(getLocalTime(&t,3000)) timeSynced=true;
      }
    }
  }
}

// ==================== WEATHER ====================
void fetchWeather() {
  // Re-check WiFi status before API call
  if(WiFi.status()!=WL_CONNECTED){staConnected=false;return;}
  if(apiKey.length()==0||city.length()==0) return;
  
  Serial.println("Fetching weather for: "+city);
  HTTPClient http;
  http.setTimeout(10000); // 10s timeout
  
  // OpenWeatherMap Current Weather API
  String url="http://api.openweathermap.org/data/2.5/weather?q="
    +city+"&appid="+apiKey+"&units=metric";
  http.begin(url);
  int code=http.GET();
  
  if(code==200){
    String body=http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError err=deserializeJson(doc,body);
    if(!err){
      wTemp=doc["main"]["temp"].as<float>();
      wHumid=doc["main"]["humidity"].as<float>();
      wDesc=doc["weather"][0]["description"].as<String>();
      wLat=doc["coord"]["lat"].as<float>();
      wLon=doc["coord"]["lon"].as<float>();
      weatherValid=true;
      Serial.printf("Weather: %.1fC, %.0f%%, %s\n",wTemp,wHumid,wDesc.c_str());
    } else {
      Serial.println("JSON parse error: "+String(err.c_str()));
    }
  } else {
    Serial.println("Weather API error: "+String(code));
  }
  http.end();
  
  // OpenWeatherMap Air Pollution API (for AQI)
  if(weatherValid && wLat!=0){
    String aUrl="http://api.openweathermap.org/data/2.5/air_pollution?lat="
      +String(wLat,6)+"&lon="+String(wLon,6)+"&appid="+apiKey;
    http.begin(aUrl);
    http.setTimeout(10000);
    code=http.GET();
    if(code==200){
      String body=http.getString();
      DynamicJsonDocument doc(512);
      DeserializationError err=deserializeJson(doc,body);
      if(!err){
        wAqi=doc["list"][0]["main"]["aqi"].as<int>();
        Serial.println("AQI: "+String(wAqi));
      }
    } else {
      Serial.println("AQI API error: "+String(code));
    }
    http.end();
  }
  lastWeatherFetch=millis();
}

// ==================== DISPLAY SCREENS ====================
void drawTimeScreen() {
  if(millis()-lastStaticDraw<500) return;
  lastStaticDraw=millis();
  struct tm t;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  if(!getLocalTime(&t)){
    display.setTextSize(1);
    display.setCursor(15,20);display.print("Time not synced");
    display.setCursor(10,38);display.print("Connect WiFi first");
    display.display(); return;
  }
  const char* days[]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
  display.setTextSize(1);
  display.setCursor(0,0);display.print(days[t.tm_wday]);
  char dateBuf[16]; strftime(dateBuf,sizeof(dateBuf),"%d %b %Y",&t);
  int16_t x1,y1; uint16_t w,h;
  display.getTextBounds(dateBuf,0,0,&x1,&y1,&w,&h);
  display.setCursor(SCREEN_WIDTH-w,0);display.print(dateBuf);
  display.drawLine(0,11,127,11,SSD1306_WHITE);
  char timeBuf[9]; strftime(timeBuf,sizeof(timeBuf),"%H:%M:%S",&t);
  display.setTextSize(2);
  display.getTextBounds(timeBuf,0,0,&x1,&y1,&w,&h);
  display.setCursor((SCREEN_WIDTH-w)/2,22);display.print(timeBuf);
  display.setTextSize(1);
  display.setCursor(30,52);display.print("IST (UTC+5:30)");
  display.display();
}

void drawWeatherScreen() {
  if(millis()-lastStaticDraw<2000) return;
  lastStaticDraw=millis();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  if(!weatherValid){
    display.setTextSize(1);
    display.setCursor(20,20);display.print("Weather N/A");
    display.setCursor(5,38);display.print("Set API key & city");
    display.display(); return;
  }
  display.setTextSize(1);
  display.setCursor(0,0);display.print("WEATHER ");display.print(city.substring(0,10));
  display.drawLine(0,10,127,10,SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0,15);display.print(String(wTemp,1));
  display.setTextSize(1);display.print((char)247);display.print("C");
  display.setCursor(80,15);display.print(wDesc.substring(0,8));
  display.setTextSize(1);
  display.setCursor(0,36);display.print("Humidity: ");display.print(String(wHumid,0));display.print("%");
  display.setCursor(0,50);display.print("AQI: ");display.print(wAqi);display.print(" ");
  const char* aq[]={"","Good","Fair","Mod","Poor","V.Poor"};
  if(wAqi>=1&&wAqi<=5) display.print(aq[wAqi]);
  display.display();
}

void drawTextScreen() {
  unsigned long now=millis();
  int tw=bannerText.length()*12;
  int16_t x1,y1; uint16_t bw,bh;
  switch(animType){
    case 0: // Static
      if(now-lastAnim<1000) return;
      lastAnim=now;
      display.clearDisplay();display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
      display.getTextBounds(bannerText,0,0,&x1,&y1,&bw,&bh);
      display.setCursor(max(0,(int)(SCREEN_WIDTH-bw)/2),(SCREEN_HEIGHT-bh)/2);
      display.print(bannerText);display.display();
      break;
    case 1: // Scroll left
      if(now-lastAnim<40) return; lastAnim=now;
      display.clearDisplay();display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
      display.setCursor(scrollX,24);display.print(bannerText);display.display();
      scrollX-=2; if(scrollX<-tw) scrollX=SCREEN_WIDTH;
      break;
    case 2: // Scroll right
      if(now-lastAnim<40) return; lastAnim=now;
      display.clearDisplay();display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
      display.setCursor(scrollX,24);display.print(bannerText);display.display();
      scrollX+=2; if(scrollX>SCREEN_WIDTH) scrollX=-tw;
      break;
    case 3: // Bounce
      if(now-lastAnim<40) return; lastAnim=now;
      display.clearDisplay();display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
      display.setCursor(bounceX,24);display.print(bannerText);display.display();
      bounceX+=bounceDir*2;
      if(bounceX>=SCREEN_WIDTH-tw||bounceX<=0) bounceDir*=-1;
      break;
    case 4: // Typewriter
      if(now-lastAnim<150) return; lastAnim=now;
      display.clearDisplay();display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,24);display.print(bannerText.substring(0,twPos));display.display();
      twPos++; if(twPos>(int)bannerText.length()) twPos=0;
      break;
    case 5: // Blink
      if(now-lastAnim<500) return; lastAnim=now;
      display.clearDisplay();
      if(blinkOn){display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
        display.getTextBounds(bannerText,0,0,&x1,&y1,&bw,&bh);
        display.setCursor(max(0,(int)(SCREEN_WIDTH-bw)/2),(SCREEN_HEIGHT-bh)/2);
        display.print(bannerText);}
      display.display(); blinkOn=!blinkOn;
      break;
  }
}

void drawHackerScreen() {
  unsigned long now=millis();
  if(now-lastHacker<80) return;
  lastHacker=now;
  display.clearDisplay();
  display.setTextSize(1);display.setTextColor(SSD1306_WHITE);
  for(int col=0;col<21;col++){
    int x=col*6;
    for(int row=0;row<8;row++){
      int y=((row*8)+rainDrops[col])%64;
      display.setCursor(x,y);display.print((char)('0'+((col+row+hackerFrame)%2)));
    }
    rainDrops[col]=(rainDrops[col]+(col%3+1))%64;
  }
  // Hooded face
  display.fillRoundRect(38,8,52,52,8,SSD1306_BLACK);
  display.drawRoundRect(38,8,52,52,8,SSD1306_WHITE);
  display.fillTriangle(44,8,64,0,84,8,SSD1306_BLACK);
  display.drawLine(44,8,64,0,SSD1306_WHITE);
  display.drawLine(64,0,84,8,SSD1306_WHITE);
  // Eyes
  display.fillRect(48,22,10,6,SSD1306_WHITE);
  display.fillRect(70,22,10,6,SSD1306_WHITE);
  display.fillRect(50,23,3,3,SSD1306_BLACK);
  display.fillRect(72,23,3,3,SSD1306_BLACK);
  // Nose
  display.drawPixel(63,33,SSD1306_WHITE);display.drawPixel(64,34,SSD1306_WHITE);display.drawPixel(65,33,SSD1306_WHITE);
  // Smile
  display.drawLine(52,40,56,44,SSD1306_WHITE);
  display.drawLine(56,44,72,44,SSD1306_WHITE);
  display.drawLine(72,44,76,40,SSD1306_WHITE);
  // Binary below smile
  display.setTextSize(1);display.setCursor(48,50);
  const char* ht[]={"101010","010101","110011","001100"};
  display.print(ht[hackerFrame%4]);
  hackerFrame++;
  display.display();
}

// ==================== SCREEN ROTATION ====================
void nextScreen() {
  for(int i=0;i<4;i++){
    int n=(curScreen+1+i)%4;
    if(screenOn[n]){curScreen=n; break;}
  }
  scrollX=SCREEN_WIDTH;bounceX=0;bounceDir=1;twPos=0;blinkOn=true;
  lastAnim=0;lastStaticDraw=0;lastHacker=0;
}

// ==================== WEB: HOME ====================
void handleHome() {
  String h=pageHead("Home",0);
  h+="<div class='card'><h2>QT Robot Status</h2><div class='row'>"
    "<div class='box'><div class='lbl'>AP</div><div class='val'>"+String(ap_ssid)+"</div></div>"
    "<div class='box'><div class='lbl'>AP IP</div><div class='val'>"+WiFi.softAPIP().toString()+"</div></div>"
    "</div><div class='row'>"
    "<div class='box'><div class='lbl'>WiFi</div><div class='val'>"+String(staConnected?"<span class='tag tg'>Connected</span>":"<span class='tag tr'>Offline</span>")+"</div></div>";
  if(staConnected) h+="<div class='box'><div class='lbl'>WiFi IP</div><div class='val'>"+WiFi.localIP().toString()+"</div></div>";
  h+="</div></div>";

  h+="<div class='card'><h2>Screen Rotation</h2><form action='/home-save' method='POST'>";
  String sn[]={"Date & Time","Weather","Text Banner","Hacker Face"};
  for(int i=0;i<4;i++){
    h+="<div class='tgl'><span>"+sn[i]+"</span>"
      "<label class='sw'><input type='checkbox' name='s"+String(i)+"'"+String(screenOn[i]?" checked":"")+"><span class='sl'></span></label></div>";
  }
  h+="<label>Rotation Interval</label><select name='ri'>";
  unsigned long ro[]={5000,10000,15000,20000,30000,60000};
  String rl[]={"5 sec","10 sec","15 sec","20 sec","30 sec","60 sec"};
  for(int i=0;i<6;i++) h+="<option value='"+String(ro[i])+"'"+String(rotInterval==ro[i]?" selected":"")+">"+rl[i]+"</option>";
  h+="</select><button class='btn' type='submit'>Save Settings</button></form></div>";
  h+=pageFoot();
  server.send(200,"text/html",h);
}

void handleHomeSave() {
  for(int i=0;i<4;i++) screenOn[i]=server.hasArg("s"+String(i));
  rotInterval=server.arg("ri").toInt();
  prefs.putBool("s0",screenOn[0]);prefs.putBool("s1",screenOn[1]);
  prefs.putBool("s2",screenOn[2]);prefs.putBool("s3",screenOn[3]);
  prefs.putULong("rotInt",rotInterval);
  bool any=false;for(int i=0;i<4;i++)if(screenOn[i])any=true;
  if(!any){screenOn[0]=true;prefs.putBool("s0",true);}
  if(!screenOn[curScreen]) nextScreen();
  server.sendHeader("Location","/");server.send(303);
}

// ==================== WEB: WIFI ====================
void handleWifi() {
  String h=pageHead("WiFi",1);
  h+="<div class='card'><h2>WiFi Connection</h2>"
    "<p class='sub'>Connect to WiFi for weather data and time sync.</p>";
  if(staConnected){
    h+="<div class='row'><div class='box'><div class='lbl'>Connected</div><div class='val'>"+sta_ssid+"</div></div>"
      "<div class='box'><div class='lbl'>IP</div><div class='val'>"+WiFi.localIP().toString()+"</div></div></div>";
  }
  h+="<button class='btn btn-blue' onclick='scanN()'>Scan Networks</button>"
    "<div id='nets' style='margin-top:10px'></div>"
    "<form action='/wifi-save' method='POST'>"
    "<label>SSID</label><input type='text' name='ssid' id='ssid' value='"+sta_ssid+"'>"
    "<label>Password</label><input type='password' name='pass'>"
    "<button class='btn' type='submit'>Connect</button></form></div>"
    "<script>function scanN(){"
    "document.getElementById('nets').innerHTML='<p style=\"color:#8b949e\">Scanning...</p>';"
    "fetch('/scan').then(r=>r.json()).then(d=>{"
    "let h='';d.n.forEach(n=>{"
    "h+='<div class=\"net\" onclick=\"document.getElementById(\\'ssid\\').value=\\''+n.s+'\\'\">'+n.s+'<span style=\"color:#8b949e\">'+n.r+' dBm</span></div>';"
    "});document.getElementById('nets').innerHTML=h;});}</script>";
  h+=pageFoot();
  server.send(200,"text/html",h);
}

void handleScan() {
  int n=WiFi.scanNetworks();
  String j="{\"n\":[";
  for(int i=0;i<n&&i<10;i++){if(i)j+=",";j+="{\"s\":\""+WiFi.SSID(i)+"\",\"r\":"+String(WiFi.RSSI(i))+"}";}
  j+="]}";server.send(200,"application/json",j);WiFi.scanDelete();
}

void handleWifiSave() {
  sta_ssid=server.arg("ssid");sta_pass=server.arg("pass");
  prefs.putString("sta_ssid",sta_ssid);prefs.putString("sta_pass",sta_pass);
  connectSTA();
  server.sendHeader("Location","/wifi");server.send(303);
}

// ==================== WEB: API ====================
void handleApi() {
  String h=pageHead("API",2);
  h+="<div class='card'><h2>Weather API</h2>"
    "<p class='sub'>Get a free key from openweathermap.org</p>"
    "<form action='/api-save' method='POST'>"
    "<label>API Key</label><input type='text' name='key' value='"+apiKey+"' placeholder='Enter API key'>"
    "<label>City Name</label><input type='text' name='city' value='"+city+"' placeholder='e.g. Kolkata'>"
    "<label>Refresh Interval</label><select name='wi'>";
  unsigned long wo[]={10000,15000,30000,60000,300000,600000,1200000};
  String wl[]={"10 sec","15 sec","30 sec","1 min","5 min","10 min","20 min"};
  for(int i=0;i<7;i++) h+="<option value='"+String(wo[i])+"'"+String(weatherInterval==wo[i]?" selected":"")+">"+wl[i]+"</option>";
  h+="</select><button class='btn' type='submit'>Save & Fetch</button></form></div>";
  if(weatherValid){
    h+="<div class='card'><h2>Current Data</h2><div class='row'>"
      "<div class='box'><div class='lbl'>Temp</div><div class='val'>"+String(wTemp,1)+" C</div></div>"
      "<div class='box'><div class='lbl'>Humidity</div><div class='val'>"+String(wHumid,0)+"%</div></div>"
      "<div class='box'><div class='lbl'>AQI</div><div class='val'>"+String(wAqi)+"</div></div>"
      "</div><div style='text-align:center;color:#8b949e;font-size:12px;margin-top:8px'>"+wDesc+"</div></div>";
  }
  h+=pageFoot();
  server.send(200,"text/html",h);
}

void handleApiSave() {
  apiKey=server.arg("key");city=server.arg("city");
  weatherInterval=server.arg("wi").toInt();
  prefs.putString("apiKey",apiKey);prefs.putString("city",city);prefs.putULong("wInt",weatherInterval);
  fetchWeather();
  server.sendHeader("Location","/api");server.send(303);
}

// ==================== WEB: DISPLAY ====================
void handleDisplay() {
  String h=pageHead("Display",3);
  h+="<div class='card'><h2>Text Banner</h2>"
    "<form action='/disp-save' method='POST'>"
    "<label>Display Text</label><textarea name='text' maxlength='200'>"+bannerText+"</textarea>"
    "<label>Animation Style</label><select name='anim'>";
  String an[]={"Static","Scroll Left","Scroll Right","Bounce","Typewriter","Blink"};
  for(int i=0;i<6;i++) h+="<option value='"+String(i)+"'"+String(animType==i?" selected":"")+">"+an[i]+"</option>";
  h+="</select><button class='btn' type='submit'>Update Display</button></form></div>";
  h+="<div class='card'><h2>Animation Guide</h2>";
  String ad[]={"Centered, no movement","Scrolls right to left","Scrolls left to right","Bounces between edges","Letters appear one by one","Blinks on and off"};
  for(int i=0;i<6;i++){
    h+="<div style='padding:6px 0;border-bottom:1px solid #21262d'>"
      "<span style='color:#58a6ff;font-weight:600'>"+an[i]+"</span>"
      "<span style='color:#8b949e;font-size:12px;margin-left:8px'>"+ad[i]+"</span></div>";
  }
  h+="</div>"+pageFoot();
  server.send(200,"text/html",h);
}

void handleDispSave() {
  bannerText=server.arg("text");animType=server.arg("anim").toInt();
  prefs.putString("banner",bannerText);prefs.putInt("anim",animType);
  scrollX=SCREEN_WIDTH;bounceX=0;twPos=0;
  server.sendHeader("Location","/display");server.send(303);
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200); delay(1000);
  Serial.println("\n=== QT Robot - Multi-Display Server ===");
  pinMode(LED_PIN,OUTPUT);digitalWrite(LED_PIN,HIGH);

  Wire.begin(SDA_PIN,SCL_PIN);
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){Serial.println("OLED fail");while(1);}
  display.clearDisplay();
  display.setTextSize(2);display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,5);display.println("QT Robot");
  display.setTextSize(1);display.setCursor(20,30);display.println("Multi Display");
  display.setCursor(20,42);display.println("OLED Server");
  display.setCursor(25,55);display.println("Starting...");
  display.display();delay(1500);

  prefs.begin("qt-robot",false);
  sta_ssid=prefs.getString("sta_ssid","");
  sta_pass=prefs.getString("sta_pass","");
  apiKey=prefs.getString("apiKey","");
  city=prefs.getString("city","");
  weatherInterval=prefs.getULong("wInt",600000);
  bannerText=prefs.getString("banner","Hello World!");
  animType=prefs.getInt("anim",0);
  rotInterval=prefs.getULong("rotInt",10000);
  screenOn[0]=prefs.getBool("s0",true);screenOn[1]=prefs.getBool("s1",true);
  screenOn[2]=prefs.getBool("s2",true);screenOn[3]=prefs.getBool("s3",true);

  for(int i=0;i<22;i++) rainDrops[i]=random(0,64);

  startAP();
  if(sta_ssid.length()>0) connectSTA();

  server.on("/",handleHome);
  server.on("/home-save",HTTP_POST,handleHomeSave);
  server.on("/wifi",handleWifi);
  server.on("/scan",handleScan);
  server.on("/wifi-save",HTTP_POST,handleWifiSave);
  server.on("/api",handleApi);
  server.on("/api-save",HTTP_POST,handleApiSave);
  server.on("/display",handleDisplay);
  server.on("/disp-save",HTTP_POST,handleDispSave);
  server.begin();

  Serial.println("Server: http://"+WiFi.softAPIP().toString());
  Serial.println("Setup complete!");
}

// ==================== LOOP ====================
void loop() {
  server.handleClient();

  // Auto-reconnect WiFi if disconnected
  checkWifiReconnect();

  // Screen rotation
  if(millis()-lastRotation>=rotInterval){
    lastRotation=millis();
    nextScreen();
  }

  // Draw current screen
  switch(curScreen){
    case 0:drawTimeScreen();break;
    case 1:drawWeatherScreen();break;
    case 2:drawTextScreen();break;
    case 3:drawHackerScreen();break;
  }

  // Fetch weather periodically
  if(staConnected && millis()-lastWeatherFetch>=weatherInterval) fetchWeather();

  delay(10);
}
