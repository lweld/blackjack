/* This program simulates a game of Blackjack between a single user and a dealer (the computer).
   Author: Liam Weld */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define SIGN 1
#define CLEANEXIT 0
#define HAND 9

typedef enum { false, true } bool;
enum rank { TWO = 2, THREE = 3, FOUR = 4, FIVE = 5, SIX = 6, SEVEN = 7, EIGHT = 8, NINE = 9, TEN, JOKER, QUEEN, KING, ACE };

void hit(int deckVal, int * userCards, int * playerTotal, char * isBust, int * hasAce, int i);
void doubleDown(int deckVal, int * userCards, int * playerTotal, char * isBust, int * hasAce, int i, float * bet);
int split(int * deck, int * userCards, int * playerTotal, char * isFirstHandBust, int * hasAce, int val, int i, float * bet);
void splitHit(int * deck, int * firstHand, int * secondHand, int * firstHandTotal, int * secondHandTotal, char * isFirstHandBust, char * isSecondHandBust, int * firstHasAce, int * secondHasAce, int val, int i);
void splitDoubleDown(int * deck, int * firstHand, int * secondHand, int * firstHandTotal, int * secondHandTotal, char * isFirstHandBust, char * isSecondHandBust, int * firstHasAce, int * secondHasAce, int val, int i, float * bet);
void splitAces(int * deck, int * firstHand, int * secondHand, int * firstHandTotal, int * secondHandTotal, char * isFirstHandBust, char * isSecondHandBust, int * firstHasAce, int * secondHasAce, int val, int i);
void createDeck(int * deck);
void shuffleDeck(int * deck);
void swap(int * i, int * random);
void getFloatInput(float * bet, float bank);
void getAlphaInput(char * alphaInput);
void getAlphanumericInput(char * alnumInput);
int getScore(int * userCards, int i, int playerTotal, int * hasAce);
void getIndividualCardValues(int * userCards, int * hasAce, int i, int * playerTotal);
void printUserCards(int * userCards, int i);
const char * enumToString(int cardVal);
char checkIfBust(int total);
char checkIfBlackjack(int * userCards);
void alterIfBlackjack(int * userCards, int * dealerCards, char userBlackjack, char dealerBlackjack, bool * takeInsurance, float insurance, float * bank, float bet);
void askForInsurance(float * insurance, int bet, bool * takeInsurance, int * dealerCards);
void getMaxSplitTotal(int * firstHandTotal, int secondHandTotal);
void setToZero(int * userCards, int * dealerCards);
void insolvent(float bank);
void printScore(int playerScore, int dealerScore);

