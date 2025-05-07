#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>


#define MAX_NAME_LENGTH 50
#define MAX_REPORT_LENGTH 1000
#define MAX_DATETIME_LENGTH 30
#define MAX_RECORDER_LENGTH 50
#define MAX_DOCUMENT_NAME_LENGTH 100
#define MAX_STATUS_LENGTH 20
#define MAX_ITEM_NAME_LENGTH 20
#define MAX_BLOTTERS 100
#define MAX_LENGTH 1000
#define MAX_LINE 1024

typedef struct Resident {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    char gender;
    struct Resident* next;
    struct BlotterReport* blotterReports;
} Resident;

typedef struct BlotterReport {
    char report[MAX_REPORT_LENGTH];
    char dateTime[MAX_DATETIME_LENGTH];
    char recorder[MAX_RECORDER_LENGTH];
    struct BlotterReport* next;
} BlotterReport;

typedef struct Item {
    int id;
    char itemName[MAX_ITEM_NAME_LENGTH];
    int itemQuantity;
    int availableQuantity;
    struct Item* next;
} Item;

Resident* head = NULL;
Item* headItem = NULL;

typedef struct DocumentRequest {
    int requestId;
    char documentName[MAX_DOCUMENT_NAME_LENGTH];
    char residentName[MAX_NAME_LENGTH];
    char dateAdded[MAX_DATETIME_LENGTH];
    char status[MAX_STATUS_LENGTH];
    struct DocumentRequest* next;
} DocumentRequest;

typedef struct Queue {
    DocumentRequest* front;
    DocumentRequest* rear;
} Queue;

void enqueue(Queue* queue, const char* documentName, const char* residentName, const char* dateAdded);
void saveResidents(const char* filename);
void loadResidents(const char* filename);
void saveRequest(Queue* queue, const char* filename);
void loadRequest(Queue* queue, const char* filename);
Queue* createQueue();
void saveInventory(const char* filename);
void loadInventory(const char* filename);
void viewRequests(Queue* queue);
void clearScreen();
void deleteRequest(Queue* queue);
void formatTime(char* output);
void documentsRequestOrder(Queue* queue, const char* role);
int generateID();
void displayResidents();
void searchResident();
void addResident();
void deleteResident();
void editResident();
void blotterReport();
void printCentered(const char* message);
const char* checkRole();
void manageResident(const char* role);
void viewBlotterReports();
void manageBlotterReports();
void userDashboard(const char* role);
void inventoryDisplay();
void addItem();
void deleteItem();
void editItem();
void inventoryManage();
void freeQueue(Queue* queue);
void freeResidents();
void freeInventory();
void freeDocumentRequest(DocumentRequest* request);
void updateRequestStatus(Queue* queue);
void exportSelectedBlotters();
void printRequest(Queue* queue);
void generateDocx(int reqID, const char* name, const char* type, const char* date);
void residentToExcel(const char* filename);
void freeInventory() {
    Item* current = headItem;
    Item* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    headItem = NULL; 
}

void freeDocumentRequest(DocumentRequest* request) {
    if (request) {
        free(request);
    }
}

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        perror("Memory allocation failed in createQueue");
        exit(EXIT_FAILURE);
    }
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

