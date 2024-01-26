# Island-Generation
Island generation using cellular automata.

Inspired by https://meltingasphalt.com/interactive/outbreak/

Made for the following challenge posted by javidx9 in OLC discord server:
```
Write a program that creates a 60x20 character "treasure map" of 6 islands, where:
     '~' represents the sea
     'O' represents the land
     '+' represents the beach
     Each island is entirely surrounded by beach, even diagonally
     Each island has a unique identifier 1-6
     Each island must be seperated from the other islands and the edge of the treasuremap by 1 cell of sea
     Diagonally opposite beaches are considered connected

Your program must be capable of generating many treasure maps, selectable by specifying an integer on stdin.

Bonus points awarded to most aesthetically pleasing treasure maps, as judged by "TBD"

Input: 726
Output:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~+++++~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+++++++++++~~~~~~~~~
~~++OOO+++++~~~~~~~+++++++~~~~~~~~~~~~~~+OOOOOOOOO+++++++++~
~~+OOOOOOOO+++~~~~++OOOOO++++~~~~~~~~~~~+OOOOOOOOOOOOOOOOO+~
~++OOOOOOOOOO++~~~+OOOOOOOOO++++++++~~~~+OOOOOO3OOOOOOOO+++~
~+OOOOO1OOOOOO+~~~+OOOOOOOOOOOOOOOO++++~+OOOOOOOOOOOOOO++~~~
~+OOOOOOOOOOOO+~~~++++OOOOOOOOOOOOOOOO+~+OOOOO+++OOOOOOO++~~
~+OOOOOOOOOOO++~~~~~~+++OOOO2OOOOOOO+++~+OOOOO+~+++++OOOO+~~
~+++OOOO++++++~~~~~~~~~++OOOOOOOOOOO+~~~+OOOO++~~~~++OOO++~~
~~~++++++~~~~~~++++++~~~++++OOOOOO+++~~~+OOO++~~~~++OO+++~~~
~~~~~~~~~~~~~+++OOOO+++~~~~++++++++~~~~~+OOOO++++++OOO+~~~~~
~~~~~~++++++++OOOOOOOO+~~~~~~~~~~~~~~~~~+++OOOOOOOOOO++~~~~~
~~~++++OOOOOOOOOOOOO+++~~~++++++++++~~~~~~++OOOOOOOO++~~~~~~
~~~+OOOOOOOOOOOOO++++~~~~++OOOOOOOO++~~~~~~++++OOO+++~~+++~~
~~~++OOOOO4OOOOOO+~~~~++++OOOOOOOOOO+++~~~~~~~+++++~~~++O++~
~~~~++OOOOOOOOOO++~~~~+OOOOOO5OOOOOOOO+~~~~~~~~~~~~~~++O6O+~
~~~~~+++OOOOOOOO+~~~~~++OOOOOOOOOOOO+++~~~~~~~~~~~++++OOO++~
~~~~~~~+++OOOOOO+~~~~~~+++OOOOOOOO+++~~~~~~~~~~~~~+OOOOO++~~
~~~~~~~~~++++++++~~~~~~~~++++++++++~~~~~~~~~~~~~~~+++++++~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```
