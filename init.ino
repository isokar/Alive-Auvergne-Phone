void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  //Serial.println("\nStarting board");

  // Allow the hardware to sort itself out
  delay(500);
  setupCoin();
}

void loop() 
{
  loopCoin();
  delay(100);
}