int main() {
	char userStrat = 'n', isBust = 'n', surrender = 'n', dealerDrew = 'n', userBlackjack, dealerBlackjack, playAgain;
	// Allocating userCards and dealerCards on the stack (since the memory occupied is small) and initializing all elements to zero.
	int *deck, userCards[HAND] = {0}, dealerCards[HAND] = {0}, i = 0, k = 0, val = 0, playerTotal = 0, dealerTotal = 0, userHasAce = 0, dealerHasAce = 0, counter = 0;
	float bank = 100.0f, bet, insurance = 0.0f;
	deck = (int *) calloc(52, sizeof(int)); // Used calloc instead of malloc to make error checking easier when creating the deck since all values are set to zero.
	if (deck == NULL) { printf("ERROR: re-start game.\n"); exit(CLEANEXIT); }
	createDeck(deck);
	shuffleDeck(deck);
	printf("\nWelcome to the fantastic game of Blackjack!\nYou begin with $100, must make a minimum bet of $2, and cannot bet more than 20%% of your available funds.\n\n");
	do {
		printf("Enter the amount you would like to bet: ");
		getFloatInput(&bet, bank);

		for (i = 0, k = 0; i < 2; i++, k++) { userCards[i] = deck[val++]; dealerCards[k] = deck[val++]; } // Sets the first two cards for both user and dealer.

		bool canSurrender = dealerCards[1] == ACE || dealerCards[1] == TEN || dealerCards[1] == JOKER || dealerCards[1] == QUEEN || dealerCards[1] == KING;
		bool canSplit = userCards[0] == userCards[1];
		bool takeInsurance = dealerCards[1] == ACE;

		printUserCards(userCards, i);
		printf("The dealer's second card is: %s\n", enumToString(dealerCards[1]));
		userBlackjack = checkIfBlackjack(userCards); dealerBlackjack = checkIfBlackjack(dealerCards); // Check if either user to dealer got a Blackjack.
		alterIfBlackjack(userCards, dealerCards, userBlackjack, dealerBlackjack, &takeInsurance, insurance, &bank, bet);
		while (userStrat != 's' && isBust == 'n' && surrender == 'n' && userBlackjack != 'b' && dealerBlackjack != 'b') {
			if (i == 2 && takeInsurance)
				askForInsurance(&insurance, bet, &takeInsurance, dealerCards);
			if (i == 2 && canSplit && canSurrender)
				printf("Enter 'h' to Hit, 's' to Stand, 'd' to Double Down, 'p' to Split, or 'u' to Surrender: ");
			else if (i == 2 && canSurrender)
				printf("Enter 'h' to Hit, 's' to Stand, 'd' to Double Down, or 'u' to Surrender: ");
			else if (i == 2 && canSplit) 
				printf("Enter 'h' to Hit, 's' to Stand, 'd' to Double Down, or 'p' to Split: ");
			else if (i == 2)
				printf("Enter 'h' to Hit, 's' to Stand, or 'd' to Double Down: ");
			else
				printf("Enter 'h' to Hit, or 's' to Stand: ");
			getAlphaInput(&userStrat);
			if (userStrat == 'h') // Hit
				hit(deck[val++], userCards, &playerTotal, &isBust, &userHasAce, i++);
			else if (userStrat == 'd' && i == 2) { // Double Down
				doubleDown(deck[val++], userCards, &playerTotal, &isBust, &userHasAce, i++, &bet);
				userStrat = 's'; // The user must stop playing after doubling down.
			} else if (userStrat == 'p' && i == 2 && userCards[0] == userCards[1]) { // Split
				val = split(deck, userCards, &playerTotal, &isBust, &userHasAce, val, i++, &bet); // Returning deck is easier than passing deck by reference, which would require that split accept a pointer to a pointer.
				userStrat = 's'; // All playing decisions are made in split, so we must ensure the while loop terminates.
			} else if (userStrat == 'u' && i == 2) { // Surrender
				bank = bank - bet * 0.5;
				surrender = 'y';
			} else if (userStrat == 's' && i == 2) // Useful when player decides to stand after being given their first two cards.
				playerTotal = getScore(userCards, i, playerTotal, &userHasAce);
			else if (userStrat == 's');
			else
				printf("INVALID INPUT. Please enter one of the letters given as an option.\n");
		}
		if (isBust == 'y') {
			bank = bank - bet - insurance; // If the user makes enough plays to go bust, the dealer couldn't have got a blackjack, so user loses insurance.
			printScore(playerTotal, getScore(dealerCards, 2, dealerTotal, &userHasAce));
			printf("You went bust. Dealer won. Better luck next time.\n");
		}
		else if (userBlackjack != 'b' && dealerBlackjack != 'b' && surrender == 'n') {
			while ((dealerTotal = getScore(dealerCards, k++, dealerTotal, &dealerHasAce)) < 17) { // Dealer hits until their score is 17.
				dealerCards[k] = deck[val++];
				dealerDrew = 'y';
			}
			if (dealerDrew == 'y') --val; // Prevents a card value from being skipped over at the start of the next round.
			printScore(playerTotal, dealerTotal);
			if (dealerTotal > 21 || playerTotal > dealerTotal) {
				bank = bank + bet;
				printf("Congratulations! You won!\n");
			}
			else if (dealerTotal > playerTotal) {
				bank = bank - bet - insurance; // This block of code wouldn't have executed if the dealer had a blackjack, so user loses insurance.
				printf("Dealer won. Better luck next time.\n");
			}
			else
				printf("It's a tie.\n");
		}
		printf("You have $%.2f left.\n\nWould you like to continue playing? Enter any letter or number for yes, or 'n' for no: ", bank);
		getAlphanumericInput(&playAgain);
		if (playAgain == 'n') {
			printf("Thank you for playing.\n");
			exit(CLEANEXIT);
		}
		else
			if (++counter % 4 == 0) { shuffleDeck(deck); val = 0; } // Re-shuffle every 4 rounds.
			if (bank <= 10) insolvent(bank);
			i = k = playerTotal = dealerTotal = insurance = userHasAce = dealerHasAce = 0; 
			isBust = userStrat = surrender = dealerDrew = 'n';
			setToZero(userCards, dealerCards);
	} while (SIGN);
}

/* Gives the user another card from the deck. */
void hit(int deckVal, int * userCards, int * playerTotal, char * isBust, int * hasAce, int i) {
	userCards[i++] = deckVal; // The value added to userCards points back to the main function, so we don't need to return it.
	printUserCards(userCards, i);
	*playerTotal = getScore(userCards, i, *playerTotal, hasAce);
	*isBust = checkIfBust(*playerTotal);
}

