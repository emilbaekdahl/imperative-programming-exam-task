/*
  Purpose:      Handling and manuipulation of data in superliga-2015-2016
  Developed by: Emil S. Bækdahl
  Date:         November 24, 2016 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure definition */
struct score {
  int home;
  int out;
  int total;
};

struct goals {
  int home;
  int out;
  int total;
};

struct team {
  char    name[4];
  struct  goals goals;
  int     points;
  int     home_wins;
  int     out_wins;
};

struct date {
  int day;
  int month;
  int year;
};

struct time {
  int hours;
  int minutes;
};

struct match {
  int     round;
  char    week_day[4];
  struct  date date;
  struct  time time;
  char    team_home[4];
  char    team_out[4];
  struct  score score;
  int     audience;
};

/* Type definitions */
typedef struct match match;
typedef struct team team;
typedef struct score score;
typedef struct time time;
typedef struct goals goals;

/* Function prototypes */
void    read_matches                          (char* file_name, match* matches);
void    read_teams                            (match* matches, int number_of_matches, team* teams, int number_of_teams);
team*   read_teams_from_matches               (match* matches, int matches_in_round);
int     number_of_lines_in_file               (char* file_name);
int     convert_string_to_number              (char* string_with_separator, int string_length);
match*  tie_matches                           (match* matches, int number_of_matches);
void    round_with_less_than_10_goals         (match* matches, int number_of_matches, int* round_number, int* total_goals);
team*   teams_winning_out                     (match* matches, int number_of_matches, team* teams, int number_of_teams);
void    team_with_fewest_home_match_audience  (match* matches, int number_of_matches, char *team_name, int *audience);
match*  matches_in_time_frame                 (match* matches, int number_of_matches, char lower[6], char upper[6], char* week_day);
time    time_from_string                      (char time_string[6]);
int     is_match_in_time_frame                (match* match, time* lower, time* upper);
void    print_result                          (match* matches, int number_of_matches, team* teams, int number_of_teams);
int     team_compare                          (void* a, void* b);


/*
 * Main
 */
int main (void) {
  char *file_name = "superliga-2015-2016";
  
  int number_of_matches = number_of_lines_in_file(file_name);
  int number_of_teams = 12;

  struct match matches[number_of_matches];
  struct team teams[number_of_teams];

  read_matches(file_name, matches);
  read_teams(matches, number_of_matches, teams, number_of_teams);

  print_result(matches, number_of_matches, teams, number_of_teams);

  return 0;
}

/*
 * Read matches into array from file with name file_name
 */
void read_matches (char* file_name, match* matches) {
  FILE* file_handle = fopen(file_name, "r");
  char buffer[100];
  int matches_length = number_of_lines_in_file(file_name); 
  int i;
  char temp_audience[7];

  for (i = 0; i < matches_length; i++) {
    fgets(buffer, sizeof(buffer), file_handle);
    sscanf(buffer, 
           " R%d %s %d/%d/%d %d.%d %s - %s %d - %d %s",
           &matches[i].round,         matches[i].week_day,    &matches[i].date.day,
           &matches[i].date.month,    &matches[i].date.year,  &matches[i].time.hours,
           &matches[i].time.minutes,  matches[i].team_home,   matches[i].team_out,
           &matches[i].score.home,    &matches[i].score.out,  temp_audience
    );
    matches[i].score.total = matches[i].score.home + matches[i].score.out;
    matches[i].audience = convert_string_to_number(temp_audience, sizeof(temp_audience));
  }
  fclose(file_handle);
}

/*
 * Read teams into array based on matches
 */
void read_teams (match* matches, int number_of_matches, team* teams, int number_of_teams) {
  int i;
  int j = 0;
  struct team empty_team = { "", {0, 0, 0}, 0, 0, 0 };

  /* Create teams with names and zero data */
  for (i = 0; i < number_of_teams; i += 2) {
    teams[i] = empty_team;
    strcpy(teams[i].name, matches[j].team_home);

    teams[i + 1] = empty_team;
    strcpy(teams[i + 1].name, matches[j].team_out);
    j++;
  }

  /* Calculate teams' goals and points */
  for (i = 0; i < number_of_matches; i++) {
    for (j = 0; j < number_of_teams; j++) {
      /* Increase team's home and out score if in match */
      if (strcmp(teams[j].name, matches[i].team_home) == 0) {     /* Current team is home team in match */
        teams[j].goals.home += matches[i].score.home;
        teams[j].goals.total += matches[i].score.home;

        /* Increase home team's points if match won or tie */
        if (matches[i].score.home > matches[i].score.out) {
          teams[j].points += 3;
        }
        else if (matches[i].score.home == matches[i].score.out) {
          teams[j].points += 1;
        }
      }

      else if (strcmp(teams[j].name, matches[i].team_out) == 0) {   /* Current team i out team in match */
        teams[j].goals.out += matches[i].score.out;
        teams[j].goals.total += matches[i].score.out;

        /* Increase home team's points if match won or tie */
        if (matches[i].score.out > matches[i].score.home) {
          teams[j].points += 3;
        }
        else if (matches[i].score.out == matches[i].score.home) {
          teams[j].points += 1;
        }
      }
    }
  }
}

/*
 * Calculate number of lines in file with name file_name
 */
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

/*
 * Helper function for read_matches
 * Converts number in string format with seperators to int
 */
int convert_string_to_number (char* string_with_separator, int string_length) {
  int i;
  int j = 0;
  char string_without_separator[string_length];

  for (i = 0; i < string_length; i++) {
    if (string_with_separator[i] != '.') {
      string_without_separator[j] = string_with_separator[i];
      j++;
    }
  }

  return atoi(string_without_separator);
}

