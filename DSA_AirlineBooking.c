#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 10
#define COLS 6
#define MAX_PASSENGERS 10000
#define HASH_SIZE 103
#define MAX_WAITLIST 5

#define C_CYAN    "\033[36m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_RED     "\033[31m"
#define C_RESET   "\033[0m"
#define C_MAGENTA "\033[35m"
#define C_WHITE   "\033[37m"

typedef struct Passenger {
    char passport[8];          
    char name[30];
    char email[40];
    int day, month, year;
    char seat_num[8];
    struct Passenger* next;    
    struct Flight* flight_ptr; 
} Passenger;

typedef struct WaitlistNode {
    Passenger* passenger;
    struct WaitlistNode* next;
} WaitlistNode;

typedef struct {
    WaitlistNode* front;
    WaitlistNode* rear;
    int size;
} Waitlist;

typedef struct Flight {
    char departure[16];
    char destination[16];
    int schedule; 
    int seat_map[ROWS][COLS]; 
    Passenger* passengers; 
    Waitlist waitlist;
} Flight;

typedef struct PassengerHashNode {
    Passenger* passenger;
    struct PassengerHashNode* next;
} PassengerHashNode;

typedef struct {
    int day, month, year;
} Date;
Date today = {30, 7, 2025};

const char* cities[] = {"", "Bangalore", "Delhi", "Mumbai", "Ahmedabad", "Kolkata"};

#define TOTAL_ROUTES 20
#define TOTAL_SCHEDULES 3
#define TOTAL_FLIGHTS (TOTAL_ROUTES * TOTAL_SCHEDULES)

Flight flights[TOTAL_FLIGHTS];

PassengerHashNode* passenger_hash[HASH_SIZE];

void printLine();
void printHeader(const char* title);
void printMenu();
bool isValidDate(int d, int m, int y, Date today);
bool isLeapYear(int y);
bool isValidEmail(const char* email);
unsigned hash_passport(const char* passport);
Passenger* findPassengerGlobal(const char* passport, Flight** flightFound);
void addPassenger();
void displayTicket();
void cancelBooking();
void showSeatMap();
void initializeFlights();
void freeAllPassengers();
bool assignSeat(Flight* flight, char* seat_num_out);
void freeSeat(Flight* flight, const char* seat_num);
void enqueueWaitlist(Waitlist* wl, Passenger* p);
Passenger* dequeueWaitlist(Waitlist* wl);
void printFlightInfo(const Flight* f);


int main() {
    initializeFlights();

    int choice;
    do {
        printMenu();
        printf(C_WHITE "Enter your choice: " C_RESET);
        if(scanf("%d", &choice)!=1) { 
            while(getchar()!='\n'); 
            printf(C_RED "Invalid input\n" C_RESET); 
            continue; 
        }
        switch(choice) {
            case 1: addPassenger(); break;
            case 2: cancelBooking(); break;
            case 3: displayTicket(); break;
            case 4: showSeatMap(); break;
            case 5: printf(C_YELLOW "Exiting. Goodbye!\n" C_RESET); break;
            default: printf(C_RED "Invalid choice\n" C_RESET);
        }
    } while(choice != 5);

    freeAllPassengers();
    
    return 0;
}

void initializeFlights() {
    int idx = 0;
    for(int dep=1; dep<=5; ++dep) {
        for(int dest=1; dest<=5; ++dest) {
            if(dep == dest) continue;
            for(int sched=1; sched<=3; ++sched) {
                Flight* f = &flights[idx++];
                strcpy(f->departure, cities[dep]);
                strcpy(f->destination, cities[dest]);
                f->schedule = sched;
                f->passengers = NULL;
                f->waitlist.front = f->waitlist.rear = NULL;
                f->waitlist.size = 0;
                for(int r=0;r<ROWS;r++)
                    for(int c=0;c<COLS;c++)
                        f->seat_map[r][c] = 0;
            }
        }
    }
}


void printLine() {
    for(int i=0;i<40;i++) printf("=");
    printf("\n");
}

