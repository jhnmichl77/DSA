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
} Resident;

Resident* head = NULL;

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

void documentsRequestOrder(Queue* queue) {
    int choice;
    char documentName[100];
char residentName[100];
    char dateAdded[20];
    do {
        printf("\nDocument Request Order:\n");
        printf("1. Add Document Request\n");
        printf("2. View Document Requests\n");
        printf("3. Delete Document Request\n");
        printf("4. Back to User Dashboard\n");
        printf("Enter your choice (1-4): ");
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
                printf("Enter date added (e.g., YYYY-MM-DD): ");
                fgets(dateAdded, sizeof(dateAdded), stdin);
                dateAdded[strcspn(dateAdded, "\n")] = 0;
                enqueue(queue, documentName, residentName, dateAdded);
                printf("Document request added.\n");
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

int generateID(){
	return 1000+rand()%9000;
}

void addResident(){
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
    head = newResident;

    printf("Resident Added Successfully!\n");
	
}

void manageResident(){
	int manageChoice;
	printf("\n[1]. Add");
   	printf("\n[2]. Edit");
	printf("\n[3]. Delete\n");
	printf("\nEnter Choice: ");
	scanf("%d", &manageChoice);
	switch(manageChoice){
		case 1: 
			addResident();
			break;
		case 2:
			//editResident();
			break;
		case 3:
			//deleteResident();
			break;
	}
}

void userDashboard(){
		
		printf("\n[1]. Display Residents");//display registered residents
    	printf("\n[2]. Manage Residents");//add, edit, delete resident data
    	printf("\n[3]. Search Residents");//search resident or (inventory)
    	printf("\n[4]. Documents Request Order");
    	printf("\n[5]. Manage Inventory");
    	printf("\n[6]. Exit\n");
}

void checkRole(){
	char captainUser[20] = "Admin.123", input[20];
	char secretary[20] = "SECRETARY";
	
	do {
        printf("Enter Password: ");
        scanf("%19s", input);  
    } while (strcmp(input, captainUser) != 0 && strcmp(input, secretary) != 0);
    
    char role[20];
    if (strcmp(input, captainUser) == 0) {
        strcpy(role, "Captain");
    } else {
        strcpy(role, "Secretary");
    }	
}

void displayResidents(){
	Resident* current = head;
	
	printf("\nResidents List:\n");
    while (current != NULL) {
        printf("ID: %d \t Name: %s \t Age: %d \t Gender: %c\n", current->id, current->name, current->age, current->gender);
        current = current->next;
    }
}

void editResident(){
	
}

void deleteResident(){
	
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


int main() {
	srand(time(NULL));
	Queue* queue = createQueue();
    int choice;

	checkRole();
	do{
		
		userDashboard();
		printf("\nEnter your choice: ");
    	scanf("%d", &choice);
    	
    	switch(choice){
    		case 1:
    			displayResidents();
    			break;
    		case 2:
    			manageResident();
    			break;
    		case 3:
    			
    			break;
    		case 4:
    			documentsRequestOrder(queue);
    			break;
    		case 5:
    			
    			break;
    		case 6:
    			printf("\nByers!");
    			exit(1);
    			break;
		}
	}while(choice!=6);
}



