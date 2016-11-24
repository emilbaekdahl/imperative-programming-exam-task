/*
  Purpose:      Handling and manuipulation of data in superliga-2015-2016
  Developed by: Emil S. Bækdahl
  Date:         November 24, 2016 
*/

// Libraries to include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type definintions
struct match {
  char round[3];
  char week_day[3];
  char date[10];
  char time[5];
  char team_home[3];
  char team_out[3];
  int team_home_score;
  int team_out_score;
  char audience[5];
};

typedef struct match match;

// Function prototypes
struct match create_match (char* round, char* week_day, char* date, char* time, char* team_home, char* team_out, int team_home_score, int team_out_score, char* audience);
int number_of_lines_in_file (char* file_name);
int remove_seperator_from_numer (char* number_with_seperator);

// Main
int main (void) {
  char *file_name = "superliga-2015-2016";

  

  FILE* fp = fopen(file_name, "r");
  char file_line[100];

  // Variables for sscanf
  char round[3];
  char week_day[3];
  char date[10];
  char time[5];
  char team_home[3];
  char team_out[3];
  int team_home_score;
  int team_out_score;
  char audience[5];

  struct match new_match;

  // while (fgets(file_line, sizeof(file_line), fp)) {
  //   // sscanf(file_line, " %s %s %s %s %s - %s %d - %d %s", round, week_day, date, time, team_home, team_out, &team_home_score, &team_out_score, audience);
  //   // new_match = create_match(round, week_day, date, time, team_home, team_out, team_home_score, team_out_score, audience);
  // }

  printf("%d\n", number_of_lines_in_file(file_name));

  return 0;
}

int number_of_lines_in_file (char* file_name) {
  FILE *fp = fopen(file_name, "r");

  int lines = 0;
  int chr;

  while ((chr = fgetc(fp)) != EOF) {
    if (chr == '\n') {
      lines++;
    }
  }

  fclose(fp);
  return lines + 1;
}

struct match create_match (char* round, char* week_day, char* date, char* time, char* team_home, char* team_out, int team_home_score, int team_out_score, char* audience) {
  struct match new_match = { *round, *week_day, *date, *time, *team_home, *team_out, team_home_score, team_out_score, *audience };
  return new_match;
}

int remove_seperator_from_numer (char* number_with_seperator) {
  return 0;
}