void printHeader(const char* title) {
    printLine();
    int padding = (40 - (int)strlen(title)) / 2;
    for(int i=0;i<padding;i++) printf(" ");
    printf(C_CYAN "%s\n" C_RESET, title);
    printLine();
}

void printMenu() {
    printHeader(" MAVERICK AIRLINES ");
    printf(C_GREEN "1. Reservation\n2. Cancel Booking\n3. Display Ticket\n4. Show Seat Map\n5. Exit\n" C_RESET);
    printLine();
}

bool isLeapYear(int y) {
    return (y%4==0 && y%100!=0) || (y%400==0);
}

bool isValidDate(int d,int m,int y, Date today) {
    if(y < today.year) return false;
    if(y == today.year && m < today.month) return false;
    if(y == today.year && m == today.month && d < today.day) return false;
    if(m < 1 || m > 12 || d < 1) return false;
    int daysPerMonth[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(m == 2 && isLeapYear(y)) daysPerMonth[2]=29;
    if(d>daysPerMonth[m]) return false;
    return true;
}

bool isValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    if (!at || at == email) return false;
    const char *dot = strchr(at, '.');
    if (!dot || dot == at+1 || strlen(dot) < 3) return false;
    return true;
}

unsigned hash_passport(const char* passport) {
    unsigned h=0;
    for(;*passport;passport++) h = h*31 + *passport;
    return h % HASH_SIZE;
}

Passenger* findPassengerGlobal(const char* passport, Flight** flightFound) {
    unsigned idx = hash_passport(passport);
    PassengerHashNode* node = passenger_hash[idx];
    while(node) {
        if(strcmp(node->passenger->passport, passport) == 0) {
            if(flightFound) *flightFound = node->passenger->flight_ptr;
            return node->passenger;
        }
        node = node->next;
    }
    return NULL;
}

void addPassengerToHash(Passenger* p) {
    unsigned h = hash_passport(p->passport);
    PassengerHashNode* node = malloc(sizeof(PassengerHashNode));
    node->passenger = p;
    node->next = passenger_hash[h];
    passenger_hash[h] = node;
}