/* Gives the user another card from the deck while doubling their bet. */
void doubleDown(int deckVal, int * userCards, int * playerTotal, char * isBust, int * hasAce, int i, float * bet) {
	*bet = *bet + *bet;
	hit(deckVal, userCards, playerTotal, isBust, hasAce, i++);
}

/* If a user has two cards, each card is made into a separate hand and each of these separate hands recieve
   an additional card. Afterwards, the user is given an option to hit, double down or stand on these two hands. */
int split(int * deck, int * firstHand, int * firstHandTotal, char * isFirstHandBust, int * firstHasAce, int val, int i, float * bet) {
	char isSecondHandBust, userStrat = '\0';
	int *secondHand, secondHandTotal = 0, secondHasAce = 0;
	secondHand = (int *) calloc(11, sizeof(int));
	*bet = *bet + *bet; // When splitting their hand the user must double their bet.
	secondHand[0] = firstHand[1];
	firstHand[1] = deck[val++];
	secondHand[1] = deck[val++];
	printf("First split hand. "); printUserCards(firstHand, i);
	printf("Second split hand. "); printUserCards(secondHand, i);
	*firstHandTotal = getScore(firstHand, i, *firstHandTotal, firstHasAce);
	secondHandTotal = getScore(secondHand, i, secondHandTotal, &secondHasAce);
	if (firstHand[0] == ACE && secondHand[0] == ACE) { // Splitting Aces
		splitAces(deck, firstHand, secondHand, firstHandTotal, &secondHandTotal, isFirstHandBust, &isSecondHandBust, firstHasAce, &secondHasAce, val++, i++);
		++val; userStrat = 's';
	}
	while (userStrat != 's') {
		if (i == 2) 
			printf("Enter 'h' to Hit, 's' to Stand, or 'd' to Double Down: ");
		else
			printf("Enter 'h' to Hit, or 's' to Stand: ");
		getAlphaInput(&userStrat);
		if (userStrat == 'h') { // Hit
			splitHit(deck, firstHand, secondHand, firstHandTotal, &secondHandTotal, isFirstHandBust, &isSecondHandBust, firstHasAce, &secondHasAce, val++, i++);
			if (*isFirstHandBust == 'y' && isSecondHandBust == 'y') return ++val;
			++val;
		} else if (userStrat == 'd' && i == 2) { // Double Down
			splitDoubleDown(deck, firstHand, secondHand, firstHandTotal, &secondHandTotal, isFirstHandBust, &isSecondHandBust, firstHasAce, &secondHasAce, val++, i, bet);
			++val; userStrat = 's';
		}
	}
	getMaxSplitTotal(firstHandTotal, secondHandTotal); // Ensures the total score used in main is the optimal between the split hands.
	return val;
}

/* Gives the user an additional card on both hands that resulted from them splitting their initial two cards.
   Called in either the function, split or splitAces, when a user decides to hit. */
void splitHit(int * deck, int * firstHand, int * secondHand, int * firstHandTotal, int * secondHandTotal, char * isFirstHandBust, char * isSecondHandBust, int * firstHasAce, int * secondHasAce, int val, int i) {
	printf("First split hand. ");
	hit(deck[val++], firstHand, firstHandTotal, isFirstHandBust, firstHasAce, i++);
	--i;
	printf("Second split hand. ");
	hit(deck[val], secondHand, secondHandTotal, isSecondHandBust, secondHasAce, i);
	if (*isFirstHandBust == 'y' && *isSecondHandBust == 'y') return; // User is only bust if both of their hands are bust.
	if (*isFirstHandBust == 'y' && (*isSecondHandBust == 'n' || *isSecondHandBust == 'w'))
		*isFirstHandBust = *isSecondHandBust; // Ensure the bust status returned to main is the optimal bust status between the split hands.
}

/* Gives the user an additional card on both hands that resulted from them splitting their initial two cards, while doubling their bet.
   Called in the function, split, when a user decides to double down. */
void splitDoubleDown(int * deck, int * firstHand, int * secondHand, int * firstHandTotal, int * secondHandTotal, char * isFirstHandBust, char * isSecondHandBust, int * firstHasAce, int * secondHasAce, int val, int i, float * bet) {
	float tempBet = 0.0f; // Creating a dummy bet to fulfill the argument requirements of doubleDown.
	*bet = *bet + *bet; // Doubling the user's already doubled bet.
	printf("First split hand. ");
	doubleDown(deck[val++], firstHand, firstHandTotal, isFirstHandBust, firstHasAce, i++, &tempBet);
	--i;
	printf("Second split hand. ");
	doubleDown(deck[val], secondHand, secondHandTotal, isSecondHandBust, secondHasAce, i, &tempBet);
}

