/************************************************************************
*
* Copyright (C) 2017 by Peter Harrison. www.micromouseonline.com
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without l> imitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
************************************************************************/

#include <vector>
#include <mazedata.h>
#include "gtest/gtest.h"
#include "mazeprinter.h"
#include "mazedata.h"
#include "maze.h"
#include "mazefiler.h"

/////////////////////

class MazeTest : public ::testing::Test {
protected:
  Maze *maze;

  virtual void SetUp() {
    maze = new Maze(16);
    maze->resetToEmptyMaze();
  }

  virtual void TearDown() {
    delete maze;
  }

};

/*
TEST_F(MazeTest, MakeData) {
  Maze maze(16);
  for (int i = 0; i < mazeCount; i++) {
    MazeDataSource src = mazeList[i];

    if (src.size == 256) {
      maze.copyMazeFromFileData(src.data, src.size);
      maze.clearGoalArea();
      maze.addToGoalArea(0x77);
      maze.addToGoalArea(0x78);
      maze.addToGoalArea(0x88);
      maze.addToGoalArea(0x87);
      MazePrinter::printCDecl(&maze, src.title);
    };
  }
}
*/

TEST_F(MazeTest, GoalAreaInitialisDefault) {
  Maze maze16(16);
  EXPECT_EQ(0, maze16.goalAreaSize());
  EXPECT_EQ(0, maze16.goal());
  maze16.resetToEmptyMaze();
  EXPECT_EQ(4, maze16.goalAreaSize());
  Maze maze32(32);
  EXPECT_EQ(0, maze32.goalAreaSize());
}


TEST_F(MazeTest, GoalAreaClearsEmpty) {
  Maze maze16(16);
  maze16.clearGoalArea();
  EXPECT_EQ(0, maze16.goalAreaSize());
}


TEST_F(MazeTest, GoalAreaFindCellInGoal) {
  Maze maze16(16);
  maze16.resetToEmptyMaze();
  EXPECT_TRUE(maze16.goalContains(7, 7));
  EXPECT_TRUE(maze16.goalContains(7, 8));
  EXPECT_TRUE(maze16.goalContains(8, 7));
  EXPECT_TRUE(maze16.goalContains(8, 8));
}




TEST_F(MazeTest, CopyMaze) {
  maze->copyMazeFromFileData(emptyMaze, 256);
  for (uint16_t cell = 0; cell < maze->numCells(); cell++) {
    EXPECT_EQ(emptyMaze[cell] & 0x0f, maze->walls(cell));
    EXPECT_TRUE(maze->isVisited(cell));
  }
}

TEST_F(MazeTest, SetClearUnknowns_TestoneCell) {
  maze->resetToEmptyMaze();
  EXPECT_EQ(0x00, maze->walls(0x22));
  EXPECT_EQ(0x0E, maze->walls(0x00));
  EXPECT_EQ(0x08, maze->walls(0x01));
}

TEST_F(MazeTest, SetClearUnknowns_NoChangeInExploredMaze) {
  maze->copyMazeFromFileData(emptyMaze, 256);
  Maze setMaze(16);
  Maze clearMaze(16);
  setMaze.copyMazeFromFileData(emptyMaze, 256);
  setMaze.setUnknowns();
  clearMaze.copyMazeFromFileData(emptyMaze, 256);
  clearMaze.clearUnknowns();
  for (uint16_t cell = 0; cell < maze->numCells(); cell++) {
    EXPECT_EQ(setMaze.walls(cell), clearMaze.walls(cell));
  }
}

TEST_F(MazeTest, SetClearUnknowns_AllDifferentInUnExploredMaze) {
  Maze closedMaze(16);
  Maze openMaze(16);
  openMaze.resetToEmptyMaze();
  closedMaze.resetToEmptyMaze();
  closedMaze.setUnknowns();
  openMaze.clearUnknowns();
  EXPECT_EQ(closedMaze.closedWalls(0), openMaze.openWalls(0));
  for (uint16_t cell = 1; cell < maze->numCells(); cell++) {
    EXPECT_NE(closedMaze.closedWalls(cell), openMaze.openWalls(cell));
  }
}

TEST_F(MazeTest, SetAndGetGoal) {
  EXPECT_EQ(maze->cellID(7, 7), maze->goal());
  maze->setGoal(99);
  EXPECT_EQ(99, maze->goal());
}

