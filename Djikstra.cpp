#include <Arduino.h>

#define MAZE_SIZE 5
#define MAX_NODES (MAZE_SIZE * MAZE_SIZE)
#define INF 9999

#define FRONT_SENSOR A0
#define LEFT_SENSOR  A1
#define RIGHT_SENSOR A2
#define THRESHOLD 200

#define MOTOR_LEFT_1 4
#define MOTOR_LEFT_2 6
#define MOTOR_RIGHT_1 5
#define MOTOR_RIGHT_2 8

enum Direction { NORTH, EAST, SOUTH, WEST };


int adj[MAX_NODES][MAX_NODES]; 
bool visited[MAZE_SIZE][MAZE_SIZE];
int dist[MAX_NODES];
int prevNode[MAX_NODES];
Direction dir = NORTH;

int currentX = 0;
int currentY = 0;


int nodeIndex(int x, int y) {
  return y * MAZE_SIZE + x;
}

bool isWall(int sensorPin) {
  return analogRead(sensorPin) > THRESHOLD;
}

bool isInside(int x, int y) {
  return x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE;
}

void addEdge(int x1, int y1, int x2, int y2) {
  if (isInside(x2, y2)) {
    int u = nodeIndex(x1, y1);
    int v = nodeIndex(x2, y2);
    adj[u][v] = 1;
    adj[v][u] = 1;
  }
}

// === Movement ===
void stopMotors() {
  digitalWrite(MOTOR_LEFT_1, LOW);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, LOW);
}

void goStraight() {
  digitalWrite(MOTOR_LEFT_1, HIGH);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, HIGH);
  digitalWrite(MOTOR_RIGHT_2, LOW);
  delay(300);
  stopMotors();
}

void turnLeft() {
  digitalWrite(MOTOR_LEFT_1, LOW);
  digitalWrite(MOTOR_LEFT_2, HIGH);
  digitalWrite(MOTOR_RIGHT_1, HIGH);
  digitalWrite(MOTOR_RIGHT_2, LOW);
  delay(400);
  stopMotors();
  dir = static_cast<Direction>((dir + 3) % 4);
}

void turnRight() {
  digitalWrite(MOTOR_LEFT_1, HIGH);
  digitalWrite(MOTOR_LEFT_2, LOW);
  digitalWrite(MOTOR_RIGHT_1, LOW);
  digitalWrite(MOTOR_RIGHT_2, HIGH);
  delay(400);
  stopMotors();
  dir = static_cast<Direction>((dir + 1) % 4);
}

void turnAround() {
  turnRight();
  turnRight();
}


void exploreMaze() {
  for (int y = 0; y < MAZE_SIZE; y++) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      visited[y][x] = false;
      for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
          adj[i][j] = 0;
        }
      }
    }
  }


  exploreDFS(currentX, currentY);
}


void exploreDFS(int x, int y) {
  visited[y][x] = true;


  if (!isWall(FRONT_SENSOR)) {
    int nx = x, ny = y;
    switch (dir) {
      case NORTH: ny--; break;
      case EAST:  nx++; break;
      case SOUTH: ny++; break;
      case WEST:  nx--; break;
    }
    if (isInside(nx, ny) && !visited[ny][nx]) {
      addEdge(x, y, nx, ny);
      goStraight();
      currentX = nx;
      currentY = ny;
      exploreDFS(nx, ny);
      turnAround();
      goStraight();
      turnAround();
      currentX = x;
      currentY = y;
    }
  }


  turnLeft();
  if (!isWall(FRONT_SENSOR)) {
    int nx = x, ny = y;
    switch (dir) {
      case NORTH: ny--; break;
      case EAST:  nx++; break;
      case SOUTH: ny++; break;
      case WEST:  nx--; break;
    }
    if (isInside(nx, ny) && !visited[ny][nx]) {
      addEdge(x, y, nx, ny);
      goStraight();
      currentX = nx;
      currentY = ny;
      exploreDFS(nx, ny);
      turnAround();
      goStraight();
      turnAround();
      currentX = x;
      currentY = y;
    }
  }
  turnRight(); 

 
  turnRight();
  if (!isWall(FRONT_SENSOR)) {
    int nx = x, ny = y;
    switch (dir) {
      case NORTH: ny--; break;
      case EAST:  nx++; break;
      case SOUTH: ny++; break;
      case WEST:  nx--; break;
    }
    if (isInside(nx, ny) && !visited[ny][nx]) {
      addEdge(x, y, nx, ny);
      goStraight();
      currentX = nx;
      currentY = ny;
      exploreDFS(nx, ny);
      // backtrack
      turnAround();
      goStraight();
      turnAround();
      currentX = x;
      currentY = y;
    }
  }
  turnLeft(); 
}


void dijkstra(int start, int goal) {
  bool visited[MAX_NODES] = {false};

  for (int i = 0; i < MAX_NODES; i++) {
    dist[i] = INF;
    prevNode[i] = -1;
  }

  dist[start] = 0;

  for (int count = 0; count < MAX_NODES - 1; count++) {
    int u = -1;
    int minDist = INF;
    for (int i = 0; i < MAX_NODES; i++) {
      if (!visited[i] && dist[i] <= minDist) {
        minDist = dist[i];
        u = i;
      }
    }

    if (u == -1) break;
    visited[u] = true;

    for (int v = 0; v < MAX_NODES; v++) {
      if (!visited[v] && adj[u][v] && dist[u] + adj[u][v] < dist[v]) {
        dist[v] = dist[u] + adj[u][v];
        prevNode[v] = u;
      }
    }
  }
}


void replayPath(int start, int goal) {
  int path[MAX_NODES];
  int length = 0;
  for (int at = goal; at != -1; at = prevNode[at]) {
    path[length++] = at;
  }

  for (int i = length - 2; i >= 0; i--) {
    int from = path[i + 1];
    int to = path[i];

    int fx = from % MAZE_SIZE;
    int fy = from / MAZE_SIZE;
    int tx = to % MAZE_SIZE;
    int ty = to / MAZE_SIZE;

    if (fx == tx && ty == fy - 1) {
      while (dir != NORTH) turnRight();
    } else if (fx == tx && ty == fy + 1) {
      while (dir != SOUTH) turnRight();
    } else if (fy == ty && tx == fx + 1) {
      while (dir != EAST) turnRight();
    } else if (fy == ty && tx == fx - 1) {
      while (dir != WEST) turnRight();
    }

    goStraight();
    currentX = tx;
    currentY = ty;
  }
}

void setup() {
  pinMode(FRONT_SENSOR, INPUT);
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(MOTOR_LEFT_1, OUTPUT);
  pinMode(MOTOR_LEFT_2, OUTPUT);
  pinMode(MOTOR_RIGHT_1, OUTPUT);
  pinMode(MOTOR_RIGHT_2, OUTPUT);

  delay(1000);
  exploreMaze();

  int start = nodeIndex(0, 0);
  int goal = nodeIndex(MAZE_SIZE - 1, MAZE_SIZE - 1);

  dijkstra(start, goal);
  replayPath(start, goal);
}


void loop() {

}
