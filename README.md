# Online-Library-Management-System

Developed an Online Library Management System (OLMS) that not only provides basic functionalities of a library but also ensures data security and concurrency control. The system employs socket programming to enable multiple clients to access the library database concurrently. Additionally, system calls are utilized extensively for various operations such as process management, file handling, file locking, multithreading, and interprocess communication, ensuring optimal performance and resource utilization.

## Description

- The **client.c** file simulates the options available to the clients, either as members or as administrators.  
- The **server.c** file simulates the server that holds access to all the various types of records about the books, members, and administrators as well as their cart history, etc.
- The **tester.c** file is used to test the various functionalities.
- The **common.h** file defines shared structures used by both the client and server for managing data in the library management system. These include definitions for books, members, admins, borrowed books, and index structures for efficient data access.
- The **server.h** file contains declarations for server-side functions and shared mutex variables, enabling the testing of critical operations such as member and admin sign-up, login, password reset, and book management. 

## Install and Run
Clone this repo using
```
git clone https://github.com/Aayush-Bhargav/Online-Library-Management-System.git
```
Go to the right directory using
```
cd Online-Library-Management-System
```
## Launch
Launch the server first to create a half socket and start listening for connections using
```
gcc server.c
./a.out
```
Launch the client using
```
gcc client.c
./a.out
```
Socket connection will be established, and the client and the server can then communicate with each other.

## Running the tester
Make sure you have CUnit installed first. The following command is for Ubuntu systems.
```
sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev
```

To run the tester, create an object file for the server.
```
gcc -c server.c -o server.o -pthread
```
Next, run the following command.
```
gcc -DTEST_MODE -c server.c -o server.o -pthread
```
Next, compile and link **tester.c** with **server.o**
```
gcc -o tester tester.c server.o -lcunit -pthread
```
Finally, run the tester.
```
./tester
```

## Client Operations

In `client.c`, you will be given options to either sign-in/log-in as a member or sign-in/log-in as an administrator. Initially, you always have to sign in (equivalent to creating an account) before logging in. Once you create an account, then you are free to log in. In case, while logging in, you forget your password, options have been provided to reset your password using a forgot_password function that just takes in your Gmail to verify it is indeed you and allows you to change your password.

For the administrators, in order to ensure that only authorized personnel are able to sign up or log in as administrators, there is a common key that they need to know and only once they enter that key will they be able to sign up or log in as admins.

### Member Funtions/Options

Once you log in as a member, you have the opportunity to:
- Add book to cart.
- View cart.
- Modify cart.
- Borrow book (this is like confirming payment for the items in your cart).
- Return book.
- View borrowed books.
- View available books.

### Administrator Functions/Options

Once you log in as an admin, you have the opportunity to:
- View members.
- Add a book to the library.
- Remove a book from the library.
- Modify a book's details.
- View books.
- View books borrowed by a specific member.
- View specific book details.
- View admin details.

## Notes

- The member ID, admin ID, and book ID are all assigned by the server and in an incremental fashion starting with ID=1.
- Several text files are used as databases to store information:
  - `member.txt` -> holds all the member information (Name, Email, Password, ID)
  - `admin.txt` -> holds all the admin information (Name, Email, ID)
  - `book.txt` -> holds all the book information (Name, Author, ID, Number of Copies, Price for borrowing for one day)
  - `records.txt` -> holds details about members and their corresponding cart information
  - `borrowed_books.txt` -> holds details about members and the list of books they have borrowed

Whatever you choose to do in `client.c` will be communicated to `server.c` using the socket established and all the actions on the files will be done by the server ensuring the client doesn't have direct access to the server. The server is a concurrent server and can support multiple client connections simultaneously.

In order to prevent any race conditions, mutex locks have been applied in the critical sections ensuring that the data is consistent.


  
