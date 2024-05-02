#include "study_in_pink2.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

//! 3.1 MAPS ELEMENTS 

// MapElement constructor implementation
MapElement::MapElement(ElementType in_type) : type(in_type) {}

// MapElement destructor implementation
MapElement::~MapElement() {}

// Implementation of getType method
ElementType MapElement::getType() const {
    return type;
}

// Implementation of Path class (derived from MapElement)
Path::Path() : MapElement(PATH) {}

// Implementation of Wall class (derived from MapElement)
Wall::Wall() : MapElement(WALL) {}

// Implementation of FakeWall class (derived from MapElement)
FakeWall::FakeWall(int in_req_exp) : MapElement(FAKE_WALL) {
    Position pos = getPosition();
    int r = pos.getRow();
    int c = pos.getCol();
    in_req_exp = (r * 257 + c * 139 + 89) % 900 + 1;
    req_exp = in_req_exp;
}

// Implementation of getPosition method
Position FakeWall::getPosition() const {
    return position;
}

// Implementation of getReqExp() method
int FakeWall::getReqExp() const {
    return req_exp;
}

//! 3.2 Maps

// Constructor implementation for Map class
Map::Map(int num_rows, int num_cols, int num_walls, Position *array_walls, int num_fake_walls, Position *array_fake_walls) {
    this->num_rows = num_rows;
    this->num_cols = num_cols;

    // Allocate memory for the map
    map = new MapElement**[num_rows];
    for (int i = 0; i < num_rows; ++i) {
        map[i] = new MapElement*[num_cols];
        for (int j = 0; j < num_cols; ++j) {
            // Initialize each element as Path by default
            map[i][j] = new Path();
        }
    }

    // Place walls on the map
    for (int i = 0; i < num_walls; ++i) {
        int wall_row = array_walls[i].getRow();
        int wall_col = array_walls[i].getCol();
        delete map[wall_row][wall_col]; // Delete the existing Path object
        map[wall_row][wall_col] = new Wall(); // Place a Wall object
    }

    // Place fake walls on the map
    for (int i = 0; i < num_fake_walls; ++i) {
        int fake_wall_row = array_fake_walls[i].getRow();
        int fake_wall_col = array_fake_walls[i].getCol();
        delete map[fake_wall_row][fake_wall_col]; // Delete the existing Path object
        map[fake_wall_row][fake_wall_col] = new FakeWall(); // Place a FakeWall object
    }
}

// Destructor implementation for Map class
Map::~Map() {
    // Deallocate memory for the map
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            delete map[i][j];
        }
        delete[] map[i];
    }
    delete[] map;
}

//! 3.3 Position
Position::Position(int r, int c) : r(r), c(c) {}
int Position::getRow() const {
    return r;
}
int Position::getCol() const {
    return c;
}
void Position::setRow(int r) {
    this->r = r;
}
void Position::setCol(int c) {
    this->c = c;
}
string Position::str() const {
    return "(" + to_string(r) + ", " + to_string(c) + ")";
}
bool Position::isEqual(Position pos) const {
    return r == pos.getRow() && c == pos.getCol();
}

//! 3.4 Moving objects
const Position Position::npos = Position(-1, -1);
MovingObject::MovingObject(int index, const Position pos, Map *map, const string &name) 
: index(index), pos(pos), map(map), name(name){}
Position MovingObject::getCurrentPosition() const {
    return pos;
}
bool Map::isValid(const Position &pos, MovingObject *mv_obj) const {
    // Get the row and column indices of the position
    int row = pos.getRow();
    int col = pos.getCol();

    // Check if the position is within the bounds of the map
    if (row < 0 || row >= num_rows || col < 0 || col >= num_cols) {
        return false;
    }

    // Get the type of the map element at the given position
    ElementType element_type = map[row][col]->getType();

    // Check if the moving object can move to the current position based on the map element type
    switch (element_type) {
        case PATH:
            return true; // Objects can always move on a path
        case WALL:
            return false; // Objects cannot move through a wall
        case FAKE_WALL:
            // For Sherlock, FakeWall is always valid to move on
            if (dynamic_cast<Sherlock *>(mv_obj)) {
                return true;
            }
            // For Watson, FakeWall is valid if Watson's EXP meets the requirement
            else if (dynamic_cast<Watson *>(mv_obj)) {
                int required_exp = dynamic_cast<FakeWall *>(map[row][col])->getReqExp();
                int watson_exp = dynamic_cast<Watson *>(mv_obj)->exp;
                return watson_exp >= required_exp;
            }
            // For other moving objects, FakeWall is not valid
            else {
                return false;
            }
        default:
            return false; // Unknown element type
    }
}


