
int led = 9;           // the PWM pin the LED is attached
int brightness = 0;    // how bright the LED
int fadeAmount = 2;    // how many points to fade the LED

int ThermistorPin = 0;
int Vo;
float R1 = 10000; 
float refT = 31.00; // Hot Temperature
float refT2 = 32.00; // Hot Temperature
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  Tf = (Tc * 9.0)/ 5.0 + 32.0; 
  
  // FadeBlinkAlarmSystem
  analogWrite(led, brightness);
  
  if(Tc > refT && brightness <= 250) {
      brightness = brightness + fadeAmount;
  } else if (Tc < refT && brightness != 0 ) {
    brightness = brightness - fadeAmount;
  }

  Serial.print("Temperature: "); 
  Serial.print(Tf);
  Serial.print(" F; ");
  Serial.print(Tc);
  Serial.print(" C - brightness -> ");   
  Serial.println(brightness);   

  delay(30);
}
