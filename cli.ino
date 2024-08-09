
void SetSimpleCli(){

  cmdSsid = cli.addSingleArgCmd("ssid");
  cmdSsid.setDescription(" Set WiFi SSID");

  cmdPassw = cli.addSingleArgCmd("passw");
  cmdPassw.setDescription(" Set WiFi password");

  cmdHost = cli.addSingleArgCmd("host");
  cmdHost.setDescription(" Set NTP host/pool address");

  cmdRegion = cli.addSingleArgCmd("region");
  cmdRegion.setDescription(" Set destination region");

  cmdTZdata = cli.addSingleArgCmd("tzdata");
  cmdTZdata.setDescription(" Set tzdata");

  cmdPoll = cli.addSingleArgCmd("poll");
  cmdPoll.setDescription(" NTP poll interval");

  cmdShow = cli.addSingleArgCmd("show");
  cmdShow.setDescription(" Show configuration");

  cmdSave = cli.addSingleArgCmd("save");
  cmdSave.setDescription(" Save configuration to EEPROM");

  cmdReboot = cli.addSingleArgCmd("reboot");
  cmdReboot.setDescription(" Reboot hard | soft");

  cmdList = cli.addSingleArgCmd("list");
  cmdList.setDescription(" Print regions list");

  cmdHelp = cli.addSingleArgCmd("help");
  cmdHelp.setDescription(" Get help");

}


void  loop_cli_mode(){
  String input;
  char emptyArg[] = "Argument is empty, do nothing";
  // uint8_t argNum = 0;
  uint8_t argLen = 0;

  Serial.print("> ");
  readStringWEcho(input, MAX_ALLOWED_INPUT);

  if (input.length() > 0) {
    cli.parse(input);
  }

  if (cli.available()) {
    Command c = cli.getCmd();
    // argNum = c.countArgs();
    argLen = c.getArg(0).getValue().length();

    if (c == cmdSsid) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(ssid, 0, sizeof(ssid));
        c.getArg(0).getValue().toCharArray(ssid, sizeof(ssid)-1 );
        Serial.println("SSID set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdPassw) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(passw, 0, sizeof(passw));
        c.getArg(0).getValue().toCharArray(passw, sizeof(passw)-1 );
        Serial.println("WiFi password set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdHost) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(host, 0, sizeof(host));
        c.getArg(0).getValue().toCharArray(host, sizeof(host)-1 );
        Serial.println("NTP host is \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdPoll) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        poll_interval = c.getArg(0).getValue().toInt();
        Serial.println("Poll interval set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdRegion) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        region = c.getArg(0).getValue().toInt();
        Serial.print("Region set to \"");
        Serial.print(region_name[region]);
        Serial.println("\"");
      }
    } else if (c == cmdTZdata) {
      if ( argLen == 0 ) {
        Serial.println(emptyArg);
      }else{
        memset(tzdata, 0, sizeof(tzdata));
        c.getArg(0).getValue().toCharArray(tzdata, sizeof(tzdata)-1 );
        Serial.println("TZdata set to \"" + c.getArg(0).getValue() + "\"");
      }
    } else if (c == cmdSave) {
      eeprom_save();
      Serial.println("Configuration saved to EEPROM");
    } else if (c == cmdShow) {
      Serial.print("Poll interval = \"");Serial.print(poll_interval);Serial.println("\"");
      Serial.print("WiFi SSID = \"");Serial.print(ssid);Serial.println("\"");
      Serial.print("WiFi password = \"");Serial.print(passw);Serial.println("\"");
      Serial.print("Host = \"");Serial.print(host);Serial.println("\"");
      Serial.print("Region = \"");Serial.print(region_name[region]);Serial.println("\"");
      Serial.print("TZdata = \"");Serial.print(tzdata);Serial.println("\"");
    } else if (c == cmdList) {
      for ( uint8_t i = 0; i < REGION_COUNT; i++ ) {
        Serial.print(i);
        Serial.print(" ");
        Serial.println(region_name[i]);
      }
    } else if (c == cmdReboot) {
      if ( ( argLen == 0 ) || c.getArg(0).getValue().equalsIgnoreCase("soft") ) {
        Serial.println("Reboot...");
        delay(3000);
        ESP.restart();
      }else if ( c.getArg(0).getValue().equalsIgnoreCase("hard") ){
        Serial.println("Reset...");
        delay(3000);
        ESP.reset();
      }else{
        Serial.println("Unknown argument, allowed only \"hard\" or \"soft\"");
      }
    } else if (c == cmdHelp) {
      Serial.println("Help:");
      Serial.println(cli.toString());
    }

  }

  if (cli.errored()) {
    CommandError cmdError = cli.getError();

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
      Serial.print("Did you mean \"");
      Serial.print(cmdError.getCommand().toString());
      Serial.println("\"?");
    }
  }
}


void readStringWEcho(String& input, size_t char_limit) { // call with char_limit == 0 for no limit
  for(;;) {
    if (Serial.available()) {
      char c = Serial.read();
      if ((uint8_t)c == 8) {
        if ( input.length() ) {
          clearString(input.length());
          input.remove(input.length()-1);
          Serial.print(input);
        }
        continue;
      }
      if ( ((uint8_t)c == 10) || ((uint8_t)c == 13) ){
        Serial.println();
        return;
      }
      if ( ((uint8_t)c < 32) || ((uint8_t)c > 126)) {
        Serial.print((char)7);
        continue;
      }
      input += c;
      Serial.print(c);
      if (char_limit && (input.length() >= char_limit)) {
        return;
      }
    }
  }
}

void clearString( uint16_t len ){
  char stmp[MAX_ALLOWED_INPUT+7];
  memset(stmp+1,' ',len+2);
  stmp[0]='\r';
  stmp[len+3]='\r';
  stmp[len+4]='>';
  stmp[len+5]=' ';
  stmp[len+6]=0;
  Serial.print(stmp);
}
