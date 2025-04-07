#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>

typedef struct Resident {
    int id;
    char name[50];
    int age;
    char gender;
    struct Resident* next;
    struct BlotterReport* blotterReports; 
} Resident;

typedef struct BlotterReport {
    char report[1000];
    char dateTime[30];
    char recorder[50];
    struct BlotterReport* next; 
} BlotterReport;

typedef struct Item{
	int id;
	char itemName[20];
	int itemQuantity;
	int availableQuantity;
	struct Item* next;
}Item;

Resident* head = NULL;
Item* headItem = NULL;

typedef struct DocumentRequest {
    char documentName[100];
    char residentName[100];
    char dateAdded[20];
    struct DocumentRequest* next;
} DocumentRequest;

typedef struct Queue {
    DocumentRequest* front;
    DocumentRequest* rear;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

void enqueue(Queue* queue, const char* documentName, const char* residentName, const char* dateAdded) {
    DocumentRequest* newRequest = (DocumentRequest*)malloc(sizeof(DocumentRequest));
    strcpy(newRequest->documentName, documentName);
    strcpy(newRequest->residentName, residentName);
    strcpy(newRequest->dateAdded, dateAdded);
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
        printf("\n%d%s: \nDocument Name: %s \nResident Name: %s \nDate Added: %s\n", count, suffix, current->documentName, current->residentName, current->dateAdded);
        current = current->next;
        count++;
    }
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
 
    current = queue->front;
 
    while (1) {
        int i = 0;
        
        printf("Select a document to delete (Enter 0 to exit):\n");
        current = queue->front;
        for (i; i < count; i++) {
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
                    return;
                }
                previous = current;
                current = current->next;
                index++;
            }
        } else if (ch == '0') {
            printf("Deletion canceled.\n");
            return;
        }
    }
}

