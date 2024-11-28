#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Mutex declaration (if shared across multiple functions)
extern pthread_mutex_t mutex3;
extern pthread_mutex_t mutex4;

// Function prototypes
void memberSignUp(int new_fd);
void adminSignUp(int new_fd);
void memberLogIn(int new_fd);
void adminLogIn(int new_fd);
void resetPassword(int new_fd,struct member *update);
int getMemberOffset(int memberId, int fd);
int getAdminOffset(int adminId, int fd);
int getBookOffset(int bookId, int fd);
void add_book(int new_fd);
void modify_book(int new_fd);
void remove_book(int new_fd);
void view_books(int new_fd);
int find_smallest_member_id();
int find_smallest_admin_id();
void setwritelock(int fd, struct flock* lock);
void unlock(int fd, struct flock* lock);
void add_book_to_cart(int new_fd,int id);
void view_cart(int new_fd,int id);
void modify_cart(int new_fd,int id);
void borrow_book(int new_fd,int id);
void view_borrowed(int new_fd,int id);
void return_book(int new_fd,int id);

#endif