void enqueue(Queue* queue, const char* documentName, const char* residentName, const char* dateAdded) {
    DocumentRequest* newRequest = (DocumentRequest*)malloc(sizeof(DocumentRequest));
    if (newRequest == NULL) {
        perror("Memory allocation failed in enqueue");
        exit(EXIT_FAILURE);
    }
    newRequest->requestId = generateID();
    strncpy(newRequest->documentName, documentName, MAX_DOCUMENT_NAME_LENGTH - 1);
    newRequest->documentName[MAX_DOCUMENT_NAME_LENGTH - 1] = '\0';
    strncpy(newRequest->residentName, residentName, MAX_NAME_LENGTH - 1);
    newRequest->residentName[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(newRequest->dateAdded, dateAdded, MAX_DATETIME_LENGTH - 1);
    newRequest->dateAdded[MAX_DATETIME_LENGTH - 1] = '\0';
    strcpy(newRequest->status, "Pending");
    newRequest->next = NULL;
    if (queue->rear == NULL) {
        queue->front = newRequest;
        queue->rear = newRequest;
    } else {
        queue->rear->next = newRequest;
        queue->rear = newRequest;
    }
}

void viewRequests(Queue* queue) {
    DocumentRequest* current = queue->front;
    if (current == NULL) {
        printf("No document requests available.\n");
        return;
    }
    int count = 1;
    printf("Document Requests:\n");
    while (current != NULL) {
        char suffix[3] = "th";
        if (count % 10 == 1 && count % 100 != 11) {
            strcpy(suffix, "st");
        } else if (count % 10 == 2 && count % 100 != 12) {
            strcpy(suffix, "nd");
        } else if (count % 10 == 3 && count % 100 != 13) {
            strcpy(suffix, "rd");
        }
        printf("\n%d%s: \nRequest ID: %d\nDocument Name: %s \nResident Name: %s \nDate Added: %s\n Status: %s\n",
               count, suffix, current->requestId, current->documentName, current->residentName, current->dateAdded, current->status);
        current = current->next;
        count++;
    }
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void deleteRequest(Queue* queue) {
    if (queue->front == NULL) {
        printf("No document requests to delete.\n");
        return;
    }

    viewRequests(queue);

    DocumentRequest* current = queue->front;
    int selectedIndex = 0;
    int count = 0;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    while (1) {
        printf("Use arrow keys to select a document. Press ENTER to delete.\n\n");
        current = queue->front;
        for (int i = 0; i < count; i++) {
            if (i == selectedIndex) {
                printf("-> %s\n", current->documentName);
            } else {
                printf("   %s\n", current->documentName);
            }
            current = current->next;
        }

        int ch = _getch();

        if (ch == 72) {  
            selectedIndex--;
            if (selectedIndex < 0) {
                selectedIndex = count - 1;
            }
        } else if (ch == 80) {  
            selectedIndex++;
            if (selectedIndex >= count) {
                selectedIndex = 0;
            }
        } else if (ch == 13) {  
            current = queue->front;
            DocumentRequest* previous = NULL;
            int index = 0;

            while (current != NULL) {
                if (index == selectedIndex) {
                    if (previous == NULL) {
                        queue->front = current->next;
                        if (queue->front == NULL) {
                            queue->rear = NULL;
                        }
                    } else {
                        previous->next = current->next;
                        if (previous->next == NULL) {
                            queue->rear = previous;
                        }
                    }
                    free(current);                  
                    printf("Document request deleted.\n");
                    Sleep(1000);
                    return;
                }
                previous = current;
                current = current->next;
                index++;
            }
        } else if (ch == '0') {
            printf("Deletion canceled.\n");
            Sleep(1000);
            return;
        }
    }
}

void formatTime(char* output) {
    time_t rawTime;
    struct tm* timeinfo;

    time(&rawTime);
    timeinfo = localtime(&rawTime);

    strftime(output, MAX_DATETIME_LENGTH, "%m/%d/%Y %H:%M:%S", timeinfo);
}

void documentsRequestOrder(Queue* queue, const char* role) {
	clearScreen();
    int choice;
    char documentName[MAX_DOCUMENT_NAME_LENGTH];
    char residentName[MAX_NAME_LENGTH];
    char dateAdded[MAX_DATETIME_LENGTH];
    formatTime(dateAdded);
    do {
        printf("\nDocument Request Order:\n");
        printf("[1]. Add Document Request\n");
        printf("[2]. View Document Requests\n");
        printf("[3]. Delete Document Request\n");
        if (strcmp(role, "Captain") == 0) {
            printf("[4]. Update Request Status\n");
            printf("[5]. Back to User Dashboard\n");
            printf("[6]. Print A Request\n");
            
        } else {
            printf("[4]. Back to User Dashboard\n");
        }
        
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            choice = 0;
            continue;
        }
        getchar();
        switch (choice) {
            case 1:
                printf("\nEnter document name: ");
                fgets(documentName, sizeof(documentName), stdin);
                documentName[strcspn(documentName, "\n")] = 0;
                printf("Enter resident name: ");
                fgets(residentName, sizeof(residentName), stdin);
                residentName[strcspn(residentName, "\n")] = 0;
                printf("Date Requested: %s", dateAdded);
                enqueue(queue, documentName, residentName, dateAdded);
                clearScreen();
                
                printf("\nDocument request added.\n");
                Sleep(2000);
                clearScreen();
                break;
            case 2:
                viewRequests(queue);                
                break;
            case 3:
                deleteRequest(queue);
                break;
            case 4:
                if (strcmp(role, "Captain") == 0) {
                    updateRequestStatus(queue);
                    break;
                }
            case 5:
                printf("Returning to User Dashboard.\n");
                return;
                clearScreen();
                break;
            case 6: 
            if (strcmp(role, "Captain") == 0) {
                    printRequest(queue);
				}
            	return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}

int generateID() {
    return 1000 + rand() % 9000;
}

void searchResident() {
	system("cls");
	displayResidents();
    char searchName[MAX_NAME_LENGTH];
    printf("Enter the name of the resident to search: ");
    fgets(searchName, sizeof(searchName), stdin);
    searchName[strcspn(searchName, "\n")] = 0;
    Resident* current = head;
    int found = 0;

    printf("\nSearch Results:\n");
    while (current != NULL) {
        if (strstr(current->name, searchName) != NULL) {
            int blotterCount = 0;
            BlotterReport* report = current->blotterReports;
            while (report != NULL) {
                blotterCount++;
                report = report->next;
            }
            printf("\nID: %d \nName: %s \nAge: %d \nGender: %c \nPending Blotter: %d\n",
                current->id, current->name, current->age, current->gender, blotterCount);
            found = 1;
            if (current->blotterReports != NULL) {
                printf("Blotter Description:\n");
                report = current->blotterReports;
                while (report != NULL) {
                    printf(" - %s (Recorded by: %s on %s)\n", report->report, report->recorder, report->dateTime);
                    report = report->next;
                }
            } else {
                printf("No pending blotter reports for this resident.\n");
            }
        }
        current = current->next;
    }
    if (!found) {
        printf("No residents found with the name '%s'.\n", searchName);
    }
}

void addResident() {
    Resident* newResident = (Resident*)malloc(sizeof(Resident));
    if (newResident == NULL) {
        perror("Memory allocation failed in addResident");
        exit(EXIT_FAILURE);
    }
    char residentGender;
    int id = generateID();
    newResident->id = id;
    printf("\nResident ID: %d\n", id);
    printf("Name: ");
    fgets(newResident->name, sizeof(newResident->name), stdin);
    newResident->name[strcspn(newResident->name, "\n")] = 0;

    printf("Age: ");
    if (scanf("%d", &newResident->age) != 1)
    {
        printf("Invalid age entered.\n");
        free(newResident);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    printf("Gender (M/F): ");
    scanf(" %c", &residentGender);
    newResident->gender = toupper(residentGender);

    newResident->next = head;
    newResident->blotterReports = NULL;
    head = newResident;

    printf("Resident Added Successfully!\n");
    system("cls");
}

void deleteResident() {
    int residentID;

    printf("\nEnter Resident ID: ");
    if (scanf("%d", &residentID) != 1) {
        printf("Invalid ID entered.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    Resident* current = head;
    Resident* prev = NULL;

    while (current != NULL) {
        if (current->id == residentID) {
            BlotterReport* report = current->blotterReports;
            while (report != NULL) {
                BlotterReport* temp = report;
                report = report->next;
                free(temp);
            }
            if (prev == NULL) {
                head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf("\nResident with ID#: %d has been deleted successfully!\n", residentID);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("\nResident with ID#: %d not found!\n", residentID);
}

void editResident() {
    int residentID, choice;
    int newAge;
    char newGender;
    Resident* current = head;

    printf("\nEnter Resident ID: ");
    if (scanf("%d", &residentID) != 1) {
        printf("Invalid ID entered.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    while (current != NULL) {
        if (current->id == residentID) {
            printf("ID: %d \t Name: %s \t Age: %d \t Gender: %c\n", current->id, current->name, current->age, current->gender);
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        printf("\nResident ID not found!\n");
        return;
    }

    printf("\nChoose which part:");
    printf("\n[1]. Name");
    printf("\n[2]. Age");
    printf("\n[3]. Gender");
    printf("\n[4]. Return\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid choice entered.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    switch (choice) {
        case 1:
            printf("\nEnter new name: ");
            getchar();
            fgets(current->name, sizeof(current->name), stdin);
            current->name[strcspn(current->name, "\n")] = 0;
            break;
        case 2:
            printf("Enter new Age: ");
            if (scanf("%d", &newAge) != 1) {
                printf("Invalid age entered.\n");
                while (getchar() != '\n');
                return;
            }
            while (getchar() != '\n');
            current->age = newAge;
            break;
        case 3:
            printf("Gender (M/F): ");
            scanf(" %c", &newGender);
            current->gender = toupper(newGender);
            break;
        case 4:
            return;
            break;
        default:
            printf("Invalid choice.\n");
    }
}

void blotterReport() {
	clearScreen();
    char blotterReport[MAX_REPORT_LENGTH];
    char dateTime[MAX_DATETIME_LENGTH];
    char recorder[MAX_RECORDER_LENGTH];
    int complainantID;

    formatTime(dateTime);
    printf("Complainant ID: ");
    if (scanf("%d", &complainantID) != 1) {
        printf("Invalid Complainant ID.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    Resident* current = head;
    while (current != NULL) {
        if (complainantID == current->id) {
            printf("Name of Complainant: %s\n", current->name);
            break;
        }
        current = current->next;
    }
    if (current == NULL) {
        printf("\nResident not found!");
        return;
    }
    printf("Date reported: %s", dateTime);
    printf("\nBLOTTER REPORT: \n");
    getchar();
    fgets(blotterReport, sizeof(blotterReport), stdin);
    blotterReport[strcspn(blotterReport, "\n")] = 0;

    printf("\nRecorded by: ");
    fgets(recorder, sizeof(recorder), stdin);
    recorder[strcspn(recorder, "\n")] = 0;

    BlotterReport* newReport = (BlotterReport*)malloc(sizeof(BlotterReport));
    if (newReport == NULL) {
        perror("Memory allocation failed in blotterReport");
        exit(EXIT_FAILURE);
    }
    strncpy(newReport->report, blotterReport, MAX_REPORT_LENGTH - 1);
    newReport->report[MAX_REPORT_LENGTH - 1] = '\0';
    strncpy(newReport->dateTime, dateTime, MAX_DATETIME_LENGTH - 1);
    newReport->dateTime[MAX_DATETIME_LENGTH - 1] = '\0';
    strncpy(newReport->recorder, recorder, MAX_RECORDER_LENGTH - 1);
    newReport->recorder[MAX_RECORDER_LENGTH - 1] = '\0';
    newReport->next = current->blotterReports;
    current->blotterReports = newReport;
    printf("\nReport Successfully Saved\n");
}

void printCentered(const char* message) {
    int consoleWidth = 120;
    int messageLength = strlen(message);
    int padding = (consoleWidth - messageLength) / 2;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s\n", message);
}

const char* checkRole() {
    const char captainPassword[] = "Admin.123";
    const char secretaryPassword[] = "123";
    char input[20];
    system("cls");
    printCentered("===============================");
    printCentered("Please log in to the account!");
    printCentered("===============================\n");
    do {
        printf("Enter Password: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        system("cls");
    } while (strcmp(input, captainPassword) != 0 && strcmp(input, secretaryPassword) != 0);

    if (strcmp(input, captainPassword) == 0) {
        printCentered("Welcome Captain!");
        return "Captain";
    } else {
        printCentered("Welcome Secretary!");
        return "Secretary";
    }
}

void manageResident(const char* role) {
	clearScreen();
    int manageChoice;
    printf("\n[1]. Add");
    printf("\n[2]. Edit");
    printf("\n[3]. Delete");
    printf("\n[4]. Return\n");
    printf("Enter Choice: ");
    if (scanf("%d", &manageChoice) != 1) {
        printf("Invalid choice entered.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    switch (manageChoice) {
        case 1:
            addResident();
            break;
        case 2:
            if (strcmp(role, "Captain") == 0) {
                editResident();
            } else {
                printf("\nAccess denied!");
                printf("\n");
            }
            break;
        case 3:
            if (strcmp(role, "Captain") == 0) {
                deleteResident();
            } else {
                printf("\nAccess denied!\n");
                clearScreen();
            }
            break;
        case 4:
            printf("Returning to User Dashboard.\n");
            clearScreen();
            break;
        default:
            printf("Invalid choice. Please try again.\n");
    }
}

void viewBlotterReports() {
	char yn;
    Resident* current = head;
    if (current == NULL) {
        printf("No residents registered.\n");
        return;
    }

    printf("\nBlotter Reports:\n");
    while (current != NULL) {
        if (current->blotterReports != NULL) {
            BlotterReport* report = current->blotterReports;
            printf("Resident ID: %d, Name: %s\n", current->id, current->name);
            while (report != NULL) {
                printf(" - %s (Recorded by: %s on %s)\n", report->report, report->recorder, report->dateTime);
                report = report->next;
            }
        }
        current = current->next;
    }
    
}

void manageBlotterReports() {
    int choice;
    do {
        printf("\nBlotter Report Management:\n");
        printf("[1]. Add Blotter Report\n");
        printf("[2]. View Blotter Reports\n");
        printf("[3]. Export Selected Blotters to Word File\n");
        printf("[4]. Return\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice entered.\n");
            while (getchar() != '\n');
            choice = 0;
            continue;
        }
        while (getchar() != '\n');
        switch (choice) {
            case 1:
                blotterReport();
                break;
            case 2:
                viewBlotterReports();
                break;
            case 3:
                exportSelectedBlotters(); 
                break;
            case 4:
                printf("Returning to User Dashboard.\n");
                clearScreen();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}

void exportSelectedBlotters() {
    char targetID[100];
    printf("Enter Resident ID to export blotters: ");
    fgets(targetID, sizeof(targetID), stdin);
    targetID[strcspn(targetID, "\n")] = '\0'; 

    FILE *input = fopen("residents.txt", "r");
    FILE *output = fopen("GeneratedBlotter.doc", "w");
    if (!input || !output) {
        printf("Error opening files.\n");
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }

    char line[MAX_LINE];
    bool inTargetResident = false;
    bool found = false;

    while (fgets(line, sizeof(line), input)) {
        if (strncmp(line, "RESIDENT,", 9) == 0) {
            char currentID[100];
            sscanf(line, "RESIDENT,%[^,]", currentID);
            inTargetResident = strcmp(currentID, targetID) == 0;

            if (inTargetResident) {
                fprintf(output, "Resident ID: %s\n\n", targetID);
            }
        } else if (inTargetResident && strncmp(line, "BLOTTER,", 8) == 0) {
            char details[256], datetime[256], complainant[256];

            
            if (sscanf(line, "BLOTTER,%[^,],%[^,],%[^\n]", details, datetime, complainant) == 3) {
                fprintf(output, "Blotter Report:\n");
                fprintf(output, "- Details     : %s\n", details);
                fprintf(output, "- Date/Time   : %s\n", datetime);
                fprintf(output, "- Prepared By : %s\n", complainant);
                fprintf(output, "\n--------------------------------------------------\n\n");
                found = true;
            }
        } else if (strncmp(line, "ENDRESIDENT", 11) == 0) {
            inTargetResident = false;
        }
    }

    fclose(input);
    fclose(output);

    if (found) {
        printf("Formatted blotter(s) exported to 'Selected_Blotters.doc'.\n");
    } else {
        printf("No blotters found for Resident ID '%s'.\n", targetID);
        remove("Selected_Blotters.doc");
    }
}


void userDashboard(const char* role) {
    printCentered("===============================");
    printCentered("Dashboard");
    printCentered("===============================\n");
    printf("\n[1]. Display Residents"); // display registered residents
    printf("\n[2]. Manage Residents"); // add, edit, delete resident data
    printf("\n[3]. Search Residents"); // search resident
    printf("\n[4]. Documents Request Order"); //add, view, delete documents
    printf("\n[5]. Blotter Reports"); // add, view blotter reports
    printf("\n[6]. Manage Inventory"); //bims inventory
    printf("\n[7]. Clear Text Screen"); // clear texts in the screen
     if (strcmp(role, "Captain") == 0) {
        printf("\n[8]. Update Request Status");
        printf("\n[9]. Exit");
        printf("\n[10]. Save Residents Data\n");
    } else {
        printf("\n[8]. Exit\n");
    }
}

void displayResidents() {
	system("cls");
    Resident* current = head;

    if (current == NULL) {
        printf("\nNo residents registered.\n");
        return;
    }

    printf("\nResidents List:\n");
    while (current != NULL) {
        int blotterCount = 0;
        BlotterReport* report = current->blotterReports;
        while (report != NULL) {
            blotterCount++;
            report = report->next;
        }

        printf("ID: %d \t Name: %s \t Age: %d \t Gender: %c \t Pending Blotter: %d\n\n",
            current->id, current->name, current->age, current->gender, blotterCount);
        current = current->next;
    }
}

void inventoryDisplay() {
    Item* currentItem = headItem;

    if (currentItem == NULL) {
        printf("\nNo Item available for display");
        return;
    }

    printf("\n+------------+----------------------+------------+-------------------+\n");
    printf("| %-10s | %-20s | %-10s | %-10s |\n", "Item ID", "Item Name", "Quantity", "Available");
    printf("+------------+----------------------+------------+-------------------+\n");

    while (currentItem != NULL) {
        printf("| %-10d | %-20s | %-10d | %-10d |\n", currentItem->id, currentItem->itemName, currentItem->itemQuantity, currentItem->availableQuantity);
        printf("+------------+----------------------+------------+-------------------+\n");
        currentItem = currentItem->next;
    }
}

void addItem() {

    Item* item = (Item*)malloc(sizeof(Item));
    if (item == NULL) {
        perror("Memory allocation failed in addItem");
        exit(EXIT_FAILURE);
    }

    int itemID = generateID();
    item->id = itemID;

    printf("\nItem Name: ");
    getchar();
    fgets(item->itemName, sizeof(item->itemName), stdin);
    item->itemName[strcspn(item->itemName, "\n")] = 0;

    printf("Quantity: ");
    if (scanf("%d", &item->itemQuantity) != 1) {
        printf("Invalid quantity entered.\n");
        free(item);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    item->availableQuantity = item->itemQuantity;

    item->next = headItem;
    headItem = item;
	clearScreen();
    printf("Item Added Successfully!\n");
    Sleep(2000);
    clearScreen();
}

void deleteItem() {
	clearScreen();
    inventoryDisplay();
    int deleteItemId;

    printf("ItemID: ");
    if (scanf("%d", &deleteItemId) != 1) {
    	clearScreen();
        printf("Invalid Item ID entered.\n");
        Sleep(2000);
        clearScreen();
		while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    Item* current = headItem;
    Item* prev = NULL;

    while (current != NULL) {
        if (current->id == deleteItemId) {
            if (prev == NULL) {
                headItem = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf("Item %d successfully deleted!\n", deleteItemId);
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Item %d not found in the list.\n", deleteItemId);
}

void editItem() {
clearScreen();
    inventoryDisplay();

    int editItemId, choice;

    Item* current = headItem;

    printf("Item ID: ");
    if (scanf("%d", &editItemId) != 1) {
    	clearScreen();
        printf("Invalid Item ID entered.\n");
        Sleep(2000);
        clearScreen();
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    while (current != NULL) {
        if (current->id == editItemId) {
            printf("\nChoose Which to edit: ");
            printf("\n[1]. Item Name\n");
            printf("[2]. Item Quantity\n");
            printf("Choice: ");
            if (scanf("%d", &choice) != 1) {
                printf("Invalid choice entered.\n");
                while (getchar() != '\n');
                return;
            }
            while (getchar() != '\n');

            switch (choice) {
                case 1:
                    printf("Item Name: ");
                    getchar();
                    fgets(current->itemName, sizeof(current->itemName), stdin);
                    current->itemName[strcspn(current->itemName, "\n")] = 0;
                    break;
                case 2:
                    printf("Item Quantity: ");
                    if (scanf("%d", &current->itemQuantity) != 1) {
                        printf("Invalid quantity entered.\n");
                        while (getchar() != '\n');
                        return;
                    }
                    while (getchar() != '\n');
                    break;
                default:
                    printf("Invalid choice.\n");
            }
            return;
        }
        current = current->next;
    }
    printf("Item ID not found.\n");
}

void inventoryManage() {
	clearScreen();
    inventoryDisplay();

    int manageChoice;
    printf("\n[1]. Add");
    printf("\n[2]. Edit");
    printf("\n[3]. Delete");
    printf("\nEnter Choice: ");
    if (scanf("%d", &manageChoice) != 1) {
        printf("Invalid choice entered.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    switch (manageChoice) {
        case 1:
            addItem();
            break;
        case 2:
            editItem();
            break;
        case 3:
            deleteItem();
            break;
        default:
            printf("Invalid choice.\n");
            Sleep(2000);
    }
}

void saveResidents(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("ERROR OPENING FILE!\n");
        return;
    }

    Resident* current = head;
    while (current != NULL) {
        fprintf(file, "RESIDENT,%d,%s,%d,%c\n", current->id, current->name, current->age, current->gender);

        BlotterReport* report = current->blotterReports;
        while (report != NULL) {
            fprintf(file, "BLOTTER,%s,%s,%s\n", report->report, report->dateTime, report->recorder);
            report = report->next;
        }

        fprintf(file, "ENDRESIDENT\n");
        current = current->next;
    }

    fclose(file);
    printf("Residents (and blotters) saved successfully to %s\n", filename);
}

void loadResidents(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR OPENING FILE FOR READING!\n");
        return;
    }

    char line[1200];
    Resident* currentResident = NULL;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "RESIDENT,", 9) == 0) {
            Resident* newResident = (Resident*)malloc(sizeof(Resident));
            if (newResident == NULL)
             {
                perror("Memory allocation error in loadResidents");
                exit(EXIT_FAILURE);
             }
            sscanf(line, "RESIDENT,%d,%49[^,],%d,%c", &newResident->id, newResident->name, &newResident->age, &newResident->gender);
            newResident->next = head;
            newResident->blotterReports = NULL;
            head = newResident;
            currentResident = newResident;

        } else if (strncmp(line, "BLOTTER,", 8) == 0 && currentResident != NULL) {
            BlotterReport* report = (BlotterReport*)malloc(sizeof(BlotterReport));
             if (report == NULL)
             {
                perror("Memory allocation error in loadResidents");
                exit(EXIT_FAILURE);
             }
            sscanf(line, "BLOTTER,%999[^,],%29[^,],%49[^\n]", report->report, report->dateTime, report->recorder);
            report->next = currentResident->blotterReports;
            currentResident->blotterReports = report;

        } else if (strcmp(line, "ENDRESIDENT") == 0) {
            currentResident = NULL;
        }
    }

    fclose(file);
    printf("Residents and blotters loaded from %s\n", filename);
}

void saveRequest(Queue* queue, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("ERROR OPENING FILE!\n");
        return;
    }

    DocumentRequest* current = queue->front;
    while (current != NULL) {
        fprintf(file, "%d,%s,%s,%s,%s\n", current->requestId, current->documentName, current->residentName, current->dateAdded, current->status);
        current = current->next;
    }
    fclose(file);
    printf("Document Request Save Successfully\n");
}

void loadRequest(Queue* queue, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("No document request file found at %s.\n", filename);
        return;
    }

    char line[300];
    DocumentRequest* newRequest;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        newRequest = (DocumentRequest*)malloc(sizeof(DocumentRequest));
        if (newRequest == NULL) {
            perror("Memory allocation error in loadRequest");
            exit(EXIT_FAILURE);
        }
        if (sscanf(line, "%d,%99[^,],%99[^,],%29[^,],%19[^\n]", &newRequest->requestId, newRequest->documentName, newRequest->residentName, newRequest->dateAdded, newRequest->status) == 5) {
            newRequest->next = NULL;
            if (queue->rear == NULL) {
                queue->front = newRequest;
                queue->rear = newRequest;
            } else {
                queue->rear->next = newRequest;
                queue->rear = newRequest;
            }
        }
    }

    fclose(file);
    printf("Document requests loaded from %s\n", filename);
}

void saveInventory(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error saving inventory file!\n");
        return;
    }

    Item* current = headItem;
    while (current != NULL) {
        fprintf(file, "%d,%s,%d,%d\n", current->id, current->itemName, current->itemQuantity, current->availableQuantity);
        current = current->next;
    }

    fclose(file);
    printf("Inventory saved to %s\n", filename);
}

void loadInventory(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("No inventory file found at %s.\n", filename);
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        Item* item = (Item*)malloc(sizeof(Item));
        if (item == NULL) {
            perror("Memory allocation error in loadInventory");
            exit(EXIT_FAILURE);
        }
        if (sscanf(line, "%d,%19[^,],%d,%d", &item->id, item->itemName, &item->itemQuantity, &item->availableQuantity) != 4) {
            free(item);
            continue;
        }
        item->next = headItem;
        headItem = item;
    }

    fclose(file);
    printf("Inventory loaded from %s\n", filename);
}

void residentToExcel(const char* filename){
	FILE* file = fopen(filename, "w");
	if(file == NULL){
		printf("\nError opening file");
		return;
	}
	fprintf(file, "ID, Name, Age, Gender\n");
	Resident* current = head;
	while(current !=NULL){
		fprintf(file, "%d,%s,%d,%c\n", current->id, current->name,current->age,current->gender);
		current = current->next;
	}
	fclose(file);
	printf("residents.txt successfully saved to Excel!");
}

void freeQueue(Queue* queue) {
    if (queue == NULL) return;
    DocumentRequest* current = queue->front;
    DocumentRequest* next;
    while (current != NULL) {
        next = current->next;
        freeDocumentRequest(current);
        current = next;
    }
    queue->front = queue->rear = NULL;
    free(queue);
}

void freeResidents() {
    Resident* current = head;
    Resident* next;
    while (current != NULL) {
        next = current->next;
        BlotterReport* report = current->blotterReports;
        while (report != NULL) {
            BlotterReport* reportTemp = report;
            report = report->next;
            free(reportTemp);
        }
        free(current);
    }
    head = NULL;
}

void updateRequestStatus(Queue* queue) {
    if (queue->front == NULL) {
        printf("No document requests to update.\n");
        return;
    }

    viewRequests(queue);
    int requestID;
    printf("Enter the ID of the document request to update: ");
    if (scanf("%d", &requestID) != 1) {
        printf("Invalid request ID.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    DocumentRequest* current = queue->front;
    while (current != NULL) {
        if (current->requestId == requestID) {
            printf("Current Status: %s\n", current->status);
            printf("Enter new status (e.g., Approved, Denied, Completed): ");
            fgets(current->status, sizeof(current->status), stdin);
            current->status[strcspn(current->status, "\n")] = 0;
            printf("Status updated successfully.\n");
            return;
        }
        current = current->next;
    }
    printf("Document request with ID %d not found.\n", requestID);
}
void printRequest(Queue* queue) {
    int reqID, fileReqID;
    char line[256];
    char name[50], type[50], status[20], date[20];


    system("cls");
    viewRequests(queue);
    printf("\nEnter Request ID: ");
    scanf("%d", &reqID);

    FILE* file = fopen("requests.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open request.txt\n");
        return;
    }

    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        
        sscanf(line, "%d,%[^,],%[^,],%[^,],%[^\n]",
       &fileReqID, type, name, date, status);{
            if (fileReqID == reqID) {
                found = 1;
                if (strcmp(status, "Approved") == 0) {
                    generateDocx(reqID, name, type, date);
                } else {
                    printf("Request ID %d is not approved. Status: %s\n", reqID, status);
                }
                break;
            }
        }
    }

    fclose(file);

    if (!found) {
        printf("Request ID %d not found.\n", reqID);
    }
}

void generateDocx(int reqID, const char* name, const char* type, const char* date) {
    char filename[100];
    sprintf(filename, "Request_%d.doc", reqID);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not create %s\n", filename);
        return;
    }

    fprintf(file, "Request ID: %d\n", reqID);
    fprintf(file, "Resident Name: %s\n", name);
    fprintf(file, "Document Type: %s\n", type);
    fprintf(file, "Date Requested: %s\n", date);
    fprintf(file, "Status: Approved");

    fclose(file);

    printf("Document generated: %s\n", filename);
}


int main() {
    srand(time(NULL));
    Queue* queue = createQueue();
    int choice;

    loadResidents("residents.txt");
    loadRequest(queue, "requests.txt");
    loadInventory("inventory.txt");

    const char* role = checkRole();

    do {
        userDashboard(role);
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            choice = 0;
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                displayResidents();
                
                break;
            case 2:
                manageResident(role);
                 
                break;
            case 3:
                searchResident();
                 
                break;
            case 4:
                documentsRequestOrder(queue, role);
                 
                break;
            case 5:
                manageBlotterReports();
                 
                break;
            case 6:
                inventoryManage();
                 
                break;
            case 7:
                clearScreen();
                break;
             case 8:
                if (strcmp(role, "Captain") == 0) {
                    updateRequestStatus(queue);
                    break;
                }
            case 9:
                saveResidents("residents.txt");
                saveRequest(queue, "requests.txt");
                saveInventory("inventory.txt");
                 
                printf("\nGoodbye!\n");
                freeQueue(queue);
                freeResidents();
                freeInventory();
                return 0;
            case 10:
            	residentToExcel("residents.csv");
            	printf("\n");
            	break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);
    return 0;
}
 