void formatTime(char *output) {
    time_t rawTime;
    struct tm * timeinfo;
    
    time(&rawTime);
    timeinfo = localtime(&rawTime);
    
    sprintf(output, "%d/%d/%d %d:%d:%d", timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_year + 1900,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void documentsRequestOrder(Queue* queue) {
    int choice;
    char documentName[100];
    char residentName[100];
    char dateAdded[30];
    formatTime(dateAdded);
    do {
        printf("\nDocument Request Order:\n");
        printf("[1]. Add Document Request\n");
        printf("[2]. View Document Requests\n");
        printf("[3]. Delete Document Request\n");
        printf("[4]. Back to User Dashboard\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1:
                printf("Enter document name: ");
                fgets(documentName, sizeof(documentName), stdin);
                documentName[strcspn(documentName, "\n")] = 0;
                printf("Enter resident name: ");
                fgets(residentName, sizeof(residentName), stdin);
                residentName[strcspn(residentName, "\n")] = 0;
                printf("Date Requested: %s", dateAdded);
                enqueue(queue, documentName, residentName, dateAdded);
                printf("\nDocument request added.\n");
                break;
            case 2:
                viewRequests(queue);
                break;
            case 3:
                deleteRequest(queue);
                break;
            case 4:
                printf("Returning to User Dashboard.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}

int generateID() {
    return 1000 + rand() % 9000;
}

void searchResident() {
    char searchName[50];
    printf("Enter the name of the resident to search: ");
    getchar();
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
    char residentGender;
    int id = generateID();
    newResident->id = id;
    printf("Resident ID: %d\n", id);
    printf("Name: ");
    getchar(); 
    fgets(newResident->name, sizeof(newResident->name), stdin);
    newResident->name[strcspn(newResident->name, "\n")] = 0; 

    printf("Age: ");
    scanf("%d", &newResident->age);
    
    printf("Gender (M/F): ");
    scanf(" %c", &residentGender); 
    newResident->gender = toupper(residentGender);
    
    newResident->next = head; 
    newResident->blotterReports = NULL; 
    head = newResident;

    printf("Resident Added Successfully!\n");
}

void deleteResident() {
    int residentID;
    
    printf("\nEnter Resident ID: ");
    scanf("%d", &residentID);
    
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
    printf("\nResident with ID#: %d not found!", residentID);
}

void editResident() {
    int residentID, choice;
     int newAge;
     char newGender;
    Resident* current = head;
    
    printf("\nEnter Resident ID: ");
    scanf("%d", &residentID);
    
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
    printf("\n[4]. Return");
    printf("\nEnter choice: ");
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            printf("\nEnter new name: ");
            getchar(); 
            fgets(current->name, sizeof(current->name), stdin);
            current->name[strcspn(current->name, "\n")] = 0; 
            break;
        case 2:
            printf("Enter new Age: ");
            scanf("%d", &newAge);
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
    }
}
void blotterReport() {
    char blotterReport[1000];
    char dateTime[30];
    char recorder[50];
    int complainantID;

    formatTime(dateTime);
    printf("Complainant ID: ");
    scanf("%d", &complainantID);

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
    getchar(); 
    fgets(recorder, sizeof(recorder), stdin);
    recorder[strcspn(recorder, "\n")] = 0; 

    BlotterReport* newReport = (BlotterReport*)malloc(sizeof(BlotterReport));
    strcpy(newReport->report, blotterReport);
    strcpy(newReport->dateTime, dateTime);
    strcpy(newReport->recorder, recorder);
    newReport->next = current->blotterReports; 
    current->blotterReports = newReport;
    printf("\nReport Successfully Saved\n");
}
const char* checkRole() {
    const char captainPassword[] = "Admin.123";
    const char secretaryPassword[] = "123";
    char input[20];

    do {
        printf("Enter Password: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; 
    } while (strcmp(input, captainPassword) != 0 && strcmp(input, secretaryPassword) != 0);

    if (strcmp(input, captainPassword) == 0) {
        return "Captain";
    } else {
        return "Secretary";
    }
}


void manageResident(const char* role) {
    int manageChoice;
    printf("\n[1]. Add");
    printf("\n[2]. Edit");
    printf("\n[3]. Delete\n");
    printf("\nEnter Choice: ");
    scanf("%d", &manageChoice);
    switch (manageChoice) {
        case 1: 
            addResident();
            break;
        case 2:
            if (strcmp(role, "Captain") == 0) {
                editResident();	
            } else {
                printf("Access denied!");
            }
            break;
        case 3:
        	if(strcmp(role, "Captain")==0){
        	deleteResident();	
			} else {
                printf("Access denied!");
            }          
            break;
    }
}
void viewBlotterReports() {
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
        printf("[3]. Back to User Dashboard\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                blotterReport();
                break;
            case 2:
                viewBlotterReports();
                break;
            case 3:
                printf("Returning to User Dashboard.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);
}
void userDashboard() {
    printf("\n[1]. Display Residents"); // display registered residents
    printf("\n[2]. Manage Residents"); // add, edit, delete resident data
    printf("\n[3]. Search Residents"); // search resident
    printf("\n[4]. Documents Request Order"); //add, view, delete documents
    printf("\n[5]. Blotter Reports"); // add, view blotter reports
    printf("\n[6]. Manage Inventory"); //bims inventory
    printf("\n[7]. Exit\n");
}

void displayResidents() {
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
        
        printf("ID: %d \t Name: %s \t Age: %d \t Gender: %c \t Pending Blotter: %d\n", 
               current->id, current->name, current->age, current->gender, blotterCount);
        current = current->next;
    }
}

void inventoryDisplay(){
    Item* currentItem = headItem;
    
    if(currentItem == NULL){
        printf("\nNo Item available for display");
        return;
    }
    
    printf("\n+------------+----------------------+------------+-------------------+\n");
    printf("| %-10s | %-20s | %-10s | %-10s |\n", "Item ID", "Item Name", "Quantity", "Available");
    printf("+------------+----------------------+------------+-------------------+\n");
    
    while(currentItem != NULL){
        printf("| %-10d | %-20s | %-10d | %-10d |\n", currentItem->id, currentItem->itemName, currentItem->itemQuantity, currentItem->availableQuantity);
        printf("+------------+----------------------+------------+-------------------+\n");
        currentItem = currentItem->next;
    }    
}

void addItem(){
	
	Item* item = (Item*)malloc(sizeof(Item));
	
	int itemID = generateID();
	item->id = itemID;
	
	printf("Item Name: ");
	getchar(); 
    fgets(item->itemName, sizeof(item->itemName), stdin);
    item->itemName[strcspn(item->itemName, "\n")] = 0;
	
	printf("Quantity: ");
	scanf("%d", &item->itemQuantity);
	
	item->next = headItem;
    headItem = item;

    printf("Item Added Successfully!\n");
	
	 
}

void deleteItem(){
	inventoryDisplay();
	int deleteItemId;
	
	printf("ItemID: ");
	scanf("%d", &deleteItemId);
	
	Item* current = headItem;
	Item* prev = NULL;
	
	while(current!=NULL){
		if(current->id == deleteItemId){
			if (prev == NULL) {
                headItem = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
			printf("Item %d successfully delete!", deleteItemId);
			return;
		}
		prev = current;
    	current = current->next;
	}
		
	printf("Item %d not found in the list.\n", deleteItemId);
}

void editItem(){x
	
	inventoryDisplay();
	
	int editResidentId, choice;
	
	Item* current = headItem;
	
	printf("Item ID: ");
	scanf("%d", &editResidentId);
	
	while(current!=NULL){
		if(current->id==editResidentId){
			printf("\nChoose Which to edit: ");
			printf("[1]. Item Name");
			printf("[2]. Item Quantity");
			printf("Choice: ");
			scanf("%d", &choice);
			
			switch(choice){
				case 1:
					printf("Item Name: ");
					getchar(); 
    				fgets(current->itemName, sizeof(current->itemName), stdin);
    				current->itemName[strcspn(current->itemName, "\n")] = 0;
    				break;
    			case 2:
    				printf("Item Quantity: ");
    				scanf("%d", &current->itemQuantity);
    				break;
			}
			current= current->next;
		}
	}
}

void inventoryManage(){
	inventoryDisplay();
	
	int manageChoice;
    printf("\n[1]. Add");
    printf("\n[2]. Edit");
    printf("\n[3]. Delete\n");
    printf("\nEnter Choice: ");
    scanf("%d", &manageChoice);
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
    }
}

void freeQueue(Queue* queue) {
    DocumentRequest* current = queue->front;
    DocumentRequest* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}

void freeResidents() {
    Resident* current = head;
    while (current != NULL) {
        Resident * temp = current;
        current = current->next;
        BlotterReport* report = temp->blotterReports;
        while (report != NULL) {
            BlotterReport* reportTemp = report;
            report = report->next;
            free(reportTemp);
        }
        free(temp);
    }
}

int main() {
    srand(time(NULL));
    Queue* queue = createQueue();
    int choice;

    const char* role = checkRole();
    
    do {
        userDashboard();
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        
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
                documentsRequestOrder(queue);
                break;
            case 5:
                manageBlotterReports(); 
                break;
            case 6:
            	inventoryManage();
                break;
            case 7:
                printf("\nByers!");
                freeQueue(queue);
                freeResidents();
                exit(1);
                break;
        }
    } while (choice != 7);
}
