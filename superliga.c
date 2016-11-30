/* =========================================================================== */
/* | Purpose:      Handling and manuipulation of data in superliga-2015-2016 | */
/* | Developed by: Emil S. Bækdahl                                           | */
/* | Finish date:  20/11/2016                                                | */
/* =========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_MATCHES 198
#define NUMBER_OF_TEAMS 12

/* ========================= */
/* | Structure definitions | */
/* ========================= */
typedef struct date {
  int day;
  int month;
  int year;
} date;

typedef struct time {
  int hours;
  int minutes;
} time;

typedef struct team {
  char name[4];
  struct { 
    int home;
    int out; 
    int total; 
    int fore; 
    int against; 
    int difference;
  } goals;
  struct {
    struct {
      int home; 
      int out; 
      int total;
    } wins; 
    int old_wins; 
    int loses; 
    int ties; 
    int total;
  } matches;
  int points;
  int total_home_audience;
} team;

typedef struct match {
  int round;
  char week_day[4];
  struct date date;
  struct time time;
  char team_home[4];
  char team_out[4];
  struct { 
    int home; 
    int out; 
    int total;
  } score;
  int audience;
} match;

/* Array types for keeping track of size and content of dynamic arrays */
typedef struct team_array {
  int size;
  team* teams;
} team_array;

typedef struct match_array {
  int size;
  match* matches;
} match_array;

/* =================== */
/* Function prototypes */
/* =================== */

/* Logic functions */
void read_matches (char* file_name, match* matches);
void read_teams (match* matches, team* teams);
match_array tie_matches (match* matches);
int round_with_less_than_10_goals (match* matches, int* round_number, int* total_goals);
team_array teams_winning_out (team* teams);
void team_with_fewest_home_match_audience (match* matches, team* teams, char *team_name, int *audience);
match_array matches_in_time_frame (match* matches, char lower[6], char upper[6], char* week_day);

/* Functions for tasks */
void print_tie_matches (match* matches);
void print_round_width_less_than_10_goals (match* matches);
void print_teams_winning_out (match* matches, team* teams);
void print_team_with_fewest_home_match_audience (match* matches, team* teams);
void print_matches_in_time_frame (match* matches, char* start, char* end, char* week_day);
void print_team_winning_out (team_array* teams);
void print_result (match* matches, team* teams);

/* Helper functions */
int convert_string_to_number (char* string_with_separator, int string_length);
int is_match_in_time_frame (match* match, time* lower, time* upper);
time time_from_string (char time_string[6]);
int team_sort_by_points (team* team_1, team* team_2);
int team_sort_by_goal_difference (team* team_1, team* team_2);
int team_sort_by_total_goals (team* team_1, team* team_2);
int team_sort_by_name (team* team_1, team* team_2);
int team_compare (const void* a, const void* b);

/* Print helpers */
void print_menu ();
void print_matches (match_array* matches);
void print_round (match* matches, int round_number);
void print_teams (team_array* teams);

/* ======== */
/* | Main | */
/* ======== */
int main (int argc, char* argv[]) {
  int i, selector;
  char *file_name = "superliga-2015-2016";
  
  /* Init and load matches and teams */
  struct match matches[NUMBER_OF_MATCHES];
  struct team teams[NUMBER_OF_TEAMS];

  read_matches(file_name, matches);
  read_teams(matches, teams);

  /* Print everything to stdio if --print is passed */
  for (i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--print") == 0) {
      printf("TIE MATCHES WITH MORE THAN 4 TOTAL GOALS\n");
      print_tie_matches(matches);

      printf("ROUND WITH LESS THAN 10 TOTAL GOALS\n");
      print_round_width_less_than_10_goals(matches);

      printf("TEAM WINNING MORE MATCHES OUT THAN HOME\n");
      print_teams_winning_out(matches, teams);

      printf("TEAM WITH FEWEST TOTAL HOME MATCH AUDIENCE IN 2015\n");
      print_team_with_fewest_home_match_audience(matches, teams);

      printf("ALL MATCHES BETWEEN 13:15 AND 14:15 ON A SUNDAY");
      print_matches_in_time_frame(matches, "13:15", "14:15", "Son");

      print_result(matches, teams);
      return 0;
    }
  }

  /* Dialog */ 
  while (1) {
    print_menu();
    scanf(" %d", &selector);

    /* Perform action based on input */
    if (selector == 0) {
      return 0;
    }
    else if (selector == 1) {
      print_tie_matches(matches);
    }
    else if (selector == 2) {
      print_round_width_less_than_10_goals(matches);
    }
    else if (selector == 3) {
      print_teams_winning_out(matches, teams);
    }
    else if (selector == 4) {
      print_team_with_fewest_home_match_audience(matches, teams);
    }
    else if (selector == 5) {
      char start[6]; char end[6]; char week_day[4];
      printf("Enter start time (hh:mm), end time (hh:mm) and day (ex. Mon, Son) (sepereated by space): ");
      scanf(" %s %s %s", start, end, week_day);
      print_matches_in_time_frame(matches, start, end, week_day);
    }
    else if (selector == 6) {
      print_result(matches, teams);
    }
  }

  return 0;
}