void removePassengerFromHash(Passenger* p) {
    unsigned h = hash_passport(p->passport);
    PassengerHashNode* node = passenger_hash[h];
    PassengerHashNode* prev = NULL;
    while(node) {
        if(node->passenger == p) {
            if(prev) prev->next = node->next;
            else passenger_hash[h] = node->next;
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

bool assignSeat(Flight* f, char* seat_num_out) {
    for(int r=0; r<ROWS; ++r) {
        for(int c=0; c<COLS; ++c) {
            if(f->seat_map[r][c] == 0) {
                f->seat_map[r][c] = 1;
                sprintf(seat_num_out, "%d-%c", r+1, 'A'+c);
                return true;
            }
        }
    }
    return false; 
}

void freeSeat(Flight* f, const char* seat_num) {
    int row; char colChar;
    if(sscanf(seat_num, "%d-%c", &row, &colChar) == 2) {
        int col = colChar - 'A';
        if(row >=1 && row <= ROWS && col >=0 && col < COLS) {
            f->seat_map[row-1][col] = 0;
        }
    }
}

void enqueueWaitlist(Waitlist* wl, Passenger* p) {
    WaitlistNode* node = malloc(sizeof(WaitlistNode));
    node->passenger = p;
    node->next = NULL;
    if(wl->rear) wl->rear->next = node;
    else wl->front = node;
    wl->rear = node;
    wl->size++;
}

Passenger* dequeueWaitlist(Waitlist* wl) {
    if(wl->front == NULL) return NULL;
    WaitlistNode* node = wl->front;
    Passenger* p = node->passenger;
    wl->front = node->next;
    if(wl->front == NULL) wl->rear = NULL;
    free(node);
    wl->size--;
    return p;
}


void addPassenger() {
    char temp[40];
    char passport[8];
    char name[30];
    char email[40];
    int d, m, y;

    printHeader("FLIGHT SELECTION");

    int dep_city;
    do {
        printf("Select departure city:\n");
        for(int i=1; i<=5; i++) printf("%d. %s\n", i, cities[i]);
        printf("Choice (1-5): ");
        if(scanf("%d", &dep_city)!=1 || dep_city < 1 || dep_city > 5) {
            while(getchar() != '\n'); 
            printf(C_RED "Invalid city. Try again.\n" C_RESET);
            dep_city = -1;
        }
    } while(dep_city == -1);

    int dest_city;
    do {
        printf("Select destination city (different from departure):\n");
        for(int i=1; i<=5; i++) if(i!=dep_city) printf("%d. %s\n", i, cities[i]);
        printf("Choice: ");
        if(scanf("%d", &dest_city)!=1 || dest_city < 1 || dest_city > 5 || dest_city == dep_city) {
            while(getchar() != '\n'); 
            printf(C_RED "Invalid city. Try again.\n" C_RESET);
            dest_city = -1;
        }
    } while(dest_city == -1);

    Flight* route_flights[3];
    int count=0;
    for(int i=0; i<TOTAL_FLIGHTS; i++) {
        if(strcmp(flights[i].departure, cities[dep_city]) == 0 &&
           strcmp(flights[i].destination, cities[dest_city]) == 0) {
            route_flights[count++] = &flights[i];
        }
    }

    if(count != 3) {
        printf(C_RED "Error: flights not found for this route.\n" C_RESET);
        return;
    }

    printf("Available flight schedules for %s -> %s:\n", cities[dep_city], cities[dest_city]);
    for(int i=0; i<3; i++) {
        printf("%d. ", i+1);
        switch(route_flights[i]->schedule) {
            case 1: printf("8am-10am"); break;
            case 2: printf("11am-1pm"); break;
            case 3: printf("3pm-5pm"); break;
            default: printf("Unknown"); break;
        }
        int freeSeats=0;
        for(int r=0;r<ROWS;r++) {
            for(int c=0;c<COLS;c++) {
                if(route_flights[i]->seat_map[r][c]==0) freeSeats++;
            }
        }
        printf("  | Seats available: %d\n", freeSeats);
    }

    int sched_choice = 0;
    do {
        printf("Choose schedule (1-3): ");
        if(scanf("%d", &sched_choice) != 1 || sched_choice < 1 || sched_choice > 3) {
            while(getchar() != '\n');
            printf(C_RED "Invalid choice. Try again.\n" C_RESET);
            sched_choice = 0;
        }
    } while(sched_choice == 0);

    Flight* chosen_flight = route_flights[sched_choice - 1];

    do {
        printf("Enter Passport Number (6 chars): ");
        scanf("%7s", passport);
        if(strlen(passport) != 6 || findPassengerGlobal(passport, NULL)) {
            printf(C_RED "Invalid or duplicate passport. Try again.\n" C_RESET);
        } else break;
    } while(1);

    getchar(); 
    printf("Enter Name: ");
    fgets(name, 30, stdin);
    name[strcspn(name, "\n")] = 0;

    do {
        printf("Flight Date (DD-MM-YYYY): ");
        if(scanf("%d-%d-%d", &d, &m, &y) != 3 || !isValidDate(d,m,y,today)) {
            while(getchar() != '\n');
            printf(C_RED "Invalid date. Try again.\n" C_RESET);
        } else break;
    } while(1);

    do {
        printf("Enter Email: ");
        scanf("%39s", email);
        if(!isValidEmail(email)) {
            printf(C_RED "Invalid email format. Try again.\n" C_RESET);
        } else break;
    } while(1);

    Passenger* p = malloc(sizeof(Passenger));
    strcpy(p->passport, passport);
    strcpy(p->name, name);
    strcpy(p->email, email);
    p->day = d; p->month = m; p->year = y;
    p->flight_ptr = chosen_flight;

    if(assignSeat(chosen_flight, p->seat_num)) {
        p->next = chosen_flight->passengers;
        chosen_flight->passengers = p;
        addPassengerToHash(p);
        printHeader("RESERVATION SUCCESSFUL!");
        printf(C_GREEN "Seat Number: %s\nDon't forget your passport!\n" C_RESET, p->seat_num);
    } else {
        if(chosen_flight->waitlist.size < MAX_WAITLIST) {
            enqueueWaitlist(&chosen_flight->waitlist, p);
            printHeader("WAITLISTED");
            printf(C_YELLOW "Flight full, added to waitlist. We'll notify if seat becomes available.\n" C_RESET);
        } else {
            printf(C_RED "Flight and waitlist full. Try another flight.\n" C_RESET);
            free(p);
        }
    }
    printLine();
}


void cancelBooking() {
    char passport[8];
    Flight* flight_of_passenger = NULL;
    printHeader("CANCEL BOOKING");
    printf("Enter Passport Number: ");
    scanf("%7s", passport);
    Passenger* p = findPassengerGlobal(passport, &flight_of_passenger);
    if(!p) {
        printf(C_RED "No booking found with this passport.\n" C_RESET);
        return;
    }

    Passenger* cur = flight_of_passenger->passengers;
    Passenger* prev = NULL;
    while(cur && cur != p) {
        prev = cur;
        cur = cur->next;
    }
    if(!cur) { 
        printf(C_RED "Internal error.\n" C_RESET);
        return;
    }
    if(prev) prev->next = cur->next;
    else flight_of_passenger->passengers = cur->next;

    removePassengerFromHash(p);

    freeSeat(flight_of_passenger, p->seat_num);
    printf(C_GREEN "Booking cancelled successfully.\n" C_RESET);

    Passenger* waitlisted_passenger = dequeueWaitlist(&flight_of_passenger->waitlist);
    if(waitlisted_passenger) {
        if(assignSeat(flight_of_passenger, waitlisted_passenger->seat_num)) {
            waitlisted_passenger->next = flight_of_passenger->passengers;
            flight_of_passenger->passengers = waitlisted_passenger;
            addPassengerToHash(waitlisted_passenger);
            printf(C_GREEN "Waitlisted passenger promoted:\nName: %s, Passport: %s\n", waitlisted_passenger->name, waitlisted_passenger->passport);
        } else {
            printf(C_RED "Error: Could not assign seat to waitlisted passenger!\n" C_RESET);
            free(waitlisted_passenger);
        }
    }
    free(p);
    printLine();
}


void displayTicket() {
    char passport[8];
    Flight* flight_of_passenger = NULL;
    printHeader("DISPLAY TICKET");
    printf("Enter Passport Number: ");
    scanf("%7s", passport);
    Passenger* p = findPassengerGlobal(passport, &flight_of_passenger);
    if(!p) {
        printf(C_RED "No booking found with this passport.\n" C_RESET);
        return;
    }

    printHeader("BOARDING PASS");

    printf("Name       : %s\n", p->name);
    printf("Email      : %s\n", p->email);
    printf("From       : %s\n", flight_of_passenger->departure);
    printf("To         : %s\n", flight_of_passenger->destination);
    printf("Date       : %02d-%02d-%04d\n", p->day, p->month, p->year);
    printf("Seat       : %s\n", p->seat_num);
    printf("Schedule   : ");
    switch(flight_of_passenger->schedule) {
        case 1: printf("8am-10am"); break;
        case 2: printf("11am-1pm"); break;
        case 3: printf("3pm-5pm"); break;
        default: printf("Unknown"); break;
    }
    printf("\n");
    printf(C_MAGENTA "Gate       : %d\n" C_RESET, flight_of_passenger->schedule*flight_of_passenger->schedule + flight_of_passenger->schedule*flight_of_passenger->schedule*flight_of_passenger->schedule);
    printLine();
}


void showSeatMap() {
    int dep_city, dest_city;
    printHeader("SHOW SEAT MAP");

    do {
        printf("Select departure city:\n");
        for(int i=1; i<=5; i++) printf("%d. %s\n", i, cities[i]);
        printf("Choice (1-5): ");
        if(scanf("%d", &dep_city) != 1 || dep_city < 1 || dep_city > 5) {
            while(getchar() != '\n');
            printf(C_RED "Invalid city. Try again.\n" C_RESET);
            dep_city = -1;
        }
    } while(dep_city == -1);

    do {
        printf("Select destination city (different from departure):\n");
        for(int i=1; i<=5; i++) if(i != dep_city) printf("%d. %s\n", i, cities[i]);
        printf("Choice: ");
        if(scanf("%d", &dest_city) != 1 || dest_city < 1 || dest_city > 5 || dest_city == dep_city) {
            while(getchar() != '\n');
            printf(C_RED "Invalid city. Try again.\n" C_RESET);
            dest_city = -1;
        }
    } while(dest_city == -1);

    Flight* route_flights[3];
    int count=0;
    for(int i=0; i<TOTAL_FLIGHTS; i++) {
        if(strcmp(flights[i].departure, cities[dep_city])==0 &&
           strcmp(flights[i].destination, cities[dest_city])==0) {
            route_flights[count++] = &flights[i];
        }
    }
    if(count != 3) {
        printf(C_RED "Error: flights not found for this route.\n" C_RESET);
        return;
    }

    printf("Available flight schedules:\n");
    for(int i=0; i<count; i++) {
        printf("%d. ", i+1);
        switch(route_flights[i]->schedule) {
            case 1: printf("8am-10am"); break;
            case 2: printf("11am-1pm"); break;
            case 3: printf("3pm-5pm"); break;
            default: printf("Unknown"); break;
        }
        int freeSeats=0;
        for(int r=0;r<ROWS;r++) for(int c=0;c<COLS;c++) if(route_flights[i]->seat_map[r][c] == 0) freeSeats++;
        printf(" - Free Seats: %d\n", freeSeats);
    }

    int sched_choice=0;
    do {
        printf("Choose schedule (1-3): ");
        if(scanf("%d", &sched_choice) != 1 || sched_choice <1 || sched_choice >3) {
            while(getchar() != '\n');
            printf(C_RED "Invalid choice. Try again.\n" C_RESET);
            sched_choice = 0;
        }
    } while(sched_choice == 0);

    Flight* chosen_flight = route_flights[sched_choice - 1];

    printHeader("SEAT MAP");
    printf("Flight: %s -> %s  Schedule: ", chosen_flight->departure, chosen_flight->destination);
    switch(chosen_flight->schedule) {
        case 1: printf("8am-10am"); break;
        case 2: printf("11am-1pm"); break;
        case 3: printf("3pm-5pm"); break;
        default: printf("Unknown");
    }
    printf("\n\n");

    printf("    ");
    for(int c=0; c<COLS; c++) printf("  %c ", 'A'+c);
    printf("\n");
    for(int r=0; r<ROWS; r++) {
        printf("Row%2d ", r+1);
        for(int c=0; c<COLS; c++) {
            if(chosen_flight->seat_map[r][c] == 0) 
                printf(C_GREEN " [ ]" C_RESET);
            else
                printf(C_RED " [X]" C_RESET);
        }
        printf("\n");
    }
    printf("\nWaitlist size: %d\n", chosen_flight->waitlist.size);
    printLine();
}


void freeAllPassengers() {
    for(int i=0; i<TOTAL_FLIGHTS; i++) {
        Passenger* p = flights[i].passengers;
        while(p) {
            Passenger* nxt = p->next;
            free(p);
            p = nxt;
        }
        while(flights[i].waitlist.front) {
            Passenger* w_pass = dequeueWaitlist(&flights[i].waitlist);
            free(w_pass);
        }
    }
    for(int i=0; i<HASH_SIZE; i++) {
        PassengerHashNode* node = passenger_hash[i];
        while(node) {
            PassengerHashNode* nxt = node->next;
            free(node);
            node = nxt;
        }
        passenger_hash[i] = NULL;
    }
}
