#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ROW 10
#define COLUMN 20
#define RES_LEN 4
#define SAFE_DIST 3

typedef struct {
    bool full;
    char ID;
    int seats[COLUMN];
} row_info;

typedef struct {
    row_info rows[ROW];
} theater_obj;

typedef struct {
    char number[RES_LEN + 1];
    int count;
    int **saved_seats;
} res_info;

typedef struct {
    res_info *res_list;
} reservations;

/* Initialize theater struct */
void init_theater(theater_obj *theater) {
    char ID = 'A';
    for(int i=0; i < ROW; i++) {
        row_info cur_row;
        cur_row.full = false;
        cur_row.ID = ID;
        memset(cur_row.seats, 0, sizeof(cur_row.seats));
        theater->rows[i] = cur_row;
        ID += 1;
    }
}

/* Assign seats to current reservation holder */
void assign_seats(theater_obj *theater, res_info *cur_res, int row_ID, int seat_ID, int seat_count) {
    int seat_counter = 0;

    printf("\n\nSeats for Reservation %s:\n", cur_res->number);
    for(int i=seat_ID; i < (seat_ID + seat_count) + SAFE_DIST; i++) {
        // Ensure no one can sit within 3 seats of reservations
        theater->rows[row_ID].seats[i] = 1;

        // Keep track of reserved seats
        if (i < (seat_ID + seat_count)) {
            cur_res->saved_seats[seat_counter][0] = (int)theater->rows[row_ID].ID;
            cur_res->saved_seats[seat_counter][1] = i;
            printf("%c %d\n", cur_res->saved_seats[seat_counter][0], cur_res->saved_seats[seat_counter][1]);
            seat_counter++;
        }

        // Keep track of if row is full
        if (i == COLUMN-1)
            theater->rows[row_ID].full = true;
    }
}

/* Find free seats for current reservation holder */
void find_seats(theater_obj *theater, res_info *cur_res) {
    int seat_count, k, row_ID, seat_ID;
    bool flag;

    // Initialize number of seats needed
    seat_count = cur_res->count;

    // Start with Row A and check up to J
    for(int i=0; i < ROW; i++) {
        if (theater->rows[i].full)
            continue;

        // Iterate through row
        for(int j=0; j < (COLUMN - seat_count); j++) {
            flag = true;
            k = 0;

            while(k < seat_count && flag == true) {
                if (theater->rows[i].seats[j + k])
                    flag = false;
                k++;
            }

            if (flag) {
                row_ID = i;
                seat_ID = j;
                assign_seats(theater, cur_res, row_ID, seat_ID, seat_count);
                return;
            }
        }
    }

}

/* Count the total number of reservations to be accounted for */
int count_lines(FILE *fp) {
    int count = 0;
    char c;

    while (!feof(fp)) {
        c = getc(fp);
        if (c == '\n')
            count += 1;
    }

    return count;
}


int main(__attribute__((unused))int argc, char **argv) {
    int num_lines;
    char *filename = argv[1];
    FILE *fp;
    reservations res_struct;
    theater_obj theater;

    fp = fopen(filename, "r");
    if (fp == 0)
    {
        perror("Invalid input file\n");
        exit(-1);
    }

    num_lines = count_lines(fp);
    res_struct.res_list = malloc(sizeof(res_info) * num_lines);
    fseek(fp, 0, SEEK_SET);

    // Establish struct that holds relevant info about reservations
    for(int i=0; i < num_lines; i++) {
        res_info res_obj;
        fscanf(fp, "%s", res_obj.number);
        fscanf(fp, "%d", &res_obj.count);
        res_obj.saved_seats = malloc(sizeof(int*) * res_obj.count);
        for(int j=0; j < res_obj.count; j++)
            res_obj.saved_seats[j] = malloc(sizeof(int) * 2);
        res_struct.res_list[i] = res_obj;
    }

    // Assign seats and update changes in theater struct
    init_theater(&theater);
    for(int i=0; i < num_lines; i++) {
        find_seats(&theater, &res_struct.res_list[i]);
    }

    fclose(fp);
    printf("\nRESERVATIONS COMPLETE. PROGRAM TERMINATED.");
    return 0;
}
