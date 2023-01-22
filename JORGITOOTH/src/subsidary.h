// Look to previous states and log
bool pressed[3];

// Initialize Final Values to send via BLE
char MODE_SELECT;
int TIME_SELECT;
bool HEAD_PAT = false;
bool QUITTER = false;

int count, mosh;
int LEFTOVER;
char modes[4] = {'s', 'c', 'e', 'm'};

void resetStates(){
  for (int n = 0; n < 3; n++){
    pressed[n] = 0;
  }
}

// Calculate the hours
int hours(int counter){
  return (counter * 10) / 60;
}

// Calculate the minutes
int minutes(int counter){
  return (counter * 10) % 60;
}

// Count Up function for Button
void countUp(){
  if (count == 12){
    count = 0;
  }
  else{
    count++;
  }
}

void upMode(){
  if (mosh == 4){
    mosh = 0;
  }
  else{
    mosh++;
  }
}

// Find char to relay
char findMode(int num){
  return modes[num];
}