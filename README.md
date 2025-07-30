A console-based airline booking system implemented in C to manage flight reservations, cancellations, and ticketing with support for multiple routes and schedules. The system uses advanced data structures and simple graphical console output to provide an enhanced user experience and efficient seat management.

Features
Supports 20 distinct flight routes (departure-destination pairs) with 3 scheduled flights each (total 60 flights).

Each flight maintains its own seat map (10 rows × 6 seats) and passenger waitlist.

Implements fast passenger lookup using a hash table indexed by passport numbers.

Handles seat allocation, booking cancellation, and automatic waitlist promotion.

Validates inputs such as date, email, and passport uniqueness.

Displays seat maps with colored and styled console output for better visual clarity.

Menu-driven interface with detailed prompts for route, schedule, and passenger details.

Installation and Usage
1. Compile the source code using a GCC-compatible compiler:
    gcc DSA_AirlineBooking.c -o DSA_AirlineBooking

2. Run the executable:
    ./DSA_AirlineBooking

3. Follow the menu options to reserve tickets, display boarding passes, cancel bookings, and view seat maps for specific flights.

File Structure
airline_booking.c: Main source code file containing the system implementation.

Technologies Used
C programming language

Linked lists, hash tables, queues, and arrays for data management

ANSI escape codes for console text coloring and simple graphical layouts
   
