#include <WiFi.h>
#include <aWOT.h>
#include <Keypad.h>
#include <ArduinoJson.h>
#include <UUID.h>
#include <rgb_lcd.h>
#include <sha2.h>
#include <ESP_Mail_Client.h>

const char indexHtml[] = "<!DOCTYPE html>\n"
"<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\" />\n"
"<script src=\"https://unpkg.com/react@18/umd/react.development.js\" crossorigin></script>\n"
"<script src=\"https://unpkg.com/react-dom@18/umd/react-dom.development.js\" crossorigin></script>\n"
"<script src=\"https://unpkg.com/@babel/standalone@7/babel.min.js\"></script>\n"
"<script src=\"https://unpkg.com/@mui/material@latest/umd/material-ui.production.min.js\"></script>\n"
"<script src=\"https://unpkg.com/@emotion/react@11/dist/emotion-react.umd.min.js\"></script>\n"
"<script src=\"https://unpkg.com/@emotion/styled@11/dist/emotion-styled.umd.min.js\"></script>\n"
"<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Snowstorm/20131208/snowstorm-min.js\"></script>\n"
"<title>Akatsuki Corp. admin panel</title>\n"
"<link\n"
"  rel=\"stylesheet\"\n"
"  href=\"https://fonts.googleapis.com/icon?family=Material+Icons\"\n"
"/>\n"
"<style>html, body, #root, form, form > div { height: 100%; }</style>\n"
"<div id=\"root\"></div>\n"
"<script type=\"text/babel\">\n"
"snowStorm.snowColor='#99ccff';snowStorm.useTwinkleEffect=true;\n"
"    const { Button, OutlinedInput, Container, Box, Typography, Paper, Stack, Grow, Divider, Icon, ThemeProvider, CssBaseline, createTheme } = MaterialUI\n"
"\n"
"    const darkTheme = createTheme({\n"
"        palette: {\n"
"            mode: \"dark\"\n"
"        }\n"
"    })\n"
"\n"
"    function Main() {\n"
"        const [statusText, setStatusText] = React.useState(\"\")\n"
"        const [inputValue, setInputValue] = React.useState(\"\")\n"
"        const [accessToken, setAccessToken] = React.useState(localStorage.getItem(\"token\") ?? \"\")\n"
"        const [hasError, setHasError] = React.useState(false)\n"
"        const [safes, setSafes] = React.useState([])\n"
"\n"
"        async function makeRequest(path, body) {\n"
"            return new Promise(resolve => {\n"
"                fetch(`/${path}${accessToken\n"
"                        ? \"?token=\" + accessToken\n"
"                        : \"\"\n"
"                    }`, Object.assign({\n"
"                        headers: {\n"
"                            \"Content-Type\": \"application/json\"\n"
"                        },\n"
"                    }, body ? {\n"
"                        method: \"POST\",\n"
"                        body: JSON.stringify(body)\n"
"                    } : {})).then(req => req.json()\n"
"                        .then(res => {\n"
"                            if (res.error) {\n"
"                                setStatusText(`Error: ${res.error}`)\n"
"                                setTimeout(() => {\n"
"                                    if (res.error != \"Unauthorized\") return;\n"
"                                    localStorage.clear()\n"
"                                    location.reload()\n"
"                                }, 200)\n"
"                                setHasError(true)\n"
"                                return\n"
"                            }\n"
"                            setHasError(false)\n"
"                            resolve(res)\n"
"                        }))\n"
"            })\n"
"        }\n"
"\n"
"        function onSubmit(event) {\n"
"            event.preventDefault()\n"
"            makeRequest(\"login\", {\n"
"                password: inputValue\n"
"            }).then(result => {\n"
"                setAccessToken(result.token)\n"
"            })\n"
"        }\n"
"\n"
"        React.useEffect(() => {\n"
"            if (!accessToken) return\n"
"            setStatusText(`Logged in with token ${accessToken}`)\n"
"            localStorage.setItem(\"token\", accessToken)\n"
"            setInterval(() => makeRequest(\"safes/list\").then(setSafes), 500);\n"
"        }, [accessToken])\n"
"\n"
"        function unlockSafe(safe) {\n"
"            makeRequest(safe.unlocked ? \"safes/lock\" : \"safes/unlock\", { id: safe.id })\n"
"                .then(() =>\n"
"                    makeRequest(\"safes/list\").then(setSafes))\n"
"        }\n"
"\n"
"        return (\n"
"            <ThemeProvider theme={darkTheme}>\n"
"                <CssBaseline />\n"
"                <form onSubmit={onSubmit} style={{ textAlign: \"center\", height: accessToken ? \"unset\" : \"100%\" }}>\n"
"                    {\n"
"                        !accessToken ? <>\n"
"                        <Paper component={Stack} direction=\"column\" justifyContent=\"center\">\n"
"                            <div>\n"
"                                <OutlinedInput\n"
"                                    type=\"password\"\n"
"                                    placeholder=\"Password...\"\n"
"                                    onChange={event => setInputValue(event.target.value)}\n"
"                                    variant=\"standard\" />\n"
"                            <Typography style={{ color: \"red\", margin: \"5px\" }}>{statusText}</Typography>\n"
"                            </div>\n"
"                        </Paper>\n"     
"                        </> : <>\n"
"                            <Box component=\"div\" sx={{ display: \"inline-block\" }}>\n"
"                                <Typography style={{ margin: \"6.5px\", display: \"inline-block\" }}>{statusText}</Typography>\n"
"                                <Button style={{ marginTop: \"-3.8px\" }}>Allow safe movement</Button>\n"
"                            </Box>\n"
"                            <Divider style={{ margin: \"0px 0px -5px 0px\" }} />\n"
"                        </>\n"
"                    }\n"
"                </form>\n"
"                <Container style={{\n"
"                    display: \"flex\",\n"
"                    justifyContent: \"space-around\",\n"
"                    columnGap: \"10px\",\n"
"                    rowGap: \"10px\",\n"
"                    flexWrap: \"wrap\",\n"
"                    padding: \"20px\",\n"
"                }}>\n"
"                    {safes.map(safe => (\n"
"                        <Grow in={!!accessToken} key={safe.id}>\n"
"                            <Box sx={{\n"
"                                backgroundColor: 'primary.dark',\n"
"                                flexDirection: \"row\",\n"
"                                display: \"flex\",\n"
"                                color: '#ffffff',\n"
"                                width: \"17%\",\n"
"                                padding: \"30px\",\n"
"                                boxShadow: \"-0px 0px 89px 17px rgba(255,255,255,0.15)\"\n"
"                            }}>\n"
"                                <Container sx={{ flexDirection: \"column\", paddingLeft: \"0 !important\" }}>\n"
"                                    <Typography>ID: {safe.id}</Typography>\n"
"                                    <Typography>PIN: {safe.pin}</Typography>\n"
"                                    {safe.quarantine ? <Typography>(quarantine)</Typography> : null}\n"
"                                </Container>\n"
"                                <Icon style={{ alignSelf: \"center\" }} onClick={unlockSafe.bind(null, safe)}>\n"
"                                    {safe.unlocked ? \"lock_open\" : \"lock\"}\n"
"                                </Icon>\n"
"                            </Box>\n"
"                        </Grow>\n"
"                    ))}\n"
"                </Container>\n"
"            </ThemeProvider>\n"
"        )\n"
"    }\n"
"\n"
"    const container = document.querySelector(\"#root\")\n"
"    const root = ReactDOM.createRoot(container)\n"
"    root.render(<Main />)\n"
"</script>\n";