/* Gives the user the option of recieving an additional card on both hands that resulted from them splitting their initial two cards.
   Called in the function, split, when the user's initial two cards are Aces. */
void splitAces(int * deck, int * firstHand, int * secondHand, int * firstHandTotal, int * secondHandTotal, char * isFirstHandBust, char * isSecondHandBust, int * firstHasAce, int * secondHasAce, int val, int i) {
	char userInput;
	printf("Since you've split Aces you can only hit once.\nWould you like to hit once on each hand? Enter 'y' for yes, or any letter for no: ");
	getAlphaInput(&userInput);
	if (userInput == 'y')
		splitHit(deck, firstHand, secondHand, firstHandTotal, secondHandTotal, isFirstHandBust, isSecondHandBust, firstHasAce, secondHasAce, val, i++); // The two if statements in splitHit are unnecessary.
}

/* Creates a standard 52 card deck. */
void createDeck(int * deck) {
	for (int suit = 0; suit < 4; suit++)
		for (int rank = TWO; rank <= ACE; rank++) // Iterate through the enums.
			*deck++ = rank;
}

/* Shuffles a standard 52 card deck. */
void shuffleDeck(int * deck) {
	int random;
	srand(time(0)); // Seeds the random number generator with current time to generate a random sequence with each run.
	for (int i = 0; i < 52; i++) {
		random = rand() % 52;
		swap(deck+i, deck+random);
	}
}

/* Swaps the contents of two characters. */
void swap(int * i, int * random) {
	int temp = *i;
	*i = *random;
	*random = temp;
}

/* Gets input from the user when the input is meant to be a float (i.e., the user's bet value). */
void getFloatInput(float * bet, float bank) {
	while (scanf("%f", bet) != 1) {
		while((getchar()) != '\n'); // Removes all input provided after the first char.
		printf("INVALID INPUT. Please enter a number to bet: ");
	}
	while((getchar()) != '\n');
	if (*bet < 2.0f) { // Ensures bet meets minimum of $2.
		printf("Your bet of %.2f is under our minmum betting amount of $2. Please make a bet of $2 or more: ", *bet);
		getFloatInput(bet, bank);
	}
	else if (*bet > bank * 0.2) { // Ensures bet isn't over 20% of user's available funds.
		printf("Don't be so risky. Enter a max bet of $%.2f since you have $%.2f in your bank: ", bank * 0.2, bank);
		getFloatInput(bet, bank);
	}
}

/* Gets input from the user when the input is meant to be a letter. */
void getAlphaInput(char * alphaInput) {
	scanf("%c", alphaInput); while((getchar()) != '\n');
	while (!isalpha(*alphaInput)) {
		printf("INVALID INPUT. Please enter an alphabet: ");
		scanf("%c", alphaInput); while((getchar()) != '\n');
	}
	*alphaInput = tolower(*alphaInput);
}

/* Gets input from the user when the input is meant to be either a letter or number. */
void getAlphanumericInput(char * alnumInput) {
	scanf("%c", alnumInput); while((getchar()) != '\n');
	while(!isalnum(*alnumInput)) {
		printf("INVALID INPUT. Please enter a letter or a number: ");
		scanf("%c", alnumInput); while((getchar()) != '\n');
	}
	*alnumInput = tolower(*alnumInput);
}

/* Returns the total score for the given card hand. */
int getScore(int * userCards, int i, int totalScore, int * hasAce) {
	if (totalScore == 0)
		for (int k = 0; k < i; k++)
			getIndividualCardValues(userCards, hasAce, k, &totalScore);
	else
		getIndividualCardValues(userCards, hasAce, --i, &totalScore); // Decrementing `i` since it is incremented in main, which would give wrong index position by +1.
	if (totalScore > 21 && *hasAce > 0) { // Converts Ace value of 11 to 1 when convenient for the user.
		printf("hasAce %d\n", *hasAce);
		*hasAce = *hasAce - 1;
		totalScore = totalScore - 10;
	}
	return totalScore;
}

/* Gets the value of a single card. */
void getIndividualCardValues(int * userCards, int * hasAce, int k, int * totalScore) {
	if (userCards[k] == TEN || userCards[k] == JOKER || userCards[k] == QUEEN || userCards[k] == KING)
		*totalScore = *totalScore + 10;
	else if (userCards[k] == ACE) {
		*hasAce = *hasAce + 1;
		*totalScore = *totalScore + 11;
	}
	else
		*totalScore = *totalScore + userCards[k];
}

