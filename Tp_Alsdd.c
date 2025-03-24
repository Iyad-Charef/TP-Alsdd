
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE 256
#define MAX_HISTORY 5
#define MAX_APPOINTMENTS 10

/* ---------- Data Structures ---------- */

typedef struct Employee {
    char id[9];                   // Up to 8 characters
    char fullName[100];
    int totalConsultations;
    char lastConsultDate[11];     // Format: DD/MM/YYYY
    char returnToWorkDate[11];    // Optional: empty string if not provided
    int historyCount;
    char history[MAX_HISTORY][30]; // Up to 5 past consultation reasons
    struct Employee *next;
} Employee;

typedef struct Consultation {
    char employeeID[9];
    char employeeName[100];
    char time[10];
    char reason[30];
    int priority;      // Lower number = higher priority
    struct Consultation *next;
} Consultation;

/* ---------- Helper Functions ---------- */

int stringToInt(const char *str) {
    int result = 0;
    int sign = 1;

    // Check for negative numbers
    if (*str == '-') {
        sign = -1;
        str++;
    }

    // Convert each character to the corresponding integer value
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        } else {
            // Handle invalid characters
            break;
        }
        str++;
    }

    return result * sign;
}


int parseField(char **start, char delimiter, char *dest) {
    char *end = strchr(*start, delimiter);
    if (end) {
        *end = '\0';
        strcpy(dest, *start);
        *start = end + 1;
        return 0;
    }
    else {
        strcpy(dest, *start);
        *start = NULL;
        return 1;
    }
}

void parseHistory(char *start, Employee *newEmp) {
    newEmp->historyCount = 0;
    char *histStart = start;
    char *histEnd = strchr(histStart, ',');
    while (histEnd && newEmp->historyCount < MAX_HISTORY) {
        *histEnd = '\0';
        strcpy(newEmp->history[newEmp->historyCount], histStart);
        newEmp->history[newEmp->historyCount][29] = '\0';
        newEmp->historyCount++;
        histStart = histEnd + 1;
        histEnd = strchr(histStart, ',');
    }
    if (newEmp->historyCount < MAX_HISTORY) {
        strcpy(newEmp->history[newEmp->historyCount], histStart);
        newEmp->history[newEmp->historyCount][29] = '\0';
        newEmp->historyCount++;
    }
}

void appendEmp(Employee **head, Employee **tail, Employee *newEmp) {
    if (!*head) {
        *head = newEmp;
        *tail = newEmp;
    } else {
        (*tail)->next = newEmp;
        *tail = newEmp;
    }
}

/* ---------- Employee Records Functions ---------- */

/* Load employee records from EmpRecords.txt into a linked list */
Employee* loadEmployees(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
         printf("Error opening employee records file: %s\n", filename);
         return NULL;
    }
    
    Employee *head = NULL, *tail = NULL;
    char line[MAX_LINE];
    
    while (fgets(line, MAX_LINE, fp)) {
        // Remove newline characters and replace with null terminator
        int len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            if (len > 1 && line[len - 2] == '\r') {
                line[len - 2] = '\0';
            }
        }
        if (strlen(line) == 0) {
            continue;
        }

        Employee *newEmp = (Employee *)malloc(sizeof(Employee));
          if (!newEmp) exit(1);
          memset(newEmp, 0, sizeof(Employee));

        // Parse the line for each field
        char *start = line;
        parseField(&start, ';', newEmp->id);
        parseField(&start, ';', newEmp->fullName);
        char totalConsultationsStr[10];
        parseField(&start, ';', totalConsultationsStr);
        newEmp->totalConsultations = stringToInt(totalConsultationsStr);
        parseField(&start, ';', newEmp->lastConsultDate);
        // Check if returnToWorkDate is present
        if (start && strchr(start, ';')) {
            parseField(&start, ';', newEmp->returnToWorkDate);
        } else {
            strcpy(newEmp->returnToWorkDate, "");
        }

        // Parse history if present
        if (start && *start != '\0') {
            parseHistory(start, newEmp);
        } else {
            newEmp->historyCount = 0;
        }

        newEmp->next = NULL;

        // Append the new record to the linked list.
        appendEmp(&head, &tail, newEmp);
    }
    fclose(fp);
    return head;
}

/* Save the employee linked list back to EmpRecords.txt */
void saveEmployees(const char *filename, Employee *head) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
         printf("Error opening file for writing: %s\n", filename);
         return;
    }
    while(head) {
         fprintf(fp, "%s;%s;%d;%s;%s;", head->id, head->fullName, head->totalConsultations,
                 head->lastConsultDate, head->returnToWorkDate);
         for (int i = 0; i < head->historyCount; i++) {
             fprintf(fp, "%s", head->history[i]);
             if (i < head->historyCount - 1)
                 fprintf(fp, ",");
         }
         fprintf(fp, "\n");
         head = head->next;
    }
    fclose(fp);
}

/* Find an employee by ID in the linked list */
Employee* findEmployee(Employee *head, const char *id) {
    while (head) {
         if(strcmp(head->id, id) == 0)
              return head;
         head = head->next;
    }
    return NULL;
}

/* Simple print of all employee records (for debugging purposes) */
void printEmployees(Employee *head) {
    printf("=== Employee Records ===\n");
    while(head) {
        printf("ID: %s, Name: %s, Consultations: %d, Last: %s, Return: %s\nHistory: ",
               head->id, head->fullName, head->totalConsultations,
               head->lastConsultDate, head->returnToWorkDate);
        for (int i = 0; i < head->historyCount; i++)
            printf("[%s] ", head->history[i]);
        printf("\n");
        head = head->next;
    }
    printf("========================\n");
}


/* ---------- Main Function ---------- */

int main(void) {
    printf("hello there");
    Employee *empList = loadEmployees("EmpRecords.txt");
    if (!empList) {
         printf("Unable to load employee records.\n");
         return 1;
    }
    printEmployees(empList);
    return 0;
}