WiFiServer server(80);
Application app;

const int rows = 4;
const int cols = 4;
char keys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(
  makeKeymap(keys),
  (byte[]){
    13, 12, 14, 27
  },
  (byte[]){
    26, 25, 33, 32
  },
  rows, cols
);

rgb_lcd lcd;

struct Safe {
  char pin[5];
  int id;
  bool unlocked;
  bool quarantine;
};
const int safes_count = 40;
Safe safes[safes_count] = {};

Sha256 sha256;
String hexHash(uint8_t* hash) {
  String result;
  for (int i = 0; i<32; i++) {
    result += "0123456789abcdef"[hash[i]>>4];
    result += "0123456789abcdef"[hash[i]&0xf];
  }
  return result;
}

// TODO: multiple doors
const int RELAY = 5;
void changeSafeState(int i, bool state) {
  pinMode(RELAY, OUTPUT);
  safes[i].unlocked = state;
  if (state) {
    digitalWrite(RELAY, HIGH);
    delay(100);
    digitalWrite(RELAY, LOW);
  }
}

SMTPSession smtp;

void alertAdmin() {
  ESP_Mail_Session session;
  SMTP_Message message;
  
  session.server.host_name = "smtp.outlook.com";
  session.server.port = 587;
  session.login.email = "sti2dakatsuki@outlook.fr";
  session.login.password = "linux081205bit";
  session.login.user_domain = "";

  message.sender.name = "ESP";
  message.sender.email = "sti2dakatsuki@outlook.fr";
  message.subject = "Alerte alerte alerte";
  message.addRecipient("Administrateur", "sti2dakatsuki@outlook.fr");

  String html = "<h1>Attention, un trou du cul essaie de voler des téléphones. C'est peut-être un terroriste, veuillez évacuer la salle dans les moindres délais. Appellez la police si ce n'est pas déjà fait. Si vous trouvez l'individu, servez-vous du plot à l'extérieur.</h1>";
  message.html.content = html.c_str();
  message.text.charSet = "us-ascii";

  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message))
    Serial.print("Error sending email: ");
    Serial.println(smtp.errorReason());
}

