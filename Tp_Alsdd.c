
/*    Hi this program was made by iyad charef fro the Tp of Alsdd, i hope you like it    */
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

// remove newline characters (\n,\r) from a string

void removeNewline(char *line) {
    int len = strlen(line);
    if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') {
            line[len - 2] = '\0';
        }
    }
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
        newEmp->historyCount++;
        histStart = histEnd + 1;
        histEnd = strchr(histStart, ',');
    }
    if (newEmp->historyCount < MAX_HISTORY) {
        strcpy(newEmp->history[newEmp->historyCount], histStart);
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

/* Find an employee by ID in the linked list */
Employee* findEmployee(Employee *head, const char *id) {
    while (head) {
         if(strcmp(head->id, id) == 0)
              return head;
         head = head->next;
    }
    return NULL;
}

//clears leftovers like \n after scanf
void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


/* ---------- Employee Records Functions ---------- */

/* Load employee records from EmpRecords.txt into a linked list */
void loadEmployees(const char *filename, Employee **head, Employee **tail) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening employee records file: %s\n", filename);
        return ;
    }

    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, fp)) {
        // Remove newline characters and replace with null terminator
        removeNewline(line);
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
        if (start && strchr(start, '/')) {
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
        appendEmp(head, tail, newEmp);
    }
    fclose(fp) ;
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

/* Add an employee. Used for new Pre-employment Visit appointments */
void addEmployee(Employee **head, Employee **tail, const char *id, const char *name, int consultations,
    const char *lastDate, const char *returnDate, char history[][30], int historyCount) {
    Employee *newEmp = (Employee *)malloc(sizeof(Employee));
    if (!newEmp) exit(1);
    memset(newEmp, 0, sizeof(Employee));
    strncpy(newEmp->id, id, 8);
    newEmp->id[8] = '\0';
    strncpy(newEmp->fullName, name, sizeof(newEmp->fullName)-1);
    newEmp->totalConsultations = consultations;
    strncpy(newEmp->lastConsultDate, lastDate, 10);
    newEmp->lastConsultDate[10] = '\0'; 

    if (returnDate) {
        strncpy(newEmp->returnToWorkDate, returnDate, 10);
        newEmp->returnToWorkDate[10] = '\0'; 
    } else {
        newEmp->returnToWorkDate[0] = '\0';
    }

    newEmp->historyCount = historyCount;
    for (int i = 0; i < historyCount; i++){
        strncpy(newEmp->history[i], history[i], 29);
        newEmp->history[i][29] = '\0';
    }
    newEmp->next = NULL;

    // Append the new employee.
    appendEmp(head, tail, newEmp);
}

/* Remove an employee from the list */
void removeEmployee(Employee **head, Employee **tail, const char *id) {
    Employee *current = *head;
    Employee *previous = NULL;

    while (current != NULL && strcmp(current->id, id) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Employee with ID %s not found.\n", id);
        return;
    }

    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }

    if (current == *tail) {
        *tail = previous;
    }

    free(current);
    printf("Employee with ID %s removed.\n", id);
}

/* Update an employee's information */
void updateEmployee(Employee *head, const char *id, const char *newName,
     int newConsultations, const char *newLastDate, const char *newReturnDate,
      char newHistory[30]) {
    Employee *emp = findEmployee(head, id);
    if (!emp) {
        printf("Employee with ID %s not found.\n", id);
        return;
    }

    if (newName) {
        strncpy(emp->fullName, newName, sizeof(emp->fullName) - 1);
        emp->fullName[sizeof(emp->fullName) - 1] = '\0';
    }

    emp->totalConsultations = newConsultations;

    if (newLastDate) {
        strncpy(emp->lastConsultDate, newLastDate, 10);
        emp->lastConsultDate[10] = '\0';
    }

    if (newReturnDate) {
        strncpy(emp->returnToWorkDate, newReturnDate, 10);
        emp->returnToWorkDate[10] = '\0';
    } else {
        emp->returnToWorkDate[0] = '\0';
    }

    if (emp->historyCount < MAX_HISTORY) {
        strncpy(emp->history[emp->historyCount], newHistory, 29);
        emp->history[emp->historyCount][29] = '\0';
        emp->historyCount++;
    } else {
        // Remove the oldest and add the new consultation.
        for (int i = 1; i < MAX_HISTORY; i++)
            strcpy(emp->history[i - 1], emp->history[i]);
        strncpy(emp->history[MAX_HISTORY - 1], newHistory, 29);
        emp->history[MAX_HISTORY - 1][29] = '\0';
    }

    printf("Employee with ID %s updated.\n", id);
}

/*print of all employee records (for debugging purposes) */
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

/* ---------- Consultation (Appointment) Queue Functions ---------- */