TEST_F(MazeTest, ResetData_16x16_SetsEmptyMaze) {
  maze->resetToEmptyMaze();
  EXPECT_EQ(256, maze->numCells());
  for (int i = 0; i < maze->numCells(); i++) {
    EXPECT_EQ(emptyMaze[i] & 0x0f, maze->walls(i));
  }
}

TEST_F(MazeTest, ResetData_OnlyHomeCellVisited) {
  maze->resetToEmptyMaze();
  for (int i = 0; i < maze->numCells(); i++) {
    if (i != maze->home()) {
      EXPECT_FALSE(maze->isVisited(i));
    }
  }
}

TEST_F(MazeTest, RealExitAndWalls) {
  maze->resetToEmptyMaze();
  //  EXPECT_EQ(999, maze->xWalls[0x01]);
  EXPECT_TRUE(maze->hasExit(0x01, SOUTH));
  EXPECT_TRUE(maze->hasExit(0x01, EAST));
  EXPECT_FALSE(maze->hasRealExit(0x01, EAST));
  EXPECT_TRUE(maze->hasWall(0x01, WEST));
  EXPECT_TRUE(maze->hasRealWall(0x01, WEST));
  EXPECT_TRUE(maze->isSeen(0x01, WEST));
}

TEST_F(MazeTest, SetClearAndGetWalls) {
  maze->resetToEmptyMaze();
  maze->setWall(0x22, NORTH);
  maze->setWall(0x22, EAST);
  maze->setWall(0x22, SOUTH);
  maze->setWall(0x22, WEST);
  EXPECT_FALSE(maze->hasExit(0x22, EAST));
  EXPECT_FALSE(maze->hasExit(0x32, WEST));
  EXPECT_FALSE(maze->hasExit(0x22, SOUTH));
  EXPECT_FALSE(maze->hasExit(0x21, NORTH));
  EXPECT_FALSE(maze->hasExit(0x22, WEST));
  EXPECT_FALSE(maze->hasExit(0x12, EAST));
  EXPECT_FALSE(maze->hasExit(0x22, NORTH));
  EXPECT_FALSE(maze->hasExit(0x23, SOUTH));
  EXPECT_TRUE(maze->hasWall(0x22, EAST));
  EXPECT_TRUE(maze->hasWall(0x32, WEST));
  EXPECT_TRUE(maze->hasWall(0x22, SOUTH));
  EXPECT_TRUE(maze->hasWall(0x21, NORTH));
  EXPECT_TRUE(maze->hasWall(0x22, WEST));
  EXPECT_TRUE(maze->hasWall(0x12, EAST));
  EXPECT_TRUE(maze->hasWall(0x22, NORTH));
  EXPECT_TRUE(maze->hasWall(0x23, SOUTH));
  maze->clearWall(0x22, NORTH);
  maze->clearWall(0x22, EAST);
  maze->clearWall(0x22, SOUTH);
  maze->clearWall(0x22, WEST);
  EXPECT_TRUE(maze->hasExit(0x22, EAST));
  EXPECT_TRUE(maze->hasExit(0x32, WEST));
  EXPECT_TRUE(maze->hasExit(0x22, SOUTH));
  EXPECT_TRUE(maze->hasExit(0x21, NORTH));
  EXPECT_TRUE(maze->hasExit(0x22, WEST));
  EXPECT_TRUE(maze->hasExit(0x12, EAST));
  EXPECT_TRUE(maze->hasExit(0x22, NORTH));
  EXPECT_TRUE(maze->hasExit(0x23, SOUTH));
  EXPECT_FALSE(maze->hasWall(0x22, EAST));
  EXPECT_FALSE(maze->hasWall(0x32, WEST));
  EXPECT_FALSE(maze->hasWall(0x22, SOUTH));
  EXPECT_FALSE(maze->hasWall(0x21, NORTH));
  EXPECT_FALSE(maze->hasWall(0x22, WEST));
  EXPECT_FALSE(maze->hasWall(0x12, EAST));
  EXPECT_FALSE(maze->hasWall(0x22, NORTH));
  EXPECT_FALSE(maze->hasWall(0x23, SOUTH));
}



