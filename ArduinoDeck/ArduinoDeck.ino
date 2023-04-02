#define ARRAY_COL_4 9
#define ARRAY_COL_3 8
#define ARRAY_COL_2 7
#define ARRAY_COL_1 6
#define ARRAY_ROW_1 5
#define ARRAY_ROW_2 4
#define ARRAY_ROW_3 3
#define ARRAY_ROW_4 2

int array_row_1_state = 0;
int array_row_2_state = 0;
int array_row_3_state = 0;
int array_row_4_state = 0;

int array_index = 0;

int col_array[4] = { ARRAY_COL_1, ARRAY_COL_2, ARRAY_COL_3, ARRAY_COL_4 };

void setup() {
  // put your setup code here, to run once:
  pinMode(ARRAY_COL_1, OUTPUT);
  pinMode(ARRAY_COL_2, OUTPUT);
  pinMode(ARRAY_COL_3, OUTPUT);
  pinMode(ARRAY_COL_4, OUTPUT);
  pinMode(ARRAY_ROW_1, INPUT_PULLUP);
  pinMode(ARRAY_ROW_2, INPUT_PULLUP);
  pinMode(ARRAY_ROW_3, INPUT_PULLUP);
  pinMode(ARRAY_ROW_4, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  array_index = 0;
  // put your main code here, to run repeatedly:
  for (int i = 0; i <= 3; i++) {
    digitalWrite(col_array[0], HIGH);
    digitalWrite(col_array[1], HIGH);
    digitalWrite(col_array[2], HIGH);
    digitalWrite(col_array[3], HIGH);
    // array_row_1_state = 1;
    // array_row_2_state = 1;
    // array_row_3_state = 1;
    // array_row_4_state = 1;

    digitalWrite(col_array[i], LOW);
    array_row_1_state = digitalRead(ARRAY_ROW_1);
    array_row_2_state = digitalRead(ARRAY_ROW_2);
    array_row_3_state = digitalRead(ARRAY_ROW_3);
    array_row_4_state = digitalRead(ARRAY_ROW_4);

    if (array_row_1_state == LOW) {
      char str[10];                          // create a character array to hold the resulting string
      sprintf(str, "S%d", array_index + 1);  // concatenate "S" with the value of (i+1) and store it in str
      Serial.println(str);
      delay(200);
    } else if (array_row_2_state == LOW) {
      char str[10];                          // create a character array to hold the resulting string
      sprintf(str, "S%d", array_index + 5);  // concatenate "S" with the value of (i+1) and store it in str
      Serial.println(str);
      delay(200);
    } else if (array_row_3_state == LOW) {
      char str[10];                          // create a character array to hold the resulting string
      sprintf(str, "S%d", array_index + 9);  // concatenate "S" with the value of (i+1) and store it in str
      Serial.println(str);
      delay(200);
    } else if (array_row_4_state == LOW) {
      char str[10];                           // create a character array to hold the resulting string
      sprintf(str, "S%d", array_index + 13);  // concatenate "S" with the value of (i+1) and store it in str
      Serial.println(str);
      delay(200);
    } else {
    }
    array_index++;
  }
}
