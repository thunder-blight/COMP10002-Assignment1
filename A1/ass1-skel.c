/* Program to carry out a heuristic search in text input, looking for
   and scoring matches in each line against the strings provided on
   the commandline.

   Skeleton written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2026, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Students: You need to have read all of Chapter 7 and before making
   a start on this activity.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial
   skeleton file the program contained in this submission is my
   own work, except where explicitly noted below in connection with
   the use of AI.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or
   payment, may not be submitted for assessment in this subject.

   (2) I also certify that I have not provided a copy of this work
   in either softcopy or hardcopy or any other form to any other
   student, and nor will I do so until after the marks are released.
   I understand that providing my work to other students, regardless
   of my intention or any undertakings made to me by that other
   student, is also Academic Misconduct.

   (3) I declare that I have used generative AI tools in connection
   with the following aspects of the assignment (please select the
   option that applies best in your case and delete the other three
   choices):

   -- I made zero use of AI, and the program is wholly the result
   my own knowledge and development.

   (4) The AI system(s) that I used are:

   "None"

   (5) I understand that submitting for assessment work developed
   in collaboration with other people constitutes Academic Misconduct,
   and may be penalized by mark deductions, or by other penalties
   determined via the University of Melbourne Academic Honesty
   Policy, as described at https://academicintegrity.unimelb.edu.au.

   Signed by: ****** ****** *******
   Dated:     --/09/2026 

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

/**********************************************************************/

// this constant controls whether or not extra output is generated,
// make sure is set to 0 for your final submission
#define DEBUG 0

#define MAX_LINES 999           // max number of input lines
#define MAX_CHARS  80           // max number of characters per line

#define TOP_SCORES  5           // number of top-scoring lines to keep
#define MAX_MATCH  10           // maximum match length permitted

// add in any further #defines that you want here

/**********************************************************************/

typedef char line_t[MAX_CHARS+1];

// add in any further typedefs that you want here

/**********************************************************************/

int     read_one_line(line_t line, int max);
int     fill_lines(line_t lines[], int lens[]);
double  average_length(int lens[], int nlines);
void    print_line_info(int index, line_t lines[], int lens[]);
void    print_stage1(line_t lines[], int lens[], int nlines);
int     prefix_match_len(const char *line_pos, const char *term);
int     score_line(line_t line, int len, char *terms[], int nterms);
void    score_all_lines(line_t lines[], int lens[], int nlines,
                char *terms[], int nterms, int scores[]);
void    print_score_info(int index, line_t lines[], int scores[]);
void    print_stage2(line_t lines[], int scores[], int nlines);
int	    find_best_unused(int scores[], int nlines, int used[]);
void    print_stage3(line_t lines[], int scores[], int nlines);
void    print_stage(int stg);
void    tadaa(void);

// add in any further function prototypes that you want here

/**********************************************************************/

int
main(int argc, char *argv[]) {
	
	// you have to write the body of the main function, but don't
	// make it too long, it should control the traffic flow and use
	// functions to do the actual work

	line_t	lines[MAX_LINES];
	int		lens[MAX_LINES];
	int		scores[MAX_LINES];
	int		nlines;
	char	**terms;
	int		nterms;

	// this first loop is here to show you how to access the strings
	// provided on the commandline, and will only generate output
	// when DEBUG is non-zero
	if (DEBUG) {
		for (int argnum = 1; argnum < argc; argnum++) {
			printf("argv[%d]: %s\n", argnum, argv[argnum]);
		}
		printf("\n");

	}

	// the query terms are simply the commandline arguments after the
	// program name
	terms = argv + 1;
	nterms = argc - 1;

	// read all of the input lines into the arrays, stopping early if
	// MAX_LINES is reached, any further input is silently discarded
	// as permitted by the assignment specification
	nlines = fill_lines(lines, lens);

	// stage 1: report on the array of lines that has been built
	print_stage1(lines, lens, nlines);

	// stage 2: score every stored line against the query terms, and
	// report the scores for the first and last lines
	score_all_lines(lines, lens, nlines, terms, nterms, scores);
	print_stage2(lines, scores, nlines);

	// stage 3: report the TOP_SCORES best-scoring lines, highest
	// score first, ties broken by the smaller line number
	print_stage3(lines, scores, nlines);

	// and at the end, a traditional comp10002 sign-off...
	tadaa();

	return 0;
}

/**********************************************************************/

// here are a couple of free gifts
//

void
print_stage(int stg) {
	printf("\n");
	printf("Stage %d\n", stg);
	printf("-------\n");
}

void
tadaa(void) {
	printf("\n");
	printf("tadaa!\n");
}

/**********************************************************************/

// function to read one line from stdin into character array, see Section
// 7.8 and 7.9 of the textbook to understand how this works
// returns either EOF or the length of the string placed into line[], up
// to a maximum of "max" characters

int
read_one_line(line_t line, int max) {

	int len = 0, c;

	// read input characters one by one
	while ( (c = getchar()) != EOF) {

		if (c == '\n') {
			// newline marks end of line, stop here
			line[len] = '\0';
			return len;
		}

		// check to see if still safe to store character
		if (len < max) {
			// yes, still safe
			line[len] = c;
			len += 1;
		} else {
			// nope, have reached maximum, silently discard
		}

	}

	// if here, have reached EOF, but could still be a partial line
	if (len > 0) {
		// yes, there is, close it off and send it back
		line[len] = '\0';
		return len;
	} else {
		// nah, we really are at the end of the input
		return EOF;
	}

}

