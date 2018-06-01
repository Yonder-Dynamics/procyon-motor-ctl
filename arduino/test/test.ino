#include <ArduinoSTL.h>
#include "defines.h"
#include <vector>
#include "rov.h"

ROV *a;
void setup() {
  a = new ROV();
  a->setup();
}

void loop() {
  std::vector<int> duties;

  duties.push_back(0);
  duties.push_back(255);
  duties.push_back(0);

  duties.push_back(0);
  duties.push_back(255);
  duties.push_back(0);

  a->setDuties(duties);
  delay(10);
}