/* Given a reason, return its assigned priority (lower means higher priority) */
int getPriority(const char *reason) {
    if (strcmp(reason, "Work-accident") == 0)
        return 1;
    else if (strcmp(reason, "Occupational-Disease") == 0)
        return 2;
    else if (strcmp(reason, "Return-to-Work") == 0 || strcmp(reason, "Pre-employement") == 0)
        return 3;
    else if (strcmp(reason, "Periodic") == 0)
        return 4;
    else
        return 5;  // This branch should never be reached if all reasons are perfectly typed.
} 

/* Insert a new consultation node into the queue + some logic to preserve priority order and FIFO order for same priority */
void enqueueConsultation(Consultation **head, Consultation *newNode) {
   if (*head == NULL || newNode->priority < (*head)->priority) {
        newNode->next = *head;
        *head = newNode;
   } else {
        Consultation *curr = *head;
        while (curr->next != NULL && curr->next->priority <= newNode->priority)
             curr = curr->next;
        newNode->next = curr->next;
        curr->next = newNode;
   }
}

/* Count the number of consultations in a queue */
int countConsultations(Consultation *head) {
   int count = 0;
   while(head) {
        count++;
        head = head->next;
   }
   return count;
}

/* Remove and return the first consultation in the queue */
Consultation* dequeueConsultation(Consultation **head) {
   if (*head == NULL)
        return NULL;
   Consultation *temp = *head;
   *head = (*head)->next;
   temp->next = NULL;
   return temp;
}

/* Print the consultation queue */
void printConsultations(Consultation *head) {
   printf("=== Appointments Queue ===\n");
   while(head) {
        printf("Time: %s, ID: %s, Name: %s, Reason: %s, Priority: %d\n",
               head->time, head->employeeID, head->employeeName, head->reason, head->priority);
        head = head->next;
   }
   printf("==========================\n");
}

/* Load consultations from Consultations.txt. */
void loadConsultations(const char *filename, Consultation **Qhead) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
         printf("Error opening consultations file: %s\n", filename);
         return ;
    }
    
    char line[MAX_LINE];
    int Qcount = 0;
    
    while ((fgets(line, MAX_LINE, fp)) && (Qcount < MAX_APPOINTMENTS)) {

        removeNewline(line);

        if (strlen(line) == 0) {
            continue;
        }
        
         Consultation *cons = (Consultation*)malloc(sizeof(Consultation));
         if (!cons) exit(1);
         memset(cons, 0, sizeof(Consultation));
         
         // Parse the line for each field
        char *start = line;
        parseField(&start, ';', cons->employeeID);
        parseField(&start, ';', cons->employeeName);
        parseField(&start, ';', cons->time);
        parseField(&start, ';', cons->reason);
        printf("Reason: %s. \n", cons->reason);
        cons->priority = getPriority(cons->reason);
        cons->next = NULL;
        enqueueConsultation(Qhead, cons);
        Qcount++;
    }
    
    fclose(fp);
}

/* Write the consultation queue to Consultations.txt (for next-day appointments) */
void saveConsultations(const char *filename, Consultation *head) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
         printf("Error writing consultations file: %s\n", filename);
         return;
    }
    while(head) {
         fprintf(fp, "%s;%s;%s;%s\n", head->employeeID, head->employeeName, head->time, head->reason);
         head = head->next;
    }
    fclose(fp);
}

/* ---------- Appointment Management Functions ---------- */

/* Process (i.e. close) an appointment.
   This dequeues an appointment, asks for today’s date then updates the corresponding employee record.
   For "Pre-employment Visit", a new record is added if none is found.
*/

void processAppointment(Consultation **Qhead, Employee **Ehead, Employee **Etail) {
    if (*Qhead == NULL) {
         printf("No appointments to process today.\n");
         return;
    }
    Consultation *appoint = dequeueConsultation(Qhead);
    printf("Processing appointment: %s - %s at %s for %s\n", 
           appoint->employeeID, appoint->employeeName, appoint->time, appoint->reason);
    
     char currentDate[11];
     printf("Enter today’s date (DD/MM/YYYY): ");
     scanf("%10s", currentDate);

    char newReturnDate[11] = "";
    printf("Enter new return date (DD/MM/YYYY) or press Enter to skip: ");
    fgets(newReturnDate, sizeof(newReturnDate), stdin);
    removeNewline(newReturnDate);

    
    Employee *emp = findEmployee(*Ehead, appoint->employeeID);
    if (emp) {
         updateEmployee(*Ehead, appoint->employeeID, appoint->employeeName,
           emp->totalConsultations + 1, currentDate, newReturnDate, appoint->reason);
         
         printf("Employee record updated for %s.\n", emp->fullName);
    } else {
         if ((strcmp(appoint->reason, "Pre-employement")) == 0) {
             char history[1][30];
             strncpy(history[0], appoint->reason, 29);
             addEmployee(Ehead, Etail,  appoint->employeeID, appoint->employeeName, 1,
                         currentDate, newReturnDate, history, 1);
             printf("New employee record created for %s.\n", appoint->employeeName);
         } else {
             printf("No employee record found for %s.\n", appoint->employeeID);
         }
    }
    
    free(appoint);
}


