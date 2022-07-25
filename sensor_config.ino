void configure_sensor(void)
{
  if (!ds.search(addressIn)) {
      Serial.println("No more addresses.");
      Serial.println();
      ds.reset_search();
      delay(250);
      return;
  }

  Serial.print("ROM =");
  for (byte i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addressIn[i], HEX);
  }

  if (OneWire::crc8(addressIn, 7) != addressIn[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
  Serial.println();

  // the first ROM byte indicates which chip
  switch (addressIn[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }

}