//! 3.5 Sherlock
Sherlock::Sherlock(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp)
: MovingObject(index, init_pos, map, "Sherlock"), init_hp(init_hp), init_exp(init_exp), moving_rule(moving_rule), hp(init_hp), exp(init_exp) {}
Position Sherlock::getNextPosition() {
    if (moving_rule.empty()) {
        // No movement rule defined
        return Position::npos;
    }
    // Get the next direction from the moving rule
    char next_direction = moving_rule[0];
    // Update the moving rule for the next iteration
    moving_rule = moving_rule.substr(1) + next_direction;
    // Calculate the next position based on the direction
    int next_row = pos.getRow();
    int next_col = pos.getCol();
    switch (next_direction) {
        case 'L':
            next_col--;
            break;
        case 'R':
            next_col++;
            break;
        case 'U':
            next_row--;
            break;
        case 'D':
            next_row++;
            break;
        default:
            return Position::npos;
    }
    // Check if the next position is valid
    if (map->isValid(Position(next_row, next_col), this)) {
        return Position(next_row, next_col);
    } else {
        return Position::npos;
    }
}
void Sherlock::move() {
    Position next_pos = getNextPosition();
    if (!next_pos.isEqual(Position::npos)) {
        pos = next_pos;
    }
}
string Sherlock::str() const {
    return "Sherlock[index=" + to_string(index) + ";pos=" + pos.str() + ";moving_rule=" + moving_rule + "]";
}
//! 3.6 Watson
Watson::Watson(int index, const string & moving_rule, const Position & init_pos, Map * map, int init_hp, int init_exp) 
: MovingObject(index, init_pos, map, "Watson"), init_hp(init_hp), init_exp(init_exp), moving_rule(moving_rule), hp(init_hp), exp(init_exp) {}
string Watson::str() const {
    return "Watson[index=" + to_string(index) + ";pos=" + pos.str() + ";exp=" + to_string(exp) + "]";
}
Position Watson::getNextPosition() {
    if (moving_rule.empty()) {
        // No movement rule defined
        return Position::npos;
    }
    // Get the next direction from the moving rule
    char next_direction = moving_rule[0];
    // Update the moving rule for the next iteration
    moving_rule = moving_rule.substr(1) + next_direction;
    // Calculate the next position based on the direction
    int next_row = pos.getRow();
    int next_col = pos.getCol();
    switch (next_direction) {
        case 'L':
            next_col--;
            break;
        case 'R':
            next_col++;
            break;
        case 'U':
            next_row--;
            break;
        case 'D':
            next_row++;
            break;
        default:
            return Position::npos;
    }
    // Check if the next position is valid
    if (map->isValid(Position(next_row, next_col), this)) {
        return Position(next_row, next_col);
    } else {
        return Position::npos;
    }
}
void Watson::move() {
    Position next_pos = getNextPosition();
    if (!next_pos.isEqual(Position::npos)) {
        pos = next_pos;
    }
}
int Watson::getInitExp() const {
    return init_exp;
}