Consultation* removeLastAppointment(Consultation **head) {
    if (*head == NULL) {
        return NULL; // Queue is empty
    }

    Consultation *prev = NULL;
    Consultation *curr = *head;

    // Traverse to the end of the queue
    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    // If there is only one element in the queue
    if (prev == NULL) {
        *head = NULL;
    } else {
        prev->next = NULL;
    }

    return curr;
}


/* Add a new appointment (on-demand).
   Only three types are allowed: Work accident, Occupational disease, or Pre-employment visit.
   If the total number of today's appointments would exceed MAX_APPOINTMENTS, the lowest–priority
   appointment is moved to the next-day queue.
*/
void addNewAppointment(Consultation **Qhead, Consultation **Nhead) {
    Consultation *newApp = (Consultation*)malloc(sizeof(Consultation));
    if (!newApp) exit(1);
    memset(newApp, 0, sizeof(Consultation));
    
    printf("Enter Employee ID: ");
    fgets(newApp->employeeID, sizeof(newApp->employeeID), stdin);
    removeNewline(newApp->employeeID);
    flushInput(); 
    
    printf("Enter Employee Name: ");
    fgets(newApp->employeeName, sizeof(newApp->employeeName), stdin);
    removeNewline(newApp->employeeName);
    
    printf("Enter Consultation Time (e.g., 14h00): ");
    fgets(newApp->time, sizeof(newApp->time), stdin);
    removeNewline(newApp->time);
    
    printf("Select Consultation Reason:\n");
    printf("1. Work Accident\n2. Occupational Disease\n3. Pre-employment Visit\n");
    int choice = 0;
    scanf("%d", &choice);
    flushInput();
    if (choice == 1)
         strcpy(newApp->reason, "Work-accident");
    else if (choice == 2)
         strcpy(newApp->reason, "Occupational-Disease");
    else if (choice == 3)
         strcpy(newApp->reason, "Pre-employement");
    else {
         printf("Invalid choice. Appointment not added.\n");
         free(newApp);
         return;
    }
    newApp->priority = getPriority(newApp->reason);
    newApp->next = NULL;
    
    if(countConsultations(*Qhead) >= MAX_APPOINTMENTS) {
         // Remove the last appointment and reschedule it.
        Consultation *lastApp = removeLastAppointment(Qhead);
        if (lastApp) {
            enqueueConsultation(Nhead, lastApp);
            printf("Maximum appointments reached. Rescheduling appointment for %s to next day.\n", lastApp->employeeName);
        }
    }
    
    enqueueConsultation(Qhead, newApp);
    printf("New appointment added for %s at %s.\n", newApp->employeeName, newApp->time);
}

/* Cancel an appointment by Employee ID from today’s queue */
void cancelAppointment(Consultation **Qhead) {
    if (*Qhead == NULL) {
         printf("No appointments to cancel.\n");
         return;
    }
    
    char id[9];
    printf("Enter Employee ID to cancel appointment: ");
    scanf("%8s", id);
    flushInput();
    
    Consultation *curr = *Qhead, *prev = NULL;
    while (curr) {
         if (strcmp(curr->employeeID, id) == 0) {
              if (prev)
                  prev->next = curr->next;
              else
                  *Qhead = curr->next;
              printf("Appointment for %s cancelled.\n", curr->employeeName);
              free(curr);
              return;
         }
         prev = curr;
         curr = curr->next;
    }
    printf("No appointment found for Employee ID: %s\n", id);
}

/* Reschedule an appointment (move from today’s queue to next-day queue) */
void rescheduleAppointment(Consultation **Qhead, Consultation **Nhead) {
    if (*Qhead == NULL) {
         printf("No appointments to reschedule.\n");
         return;
    }
    
    char id[9];
    printf("Enter Employee ID to reschedule appointment: ");
    scanf("%8s", id);
    flushInput();
    
    Consultation *curr = *Qhead, *prev = NULL;
    while (curr) {
         if (strcmp(curr->employeeID, id) == 0) {
              if (prev)
                  prev->next = curr->next;
              else
                  *Qhead = curr->next;
              printf("Enter new consultation time for next day (e.g., 09h30): ");
              scanf("%9s", curr->time);
              flushInput();
              curr->next = NULL;
              enqueueConsultation(Nhead, curr);
              printf("Appointment for %s rescheduled to next day at %s.\n", curr->employeeName, curr->time);
              return;
         }
         prev = curr;
         curr = curr->next;
    }
    printf("No appointment found for Employee ID: %s\n", id);
}


