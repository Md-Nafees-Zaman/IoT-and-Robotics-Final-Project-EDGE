void bluetooth_connection() {
  if (BTSerial.connected()) {
    if (!pass_ok) {
      BTSerial.print("\nEnter Password: ");
      String pass_entry = BTSerial.readStringUntil('\n');
      while (pass_entry.length() < 3) pass_entry = BTSerial.readStringUntil('\n');
      BTSerial.println(pass_entry.substring(0, pass_entry.length() - 1));
     // BTSerial.println(pass_entry);
      pass_entry.trim();
      if (pass_entry == pass_word) {
        pass_ok = 1;
        BTSerial.print("\nWelcome");
      } else BTSerial.print("Wrong Password !!!");
    }
  } else {
    pass_ok = 0;
    BTSerial.print("\nDisconnected");
  }


  if (pass_ok && BTSerial.available()) {
    char command = BTSerial.read();
    if (command == 'a') {
      BTSerial.print("\nEnter Name: ");
      waitForName();
    } else if (command == 's') {
      showAllRecords();
    } else if (command == 'd') {
      BTSerial.print("\nEnter ID to delete: ");
      waitForDeleteID();
    } else if (command == 'l') show_logs();
  }
}