/*
 * maze.cpp
 * Name: Srishti Boral & Anika Mukherjee
 * Course: CS 106B
 * Description: This file contains functions to represent, check, and solve
 * mazes. generateValidMoves finds the open neighbors of a location,
 * validatePath checks whether a path is a correct solution, and
 * solveMazeBFS and solveMazeDFS search for a path from the entry to the
 * exit using a queue and a stack of paths.
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "SimpleTest.h"
using namespace std;


/* Returns a Set of all valid moves from the current location.
 * A valid move is one step up, down, left, or right that is inside the maze and is an open corridor.
 * Parameters: maze (the grid of walls and corridors), cur (the current location)
 * Returns: a Set of the neighboring locations you can move to
*/

Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    Vector<GridLocation> possibleNeighbors;
    int currentRow = cur.row;
    int currentCol = cur.col;

    // create top point
    GridLocation topPoint((currentRow-1), (currentCol));
    possibleNeighbors.add(topPoint);

    // create left point
    GridLocation leftPoint((currentRow),(currentCol-1));
    possibleNeighbors.add(leftPoint);

    // create right point
    GridLocation rightPoint((currentRow), (currentCol+1));
    possibleNeighbors.add(rightPoint);

    // create down point
    GridLocation downPoint((currentRow+1), (currentCol));
    possibleNeighbors.add(downPoint);

    // parse through 4 possible points and check if it's in the grid and a corridor
    for (GridLocation p : possibleNeighbors) {
        if ((p.col >= 0 && p.col < maze.numCols() && p.row >= 0 && p.row < maze.numRows()) && (maze[p.row][p.col] == true)) {
            neighbors.add(p);
        } else {
            // do nothing, the neighbor isn't in the grid
        }
    }
    return neighbors;
}

/* Checks whether a path visits any location more than once.
 * It adds each location to a Set and stops if it finds one that is already there.
 * Parameter: path (the path to check)
 * Returns: true if there are no repeated locations, false if there is a loop
*/

bool checkDuplicates(Vector<GridLocation> &path) {
    Set<GridLocation> duplicates;
    for (GridLocation index : path) {
        if (duplicates.contains(index)) {
            return false;
        }
        duplicates.add(index);
    }
    return true;
}

/* Checks whether a path is a valid solution to the maze. The path must not
 * be empty, must start at the top left corner, must end at the bottom right corner,
 * must only move to valid neighbors, and must not have a loop.
 * Parameters: maze (the grid of walls and corridors), path (the path to check)
 * Returns: nothing if the path is valid. Calls error with a message describing the problem if it is not.
 */

void validatePath(Grid<bool>& maze, Vector<GridLocation>& path) {
    if (path.isEmpty()) { // check if path is empty
        error("Path is empty!");
    }
    if (!(path[0].row == 0 && path[0].col == 0)) { // check if starts in top left corner
        error("Path does not start at top left corner!");
    }
    if (!(path[path.size()-1].row == maze.numRows()-1) || !(path[path.size()-1].col == maze.numCols()-1)) { // check if ends in bottom right corner
        error("Path does not end at bottom right corner!");
    }

    // check if current index is a valid move
    for (int i = 0; i < path.size()-1; i++) {
        if (!(generateValidMoves(maze, path[i]).contains(path[i+1]))) {
            error("This path is not valid.");
        }
    }

    // check if path is a loop
    if(!checkDuplicates(path)) {
        error("This path has a loop!");
    }
}


/* Finds a path from the top left corner to the bottom right corner using breadth-first search.
 * It keeps a queue of paths, starting with a path that only has the entry.
 * Each step dequeues a path, and if it ends at the exit, returns it.
 * Otherwise it adds a new path for each valid neighbor that
 * hasn't been visited yet. Because shorter paths are explored first, the
 * path returned is the shortest one.
 * Parameter: maze (the grid of walls and corridors)
 * Returns: a solution path from entry to exit
*/

Vector<GridLocation> solveMazeBFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Queue<Vector<GridLocation>> allPaths;

    drawMaze(maze);

    // create starting path which only has (0,0)
    Vector<GridLocation> startPath;
    GridLocation entryPoint(0,0);
    startPath.add(entryPoint);
    allPaths.enqueue(startPath);

    // define end point as bottom right corner of maze
    GridLocation exitPoint(maze.numRows() - 1, maze.numCols() - 1);

    // loop through all paths
    while(!allPaths.isEmpty()) {
        path = allPaths.dequeue();
        GridLocation currEnd = path[path.size() - 1];

        // if the current point on the path is at the end point of maze, return path
        if (currEnd == exitPoint) {
            return path;
        }

        // parse through the valid moves and see if they've been on the path
        for (GridLocation neighbor : generateValidMoves(maze, currEnd)) {
            bool seen = false;
            for (GridLocation point : path) { // parse through points on path
                if (point == neighbor) { // is point on path same as neighbor?
                    seen = true;
                    break;
                }
            }
            if (!seen) {
                Vector<GridLocation> copy = path;
                copy.add(neighbor);
                allPaths.enqueue(copy);
                highlightPath(path, "blue", 50);   // animation
            }
        }
    }
    return {};
}