/* Helper function to calculate the difference in years between two dates */
int calculateYearDifference(const char *date1, const char *date2) {
    int day1, month1, year1;
    int day2, month2, year2;

    // Parse the first date
    sscanf(date1, "%d/%d/%d", &day1, &month1, &year1);

    // Parse the second date
    sscanf(date2, "%d/%d/%d", &day2, &month2, &year2);

    // Calculate the difference in years
    int yearDifference = abs(year2 - year1);

    // Adjust the difference if the second date is before the first date in the year
    if (month2 < month1 || (month2 == month1 && day2 < day1)) {
        yearDifference--;
    }

    return yearDifference;
}


/* Schedule next day appointments */
void scheduleNextday(Employee *Ehead, Consultation **Nhead) {
    // Get today's date
    char tomorrow[11];
    printf("Enter tomorrow's date (DD/MM/YYYY): ");
    scanf("%10s", tomorrow);
    flushInput();

    while (Ehead) {
        if (strcmp(Ehead->returnToWorkDate, tomorrow) == 0 ||
            calculateYearDifference(Ehead->lastConsultDate, tomorrow) >= 1) {
            Consultation *newApp = (Consultation *)malloc(sizeof(Consultation));
            if (!newApp) exit(1);
            memset(newApp, 0, sizeof(Consultation));
            strcpy(newApp->employeeID, Ehead->id);
            strcpy(newApp->employeeName, Ehead->fullName);
            strcpy(newApp->time, "09h00"); // still underwork, so i put a Default time to make it simpler
            if (strcmp(Ehead->returnToWorkDate, tomorrow) == 0) {
                strcpy(newApp->reason, "Return-to-Work");
            } else {
                strcpy(newApp->reason, "Periodic");
            }
            newApp->priority = getPriority(newApp->reason);
            newApp->next = NULL;
            enqueueConsultation(Nhead, newApp);
        }
        Ehead = Ehead->next;
    }
}

/* ---------- Main Function ---------- */

int main(void) {
    printf("hello there \n");
    Employee *Ehead = NULL, *Etail = NULL;
    loadEmployees("EmpRecords.txt", &Ehead, &Etail);
    Consultation *Qhead = NULL ;
    loadConsultations("Consultations.txt",&Qhead);
    Consultation *Nhead = NULL;

    if (!Qhead) {
        printf("Unable to load employee records.\n");
        return 1;
    }
    printEmployees(Ehead);
    printConsultations(Qhead);

    int choice = 0;
        do {
             printf("\n--- Occupational Health Appointment Management ---\n");
             printf("1. Display Employee Records\n");
             printf("2. Display Today's Appointments\n");
             printf("3. Display Next Day's Appointments\n");
             printf("4. Process (Close) an Appointment\n");
             printf("5. Add a New Appointment\n");
             printf("6. Cancel an Appointment\n");
             printf("7. Reschedule an Appointment (to next day)\n");
             printf("8. Schedule Next Day Appointments\n");
             printf("9. Exit (Update Files)\n");
             printf("Enter your choice: ");
             if (scanf("%d", &choice) != 1) {
                 flushInput();
                 printf("Invalid input. Please enter a number.\n");
                 choice = 0; // Set choice to an invalid option to continue the loop
                 continue;
             }
             flushInput();

             switch(choice) {
                  case 1:
                       printEmployees(Ehead);
                       break;
                  case 2:
                       printConsultations(Qhead);
                       break;
                  case 3:
                       printConsultations(Nhead);
                       break;
                  case 4:
                       processAppointment(&Qhead, &Ehead, &Etail);
                       break;
                  case 5:
                       addNewAppointment(&Qhead, &Nhead);
                       break;
                  case 6:
                       cancelAppointment(&Qhead);
                       break;
                  case 7:
                       rescheduleAppointment(&Qhead, &Nhead);
                       break;
                  case 8:
                       scheduleNextday(Ehead, &Nhead);
                       break;
                  case 9:
                       printf("Exiting and updating files...\n");
                       break;
                  default:
                       printf("Invalid option. Try again.\n");
             }
             
        } while(choice != 9);
         
    
    /* Save the updated employee records and next day consultations */
    saveEmployees("EmpRecords.txt", Ehead);
    saveConsultations("Consultations.txt", Nhead);
    
    printf("Files updated. Goodbye.\n");
    return 0;
}