/*
 * Priting the main menu for user input
 */
void print_menu () {
  printf("\n"
         "====================================================================\n"
         "|                     SUPERLIGA 2015-2016 MENU                     |\n"
         "====================================================================\n");
  printf("| (0) | End program                                                |\n"
         "| (1) | List tie matches with more than 4 goals scored             |\n"
         "| (2) | List round with less than 10 goals                         |\n");
  printf("| (3) | List teams winning more matches out than home              |\n"
         "| (4) | Find the team with fewest audience at home matches in 2015 |\n"
         "| (5) | List matches in specific time frame                        |\n"
         "| (6) | Display Superliga 2015-2016 result                         |\n"
         "====================================================================\n"
         "\nEnter your choice (0-6) here: ");
}

/*
 * Read matches into array from file with name file_name
 */
void read_matches (char* file_name, match* matches) {
  FILE* file_handle = fopen(file_name, "r");
  char buffer[100];
  char temp_audience[7];
  int i;

  /* Loop lines in file and assign to matches */
  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
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
 * Helper function for read_matches
 * Converts number in string format with seperators to int
 */
int convert_string_to_number (char* string_with_separator, int string_length) {
  int i;
  int j = 0;
  char string_without_separator[10];

  /* Loop chars of original string and 
     copy to new string unless char is '.' */
  for (i = 0; i < 10; i++) {
    if (string_with_separator[i] != '.') {
      string_without_separator[j] = string_with_separator[i];
      j++;
    }
  }

  /* Return string as int */
  return atoi(string_without_separator);
}

/*
 * Read teams into array based on matches
 */
void read_teams (match* matches, team* teams) {
  int i;
  int j = 0;

  /* Default team with all zeros.
     Prevents failure in later increments */
  struct team zero_team;
  memset(&zero_team, 0, sizeof(zero_team));

  /* Create all teams with names and zero data */
  for (i = 0; i < NUMBER_OF_TEAMS; i += 2) {
    teams[i] = zero_team;
    strcpy(teams[i].name, matches[j].team_home);
    teams[i + 1] = zero_team;
    strcpy(teams[i + 1].name, matches[j].team_out);
    j++;
  }

  /* Calculate teams' goals and points */
  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
    for (j = 0; j < NUMBER_OF_TEAMS; j++) {
      /* Current team is home team in match */
      if (strcmp(teams[j].name, matches[i].team_home) == 0) {
        /* Increase total and home scored goals */
        teams[j].goals.home   += matches[i].score.home;
        teams[j].goals.total  += matches[i].score.home;

        /* Increase goals scored for and against the team */
        teams[j].goals.fore     += matches[i].score.home;
        teams[j].goals.against  += matches[i].score.out;

        /* Subtract and add to the team's goal difference */
        teams[j].goals.difference -= matches[i].score.out;
        teams[j].goals.difference += matches[i].score.home;

        /* Increae team's total played matches */
        teams[j].matches.total++;

        /* Increase team's points based on match result */
        if (matches[i].score.home > matches[i].score.out) {       /* Win */
          teams[j].points += 3;
          teams[j].matches.wins.home++;
          teams[j].matches.wins.total++;
        }
        else if (matches[i].score.home == matches[i].score.out) { /* Tie */
          teams[j].points += 1;
          teams[j].matches.ties++;
        } 
        else {                                                    /* Loose */
          teams[j].matches.loses++;
        }
      }

      /* Current team is out team in match */
      else if (strcmp(teams[j].name, matches[i].team_out) == 0) {
        /* Increase goals score for and against the team */
        teams[j].goals.out   += matches[i].score.out;
        teams[j].goals.total += matches[i].score.out;

        /* Increase goals scored for and against the team */
        teams[j].goals.fore    += matches[i].score.out;
        teams[j].goals.against += matches[i].score.home;

        /* Subtract and add to the goals difference */
        teams[j].goals.difference -= matches[i].score.home;
        teams[j].goals.difference += matches[i].score.out;

        /* Increae team's total played matches */
        teams[j].matches.total++;

        /* Increase home team's points based on match result */
        if (matches[i].score.out > matches[i].score.home) {       /* Win */
          teams[j].points += 3;
          teams[j].matches.wins.out++;
          teams[j].matches.wins.total++;
        }
        else if (matches[i].score.out == matches[i].score.home) { /* Tie */
          teams[j].points += 1;
          teams[j].matches.ties++;
        }
        else {                                                    /* Loose */
          teams[j].matches.loses++;
        }
      }
    }
  }
}

