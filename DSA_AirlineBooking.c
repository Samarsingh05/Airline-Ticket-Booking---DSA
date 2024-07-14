#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// Structure to represent a passenger
enum city
{
    BANGALORE = 1, DELHI = 2, MUMBAI = 3, AHEMDABAD = 4, KOLKATA = 5
};

char a1[4] = {'A','B','C','D'};

typedef struct passenger
{
    char passport[7]; // Increased to 7 to allow for null-terminator
    char name[25];    
    char destination[16];
    char departure[16];
    char seat_num[10];
    char email[30];
    int dd,mm,yyyy; 
    int new;
    struct passenger* next;
}PASS;


typedef struct date
{
    int dd,mm,yyyy; 
}DATE;


DATE today;

PASS* begin = NULL;


// Function to check if a year is a leap year
int isLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1;
    }
    return 0;
}
int isValidEmail(const char *email) {
    if (email == NULL) {
        return 0;
    }

    size_t len = strlen(email);
    if (len < 5) {
        return 0;
    }

    const char *atSymbol = strchr(email, '@');
    if (atSymbol == NULL) {
        return 0;
    }

    const char *dotAfterAt = strchr(atSymbol, '.');
    if (dotAfterAt == NULL) {
        return 0;
    }

    size_t domainLen = len - (dotAfterAt - email) - 1;
    if (domainLen < 2) {
        return 0;
    }

    return 1;
}