//! 3.7 Criminal
Criminal::Criminal(int index, const Position &init_pos, Map *map, Sherlock *sherlock, Watson *watson)
: MovingObject(index, init_pos, map, "Criminal"), sherlock(sherlock), watson(watson) {}
string Criminal::str() const {
    return "Criminal[index=" + to_string(index) + pos.str() + "]";
}
int Criminal::manhattanDistance(const Position &pos1, const Position &pos2) const {
    return abs(pos1.getRow() - pos2.getRow()) + abs(pos1.getCol() - pos2.getCol());
}
Position Criminal::getNextPosition() {
    const char directions[] = {'U', 'L', 'D', 'R'};
    Position next_pos = Position::npos;
    int max_distance = INT_MIN;

    for (char dir : directions) {
        Position next_candidate = pos;

        // Calculate the next candidate position based on the direction
        switch (dir) {
            case 'U':
                next_candidate.setRow(next_candidate.getRow() - 1);
                break;
            case 'L':
                next_candidate.setCol(next_candidate.getCol() - 1);
                break;
            case 'D':
                next_candidate.setRow(next_candidate.getRow() + 1);
                break;
            case 'R':
                next_candidate.setCol(next_candidate.getCol() + 1);
                break;
            default:
                continue;
        }
        // Check if the next candidate position is valid
        if (!map->isValid(next_candidate, this)) {
            continue;
        }

        // Calculate the Manhattan distance to both Sherlock and Watson
        int distance_to_sherlock = manhattanDistance(next_candidate, sherlock->getCurrentPosition());
        int distance_to_watson = manhattanDistance(next_candidate, watson->getCurrentPosition());
        int total_distance = distance_to_sherlock + distance_to_watson;

        // Update the next position if it has the greatest total distance
        if (total_distance > max_distance) {
            max_distance = total_distance;
            next_pos = next_candidate;
        }
    }

    return next_pos;
}
void Criminal::move() {
    Position next_pos = getNextPosition();
    if (!next_pos.isEqual(Position::npos)) {
        pos = next_pos;
    }
}

//! 3.8 Array of Moving Objects
ArrayMovingObject::ArrayMovingObject(int capacity) : count(0), capacity(capacity) {
    arr_mv_objs = new MovingObject*[capacity];
}
ArrayMovingObject::~ArrayMovingObject() {
    for (int i = 0; i < count; ++i) {
        delete arr_mv_objs[i];
    }
    delete[] arr_mv_objs;
}

bool ArrayMovingObject::isFull() const {
    return count == capacity;
}

bool ArrayMovingObject::add(MovingObject *mv_obj) {
    if (isFull()) {
        return false;
    }
    arr_mv_objs[count] = mv_obj;
    count++;
    return true;
}

string ArrayMovingObject::str() const {
    string result = "ArrayMovingObject[count=" + to_string(count) + ";capacity=" + to_string(capacity) + ";";
    for (int i = 0; i < count; i++) {
        result += arr_mv_objs[i]->str();
        if (i != count - 1) {
            result += ";";
        }
    }
    result += "]";
    return result;
}

//! 3.9 Program Configuration
Configuration::Configuration(const string &filepath) {
    parseFile(filepath);
}
Configuration::~Configuration() {
    delete[] arr_walls;
    delete[] arr_fake_walls;
}

string Configuration::str() const {
    stringstream ss;
    ss << "Configuration[" << endl;
    ss << "MAP_NUM_ROWS=" << map_num_rows << endl;
    ss << "MAP_NUM_COLS=" << map_num_cols << endl;
    ss << "MAX_NUM_MOVING_OBJECTS=" << max_num_moving_objects << endl;

    ss << "NUM_WALLS=" << num_walls << endl;
    ss << "ARRAY_WALLS=[";
    for (int i = 0; i < num_walls; ++i) {
        ss << "(" << arr_walls[i].getRow() << "," << arr_walls[i].getCol() << ")";
        if (i < num_walls - 1) ss << ",";
    }
    ss << "]" << endl;
}
////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////