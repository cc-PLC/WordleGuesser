#include <stdio.h>
#include <string.h>
#define WORDLE 5
#define BUFFER_SIZE (WORDLE + 2) * sizeof(char)
#define POSSIBLE_WORDLE 20000
#define FILE_NAME "Wordle.txt"



int main()
{
	FILE* fin = NULL;
	if (fopen_s(&fin, FILE_NAME, "r")) { puts("File error"); exit(-1); }
	
	int alphCount['z' + 1] = { 0 }, nthWordle = 0;
	char dataset[POSSIBLE_WORDLE][BUFFER_SIZE] = { 0 };
	// WORDLE + 1: 5 letters + '\n'
	while (fread_s(dataset[nthWordle], BUFFER_SIZE, sizeof(char), WORDLE + 1, fin))
	{
		dataset[nthWordle][WORDLE] = '\0'; // remove the '\n'
		for (int i = 0; i < WORDLE; ++i) { ++alphCount[dataset[nthWordle][i]]; }
		++nthWordle;
	}

	char hasResult = 0;
	char excludedAlph['z' + 1] = { 0 };
	// starting wordle!
	printf("Repeat\tBest Wordle\tScore\n");
	do
	{
		hasResult = 0;

		// for 0 repeated, 1 repeated, 2 repeated, ...
		int bestScore[WORDLE] = { 0 }, bestNthWordle[WORDLE] = { 0 };

		// iterate our dataset
		for (nthWordle = 0; dataset[nthWordle][0]; ++nthWordle)
		{
			int currScore = 0;
			char letterCount['z' + 1] = { 0 }, currRepeat = 0;
			for (char i = 0; i < WORDLE; ++i)
			{
				if (excludedAlph[dataset[nthWordle][i]]) { currScore = 0; break; }
				
				if (++letterCount[dataset[nthWordle][i]] > currRepeat) { currRepeat = letterCount[dataset[nthWordle][i]]; }
				currScore += alphCount[dataset[nthWordle][i]];
			}

			if (currScore > bestScore[currRepeat])
			{
				bestScore[currRepeat] = currScore;
				bestNthWordle[currRepeat] = nthWordle;
			}
		}

		for (char rpt = 1; rpt < WORDLE; ++rpt)
		{
			// exclude those letters, so we can find five new letters next time
			for (char i = 0; i < WORDLE; ++i) { excludedAlph[dataset[bestNthWordle[rpt]][i]] = 1; }
			if (!bestScore[rpt]) { continue; }
			printf("%d\t%s\t\t%d\n",
				rpt, dataset[bestNthWordle[rpt]], bestScore[rpt]);
			hasResult = 1;
		}
		puts("");
	} while (hasResult);

	char c = '\0';
	for (c = 'a'; c <= 'z'; ++c) { excludedAlph[c] = 0; }

	char mode = 0, lock[WORDLE] = { 0 };
	hasResult = 1;
	printf(
		"\n========================================\n"
		"\t-[a-z]:\tExclude (Black)\n"
		"\t+[a-z]:\tContain (Yellow)\n"
		"\t[1-5]=[a-z]:\tLock (Green)\n"
		"========================================\n"
		"Repeat\tBest Wordle\tScore\n");
	do
	{
		mode = getc(stdin);

		switch (mode)
		{
		case '\n': break; // run

		case '-':
			while (c = getc(stdin), c >= 'a' && c <= 'z') { excludedAlph[c] = 1; }
			break; // I'll eat the '\n', so break and run

		case '+':
			while (c = getc(stdin), c >= 'a' && c <= 'z');
			printf("(Coming Soon)\n");
			break; // I'll eat the '\n', so break and run

		case '1': case '2': case '3': case '4': case '5':
			if (getc(stdin) != '=') { printf("?"); continue; } // continue the do-while loop
			if (c = getc(stdin), c >= 'a' && c <= 'z') { lock[mode - '1'] = c; }
			else { printf("?"); }
			continue; // I won't eat the '\n', so continue

		default: printf("?"); continue; // continue the do-while loop
		}

		puts("");
		hasResult = 0;

		// for 0 repeated, 1 repeated, 2 repeated, ...
		int bestScore[WORDLE] = { 0 }, bestNthWordle[WORDLE] = { 0 };

		// iterate our dataset
		for (nthWordle = 0; dataset[nthWordle][0]; ++nthWordle)
		{
			int currScore = 0;
			char letterCount['z' + 1] = { 0 }, currRepeat = 0;
			for (char i = 0; i < WORDLE; ++i)
			{
				if (
					excludedAlph[dataset[nthWordle][i]] ||
					(lock[i] && lock[i] != dataset[nthWordle][i])
					) { currScore = 0; break; }

				if (++letterCount[dataset[nthWordle][i]] > currRepeat) { currRepeat = letterCount[dataset[nthWordle][i]]; }
				currScore += alphCount[dataset[nthWordle][i]];
			}

			if (currScore > bestScore[currRepeat])
			{
				bestScore[currRepeat] = currScore;
				bestNthWordle[currRepeat] = nthWordle;
			}
		}

		for (char rpt = 1; rpt < WORDLE; ++rpt)
		{
			if (!bestScore[rpt]) { continue; }
			printf("%d\t%s\t\t%d\n",
				rpt, dataset[bestNthWordle[rpt]], bestScore[rpt]);
			hasResult = 1;
		}
		puts("");
	} while (hasResult);

	return 0;
}