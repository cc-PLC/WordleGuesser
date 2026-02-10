#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WORDLE (5)
#define ELEMENT_LENGTH (WORDLE + 1)
#define BUFFER_SIZE ((ELEMENT_LENGTH + 1) * sizeof(char))
#define POSSIBLE_WORDLE (20000)
#define FILE_NAME ("Wordle.txt")


void handleErr(errno_t err) {
	char errMsg[50] = { 0 };
	strerror_s(errMsg, sizeof(errMsg), err);
	puts(errMsg);
	exit(-1);
}

char wordleGuesser(
	unsigned short* alphCount, char dataset[][BUFFER_SIZE],
	char autoBlack, char* blackList, char* yellowList, char* yellowLock, char* greenLock)
{
	char hasResult = 0;
	unsigned short bestNthWordle[WORDLE] = { 0 };
	int bestScore[WORDLE] = { 0 };

	// iterate our dataset
	for (unsigned short nthWordle = 0; dataset[nthWordle][0]; ++nthWordle)
	{
		int currScore = 0;
		char letterCount['z' + 1] = { 0 }, isValid = 1;
		for (char i = 0; i < WORDLE; ++i)
		{
			if (
				blackList[dataset[nthWordle][i]] ||
				(greenLock[i] && greenLock[i] != dataset[nthWordle][i]) ||
				(yellowLock[i] && yellowLock[i] == dataset[nthWordle][i])
				)
			{
				isValid = 0;
			}

			currScore += yellowList[dataset[nthWordle][i]] ?
				2 * POSSIBLE_WORDLE : (alphCount[dataset[nthWordle][i]] * 
					(letterCount[dataset[nthWordle][i]]? 0 : 1)
			);
			++letterCount[dataset[nthWordle][i]];
		}

		currScore *= isValid;
		for (int j = 0; j < WORDLE; ++j)
		{
			if (currScore > bestScore[j])
			{
				for (int k = WORDLE - 1; k > j; --k)
				{
					bestScore[k] = bestScore[k - 1];
					bestNthWordle[k] = bestNthWordle[k - 1];
				}
				bestScore[j] = currScore;
				bestNthWordle[j] = nthWordle;
				break;
			}
		}
	}

	for (char rpt = 0; rpt < WORDLE; ++rpt)
	{
		if (!bestScore[rpt]) { continue; }
		if (autoBlack)
		{
			// exclude those letters, so we can find five new letters next time
			for (char i = 0; i < WORDLE; ++i) { blackList[dataset[bestNthWordle[rpt]][i]] = 1; }
		}
		printf("%d\t%s\t\t%d\n", rpt + 1, dataset[bestNthWordle[rpt]], bestScore[rpt]);
		hasResult = 1;
	}
	puts("");
	return hasResult;
}

int main()
{
	FILE* fin = NULL;
	errno_t err = fopen_s(&fin, FILE_NAME, "r");
	if (err) { handleErr(err); }
	
	unsigned short alphCount['z' + 1] = { 0 };
	char dataset[POSSIBLE_WORDLE][BUFFER_SIZE] = { 0 };
	if (fin)
	{
		for (unsigned short nthWordle = 0; 
			fread(dataset[nthWordle], sizeof(char), ELEMENT_LENGTH, fin);
			++nthWordle)
		{
			if (errno) { handleErr(errno); }
			for (char i = 0; i < WORDLE; ++i) { ++alphCount[dataset[nthWordle][i]]; }
			dataset[nthWordle][WORDLE] = '\0'; // remove the '\n'
		}
	}


	char blackList['z' + 1] = { 0 }, yellowList['z' + 1] = { 0 },
		yellowLock[WORDLE] = { 0 }, greenLock[WORDLE] = { 0 };

	// starting word!
	printf("Repeat\tBest Wordle\tScore\n");
	do { ; } while (wordleGuesser(alphCount, dataset, 1, blackList, yellowList, yellowLock, greenLock));

	// include them back
	for (char c = 'a'; c <= 'z'; ++c) { blackList[c] = 0; }

	printf(
		"\n========================================\n"
		"\t-[a-z]:\t\tExclude (Black)\n"
		"\t[1-5]![a-z]:\tContain (Yellow)\n"
		"\t[1-5]=[a-z]:\tLock (Green)\n"
		"========================================\n"
		"Repeat\tBest Wordle\tScore\n");
	while (1)
	{
		char c = '\0', mode = getc(stdin);

		switch (mode)
		{
		case '\n': break; // run

		case '-':
			while (c = getc(stdin), c >= 'a' && c <= 'z') { blackList[c] = 1; }
			break; // I'll eat the '\n', so break and run

		case '1': case '2': case '3': case '4': case '5':
			c = getc(stdin);
			if (c == '=')
			{
				if (c = getc(stdin), c >= 'a' && c <= 'z') { greenLock[mode - '1'] = c; }
				else { printf("?"); }
			}
			else if (c == '!')
			{
				if (c = getc(stdin), c >= 'a' && c <= 'z') { yellowList[c] = 1; yellowLock[mode - '1'] = c; }
				else { printf("?"); }
			}
			else { printf("?"); }
			continue; // I won't eat the '\n', so continue

		default: printf("?"); continue; // continue the do-while loop
		}

		if (!wordleGuesser(alphCount, dataset, 0, blackList, yellowList, yellowLock, greenLock)) { break; }
	}

	return 0;
}