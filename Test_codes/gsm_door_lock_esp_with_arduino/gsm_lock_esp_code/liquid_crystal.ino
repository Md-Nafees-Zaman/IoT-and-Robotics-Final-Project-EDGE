void lcd_out() {
  Serial.println("Started !!");
  //checkForCall();
  char key = customkeypad.getKey();
  Serial.println(key);
  if (key == 'A') {
    adminMenu();
  }
}


void adminMenu() {
  lcd.clear();
  lcd.print("Enter Password:");
  String enteredPass = "";
  while (enteredPass.length() < 4) {
    char key = customkeypad.getKey();
    if (key) {
      enteredPass += key;
      lcd.setCursor(enteredPass.length() - 1, 1);
      lcd.print("*");
    }
  }
  if (enteredPass == adminPassword) {
    isAdmin = true;
    lcd.clear();
    lcd.print("Admin Mode");
    delay(1000);
    lcd.clear();
    lcd.print("1:Add 2:Remove");
    lcd.setCursor(0, 1);
    lcd.print("3:Pwd 4:Return");

    char choice = customkeypad.waitForKey();
    if (choice == '1') addNumber();
    else if (choice == '2') removeNumber();
    // else if (choice == '3') changePassword();
     else if (choice == '4') return;
  } else {
    lcd.clear();
    lcd.print("Wrong Password!");
    delay(2000);
    lcd.clear();
    lcd.print("System Ready");
  }
}

void addNumber() {
  // if (userCount >= 5) {
  //   lcd.clear();
  //   lcd.print("Limit Reached!");
  //   delay(2000);
  //   return;
  // }

  lcd.clear();
  lcd.print("Enter Number:");
  String newNumber = "";
  while (newNumber.length() < 11) {
    char key = customkeypad.getKey();
    Serial.println(key);
    if (key || key == '0') {
      newNumber += key;
      lcd.setCursor(newNumber.length() - 1, 1);
      lcd.print(key);
    }
  }

  newNumber = "88" + newNumber + '\n';
  Serial.println(newNumber);
  // delay(4000);

  lcd.clear();
  // lcd.print("Enter Name:");
  // String newName = "";
  // while (newName.length() < 5) {
  //   char key = customkeypad.getKey();
  //   if (key) {
  //     newName += key;
  //     lcd.setCursor(newName.length() - 1, 1);
  //     lcd.print(key);
  //   }
  // }

  // registeredUsers[userCount].number = newNumber;
  // registeredUsers[userCount].name = newName;
  // userCount++;
  userID = TOTAL_ID;
  String record = newNumber + ", " + String(userID) + ". " + "Registered\n" + " : ";

  myFile = SD.open("/userdata.txt", FILE_APPEND);
  if (myFile) {
    myFile.println(record);
    myFile.close();
    Serial.println("Record saved.");
    //lcd.print("\nRecord saved.");
    TOTAL_ID++;

    myFile = SD.open("/total_id.txt", FILE_WRITE);
    if (myFile) {
      myFile.println(TOTAL_ID);
      myFile.close();
    } else Serial.println("Error writing data in total_id.txt");


  } else {
    Serial.println("Error opening file.");
    lcd.clear();
    lcd.print("File Error");
    delay(2000);
  }

  lcd.clear();
  lcd.print("Added!");
  delay(2000);
}

void removeNumber() {
  // if (userCount >= 5) {
  //   lcd.clear();
  //   lcd.print("Limit Reached!");
  //   delay(2000);
  //   return;
  // }

  lcd.clear();
  lcd.print("Enter Number:");
  String newNumber = "";
  while (newNumber.length() < 11) {
    char key = customkeypad.getKey();
    Serial.println(key);
    if (key || key == '0') {
      newNumber += key;
      lcd.setCursor(newNumber.length() - 1, 1);
      lcd.print(key);
    }
  }

  newNumber = "88" + newNumber + '\n';
  Serial.println(newNumber);
  // delay(4000);

  lcd.clear();
  // lcd.print("Enter Name:");
  // String newName = "";
  // while (newName.length() < 5) {
  //   char key = customkeypad.getKey();
  //   if (key) {
  //     newName += key;
  //     lcd.setCursor(newName.length() - 1, 1);
  //     lcd.print(key);
  //   }
  // }

  // registeredUsers[userCount].number = newNumber;
  // registeredUsers[userCount].name = newName;
  // userCount++;
  if (phoneNumberExists(newNumber)) {

    File tempFile = SD.open("/userdata_temp.txt", FILE_WRITE);
    myFile = SD.open("/userdata.txt", FILE_READ);

    if (myFile && tempFile) {
      while (myFile.available()) {
        String record = myFile.readStringUntil('\n');
        
        // int recordID = record.substring(record.indexOf(',') + 1, record.indexOf('.')).toInt();
        // Serial.println(recordID);
        if (record != lcd_record_del) {
          tempFile.println(record);
        }
      }
      myFile.close();
      tempFile.close();

      SD.remove("/userdata.txt");
      SD.rename("/userdata_temp.txt", "/userdata.txt");

      lcd.println("Record deleted.");
      Serial.println("Record deleted.");
    } else {
      lcd.print("Deleting Error.");
      delay(1500);
      Serial.println("Error deleting record.");
    }
  }
}