/* Finds a path from the top left corner to the bottom right corner using
 * depth-first search. It works the same as solveMazeBFS but uses a stack
 * instead of a queue, so it follows one path as far as it can before backing up to try others.
 * The path returned is valid but may not be the shortest.
 * Parameter: maze (the grid of walls and corridors)
 * Returns: a solution path from entry to exit
 */

Vector<GridLocation> solveMazeDFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Stack<Vector<GridLocation>> allPaths;

    drawMaze(maze);

    Vector<GridLocation> startPath;
    GridLocation entryPoint(0,0);
    startPath.add(entryPoint);
    allPaths.push(startPath);

    // define end point as bottom right corner of maze
    GridLocation exitPoint(maze.numRows() - 1, maze.numCols() - 1);

    // loop through all paths
    while(!allPaths.isEmpty()) {
        path = allPaths.pop();
        GridLocation currEnd = path[path.size() - 1];

        // if the current point on the path is at the end point of maze, return path
        if (currEnd == exitPoint) {
            return path;
        }

        // parse through the valid moves and see if they've been on the path
        for (GridLocation neighbor : generateValidMoves(maze, currEnd)) {
            bool seen = false;
            for (GridLocation point : path) { // parse through points on path
                if (point == neighbor) { // is point on path same as neighbor?
                    seen = true;
                    break;
                }
            }
            if (!seen) {
                Vector<GridLocation> copy = path;
                copy.add(neighbor);
                allPaths.push(copy);
                highlightPath(path, "blue", 50);   // animation

            }
        }
    }
    return {};
}


/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines = stringSplit(readEntire(in), '\n');

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}


/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Vector<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0, 0}, {0, 2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution, hand-constructed maze") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Vector<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution read from file, medium maze") {
    Grid<bool> maze;
    Vector<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> notBeginAtEntry = { {1, 1}, {2, 1} };
    Vector<GridLocation> notEndAtExit = { {0, 0}, {1, 0}, {2, 0} };
    Vector<GridLocation> moveThroughWall = { {0 ,0}, {0, 1}, {1, 1}, {2, 1} };

    EXPECT_ERROR(validatePath(maze, notBeginAtEntry));
    EXPECT_ERROR(validatePath(maze, notEndAtExit));
    EXPECT_ERROR(validatePath(maze, moveThroughWall));
}

PROVIDED_TEST("solveMazeBFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMazeDFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// TODO: add your test cases here
STUDENT_TEST("generateValidMoves on bottom right corner of open 3x3 grid") {
    Grid<bool> maze =   {{true, true, true},
                        {true, true, true},
                        {true, true, true}};
    GridLocation corner = {2, 2};
    Set<GridLocation> expected = {{1, 2}, {2, 1}};
    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("generateValidMoves when surrounded by walls returns empty set") {
    Grid<bool> maze = {{false, false, false},
                       {false, true, false},
                       {false, false, false}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {};
    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves in center with walls on two sides") {
    Grid<bool> maze = {{true, false, true},
                       {false, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{1, 2}, {2, 1}};
    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("checkDuplicates finds repeats and passes paths with none") {
    Vector<GridLocation> noRepeats = {{0, 0}, {0, 1}, {1, 1}};
    Vector<GridLocation> repeats = {{0, 0}, {0, 1}, {0, 0}};
    EXPECT(checkDuplicates(noRepeats));
    EXPECT(!checkDuplicates(repeats));

}

STUDENT_TEST("validatePath on empty path should return error") {
    Grid<bool> maze = {{true, true}, {true, true}};
    Vector<GridLocation> empty = {};
    EXPECT_ERROR(validatePath(maze, empty));
}

STUDENT_TEST("validatePath on path with a loop should reurtn error") {
    Grid<bool> maze = {{true, true}, {true, true}};
    Vector<GridLocation> loop = {{0, 0}, {0, 1}, {0, 0}, {1, 0}, {1, 1}};
    EXPECT_ERROR(validatePath(maze, loop));
}

STUDENT_TEST("validatePath on path that jumps diagonally should return error") {
    Grid<bool> maze = {{true, true}, {true, true}};
    Vector<GridLocation> diagonal = {{0, 0}, {1, 1}};
    EXPECT_ERROR(validatePath(maze, diagonal));
}

STUDENT_TEST("validatePath on correct path in  3x3 grid") {
    Grid<bool> maze = {{true, true, true}, {true, true, true}, {true, true, true}};
    Vector<GridLocation> soln = {{0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 2}};
    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeBFS finds the shortest path on 5x7, matching the given solution file") {
    Grid<bool> maze;
    Vector<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);
    EXPECT_EQUAL(solveMazeBFS(maze), soln);
}

STUDENT_TEST("solveMazeBFS on 3x3 grid finds a shortest path (5)") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    Vector<GridLocation> soln = solveMazeBFS(maze);
    EXPECT_NO_ERROR(validatePath(maze, soln));
    EXPECT_EQUAL(soln.size(), 5);
}

STUDENT_TEST("solveMazeBFS on file 21x23") {
    Grid<bool> maze; readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);
    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeDFS on file 5x7") {
    Grid<bool> maze; readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);
    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMazeDFS on 3x3 grid finds a valid path") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    Vector<GridLocation> soln = solveMazeDFS(maze);
    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("BFS path is never longer than DFS path on 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> bfs = solveMazeBFS(maze);
    Vector<GridLocation> dfs = solveMazeDFS(maze);
    EXPECT(bfs.size() <= dfs.size());
}