/*
 * Returns array of tie matches where the total score is 4 or more
 */
match_array tie_matches (match* matches) {
  static match_array tie_matches;
  int i;
  int ties_counter = 0;

  /* Declare size and space for array */
  tie_matches.size = 0; tie_matches.matches = malloc(0);

  /* Loops matches */
  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
    if (matches[i].score.home == matches[i].score.out) { 
      if (matches[i].score.total >= 4) {
        /* Reallocate tie_matches to house one more match,
           add it to the array and increase its size */
        tie_matches.matches = realloc(tie_matches.matches, (ties_counter + 1) * sizeof(match));
        tie_matches.matches[ties_counter] = matches[i];
        ties_counter++; 
        tie_matches.size++;
      }
    }
  }

  return tie_matches;
}

/*
 * Find one round with a total score less than 10.
 * Return 1 if it exists and output parameters.
 */
int round_with_less_than_10_goals (match* matches, int* round_number, int* total_goals) {
  int i;
  int round_exists = 0;
  int round_counter = 1;
  int score_in_round = 0;

  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
    /* Add match's total score to score_in_score 
       while in same round */
    if (matches[i].round == round_counter) {
      score_in_round += matches[i].score.total;
    }
    /* Set output parameters and break
       if current rounds score is less than 10
       else reset counter and try with next round */
    else {
      if (score_in_round < 10) {
        *round_number = round_counter;
        *total_goals = score_in_round;
        round_exists = 1;
        break;
      }
      else {
        score_in_round = 0;
        round_counter++;
      }
    }     
  }

  return round_exists;
}

/*
 * Return array of teams that wins more as out team that home team
 */
team_array teams_winning_out (team* teams) {
  static team_array teams_winning_out; 
  int i;
  int k = 0;

  /* Declare size and space for array */
  teams_winning_out.size = 0; teams_winning_out.teams = malloc(0);


  for (i = 0; i < NUMBER_OF_TEAMS; i++) {
    if (teams[i].matches.wins.out > teams[i].matches.wins.home) {
      /* Reallocate teams_winning_out to house one more team,
        add the current team and increase size of array*/
      teams_winning_out.teams = realloc(teams_winning_out.teams, (k + 1) * sizeof(team));
      teams_winning_out.teams[k] = teams[i];
      teams_winning_out.size++;
      k++;
    }
  }

  return teams_winning_out;
}

/*
 * Returns home team and audience of match with fewest audience
 */
void team_with_fewest_home_match_audience (match* matches, team* teams, char *team_name, int *audience) {
  int i;
  int j;
  team team_with_fewest_home_match_audience;

  /* With all 2015 matches count all teams' home audience */
  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
    if (matches[i].date.year == 2015) {
      for (j = 0; j < NUMBER_OF_TEAMS; j++) {
        if (strcmp(teams[j].name, matches[i].team_home) == 0) {
          teams[j].total_home_audience += matches[i].audience;
        }
      }
    }
  }

  /* Set team to have fewest home audience.
     For later comparison */
  team_with_fewest_home_match_audience = teams[0];

  /* Set current team to have fewest audience
     if the number if smaller than the previous */
  for (i = 0; i < NUMBER_OF_TEAMS; i++) {
    if (teams[i].total_home_audience < team_with_fewest_home_match_audience.total_home_audience) {
      team_with_fewest_home_match_audience = teams[i];
    }
  }

  /* Set output parameters to house result */
  strcpy(team_name, team_with_fewest_home_match_audience.name);
  *audience = team_with_fewest_home_match_audience.total_home_audience;
}

