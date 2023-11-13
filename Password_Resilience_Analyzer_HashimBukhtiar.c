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

int evaluateStrength(char* password) {
    int pwd_length = strlen(password);
    int lengthyPassword = 0;
    if (pwd_length >= 12) {
        lengthyPassword = 1;
    }
    return hasLowercase(password) + hasUppercase(password) + hasDigit(password) + hasSpecialChar(password) + !hasRepeatingPattern(password) + lengthyPassword;
}

void outputStrength(char password[], int pwdStrength) {
    if (0 <= pwdStrength && pwdStrength<=2) { // Weak password
        printf("Password Strength: Weak\n");
    } else if (3 <= pwdStrength && pwdStrength <= 4) { // Moderate password
        printf("Password Strength: Moderate\n");
    } else if (5 <= pwdStrength && pwdStrength <= 6) { // Strong password
        printf("Password Strength: Strong\n");
    } else if (pwdStrength == 7) {
        printf("Password Strength: Very Strong\n");
    } else {
        printf("Something went wrong.\n");
        printf("Password: %s", password);
        printf("Evaluated Strength: %d", pwdStrength);
    }
}

int main() {
    int havePassword = 0;
    char* password;
    while (1) {
        int option = selectOption();
        if (option == 1) { // Test new password
            if (havePassword == 1) {
                free(password);
            }
            password = getPassword();
            if (password != NULL) {
                int passwordStrength = evaluateStrength(password);
                outputStrength(password, passwordStrength);
                havePassword = 1;
            } else {
                printf("Something went wrong, you must enter a valid password.\n");
            }
        } else if (option == 2) { // View strength of last tested password
            if (havePassword == 1) {
                outputStrength(password, evaluateStrength(password));
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