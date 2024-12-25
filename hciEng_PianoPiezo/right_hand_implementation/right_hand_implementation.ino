#define TRUE 1
#define FALSE 0
int thresh = 60; 
int piezo_5 = 1;
int piezo_6 = 2;
int piezo_7 = 3;
int piezo_down = 8;
int piezo_up = 9;
/* ORDER OF PIEZO ON RIGHTHAND (thumb leftmost - pinky rightmost)
  8  5 6 7 9
down G A B up
*/
int on5 = FALSE;
int on6 = FALSE;
int on7 = FALSE;
int on_up;
int on_down;
void setup() {
  Serial1.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() { 
  int raw5 = analogRead(piezo_5);
  int raw6 = analogRead(piezo_6);
  int raw7 = analogRead(piezo_7);
  int lower = analogRead(piezo_down);
  int higher = analogRead(piezo_up);
  int midi5 = map(raw5, 0, 1023, 0, 127);
  int midi6 = map(raw6, 0, 1023, 0, 127);
  int midi7 = map(raw7, 0, 1023, 0, 127);
  int ed_type, check_val, on_off;
  if (higher > 100 && !on_up) {
    Serial1.write(byte(255));
    Serial1.write(byte('o'));
    Serial1.write(byte(1));
    on_up = TRUE;
  }
  if (higher < 100 && on_up) on_up = FALSE;
  if (lower > 100 && !on_down) {
    Serial1.write(byte(255));
    Serial1.write(byte('o'));
    Serial1.write(byte(0));
    on_down = TRUE;
  }
  if (lower < 100 && on_down) on_down = FALSE;
  Serial.println(raw6);
  Serial.println(raw7);
  Serial.println(lower);
  Serial.println(higher);
  if (raw5 > thresh && !on5) {
    ed_type = random(1,2);
    on_off = 1;
    check_val = calcCheckVal(ed_type, 'n', 5, midi5, on_off);
    sendNote(5, midi5, on_off, ed_type, check_val);
    on5 = TRUE;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  if (raw6 > thresh && !on6) {
    ed_type = random(1,2);
    on_off = 1;
    check_val = calcCheckVal(ed_type, 'n', 6, midi6, on_off);
    sendNote(6, midi6, on_off, ed_type, check_val);
    on6 = TRUE;
  }
  if (raw7 > thresh && !on7) {
    ed_type = random(1,2);
    on_off = 1;
    check_val = calcCheckVal(ed_type, 'n', 7, midi7, on_off);
    sendNote(7, midi7, on_off, ed_type, check_val);
    on7 = TRUE;
  }

  if (raw5 < thresh && on5) {
    ed_type = random(1,2);
    on_off = 0;
    check_val = calcCheckVal(ed_type, 'n', 5, 0, on_off);
    sendNote(5, 0, on_off, ed_type, check_val);
    on5 = FALSE;
  }
  if (raw6 < thresh && on6) {
    ed_type = random(1,2);
    on_off = 0;
    check_val = calcCheckVal(ed_type, 'n', 6, 0, on_off);
    sendNote(6, 0, on_off, ed_type, check_val);
    on6 = FALSE;
  }
  if (raw7 < thresh && on7) {
    ed_type = random(1,2);
    on_off = 0;
    check_val = calcCheckVal(ed_type, 'n', 7, 0, on_off);
    sendNote(7, 0, on_off, ed_type, check_val);
    on7 = FALSE;
  }
  
  //Sending End message:
  Serial1.write('X');
  delay(10);
}
uint8_t calcCheckVal(int ed_type, char message_type, int piezo_num, int v, int on_off) {
  if (ed_type == 1) {
    return (numOnes(255)+numOnes(message_type)+numOnes(piezo_num)+numOnes(v)+on_off+1) % 2;
  }
  if (ed_type == 2) {
    return ~((byte(255) + byte(message_type) + byte(piezo_num)+byte(v)+byte(on_off)+byte(2)) & 0xFF) + 1;
  }
}
void sendNote(int piezo_num, int velocity, int on_off, int ed_type, int check_val) {
  Serial1.write(byte(255));
  Serial1.write(byte('n'));
  Serial1.write(byte(piezo_num));
  Serial1.write(byte(velocity));
  Serial1.write(byte(on_off));
  Serial1.write(byte(ed_type));
  Serial1.write(byte(check_val));
}
int numOnes(uint8_t b) {
  int ones_count = 0;
  for (int i = 0; i < 8; i++) {
    if ((b >> i) % 2 == 1) {
      ones_count++;
    }
  }
  return ones_count;
}