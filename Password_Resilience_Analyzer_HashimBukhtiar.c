#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void displayMenu() {
    printf("\n\n---------- PASSWORD RESILIENCE ANALYZER MAIN MENU ----------\n\n");
    printf("1. Test a new password\n");
    printf("2. View strength of last tested password\n");
    printf("3. Exit\n\n");
}

int selectOption() {
    char input[50];
    char *end;
    int choice;

    displayMenu();
    printf("Enter Option: ");
    while (1) {
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        choice = (int) strtol(input, &end, 10);
        if (end == input || *end != '\0' || choice < 1 || choice > 3) {
            displayMenu();
            printf("Invalid input, try again: ");
        } else {
            break;
        }
    }
    return choice;
}

char* getPassword() {
    int size = 64;
    char *password = NULL;
    int i;

    do {
        password = realloc(password, size);
        if (password == NULL) {
            printf("Failed to allocate memory.\n");
            return NULL;
        }

        printf("Enter password: ");
        int c;
        i = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
            if (i == size - 1) { 
                size *= 2;
                char *new_password = realloc(password, size);
                if (new_password == NULL) {
                    printf("Failed to allocate memory\n");
                    free(password);
                    return NULL;
                }
                password = new_password;
            }
            password[i++] = c;
        }
        password[i] = '\0'; 

        if (i < 8) {
            printf("Error: Your password must be 8 characters or longer!\n");
        }
    } while (i < 8);

    return password;
}

char** generateSubstrings(char *str, int* size) {
    int n = strlen(str);
    char **substrings = malloc(sizeof(char*) * n*(n+1)/2);
    int index = 0;

    for (int len = 2; len <= n; len++) { // Start from 2 to omit single characters
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            char *substring = (char *)malloc(sizeof(char)*(len+1));
            int subIndex = 0;

            for (int k = i; k <= j; k++) {
                substring[subIndex++] = str[k];
            }
            substring[subIndex] = '\0';
            substrings[index++] = substring;
        }
    }

    *size = index; // Set the size of the array
    return substrings; // Return the array
}

int hasLowercase(char* password) {
    int pwd_length = strlen(password);
    for (int i=0; i<pwd_length; i++) {
        if (islower(password[i])) {
            return 1;
        }
    }
    return 0;
}

int hasUppercase(char* password) {
    int pwd_length = strlen(password);
    for (int i=0; i<pwd_length; i++) {
        if (isupper(password[i])) {
            return 1;
        }
    }
    return 0;
}

int hasDigit(char* password) {
    int pwd_length = strlen(password);
    for (int i=0; i<pwd_length; i++) {
        if (isdigit(password[i])) {
            return 1;
        }
    }
    return 0;
}

int hasSpecialChar(char* password) {
    int pwd_length = strlen(password);
    for (int i=0; i<pwd_length; i++) {
        int ascii = (int) password[i];
        if ((32 <= ascii && ascii <= 47) || (58 <= ascii && ascii <= 64) 
        || (91 <= ascii && ascii <= 96) || (123 <= ascii && ascii <= 127)) {
            return 1;
        }
    }
    return 0;
}

int hasRepeatingPattern(char* password) {
    // If there is a repeating pattern, return 1
    // Else return 0
    // 123123123 -> 1
    // thisisapwd -> 0
    // thiswasapaSSword -> 0
    int length = strlen(password);
    const int minPatternLength = 2; // Only flag sequences of at least 2 characters
    for (int i = 0; i < length; i++) {
        for (int j = i + minPatternLength; j < length; j++) {
            int substringLength = j - i;
            if (j + substringLength <= length && strncmp(password + i, password + j, substringLength) == 0) {
                return 1; // Repeating sequence found
            }
        }
    }
    return 0; // No repeating sequence found
}

int hasDictionaryWord(char filename[], char* password) {
    int size;
    char** substrings = generateSubstrings(password, &size);

    FILE* file = fopen(filename, "r"); // Open the file
    if (file == NULL) {
        printf("Could not open file\n");
        return 0;
    }

    char line[256]; // Buffer to hold each line
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline character from line
        for (int i = 0; i < size; i++) {
            if (strcmp(substrings[i], line) == 0) { // Compare strings to see if password contains a dictionary word
                //printf("substring: %s | line: %s\n", substrings[i], line);
                return 0;
            }
        }
    }
    fclose(file);
    return 1;
}

int evaluateStrength(char filename[], char* password) {
    int pwd_length = strlen(password);
    int lengthyPassword = 0;
    if (pwd_length >= 12) {
        lengthyPassword = 1;
    }
    
    /*printf("Lowercase %d\n", hasLowercase(password));
    printf("Uppercase %d\n", hasUppercase(password));
    printf("Digit %d\n", hasDigit(password));
    printf("Special Char %d\n", hasSpecialChar(password));
    printf("Repeating Pattern %d\n", !hasRepeatingPattern(password));
    printf("Dictionary word %d\n", hasDictionaryWord(filename, password));
    printf("Lengthy %d\n", lengthyPassword);*/

    /*return hasLowercase(password) + hasUppercase(password) + hasDigit(password) 
    + hasSpecialChar(password) + !hasRepeatingPattern(password) 
    + hasDictionaryWord(filename, password) + lengthyPassword;*/

    return hasLowercase(password) + hasUppercase(password) + hasDigit(password) 
    + hasSpecialChar(password) + lengthyPassword;
}

void outputStrength(char password[], int pwdStrength, char filename[]) {
    // burgersss -> Weak
    // B123123b -> Moderate
    // C1234567c -> Strong
    // B123$%a%$^@! -> Password Strength: Very Strong

    switch (pwdStrength) {
        case 0 ... 2:
            printf("\033[1;31m"); // Set the text color to red
            printf("Password Strength: Weak\n");
            break;
        case 3 ... 4:
            printf("\033[1;33m"); // Set the text color to yellow
            printf("Password Strength: Moderate\n");
            break;
        case 5:
            if (!hasRepeatingPattern(password) && hasDictionaryWord(filename, password)) {
                printf("\033[0;32m"); // Set the text color to dark green
                printf("Password Strength: Very Strong\n");
                break;
            } else {
                printf("\033[1;32m"); // Set the text color to light green
                printf("Password Strength: Strong\n");
                break;
            }       
    }
    printf("\033[0m"); // Reset the text color to default
}

int main() {
    int havePassword = 0;
    char* password;
    const char filename[] = "dictionary_words.txt";
    while (1) {
        int option = selectOption();
        if (option == 1) { // Test new password
            if (havePassword == 1) {
                free(password);
            }
            password = getPassword();
            if (password != NULL) {
                int passwordStrength = evaluateStrength(filename, password);
                outputStrength(password, passwordStrength, filename);
                havePassword = 1;
            } else {
                printf("Something went wrong, you must enter a valid password.\n");
            }
        } else if (option == 2) { // View strength of last tested password
            if (havePassword == 1) {
                outputStrength(password, evaluateStrength(filename, password), filename);
            } else {
                printf("You have not created a password to test yet.\n");
            }
        } else { // Exit
            printf("\nThank you for using this program. Exiting...\n");
            free(password);
            return 0;
        }
    }
}