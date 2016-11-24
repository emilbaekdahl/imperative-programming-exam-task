/*
  Purpose:      Handling and manuipulation of data in superliga-2015-2016
  Developed by: Emil S. Bækdahl
  Date:         November 24, 2016 
*/
#include <stdio.h>

// Type definintions
struct match {
  char round[3];
  char week_day[3];
  char date[10];
  char time[5];
  char team_home[3];
  char team_out[3];
  int team_home_score[1];
  int team_out_score[1];
  int audience[5];
};

typedef struct match match;

// Function prototypes

int main (void) {
  return 0;
}