/* Prints the contents of a card hand. */
void printUserCards(int * userCards, int i) {
	printf("Your cards are: ");
	for (int j = 0; j < i; j++) {
		const char * name = enumToString(userCards[j]);
		if (i - j == 1)
			printf("and %s\n", name);
		else if (i - j == 2)
			printf("%s ", name);
		else
			printf("%s, ", name);
	}
}

/* Returns a constant string representation of a card value.
   Called in the function, printUserCards. */
const char * enumToString(int cardIdentifier) {
	switch(cardIdentifier) {
		case 2: return "two";
		case 3: return "three";
		case 4: return "four";
		case 5: return "five";
		case 6: return "six";
		case 7: return "seven";
		case 8: return "eight";
		case 9: return "nine";
		case 10: return "ten";
		case 11: return "Joker";
		case 12: return "Queen";
		case 13: return "King";
		case 14: return "Ace";
	}
	return "ERROR: incorrect card value.";
}

/* Checks if a player went bust. */
char checkIfBust(int total) {
	if (total > 21)
		return 'y';
	else if (total == 21)
		return 'w'; // Doesn't guarantee player a win, it just prevents them from taking another action ('n' would allow them to take another action).
	else
		return 'n';
}

/* Checks if a player recieved a "Blackjack", which is when their first two cards sum to 21. */
char checkIfBlackjack(int * userCards) {
	if (userCards[0] == ACE && (userCards[1] == TEN || userCards[1] == JOKER || userCards[1] == QUEEN || userCards[1] == KING))
		return 'b';
	if (userCards[1] == ACE && (userCards[0] == TEN || userCards[0] == JOKER || userCards[0] == QUEEN || userCards[0] == KING))
		return 'b';
	return 'n';
}

/* Handles payouts and messaging when a player recieves a Blackjack. */
void alterIfBlackjack(int * userCards, int * dealerCards, char userBlackjack, char dealerBlackjack, bool * takeInsurance, float insurance, float * bank, float bet) {
	if (userBlackjack == 'b' && dealerBlackjack == 'b')
		printf("It's a highly unlikely tie. Two blackjacks!\n");
	else if (userBlackjack == 'b' && dealerBlackjack == 'n') {
		*bank = *bank + bet * 1.5;
		printf("CONGRATULATIONS! Your hand dealt a Blackjack. You win 1.5x times your bet.\n");
	}
	else if (userBlackjack == 'n' && dealerBlackjack == 'b') {
		if (takeInsurance)
			askForInsurance(&insurance, bet, takeInsurance, dealerCards);
		*bank = *bank - bet + insurance;
		printf("Dealer won with a Blackjack. Better luck next time.\n");
	}
}

/* Gives the user the option of taking out insurance if the dealer's face card is an Ace.
   Called in the function, alterIfBlackjack. */
void askForInsurance(float * insurance, int bet, bool * takeInsurance, int * dealerCards) {
	*takeInsurance = 0;
	char userInput;
	printf("Given that the dealer's second card is an %s, would you like to take insurance? Enter 'y' for yes, or any letter for no: ", enumToString(dealerCards[1]));
	getAlphaInput(&userInput);
	if (userInput == 'y')
		*insurance = bet * 0.5;
}

/* When a user splits their hand, this function finds the optimal score between the two hands.
   Called in the function, split. */
void getMaxSplitTotal(int * firstHandTotal, int secondHandTotal) {
	if (*firstHandTotal > 21 && secondHandTotal > 21 && *firstHandTotal < secondHandTotal) return;
	else if (*firstHandTotal > 21)
		*firstHandTotal = secondHandTotal;
	else if (secondHandTotal > *firstHandTotal && secondHandTotal <= 21)
		*firstHandTotal = secondHandTotal;
}

/* Initializes each element in the user's and dealer's hand to zero. Used at the start of a new round. */
void setToZero(int * userCards, int * dealerCards) {
	for (int j = 0; j < HAND; j++) {
		userCards[j] = 0;
		dealerCards[j] = 0;
	}
}

void insolvent(float bank) {
	printf("Given that you cannot bet more than 20%% of your available funds, your funds of $%.2f is too small to meet our minimum bet of $2.\nYou're being booted from the Blackjack table.\n", bank);
	exit(CLEANEXIT);
}

/* Prints the final score of the user and the dealer. */
void printScore(int playerScore, int dealerScore) {
	printf("Your card score: %d\nDealer's card score: %d\n", playerScore, dealerScore);
}