#define error(message)                         \
{                                              \
  DynamicJsonDocument errorJson(512);          \
  errorJson["error"] = message;                \
  res.set("Content-Type", "application/json"); \
  serializeJson(errorJson, req);               \
}

#define success()                              \
{                                              \
  DynamicJsonDocument successJson(128);        \
  successJson["success"] = true;               \
  res.set("Content-Type", "application/json"); \
  serializeJson(successJson, req);             \
}

char accessToken[37] = {0};
bool checkAuth(Request &req, Response &res) {
  char token[37]; // UUID4 + \0
  req.query("token", token, 37);
  bool allowed = strlen(accessToken) > 0
    && strcmp(token, accessToken) == 0;
  if (!allowed)
    error("Unauthorized");
  return allowed;
}

void index(Request &req, Response &res) {
  res.set("Content-Type", "text/html");
  res.print(indexHtml);
}

void login(Request &req, Response &res) {
  DynamicJsonDocument data(512);
  deserializeJson(data, req);

  const char* password = data["password"];
  sha256.init();
  sha256.write(password);
  const char* hash = hexHash(sha256.result()).c_str();

  if (strcmp(hash, "a78a9d321ba0c6f8039ce9063481a32d62b2929ed294a13265a290769e099705") == 0) {
    UUID token;
    strcpy(accessToken, token.toCharArray());

    DynamicJsonDocument tokenJson(512);
    tokenJson["token"] = token;
    res.set("Content-Type", "application/json");
    serializeJson(tokenJson, req);
  } else
    error("Invalid password");
}

void listSafes(Request &req, Response &res) {
  if (!checkAuth(req, res)) return;

  DynamicJsonDocument safesJson(4096);
  for (int i = 0; i < safes_count; i++) {
    safesJson[i]["pin"] = safes[i].pin;
    safesJson[i]["id"] = safes[i].id;
    safesJson[i]["unlocked"] = safes[i].unlocked;
    safesJson[i]["quarantine"] = safes[i].quarantine;
  }

  res.set("Content-Type", "application/json");
  serializeJson(safesJson, req);
}

void unlockSafe(Request &req, Response &res) {
  if (!checkAuth(req, res)) return;

  DynamicJsonDocument data(128);
  deserializeJson(data, req);
  int id = data["id"];

  bool foundSafe = false;
  for (int i = 0; i < safes_count; i++) {
    if (safes[i].id == id) {
      changeSafeState(i, true);
      safes[i].quarantine = false;
      foundSafe = true;
    }
  }
  if (!foundSafe)
    error("No such safe");
  success();
}

void lockSafe(Request &req, Response &res) {
  if (!checkAuth(req, res)) return;

  DynamicJsonDocument data(128);
  deserializeJson(data, req);
  int id = data["id"];

  bool foundSafe = false;
  for (int i = 0; i < safes_count; i++) {
    if (safes[i].id == id) {
      changeSafeState(i, false);
      foundSafe = true;
    }
  }
  if (!foundSafe)
    error("No such safe");
  success();
}

void lcdPrint(const char* text, int r = 0, int g = 0, int b = 0, int x = 0, int y = 0) {
  int lcdLen = 16;
  int textLen = strlen(text);
  char padding[16];

  memset(padding, ' ', lcdLen - textLen);
  lcd.setRGB(r, g, b);
  lcd.setCursor(x, y);
  lcd.write(text);
  lcd.write(padding);
}

