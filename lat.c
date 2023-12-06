unsigned char* lat(unsigned char *data){
  //returns the latitude in MMMM.MM //
  unsigned int pos_start = findComma(data, 3);
  unsigned int pos_end = findComma(data, 4);
  unsigned char latitude[50];
  
  int i;


   // Variables for the integer and decimal parts
  int intValue = 0;
  int decimalValue = 0;
  int isDecimal = 0;  // Flag to indicate if decimal part is being processed
  int decimalPlaces = 0;
  int degrees = 0;
  int minutes = 0;
  double finalValue = 0;
  char temp = 0;
  char result[50];
  int j = 0;

  for(i = pos_start; i < pos_end; i++);
    latitude[(i - pos_start)] = data[i];  
    
  // Iterate through each character in the string
  for (i = 0; latitude[i] != '\0'; ++i) {
      if (latitude[i] == '.') {
          isDecimal = 1;  // Set the flag to process the decimal part
      } else {
          if (!isDecimal) {
              intValue = intValue * 10 + (latitude[i] - '0');
          } else {
              decimalValue = decimalValue * 10 + (latitude[i] - '0');
              ++decimalPlaces;  // Count the number of decimal places
          }
      }
  }





 // Calculate degrees and minutes
    degrees = intValue / 100;
    minutes = intValue % 100;

    // Calculate the final decimal degrees
    finalValue = degrees + (minutes + decimalValue / 10000.0) / 60.0;
    
    i = 0;
    while (finalValue > 0) {
        i++;
        result[i] = (char)(48 + (finalValue % 10));
        finalValue /= 10;
    }

    // Reverse the string
    for (j = 0; j < i / 2; ++j) {
        temp = result[j];
        result[j] = result[i - j - 1];
        result[i - j - 1] = temp;
    }

    result[i] = '\0'; // Null-terminate the string
    
    
    
    
    
  return "a";
  
}
