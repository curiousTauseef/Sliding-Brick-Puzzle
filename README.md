# Sliding Brick Puzzle
Solving a brick puzzle with search algorithm

## Abstract
This repository contains a C++ implementation of different search algorithms
to solve a Sliding Brick Puzzle. The goal of the puzzle is to get the "Master brick"
(represented in TODO? RED?) to the exit by shifting bricks around.
TODO image of SBP

Algorithms
* [Breadth-first search](https://en.wikipedia.org/wiki/Breadth-first_search)
* [Depth-first search](https://en.wikipedia.org/wiki/Depth-first_search)
* [Iterative deepening depth-first search](https://en.wikipedia.org/wiki/Iterative_deepening_depth-first_search)
* [A* search](https://en.wikipedia.org/wiki/A*_search_algorithm)



## Theory
It is assumed that the reader is familiar
with the basic concepts of different search algorithms and terms like 
complete, optimal ..etc.

## Implementation
* Search
  * Encapsulates the search algorithms and functions to run them and
    print their results
* Node
  * Represents a node in a tree. It holds the current state of the puzzle
    as well as a reference to it's parent node
* CostNode
  * Represents a node with a weight (cost) in a tree. The class is derived from
    Node. The only extension is a member variable that holds the cost
* Matrix
  * Represents one state of a puzzle. It's width, height and all the cells

The code is intently modular to add new algorithms easily.
  
## How do i run this code? (a.k.a Setup)
### Requirements
TODO
### Running the code
First run CMake in the directory with all the source files
```
$ cmake .
```

To create an executable named SBP
```
$ make
```


## Results
level2. Optimal solution length: 12
|        | BFS     | DFS     | IDDFS    | A* Manhatten | A* blocking |
|:------:|:-------:|:-------:|:--------:|:------------:|:-----------:|
| #nodes | 2008    | 493     | 0        | 0            | 0           |
| length | 12      | 489     |          | 0            | 0           |

Runtime for all algorithms for the first four level
|        | BFS     | DFS     | IDDFS    | A* Manhatten | A* blocking |
|:------:|:-------:|:-------:|:--------:|:------------:|:-----------:|
| level0 | 0       | 0       | 0        | 0            | 0           |
| level1 | 0.002   | 0.002   | 8.673    | 0.002        | 0.002       |
| level2 | 0.389   | 0.023   |          |              |             |
| level3 | 0       |         |          |              |             |
