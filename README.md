This C program contains all of the common rules of Blackjack (i.e., Hit, Stand, Double Down, Split, Surrender, Insurance) with the following exceptions:

* The user begins with $100, must make a minimum bet of $2, and cannot bet more than 20% of their available funds. If their funds drop below $10, they will not be able to make the minimum bet and thus, will be booted from the game.

* After splitting, the player is allowed to hit or double down (unless they're splitting two Aces, in which case they can only hit once). They're not allowed to split again. If they choose to hit, they will receive an additional card and each hand and can continue hitting until they bust or are satisfied with their two hands. If they choose to double down, they must stand after receiving an additional card on each hand and their bet is doubled again (eg, $5 original bet --> $10 after splitting --> $20 after doubling down on a split).

* The player can only surrender if the dealer's visible card is an Ace or 10 value.

* Insurance is always 50% of the value of the player's initial bet.