TEST_F(MazeTest, SetClearVisited_clearsVisitFlagsOnly) {
  maze->resetToEmptyMaze();
  uint16_t cell = 0x22;
  maze->updateMap(cell, 0x0F);
  EXPECT_TRUE(maze->isVisited(cell));
  EXPECT_EQ(0x0f, maze->walls(cell));
  maze->clearVisited(cell);
  EXPECT_FALSE(maze->isVisited(cell));
  EXPECT_EQ(0x0F, maze->walls(cell));
  maze->setVisited(cell);
  EXPECT_TRUE(maze->isVisited(cell));
  EXPECT_EQ(0x0F, maze->walls(cell));

}



/////////////////////
TEST_F(MazeTest, HasExit) {
  maze->resetToEmptyMaze();
  EXPECT_FALSE(maze->hasExit(0x0F, WEST));
  EXPECT_FALSE(maze->hasExit(0x0F, NORTH));
  EXPECT_FALSE(maze->hasExit(0xF0, EAST));
  EXPECT_FALSE(maze->hasExit(0xF0, SOUTH));
}

TEST_F(MazeTest, CopyCellFromFileData_GetExactCopy) {
  const uint8_t *src = japan2007ef;
  maze->resetToEmptyMaze();
  maze->copyMazeFromFileData(src, maze->numCells());
  for (int i = 0; i < maze->numCells(); i++) {
    EXPECT_EQ(src[i] & 0x0f, maze->walls(i));
    EXPECT_TRUE(maze->isVisited(i));
  }
}

TEST_F(MazeTest, OpenClosedMaze_HasExitWhenOpen) {
  maze->resetToEmptyMaze();
  maze->clearUnknowns();
  EXPECT_TRUE(maze->hasExit(0x22, WEST));
  EXPECT_TRUE(maze->hasExit(0x22, NORTH));
  EXPECT_TRUE(maze->hasExit(0x22, EAST));
  EXPECT_TRUE(maze->hasExit(0x22, SOUTH));
  maze->setUnknowns();
  EXPECT_FALSE(maze->hasExit(0x22, WEST));
  EXPECT_FALSE(maze->hasExit(0x22, NORTH));
  EXPECT_FALSE(maze->hasExit(0x22, EAST));
  EXPECT_FALSE(maze->hasExit(0x22, SOUTH));
  maze->clearUnknowns();
  EXPECT_TRUE(maze->hasExit(0x22, WEST));
  EXPECT_TRUE(maze->hasExit(0x22, NORTH));
  EXPECT_TRUE(maze->hasExit(0x22, EAST));
  EXPECT_TRUE(maze->hasExit(0x22, SOUTH));
}

TEST_F(MazeTest, NeighbourCellAddresses) {
  for (uint16_t cell = 0; cell < maze->numCells(); cell++) {
    EXPECT_EQ((cell + 1) % maze->numCells(), maze->cellNorth(cell));
    EXPECT_EQ((cell + maze->width()) % maze->numCells(), maze->cellEast(cell));
    EXPECT_EQ((cell + maze->numCells() - 1) % maze->numCells(), maze->cellSouth(cell));
    EXPECT_EQ((cell + maze->numCells() - maze->width()) % maze->numCells(), maze->cellWest(cell));
    EXPECT_EQ(maze->cellNorth(cell), maze->neighbour(cell, NORTH));
    EXPECT_EQ(maze->cellEast(cell), maze->neighbour(cell, EAST));
    EXPECT_EQ(maze->cellSouth(cell), maze->neighbour(cell, SOUTH));
    EXPECT_EQ(maze->cellWest(cell), maze->neighbour(cell, WEST));
  }
}


TEST_F(MazeTest, NeighbourInvalidDirection_ReturnsUINT16_MAX) {
  EXPECT_EQ(MAX_COST, maze->neighbour(0, -1));
}


TEST_F(MazeTest, LoadAndSave_MazeKeptSafe) {
  uint8_t backupWalls[1024] = {};
  maze->copyMazeFromFileData(japan2007ef, 256);
  maze->save(backupWalls);
  maze->resetToEmptyMaze();
  for (int i = 0; i < maze->numCells(); i++) {
    EXPECT_EQ(maze->walls(i), emptyMaze[i] & 0x0f);
  }
  maze->load(backupWalls);
  for (int i = 0; i < maze->numCells(); i++) {
    EXPECT_EQ(maze->walls(i), japan2007ef[i] & 0x0f);
  }


}
