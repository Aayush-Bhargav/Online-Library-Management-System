
#include "server.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>


// Test setup
void setup_test_environment() {
    // Clear test files
    int fd = open("member.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
    fd = open("memberIndex.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
    fd = open("admin.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
    fd = open("adminIndex.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
    fd = open("book.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
     fd = open("borrowed_books.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
     fd = open("bookIndex.txt", O_WRONLY | O_TRUNC | O_CREAT, 0776);
    close(fd);
}

// Test cleanup
void cleanup_test_environment() {
    remove("member.txt");
    remove("memberIndex.txt");
    remove("admin.txt");
    remove("adminIndex.txt");
    remove("book.txt");
    remove("borrowed_books.txt");
    remove("bookIndex.txt");
}

// Helper function to verify file contents
int verify_member_file(int expected_id, const char* expected_name, const char* expected_email, const char* expected_password) {
    int fd = open("member.txt", O_RDONLY);
    if (fd == -1) return 1;
    struct member read_member;
    read(fd, &read_member, sizeof(struct member));
    close(fd);
    if (read_member.memberId != expected_id) return 2;
    if (strcmp(read_member.name, expected_name) != 0) return 3;
    if (strcmp(read_member.email, expected_email) != 0) return 4;
    if (strcmp(read_member.password, expected_password) != 0) return 5;

    return 0; // Success
}
// Helper function to verify file contents
int verify_admin_file(int expected_id, const char* expected_name, const char* expected_email) {
    int fd = open("admin.txt", O_RDONLY);
    if (fd == -1) return 1;
    struct admin read_admin;
    read(fd, &read_admin, sizeof(struct admin));
    close(fd);
    if (read_admin.adminId != expected_id) return 2;
    if (strcmp(read_admin.name, expected_name) != 0) return 3;
    if (strcmp(read_admin.email, expected_email) != 0) return 4;

    return 0; // Success
}

// Test for memberSignUp
void test_member_signup() {

    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    // Mock client behavior
    struct member mock_member;
    strcpy(mock_member.name, "John Doe");
    strcpy(mock_member.email, "john@example.com");
    strcpy(mock_member.password, "password123");

    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        memberSignUp(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process

        // Simulate client receiving member ID assigned by the server
        int assigned_id;
        read(client_fd, &assigned_id, sizeof(int));

        // Verify assigned ID
        CU_ASSERT_EQUAL(assigned_id, 1);

        // Simulate client sending data
        write(client_fd, mock_member.name, sizeof(mock_member.name));
        write(client_fd, mock_member.email, sizeof(mock_member.email));
        write(client_fd, mock_member.password, sizeof(mock_member.password));

        // Wait for the server to complete
        wait(NULL);

        // Verify contents of member.txt
        int result = verify_member_file(assigned_id, mock_member.name, mock_member.email, mock_member.password);
        CU_ASSERT_EQUAL(result, 0); // Ensure member data was written correctly

        close(client_fd);
    }

}

// Test for getMemberOffset
void test_get_member_offset(){
    int fd = open("test_memberIndex.txt", O_RDWR | O_CREAT | O_TRUNC, 0774);
    if (fd == -1) {
        perror("open");
        CU_FAIL("Failed to create test_memberIndex.txt");
        return;
    }

    // Populate the test file with mock data
    struct memberIndex index;
    index.memberId=2;
    index.memberOffset=200;
    if (write(fd, &index, sizeof(struct memberIndex)) == -1) {
        perror("write");
        CU_FAIL("Failed to write mock data to test_memberIndex.txt");
        close(fd);
        return;
    }
    lseek(fd, 0, SEEK_SET);
    // Test Case 1: Valid memberId
    int offset = getMemberOffset(2, fd); // Look for memberId 2
    CU_ASSERT_EQUAL(offset, 200);

    // Test Case 2: Invalid memberId (not in file)
    offset = getMemberOffset(4, fd); // Look for memberId 4
    CU_ASSERT_EQUAL(offset, -1);

    // Test Case 3: Invalid memberId (negative ID)
    offset = getMemberOffset(-1, fd); // Negative memberId
    CU_ASSERT_EQUAL(offset, -1);

    // Cleanup: Close and remove the temporary file
    close(fd);
    remove("test_memberIndex.txt");
}

// Test for adminSignUp
void test_admin_signup() {

    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    // Mock client behavior
    struct admin mock_admin;
    strcpy(mock_admin.name, "Rooot");
    strcpy(mock_admin.email, "root@admin.com");

    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        adminSignUp(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // admin must enter password first in order to sign up as well because an admin can only sign up if he knows the password of the library system
        // in the program, the admin has to enter the value 2 if he wants to enter the password
        int choice=2;
        write(client_fd,&choice,sizeof(int));
        // enter incorrect password and expect -1 as reply.
        int reply;
        write(client_fd,"admin12",sizeof("admin12"));
        read(client_fd, &reply, sizeof(int));
        // Verify reply. Since you entered wrong password, reply must be -1.
        CU_ASSERT_EQUAL(reply, -1); 
        
        write(client_fd,&choice,sizeof(int));
        // enter the correct password and expect 1 as reply
        write(client_fd,"admin1234",sizeof("admin1234"));
        read(client_fd, &reply, sizeof(int));
        CU_ASSERT_EQUAL(reply, 1); 
        // Simulate client receiving admin ID assigned by the server
        int assigned_id;
        read(client_fd, &assigned_id, sizeof(int));

        // Verify assigned ID
        CU_ASSERT_EQUAL(assigned_id, 1);

        // Simulate client sending data
        write(client_fd, mock_admin.name, sizeof(mock_admin.name));
        write(client_fd, mock_admin.email, sizeof(mock_admin.email));

        // Wait for the server to complete
        wait(NULL);

        // Verify contents of admin.txt
        int result = verify_admin_file(assigned_id, mock_admin.name, mock_admin.email);
        CU_ASSERT_EQUAL(result, 0); // Ensure admin data was written correctly

        close(client_fd);
    }
}

// Test for getAdminOffset
void test_get_admin_offset(){
    int fd = open("test_AdminIndex.txt", O_RDWR | O_CREAT | O_TRUNC, 0774);
    if (fd == -1) {
        perror("open");
        CU_FAIL("Failed to create test_AdminIndex.txt");
        return;
    }

    // Populate the test file with mock data
    struct adminIndex index;
    index.adminId=2;
    index.adminOffset=200;
    if (write(fd, &index, sizeof(struct adminIndex)) == -1) {
        perror("write");
        CU_FAIL("Failed to write mock data to test_AdminIndex.txt");
        close(fd);
        return;
    }
    lseek(fd, 0, SEEK_SET);
    // Test Case 1: Valid AdminId
    int offset = getAdminOffset(2, fd); // Look for AdminId 2
    CU_ASSERT_EQUAL(offset, 200);

    // Test Case 2: Invalid AdminId (not in file)
    offset = getAdminOffset(4, fd); // Look for AdminId 4
    CU_ASSERT_EQUAL(offset, -1);

    // Test Case 3: Invalid AdminId (negative ID)
    offset = getAdminOffset(-1, fd); // Negative AdminId
    CU_ASSERT_EQUAL(offset, -1);

    // Cleanup: Close and remove the temporary file
    close(fd);
    remove("test_AdminIndex.txt");
}

// Test for memberLogIn
void test_member_login() {
    
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side


    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        memberLogIn(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // First, we need to enter member ID
        int memberId=1;
        write(client_fd, &memberId, sizeof(int));
        int reply;
        read(client_fd,&reply,sizeof(int));
        // Since we have already created a user, his member ID is 1. So, the ID we entered is valid and the reply received must not be equal to -1.
        CU_ASSERT_NOT_EQUAL(reply,-1);
        // Now, we must enter password
        // Simulate client sending password
        write(client_fd, "wrong_password", sizeof("wrong_password"));
        // The above password is incorrect, so we must get -1 as reply.
        read(client_fd,&reply,sizeof(int));
        CU_ASSERT_EQUAL(reply,-1);
        // We need to try again. For that we need to first write the value 1.
        write(client_fd, &memberId, sizeof(int));
        // Then, we need to put the correct password
        write(client_fd, "password123", sizeof("password123"));
        // Now, the reply must be equal to 1.
        read(client_fd,&reply,sizeof(int));
        CU_ASSERT_EQUAL(reply,1);
        // Server will also send the name of the member.
        char name[50];
        read(client_fd,name,sizeof(name));
        // Compare it with the name we have
        CU_ASSERT_EQUAL(strcmp(name,"John Doe"),0);
        // Function is a success. Now let us stop it by writing some random value
        reply=10;
        write(client_fd,&reply,sizeof(int));

        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}
// Test for adminLogIn
void test_admin_login() {
    
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        adminLogIn(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // First, we need to enter admin ID
        int adminId=1;
        write(client_fd, &adminId, sizeof(int));
        int reply;
        read(client_fd,&reply,sizeof(int));
        // Since we have already created a admin, his admin ID is 1. So, the ID we entered is valid and the reply received must not be equal to -1.
        CU_ASSERT_NOT_EQUAL(reply,-1);
        // Now, we must enter password
        // Simulate client sending password
        write(client_fd, "wrong_password", sizeof("wrong_password"));
        // The above password is incorrect, so we must get -1 as reply.
        read(client_fd,&reply,sizeof(int));
        CU_ASSERT_EQUAL(reply,-1);
        // Then, we need to put the correct password
        write(client_fd, "admin1234", sizeof("admin1234"));
        // Now, the reply must be equal to 1.
        read(client_fd,&reply,sizeof(int));
        CU_ASSERT_EQUAL(reply,1);
        // Server will also send the name of the admin.
        char name[50];
        read(client_fd,name,sizeof(name));
        // Compare it with the name we have
        CU_ASSERT_EQUAL(strcmp(name,"Rooot"),0);
        // Function is a success. Now let us stop it by writing some random value
        reply=10;
        write(client_fd,&reply,sizeof(int));

        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}

// Test for reset password
void test_reset_password(){
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    // Mock client behavior
    struct member mock_member;
    mock_member.memberId=1;
    strcpy(mock_member.name, "John Doe");
    strcpy(mock_member.email, "john@example.com");
    strcpy(mock_member.password, "password123");
    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        resetPassword(server_fd,&mock_member);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // First, we need to enter email

        // Case 1: Wrong email

        write(client_fd, "simply@gmail.com", sizeof("simply@gmail.com"));
        int reply;
        read(client_fd,&reply,sizeof(int));
        // reply must be equal to -1
        CU_ASSERT_EQUAL(reply,-1);

        // Case 2: Correct Email

        write(client_fd, mock_member.email, sizeof(mock_member.email));
        read(client_fd,&reply,sizeof(int));
        // Now, the value of the reply must be 1.
        CU_ASSERT_EQUAL(reply,1);
        
        // Now, we must enter new password 
        // Simulate client sending password
        write(client_fd, "newPassword", sizeof("newPassword"));
        // That is it. Password is reset.
        wait(NULL);
        close(client_fd);
    }
}
// Test for getBookOffset
void test_get_book_offset(){
    int fd = open("test_bookIndex.txt", O_RDWR | O_CREAT | O_TRUNC, 0774);
    if (fd == -1) {
        perror("open");
        CU_FAIL("Failed to create test_bookIndex.txt");
        return;
    }
    // Populate the test file with mock data
    struct bookIndex index;
    index.bookId=2;
    index.bookOffset=200;
    if (write(fd, &index, sizeof(struct bookIndex)) == -1) {
        perror("write");
        CU_FAIL("Failed to write mock data to test_bookIndex.txt");
        close(fd);
        return;
    }
    lseek(fd, 0, SEEK_SET);
    // Test Case 1: Valid bookId
    int offset = getBookOffset(2, fd); // Look for bookId 2
    CU_ASSERT_EQUAL(offset, 200);

    // Test Case 2: Invalid bookId (not in file)
    offset = getBookOffset(4, fd); // Look for bookId 4
    CU_ASSERT_EQUAL(offset, -1);

    // Test Case 3: Invalid bookId (negative ID)
    offset = getBookOffset(-1, fd); // Negative bookId
    CU_ASSERT_EQUAL(offset, -1);

    // Cleanup: Close and remove the temporary file
    close(fd);
    remove("test_bookIndex.txt");
}
// Test for add book
void test_add_book(){
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    // Mock client behavior
    struct book mock_book;
    strcpy(mock_book.name, "James Bond");
    strcpy(mock_book.author, "Arthur Conan Doyle");
    mock_book.copies=5;
    mock_book.price=100;
    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        add_book(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // First, the server will assign a book id
        int assigned;
        read(client_fd,&assigned,sizeof(assigned));
        // Since there is no book, this should return 1
        CU_ASSERT_EQUAL(assigned,1);
        write(client_fd, mock_book.name, sizeof(mock_book.name));
        write(client_fd, mock_book.author, sizeof(mock_book.author));
        write(client_fd, &mock_book.copies, sizeof(int));
        write(client_fd, &mock_book.price, sizeof(double));
        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}
// Test for modify book
void test_modify_book(){
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        modify_book(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // First, client must send the id of the book he wants to modify
        int id=1;
        int reply;
        write(client_fd,&id,sizeof(int));
        read(client_fd,&reply,sizeof(int));
        CU_ASSERT_NOT_EQUAL(reply,-1); //means valid book
        // the server will send book details. Read it.
        int copies;
        char name[50],author[50];
        double price;
        read(client_fd,&copies,sizeof(int));
        CU_ASSERT_EQUAL(copies,5);
        read(client_fd,name,sizeof(name));
        CU_ASSERT_EQUAL(strcmp(name,"James Bond"),0);
        read(client_fd,author,sizeof(author));
        CU_ASSERT_EQUAL(strcmp(author,"Arthur Conan Doyle"),0);
        read(client_fd,&price,sizeof(double));
        CU_ASSERT_EQUAL(price,100);

        // Let us modify the number of copies. For that we have to send "2" first then the updated number of copies
        id=2;
        write(client_fd,&id,sizeof(int));
        // now, let us send the updated number of copies
        copies=15;
        write(client_fd,&copies,sizeof(int));
        // Now, the update is completed.
       
        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}
void test_view_books(){
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        view_books(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // The server sends you the number of books available
        int count;
        read(client_fd,&count,sizeof(int));
        CU_ASSERT_EQUAL(count,1); // Since, 1 book is there
        int bookId,copies; 
        read(client_fd,&bookId,sizeof(int));
        CU_ASSERT_EQUAL(bookId,1);
        char name[50],author[50];
        double price;
        
        read(client_fd,name,sizeof(name));
        CU_ASSERT_EQUAL(strcmp(name,"James Bond"),0);
        read(client_fd,author,sizeof(author));
        CU_ASSERT_EQUAL(strcmp(author,"Arthur Conan Doyle"),0);
        read(client_fd,&copies,sizeof(int));
        CU_ASSERT_EQUAL(copies,15);
        read(client_fd,&price,sizeof(double));
        CU_ASSERT_EQUAL(price,100);
       
        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}
// Test for remove book
void test_remove_book(){
    // Create a socketpair for bidirectional communication
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        remove_book(server_fd);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process
        // First, client must send the id of the book he wants to modify
        int id=1;
        int reply;
        write(client_fd,&id,sizeof(int));
        read(client_fd,&reply,sizeof(int));
        CU_ASSERT_NOT_EQUAL(reply,-1); //means valid book
        // read the current number of copies
        int copies;
        read(client_fd,&copies,sizeof(int));
        CU_ASSERT_EQUAL(copies,15);
        // Book gets removed. Next time you call view, you will see nothing printed.
        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}

// Test for add_book_to_cart
void test_add_book_to_cart() {
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        close(client_fd); // Close client side in server process
        add_book_to_cart(server_fd, 1); 
        close(server_fd);
        exit(0); 
    } else {
        close(server_fd); // Close server side in client process
        
        // Simulate client sending book ID
        int bookId = 1;
        write(client_fd, &bookId, sizeof(int));

        // Read offset from server
        int offset;
        read(client_fd, &offset, sizeof(int));
        CU_ASSERT_NOT_EQUAL(offset, -1); // Ensure valid book ID

        if (offset != -1) {
            int copies;
            char name[50], author[50];
            double price;
            read(client_fd, &copies, sizeof(int));
            read(client_fd, name, sizeof(name));
            read(client_fd, author, sizeof(author));
            read(client_fd, &price, sizeof(double));

            CU_ASSERT_EQUAL(copies, 15);
            CU_ASSERT_EQUAL(strcmp(name,"James Bond"),0);
            CU_ASSERT_EQUAL(strcmp(author,"Arthur Conan Doyle"),0);
            CU_ASSERT_DOUBLE_EQUAL(price, 100, 0); // Expected price

            // Simulate client sending additional cart details
            int numberOfCopies = 5;
            int days = 1;
            write(client_fd, &numberOfCopies, sizeof(int));
            write(client_fd, &days, sizeof(int));
        }

        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}


// Test for view_cart
void test_view_cart() {
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0];
    int server_fd = sockets[1];

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd);
        view_cart(server_fd, 1);
        close(server_fd);
        exit(0);
    } else {
        // Parent process: acts as client
        close(server_fd);

        int count;
        read(client_fd, &count, sizeof(int));
        CU_ASSERT_EQUAL(count, 1); // Each execution of code adds 1 book to cart

        if (count > 0) {
            struct borrowed_book book;
            read(client_fd, &book, sizeof(struct borrowed_book));
            // Validate borrowed book details
            CU_ASSERT_EQUAL(book.bookId, 1);
            CU_ASSERT_EQUAL(book.numberOfCopies, 5);
            CU_ASSERT_EQUAL(book.numberOfDays, 1);
        }

        close(client_fd);
        wait(NULL);
    }
}

// Test for modify_cart
void test_modify_cart() {
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        CU_FAIL("Failed to create socketpair");
        return;
    }

    int client_fd = sockets[0]; // Client side
    int server_fd = sockets[1]; // Server side

    // Mock client behavior
    struct borrowed_book mock_borrowed_book;
    mock_borrowed_book.bookId = 1;
    mock_borrowed_book.numberOfCopies = 5;
    mock_borrowed_book.numberOfDays = 1;

    // Simulate server handling the client request
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        CU_FAIL("Failed to fork process");
        return;
    }

    if (pid == 0) {
        // Child process: acts as server
        close(client_fd); // Close client side in server process
        modify_cart(server_fd, 1);
        close(server_fd);
        exit(0); // Exit child process
    } else {
        // Parent process: acts as client
        close(server_fd); // Close server side in client process

        // Simulate client sending book ID
        int bookId = 1;
        write(client_fd, &bookId, sizeof(int));

        // Read offset from server
        int offset;
        read(client_fd, &offset, sizeof(int));
        CU_ASSERT_NOT_EQUAL(offset, -1); // Ensure valid book ID

        if (offset != -1) {
            // Simulate client modifying number of copies
            int choice = 2;
            write(client_fd, &choice, sizeof(int));

            // Read current number of copies
            int current_copies;
            read(client_fd, &current_copies, sizeof(int));
            CU_ASSERT_EQUAL(current_copies, 5);

            // Read available copies in library
            int available_copies;
            read(client_fd, &available_copies, sizeof(int));
            CU_ASSERT_EQUAL(available_copies, 10);

            // Simulate client sending new number of copies
            int new_copies = 0; // Remove the book from cart
            write(client_fd, &new_copies, sizeof(int));
        }

        // Wait for the server to complete
        wait(NULL);
        close(client_fd);
    }
}
int main() {
    // Initialize CUnit test registry
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    // Add test suite
    CU_pSuite suite = CU_add_suite("Member Sign-Up Suite", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    // Set up the files needed
    setup_test_environment();
    // Add test case
    CU_add_test(suite, "Test Member Sign-Up", test_member_signup);
    CU_add_test(suite, "Test Get Member Offset", test_get_member_offset);
    CU_add_test(suite, "Test Admin Sign-Up", test_admin_signup);
    CU_add_test(suite, "Test Get Admin Offset", test_get_admin_offset);
    CU_add_test(suite, "Test Member Log-In", test_member_login);
    CU_add_test(suite, "Test Admin Log-In", test_admin_login);
    CU_add_test(suite, "Test Reset Password", test_reset_password);
    CU_add_test(suite, "Test Get Book Offset", test_get_book_offset);
    CU_add_test(suite, "Test Add Book", test_add_book);
    CU_add_test(suite, "Test Modify Book", test_modify_book);
    CU_add_test(suite, "Test View Books", test_view_books);
    CU_add_test(suite, "Test Add Book to Cart", test_add_book_to_cart);
    CU_add_test(suite, "Test View Cart", test_view_cart);
    CU_add_test(suite, "Test Modify Cart", test_modify_cart);
    CU_add_test(suite, "Test Remove Book", test_remove_book);
    // Run tests
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Clean up the files
    cleanup_test_environment();
    // Clean up registry
    CU_cleanup_registry();
  
    return CU_get_error();
}