byte e_acute[8] = {
  0b00110,
  0b01100,
  0b01110,
  0b10001,
  0b11111,
  0b10000,
  0b01110,
  0b00000,
};

// TODO: multiple doors
const int SAFE = 0;
const int RELAY_STATE = 34;
void handleDoorCloses() {
  int level = analogRead(RELAY_STATE);
  if (level < 1200 && safes[SAFE].unlocked)
    changeSafeState(0, false);
}

void setup() {
  Serial.begin(115200);

  lcd.begin(16, 2);
  lcd.setRGB(0, 0, 255);
  lcd.createChar(1, e_acute);

  for (int i = 0; i < safes_count; i++) {
    sprintf(safes[i].pin, "%04d", random(0, 9999));
    safes[i].id = i + 1;
    safes[i].unlocked = true;
    safes[i].quarantine = false;
  }

  Serial.print("Connecting to Wi-Fi network...");
  WiFi.begin("Eleves", "ml$@0770924L");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.print("\nConnected. IP address: ");
  Serial.println(WiFi.localIP());

  lcdPrint(WiFi.localIP().toString().c_str(), 0, 0, 255);
  sleep(2);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.write("Casier:");

  app.get("/", &index);
  app.get("/safes/list", &listSafes);
  app.post("/safes/unlock", &unlockSafe);
  app.post("/safes/lock", &lockSafe);
  app.post("/login", &login);
  server.begin();
}

int i = 0;
char currentCode[5] = {0};
bool waitingForPin = false;
int chosenSafe = -1;
int invalidTries = 0;

void handleKeypad() {
  char key = keypad.getKey();
  if (key) {
    if (!waitingForPin) {
      lcdPrint("Casier:", 0, 0, 255);
    }
    if (key == '#') {
      if (strlen(currentCode) == 0)        
        return;

      if (waitingForPin) {
        if (safes[chosenSafe].unlocked) {
          changeSafeState(chosenSafe, false);
          strcpy(safes[chosenSafe].pin, currentCode);
          waitingForPin = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.write("Casier ferm");
          lcd.write((unsigned char)1);
          lcd.setRGB(0, 255, 0);
          chosenSafe = -1;
        } else if (strcmp(safes[chosenSafe].pin, currentCode) == 0) {
          changeSafeState(chosenSafe, true);
          waitingForPin = false;
          lcdPrint("Casier ouvert", 0, 255);
          chosenSafe = -1;
        } else {
          lcd.clear();
          lcdPrint("PIN invalide", 255);

          if (++invalidTries >= 3) {
            lcd.clear();
            alertAdmin();
            Serial.println("alerted admin");
            lcdPrint("Trop d'essais", 255);
            safes[chosenSafe].quarantine = true;
            waitingForPin = false;
            chosenSafe = -1;
            //makeNoise();
            //scream();
          }

          memset(currentCode, 0, sizeof(currentCode));
          i = 0;
        }
      } else {
        bool foundSafe = false;
        for (int i = 0; i < safes_count; i++) {
          if (atoi(currentCode) == safes[i].id) {
            foundSafe = true;
            waitingForPin = true;
            chosenSafe = i;
            invalidTries = 0;
            if (safes[i].quarantine) {
              lcd.clear();
              lcdPrint("Trop de tentatives", 255);
              waitingForPin = false;
              chosenSafe = -1;
            } else if (safes[i].unlocked) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.write("D");
              lcd.write((unsigned char)1);
              lcd.write("finir PIN:");
              lcd.setRGB(0, 255, 0);
            } else {
              lcd.clear();
              lcdPrint("PIN du casier:", 0, 255);
            }
          }
        }
        if (!foundSafe) {
          lcd.clear();
          lcdPrint("Casier invalide", 255);
        }
      }
      memset(currentCode, 0, sizeof(currentCode));
      i = 0;
      return;
    } else if (key == 'D') {
      if (i < 1) return;
      currentCode[--i] = 0;
      goto printSafe;
    } else if (key == 'A' || key == 'B' || key == 'C')
      return;
    if (waitingForPin ? i >= 4 : i >= 2)
      return;
    currentCode[i++] = key;

printSafe:
    lcd.setCursor(0, 1);
    lcd.write("    ");
    lcd.setCursor(0, 1);
    lcd.write(currentCode);
    return;
  }
}

void loop() {
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
    client.stop();
  }

  handleKeypad();
  handleDoorCloses();
}