/*
 * Returns array of tie matches where the total score is larger than 4
 */
match* tie_matches (match * matches, int number_of_matches) {
  size_t match_size = sizeof(match);                      /* Size of one match struct */
  match* tie_matches;                                     /* Array of match struct allocated to one match */
  int i;                                                  /* Main loop counter */
  int tie_matches_counter = 0;                            /* Counter for tie matches */ 

  for (i = 0; i < number_of_matches; i++) {
    if (matches[i].score.home == matches[i].score.out) {  /* Home and out score are equal */
      if (matches[i].score.total >= 4) {                  /* Total score > 4 */
        tie_matches = realloc(tie_matches, match_size);   /* Allocate space for one match */
        tie_matches[tie_matches_counter] = matches[i];    /* Add current match to tie matches */
        tie_matches_counter++; 
      }
    }
  }

  return tie_matches;
}

/*
 * Find one round with a total score less than 10. Return with output parameters
 */
void round_with_less_than_10_goals (match* matches, int number_of_matches, int* round_number, int* total_goals) {
  int i;
  int round_counter = 1;
  int score_in_round = 0;

  for (i = 0; i < number_of_matches; i++) {
    if (matches[i].round == round_counter) {      /* Still in same round */
      score_in_round += matches[i].score.total;   /* Increase the round score with the match score */
    }
    else {
      if (score_in_round < 10) {
        *round_number = round_counter;
        *total_goals = score_in_round;
        break;
      }
      else {
        score_in_round = 0;                       /* Reset the round score */
        round_counter++;                          /* Increase round count */
      }
    }     
  }
}

/*
 * Return array of teams that wins more as out team that home team
 */
team* teams_winning_out (match* matches, int number_of_matches, team* teams, int number_of_teams) {
  int i;
  int j;
  int k = 0;

  size_t team_size = sizeof(team);
  team* teams_winning_out = malloc(1);

  for (i = 0; i < number_of_teams; i++) {
    teams[i].home_wins = 0;
    teams[i].out_wins = 0;
    for (j = 0; j < number_of_matches; j++) {
      if ((strcmp(matches[j].team_home, teams[i].name) == 0) && matches[j].score.home > matches[j].score.out) {
        teams[i].home_wins++;
      }
      else if ((strcmp(matches[j].team_out, teams[i].name) == 0) && matches[j].score.home < matches[j].score.out) {
        teams[i].out_wins++;
      }

    }
    if (teams[i].out_wins > teams[i].home_wins) {
      teams_winning_out = realloc(teams_winning_out, team_size);
      teams_winning_out[k] = teams[i];
      k++;
    }
  }

  return teams_winning_out;
}

/*
 * Returns home team and audience of match with fewest audience
 */
void team_with_fewest_home_match_audience (match* matches, int number_of_matches, char *team_name, int *audience) {
  int i;
  struct match fewest_audience_match = matches[0];

  for (i = 0; i < number_of_matches; i++) {
    if (matches[i].date.year == 2015) {
      if (matches[i].audience < fewest_audience_match.audience) {
        fewest_audience_match = matches[i];
      }
    }
  }

  strcpy(team_name, fewest_audience_match.team_home);
  *audience = fewest_audience_match.audience;
}

/*
 * Returns all matches within a given time frame
 */
match* matches_in_time_frame (match* matches, int number_of_matches, char lower[6], char upper[6], char* week_day) {
  int i;
  int j = 0;

  time time_lower = time_from_string(lower);
  time time_upper = time_from_string(upper); 

  size_t match_size = sizeof(match);
  match *matches_in_frame = malloc(0);

  for (i = 0; i < number_of_matches; i++) {
    if (strcmp(matches[i].week_day, week_day) == 0) {
      if (is_match_in_time_frame(&matches[i], &time_lower, &time_upper) == 1) {
        matches_in_frame = realloc(matches_in_frame, match_size);
        matches_in_frame[j] = matches[i];
        j++;
      }
    }
  }

  return matches_in_frame;
}

/*
 * Helper for matches_in_time_frame
 * Converts time string hh:mm format to time struct
 */
time time_from_string (char time_string[6]) {
  time new_time;
  sscanf(time_string, " %d:%d", &new_time.hours, &new_time.minutes);
  return new_time;
}

/*
 * Helper for matches_in_time_frame. 
 * Returns 1 if match is between lower and upper - otherwise 0.
 */
int is_match_in_time_frame (match* match, time* lower, time* upper) {
  if (match->time.hours > lower->hours && match->time.hours < upper->hours) { /* Match is in the hour time frame */
    return 1;
  }
  else if (match->time.hours == lower->hours) {                               /* Match hour is lower hour */
    if (match->time.minutes >= lower->minutes) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else if (match->time.hours == upper->hours) {                               /* Match hour is upper hour */
    if (match->time.minutes <= upper->minutes) {
      return 1;
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }
}

/*
 * Prints tabel of results
 */
void print_result (match* matches, int number_of_matches, team* teams, int number_of_teams) {
  int i, j;

  /* Sort teams */
  qsort(teams, number_of_teams, sizeof(team), team_compare);

  /* Print teams */
  printf("Team \t Points\n");
  for (i = 0; i < number_of_teams; i++) {
    printf("%-3s \t %d\n", teams[i].name, teams[i].points);
  }
}

/*
 * Helper function for print_result
 * Sorting decision for qsort based on teams' points
 */
int team_compare (void* a, void* b) {
  team* team_1 = (team*) a;
  team* team_2 = (team*) b;

  if (team_1->points > team_2->points) {
    return -1;
  }
  else if (team_1->points < team_2->points) {
    return 1;
  }
  else {
    return 0;
  }
}