// Function to validate a date
int isValidDate(int day, int month, int year) 
{
    if (year < 2023 || month < 1 || month > 12 || day < 1)
        return 0;
    else if(year == today.yyyy )
    {
        if(month < today.mm)
            return 0;
    }
    else if(month == today.mm )
    {
        if(day < today.dd)
            return 0;
    }

    int daysInMonth[] = {0, 31, 28+isLeapYear(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


    return (day <= daysInMonth[month]);
}

int isValidPassport(char* s, PASS* begin)
{
    PASS* temp = begin;
    if(strlen(s) != 6)
        return 0; 
    while(temp != NULL)
    {
        if(strcmp(temp->passport,s) == 0)
            return 0;
        temp = temp->next;
    }
        return 1;

}

char* seat_no()
{
    srand(time(NULL));
    
    int randomNumber = rand() % 20 + 1;

    int i = rand() % 4;
    char letter;

switch(i) {
    case 0: 
        letter = 'A';
        break;
    case 1:
      letter = 'B';
      break;
    case 2:
      letter = 'C'; 
      break;
    case 3:
      letter = 'D';
      break;
  }

  char *s = malloc(10);
  sprintf(s, "%d-%c", randomNumber, letter);
  return s;

}

char* seatno(int x)
{
    char* s = (char*)malloc(sizeof(char)*3 + 2);
    int i = 1;
    char c = 'A';
    while(i<x)
    {
        c++;
        if(c == 'E')
            c = 'A';
        i++;
    }
    int y;
    if(i%4 == 0)
        y = i/4 + 48;
    else
        y = i/4 + 1 + 48;
    s[0] = (char)y;
    s[1] = '-';
    s[2] = c;
    s[3] = '\0';

    return s;

}


void details(PASS* passenger) 
{
    char em[30];
    char p[7];
    e1:
    printf("\nEnter your passport number (6 characters): ");
    scanf("%6s",p);
    if(isValidPassport(p,begin))
        strcpy(passenger->passport, p);
    else
    {
        printf("Please Enter Valid passport\n");
        goto e1;
    }


    printf("\nEnter your name (up to 30 characters): ");
    if (scanf(" %[^\n]", passenger->name) != 1) 
    {
        printf("Error reading input.\n");
    }

    e2:
    printf("\nEnter your email address (up to 30 characters): ");
    scanf("%s", em);
    if(isValidEmail(em))
        strcpy(passenger->email, em);
    else
    {
        printf("Please Enter Valid email\n");
        goto e2;
    }

}

void fromto(int dep, int des)
{
    if(dep==1)
        printf("Bengaluru\t");
    else if(dep==2)
        printf("Delhi\t");
    else if(dep==3)
        printf("Mumbai\t");
    else if(dep==4)
        printf("Ahemdabad\t");
    else if(dep==5)
        printf("Kolkata\t");
    
    if(des==1)
        printf("Bengaluru\t");
    else if(des==2)
        printf("Delhi\t");
    else if(des==3)
        printf("Mumbai\t");
    else if(des==4)
        printf("Ahemdabad\t");
    else if(des==5)
        printf("Kolkata\t");
}


void info(int dep, int des)
{

    printf("IN THE TABLE BELOW EACH COLOUMN REPRESENTS : TIME , FROM , TO , PRICE RESPECTIVELY\n\n");
    printf("1. 8am-10am\t");
    fromto(dep,des);
    printf("   2000\n");
    
    printf("2. 11am-1pm\t");
    fromto(dep,des);
    printf("   4000\n");

    printf("3. 3pm-5pm\t");
    fromto(dep,des);
    printf("   5000");

}


PASS* reserve(int x) 
{
    PASS* passenger = (PASS*)malloc(sizeof(PASS));
    int dept,des;
    l1:
    printf("Enter departure city:\n 1.Bangalore\n 2.Delhi \n 3.Mumbai \n 4.Ahemdabad \n 5.Kolkata\n\n");
    scanf("%d",&dept);
    
    printf("\nEnter destination city:\n 1.Bangalore\n 2.Delhi \n 3.Mumbai \n 4.Ahemdabad \n 5.Kolkata\n\n");
    scanf("%d",&des);
    
    if(des==dept || dept>5 || des>5)
    {
        printf("\nEnter Valid destination and departure");
        goto l1;
    }

    
    int d,m,y;
    printf("\nEnter date of flight: ");   
    l2:
    scanf("%d-%d-%d",&d,&m,&y);    //dd-mm-yyyy format

    if(isValidDate(d,m,y))
    {
        passenger->dd = d;
        passenger->mm = m;
        passenger->yyyy = y;
    }
    else
    {
        printf("\nPlease enter valid date: ");
        goto l2;
    }
    printf("\n");
    info(dept,des);
    printf("\n\nWhich flight would you choose:");
    scanf("%d",&passenger->new);

    if(dept==1)
    {
        strcpy(passenger->departure,"Banglore");
    }
    else if(dept==2)
    {
        strcpy(passenger->departure,"Delhi");
    }
    else if(dept==3)
    {
        strcpy(passenger->departure,"Mumbai");
    }
    else if(dept==4)
    {
        strcpy(passenger->departure,"Ahemdabad");
    }
    else if(dept==5)
    {
        strcpy(passenger->departure,"Kolkata");
    }

    if(des==1)
    {
        strcpy(passenger->destination,"Banglore");
    }
    else if(des==2)
    {
        strcpy(passenger->destination,"Delhi");
    }
    else if(des==3)
    {
        strcpy(passenger->destination,"Mumbai");
    }
    else if(des==4)
    {
        strcpy(passenger->destination,"Ahemdabad");
    }
    else if(des==5)
    {
        strcpy(passenger->destination,"Kolkata");
    }

    strcpy(passenger->seat_num,seat_no());
    passenger->next = NULL;

    details(passenger);

    if (begin == NULL) {
        begin = passenger;
    } else {
        PASS* current = begin;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = passenger;
    }
    printf("\nSeat booking successful at %s!",passenger->seat_num);

    printf("\n\nPAY THE TOTAL AMOUNT IN CASH OR BITCOIN.");
    return begin;

}


void display(PASS *begin) 
{   
    int check=0;
    PASS* current = begin;


    char passport[7];
    printf("\nEnter passport number to display a record: ");
    scanf("%6s", passport);

    while (current != NULL) {

        if (strcmp(current->passport, passport) == 0) 
        {   check=1;
            printf("------------------------------------------------------------------------------\n");
            printf("           BOARDING PASS\n");
            printf("\nName                     %s\n",current->name);
            printf("\nEmail ID                 %s\n",current->email);
            printf("\nDeparture                %s\n",current->departure);
            printf("\nDestination              %s\n",current->destination);
            printf("\nSeat Number              %s\n",current->seat_num);
            printf("\nDate of Departure        %d-%d-%d\n",current->dd,current->mm,current->yyyy);

            if(current->new==1)
            {
                printf("\nFlight Schedule          8am - 10am"); 
            }
            else if(current->new==2)
            {
                printf("\nFlight Schedule          11am - 1pm"); 
            }
            else if(current->new==3)
            {
                printf("\nFlight Schedule          3pm - 5pm"); 
            }
            int a=pow(current->new,2)+pow(current->new,3);
            printf("\n\nGate                      %d",a);
            printf("\n------------------------------------------------------------------------------\n");

            printf("\nTHANK YOU FOR FLYING WITH US");
            break;
        }
        current = current->next;
    }
    if(check==0)
    {
        printf("\n\nPassport Number not found.");
    }
}


void cancel(PASS **begin) {
    char passport[7];
    printf("\nEnter passport number to delete a record: ");
    scanf("%6s", passport);

    if (begin == NULL) {
        printf("\nNo records to delete.");
        return;
    }

    PASS* current = *begin;
    PASS* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->passport, passport) == 0) {
            if (previous == NULL) {
                *begin = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            printf("\nBooking has been deleted.");
            return;
        }
        previous = current;
        current = current->next;
    }

    printf("\nPassport number not found.");
}


int main() 
{
    PASS *begin;
    today.dd = 24; today.mm = 11; today.yyyy = 2023;

    //AP planes[20];

    int choice;
    int num = 1;

    do {
        printf("\n\n***** WELCOME TO MAVERICK AIRLINE SYSTEM *****");
        printf("\n\nPlease enter your choice from below (1-4):");
        printf("\n1. Reservation");
        printf("\n2. Cancel");
        printf("\n3. Display Records");
        printf("\n4. Exit");
        printf("\nEnter your choice: ");

        scanf("%d", &choice);
        printf("\n\n");
        switch (choice) {
            case 1:
                begin=reserve(num);
                num++;
                break;
            case 2:
                cancel(&begin);
                break;
            case 3:
                display(begin);
                break;
            case 4:
                // Add code to free allocated memory before exiting
                printf("\nExiting the program.\n");
                exit(1);
                
            default:
                printf("\nInvalid choice. Please choose from 1-4.");
        }
    } while (choice != 4);

    // Free allocated memory before exiting
    PASS* current = begin;
    while (current != NULL) {
        PASS* next = current->next;
        free(current);
        current = next;
    }

    return 0;
}