/*
 * Returns all matches within a given time frame
 */
match_array matches_in_time_frame (match* matches, char lower[6], char upper[6], char* week_day) {
  int i;
  int j = 0;

  /* Convert time strings to structs */
  time time_lower = time_from_string(lower);
  time time_upper = time_from_string(upper); 

  static match_array matches_in_time_frame;

  /* Declare size and space for array */
  matches_in_time_frame.size = 0; matches_in_time_frame.matches = malloc(0);

  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
    if (strcmp(matches[i].week_day, week_day) == 0) {
      if (is_match_in_time_frame(&matches[i], &time_lower, &time_upper) == 1) {
        /* Reallocate matches_in_time_frame to house one more item,
           add it to the array and increase the array's size*/
        matches_in_time_frame.matches = realloc(matches_in_time_frame.matches, (j + 1) * sizeof(match));
        matches_in_time_frame.matches[j] = matches[i];
        matches_in_time_frame.size++;
        j++;
      }
    }
  }

  return matches_in_time_frame;
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
  /* First, compare the hour of the match's time
     to the hours of the given time frame */
  if (match->time.hours > lower->hours && match->time.hours < upper->hours) {
    return 1;
  }
  /* If the hour of the match's time is equal
     to the hour of the time frame then compare minutes  */
  else if (match->time.hours == lower->hours) {
    return match->time.minutes >= lower->minutes;
  }
  else if (match->time.hours == upper->hours) {
    return match->time.minutes <= upper->minutes;
  }
  else {
    return 0;
  }
}

/*
 * Helper functiond for printing array of matches
 */
void print_matches (match_array* matches) {
  int i;

  /* Table header */
  printf("===================================================================\n"
         "| ROUND | DAY | DATE       | TIME  | TEAMS     | GOALS | AUDIENCE |\n"
         "===================================================================\n");

  /* Table body */
  for (i = 0; i < matches->size; i++) {
    printf("| %-5d | %-3s | %.2d/%.2d/%d | %.2d:%.2d | %3s - %-3s | %d - %d | %-8d |\n",
           matches->matches[i].round,        matches->matches[i].week_day,  matches->matches[i].date.day,
           matches->matches[i].date.month,   matches->matches[i].date.year, matches->matches[i].time.hours,
           matches->matches[i].time.minutes, matches->matches[i].team_home, matches->matches[i].team_out,
           matches->matches[i].score.home,   matches->matches[i].score.out, matches->matches[i].audience);
  }
  printf("===================================================================\n");
}

/*
 * Helper function for printing a round
 */
void print_round (match* matches, int round_number) {
  match_array matches_in_round;
  int i;
  int j = 0;

  /* Declare size and space for array */
  matches_in_round.size = 0; matches_in_round.matches = malloc(0);

  for (i = 0; i < NUMBER_OF_MATCHES; i++) {
    if (matches[i].round == round_number) {
      /* Reallocate array for one more element,
         add it and increase its size */
      matches_in_round.matches = realloc(matches_in_round.matches, (j + 1) * sizeof(match));
      matches_in_round.matches[j] = matches[i];
      matches_in_round.size++;
      j++;
    }
  }

  print_matches(&matches_in_round);
}

/*
 * Helper functiond for printing array of matches
 */
void print_teams (team_array* teams) {
  int i;
  /* Table header */
  printf("===============================\n"
         "| TEAM | WINS OUT | WINS HOME |\n"
         "===============================\n");

  /* Table body */
  for (i = 0; i < teams->size; i++) {
    printf("| %-4s | %-8d | %-9d |\n", teams->teams[i].name, teams->teams[i].matches.wins.out, teams->teams[i].matches.wins.home);
  }
  printf("===============================\n");
}

/*
 * Task 1: Print tie matches with more than 4 goals total
 */