/**********************************************************************/

// read lines from the stdin into the "lines" array, one per element, and
// records each line's length in the parallel "lens" array, stops at EOF
// or when MAX_LINES is reached, whichever comes first and returns the 
// number of lines actually stored

int
fill_lines(line_t lines[], int lens[]) {

	int nlines = 0, len;

	while (nlines < MAX_LINES && 
			(len = read_one_line(lines[nlines], MAX_CHARS)) != EOF) {
		
		lens[nlines] = len;
		
		// this printf is for debugging purposes only
		if (DEBUG) {
			printf("%3d: %s\n", nlines, lines[nlines]);
		}

		nlines += 1;
	}

	return nlines;
}

/**********************************************************************/

// returns the average length, in characters, of the first "nlines"
// entries of the "lens" array

double
average_length(int lens[], int nlines) {

	int i, total = 0;

	for (i = 0; i < nlines; i++) {
		total += lens[i];
	}

	return (double)total / nlines;
}

/**********************************************************************/

// prints the standard "line N: -> text -> length = N" block for a
// single line, given its index into the "lines"/"lens" arrays

void
print_line_info(int index, line_t lines[], int lens[]) {
	printf("line %3d:\n", index);
	printf("-> %s\n", lines[index]);
	printf("-> length = %3d\n", lens[index]);
}

/**********************************************************************/

// prints the Stage 1 output: the average line length, followed by
// the details of the first and last stored lines

void
print_stage1(line_t lines[], int lens[], int nlines) {

	print_stage(1);
	printf("average line = %.2f characters\n", average_length(lens, nlines));
	print_line_info(0, lines, lens);
	print_line_info(nlines-1, lines, lens);
}

/**********************************************************************/

// returns the length of the longest case-insensitive prefix shared by
// the text starting at "line_pos" and the string "term", stops at the
// first mismatch, or at the end of whichever string is shorter

int
prefix_match_len(const char *line_pos, const char *term) {

	int k = 0;

	while (line_pos[k] != '\0' && term[k] != '\0' &&
			tolower((unsigned char)line_pos[k]) == 
			tolower((unsigned char)term[k])) {
			
		k++;
	}

	return k;
}

/**********************************************************************/

// computes the score for one line of length "len" against the set of 
// "nterms" query terms, following the scoring rules from the spec: at
// every character position, find the longest prefix match against any
// term (capped at MAX_MATCH), and sum (2^match - 1) over all positions

int
score_line(line_t line, int len, char *terms[], int nterms) {

	int i, j, match, best, score = 0;

	for (i = 0; i < len; i++) {

		best = 0;
		for (j = 0; j < nterms; j++) {
			match = prefix_match_len(line + i, terms[j]);
			if (match > best) {
				best = match;
			}
		}
		if (best > MAX_MATCH) {
			best = MAX_MATCH;
		}

		score += (1 << best) - 1;
	}

	return score;
}

/**********************************************************************/

// fills in the "scores" array with the score of every one of the
// first "nlines" entries of "lines", against the "nterms" query terms

void
score_all_lines(line_t lines[], int lens[], int nlines,
				char *terms[], int nterms, int scores[]) {
		
	int i;

	for (i = 0; i < nlines; i++) {
		scores[i] = score_line(lines[i], lens[i], terms, nterms);
	}
}

/**********************************************************************/

// prints the standard "line N: -> text -> score = N" block for a
// single line, given its index into the "lines"/"scores" arrays

void
print_score_info(int index, line_t lines[], int scores[]) {
	printf("line %3d:\n", index);
	printf("-> %s\n", lines[index]);
	printf("-> score = %3d\n", scores[index]);
}

/**********************************************************************/

// prints the Stage 2 output: the scores of the first and last stores
// lines against the query terms

void
print_stage2(line_t lines[], int scores[], int nlines) {

	print_stage(2);
	print_score_info(0, lines, scores);
	print_score_info(nlines - 1, lines, scores);
}

/**********************************************************************/

// scans the first "nlines" entries of the "scores" and returns the index
// of the highest-scoring entry not yet marked as used in "used", on a
// tie, the smaller index wins, since it's only replaced by a later
// entry that scores strictly higher

int
find_best_unused(int scores[], int nlines, int used[]) {

	int i, best_idx = -1;

	for (i = 0; i < nlines; i++) {
		if (!used[i] && (best_idx == -1 || scores[i] > scores[best_idx])) {
			best_idx = i;
		}
	}

	return best_idx;
}

/**********************************************************************/

// prints the Stage 3 output: the TOP_SCORES best-scoring lines, from
// highest to lowest, ties broken by the smaller line number

void
print_stage3(line_t lines[], int scores[], int nlines) {

	int used[MAX_LINES] = {0};
	int k, best_idx;

	print_stage(3);

	for (k = 0; k < TOP_SCORES && k < nlines; k++) {
		best_idx = find_best_unused(scores, nlines, used);
		used[best_idx] = 1;
		print_score_info(best_idx, lines, scores);
	}
}

/**********************************************************************/