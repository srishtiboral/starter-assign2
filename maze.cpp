// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
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
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

bool checkDuplicates(Vector<GridLocation> path) {
    Set<GridLocation> duplicates;
    for (GridLocation index : path) {
        if (duplicates.contains(index)) {
            return false;
        }
        duplicates.add(index);
    }
    return true;
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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


// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
Vector<GridLocation> solveMazeBFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Queue<Vector<GridLocation>> allPaths;
    GridLocation entryPoint(0, 0);
    path.add(entryPoint);

    while(!allPaths.isEmpty()) {
        Vector<GridLocation> currPath = allPaths.dequeue();
        GridLocation currEndPoint((currPath[currPath.size()-1].row), (currPath[currPath.size()-1].col));

        if ((currEndPoint.row == maze.numCols()-1) && (currEndPoint.col == maze.numRows()-1)) {
            path = currPath;
        } else {
            Set<GridLocation> validNeighbors;
            validNeighbors = generateValidMoves(maze, currEndPoint);
            Set<GridLocation> viableNeighbors:
            // for every point in generateValidMoves, check if it's a point we've seen in currPath
            for (GridLocation neighbor : validNeighbors) {
                for (GridLocation point : currPath) {
                    if (!neighbor == point) {
                        viableNeighbors.add(neighbor);
                    }
                }
                Vector<GridLocation> copy = currPath;
                copy.add(neighbor);
                allPaths.enqueue(copy);
           }
        }
    }

    drawMaze(maze);
    return path;
}


// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
Vector<GridLocation> solveMazeDFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Stack<Vector<GridLocation>> allPaths;

    drawMaze(maze);
    /* TODO: Fill in the remainder of this function. */
    return path;
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