void print_tie_matches (match* matches) {
  match_array sub_matches_ties = tie_matches(matches);
  print_matches(&sub_matches_ties);
}

/*
 * Taks 2: print round with less that 10 goals total
 */
void print_round_width_less_than_10_goals (match* matches) {
  int round;
  int goals;
  /* Print if the round exists - otherwise print error */
  if (round_with_less_than_10_goals(matches, &round, &goals) == 1) {
    printf("Round %d had %d goals\n", round, goals);
    print_round(matches, round);
  }
  else {
    printf("Round with less than 10 goals scored does not exist");
  }
}

/*
 * Task 3: Print teams winning more mathces out than home
 */
void print_teams_winning_out(match* matches, team* teams) {
  team_array sub_teams_winning_out = teams_winning_out(teams);
  print_teams(&sub_teams_winning_out);
}

/*
 * Task 4: Print team with fewest total audience at home matches in 2015
 */
void print_team_with_fewest_home_match_audience (match* matches, team* teams) {
  char team_name[4];
  int audience;
  team_with_fewest_home_match_audience(matches, teams, team_name, &audience);
  printf("%s had the fewest total audience at home matches in 2015: %d\n", team_name, audience);
}

/*
 * Task 5: Print matches at a given day in a given time frame
 */
void print_matches_in_time_frame (match* matches, char* start, char* end, char* week_day) {
  match_array sub_matches_in_time_frame = matches_in_time_frame(matches, start, end, week_day);
  print_matches(&sub_matches_in_time_frame);
}

/*
 * Task 5: Print tabel of results
 */
void print_result (match* matches, team* teams) {
  int i;

  /* Sort teams with helper function */
  qsort(teams, NUMBER_OF_TEAMS, sizeof(team), team_compare);

  /* Table header */
  printf("====================================================================================\n"
         "|                                      RESULT                                      |\n"
         "====================================================================================\n"
         "| POS | TEAM | POINTS | MATCHES | WINS | LOSES | TIES | GOALS+ | GOALS- | GOALS+/- |\n"
         "====================================================================================\n");

  /* Table body */
  for (i = 0; i < NUMBER_OF_TEAMS; i++) {
    printf("| %-4d| %-5s| %-7d| %-8d| %-5d| %-6d| %-5d| %-7d| %-7d| %-9d|\n", 
            i + 1, teams[i].name,   teams[i].points,        teams[i].matches.total, 
            teams[i].matches.wins.total,  teams[i].matches.loses, teams[i].matches.ties,
            teams[i].goals.fore,    teams[i].goals.against, teams[i].goals.fore - teams[i].goals.against);
  }
  printf("====================================================================================\n");
}

/*
 * Helper function for print_result
 * Sorting decision for qsort based on teams' points
 */
int team_compare (const void* a, const void* b) {
  team* team_1 = (team*) a;
  team* team_2 = (team*) b;

  return team_sort_by_points(team_1, team_2);
}

/*
 * Helper function for team_compare. Compares teams' points
 */
int team_sort_by_points (team* team_1, team* team_2) {
  if (team_1->points > team_2->points) {
    return -1;
  }
  else if (team_1->points < team_2->points) {
    return 1;
  }
  else {
    return team_sort_by_goal_difference(team_1, team_2);
  }
}

/*
 * Helper function for team_compare. Compares teams' goal difference
 */
int team_sort_by_goal_difference (team* team_1, team* team_2) {
  if (team_1->goals.difference > team_2->goals.difference) {
    return -1;
  }
  else if (team_1->goals.difference < team_2->goals.difference) {
    return 1;
  }
  else {
    return team_sort_by_total_goals (team_1, team_2);
  }
}

/*
 * Helper function for team_compare. Compares teams' total goals
 */
int team_sort_by_total_goals (team* team_1, team* team_2) {
  if (team_1->goals.total > team_2->goals.total) {
    return -1;
  }
  else if (team_1->goals.total < team_2->goals.total) {
    return 1;
  }
  else {
    return team_sort_by_name (team_1, team_2);
  }
}

/*
 * Helper function for team_compare. Compares teams' name
 */
int team_sort_by_name (team* team_1, team* team_2) {
  return strcmp(team_1->name, team_2->name);
}
