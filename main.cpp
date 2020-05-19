#include "common.h"
#include "cmath"
#include "vector"
#include "string"

bool Init();
void CleanUp();
void Run();
void Draw();
void Generate(int x, int y);
bool VisitedNeighbors(int x, int y); //checks if visited all cells neighbors
vector<char> UnvisitedNeighbors(int x, int y); //returns cells unvisited neighbors
double ScaleNum(double n, double minN, double maxN, double min, double max);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int wallHeight = 14;
int screenWidth = 1000;
int screenHeight = 500;
int windowSize = 500;
double originx = 250;
double originy = 250;
double prevoriginx;
double prevoriginy;
double moveSpeed = .5;
double numarrays = 2000;
double viewarrays = 500;
double viewdirection = 0;
double viewwidth = 70;
int gridSize = 25;
bool l, r, w, a, s, d;
string mode = "visualize";

vector<vector<int>> walls;
vector<vector<vector<double>>> grid;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));

    int vectorWidth = windowSize/gridSize; //X
    int vectorHeight = windowSize/gridSize; //Y

    vector<vector<double>> y(vectorHeight, {1, 1, 1, 1, 0}); //U D L R walls present, 0 for not visited
    for(int i = 0; i < vectorWidth; i++){
        grid.push_back(y);
    }
    Generate(0, 0); 

    for(int i = 0; i < grid.size(); i++){
        for(int j = 0; j < grid[i].size(); j++){
            if(grid[i][j][0] == 1)
                walls.push_back({i*gridSize, j*gridSize, i*gridSize+gridSize, j*gridSize});
            if(grid[i][j][2] == 1)
                walls.push_back({i*gridSize, j*gridSize, i*gridSize, j*gridSize+gridSize});
        }
    }

    walls.push_back({500, -1, 500, 500});
    walls.push_back({-1, 500, 500, 500});

    while (gameLoop)
    {   
        cout << originx << "\t" << originy << endl;
        if(mode == "visualize" || mode == "fullvisualize"){
            int mx;
            int my;
            SDL_GetMouseState(&mx, &my);
            originx = mx;
            originy = my;
        }
        else{
            prevoriginx = originx;
            prevoriginy = originy;
            if(w == true){
                originx += cos(viewdirection) * moveSpeed;
                originy += sin(viewdirection) * moveSpeed;
            }
            if(s == true){
                originx -= cos(viewdirection) * moveSpeed;
                originy -= sin(viewdirection) * moveSpeed;
            }
            if(a == true){
                originx += cos(viewdirection-M_PI/2) * moveSpeed;
                originy += sin(viewdirection-M_PI/2) * moveSpeed;
            }
            if(d == true){
                originx += cos(viewdirection+M_PI/2) * moveSpeed;
                originy += sin(viewdirection+M_PI/2) * moveSpeed;
            }
            double mininterx = 1000;
            double minintery = 1000;
            double interx, intery;
            for(int j = 0; j < walls.size(); j++){ //calcs if movement goes through walls and backtracks if so
                double denom = (originx - prevoriginx)*(walls[j][1] - walls[j][3]) - (originy - prevoriginy)*(walls[j][0] - walls[j][2]);
                if(denom != 0){
                    double t = ((originx - walls[j][0])*(walls[j][1] - walls[j][3]) - (originy - walls[j][1])*(walls[j][0] - walls[j][2])) / denom;
                    double u = -1*((originx - prevoriginx)*(originy - walls[j][1]) - (originy - prevoriginy)*(originx - walls[j][0])) / denom;
                    if((t >= 0.0 && t <= 1.0) && (u >= 0.0 && u <=1.0)){
                        interx = originx + t*(prevoriginx - originx);
                        intery = originy + t*(prevoriginy - originy);
                        if(sqrt(pow(interx - originx, 2) + pow(intery - originy, 2)) < sqrt(pow(mininterx - originx, 2) + pow(minintery - originy, 2))){
                            mininterx = interx;
                            minintery = intery;
                        }
                    }
                }
            }
            if(mininterx != 1000){
                originx = prevoriginx;
                originy = prevoriginy;
            }
        }
        if(originx > 498)
            originx = 498;
        if(l == true){
            viewdirection -= M_PI/180;
            if(viewdirection < 0)
                viewdirection += 2*M_PI;
        }
        if(r == true){
            viewdirection += M_PI/180;
            if(viewdirection > 2*M_PI)
                viewdirection -= 2*M_PI;
        }
        Draw();
        SDL_RenderPresent(renderer);
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_UP:
                        wallHeight++;
                        break;
                    case SDLK_DOWN:
                        wallHeight--;
                        break;
                    case SDLK_LEFT:
                        l = true;
                        break;
                    case SDLK_RIGHT:
                        r = true;
                        break;
                    case SDLK_w:
                        w = true;
                        break;
                    case SDLK_s:
                        s = true;
                        break;
                    case SDLK_a:
                        a = true;
                        break;
                    case SDLK_d:
                        d = true;
                        break;
                    case SDLK_SPACE:
                        if(mode == "visualize")
                            mode = "render";
                        else if(mode == "fullvisualize")
                            mode = "visualize";
                        else
                            mode = "fullvisualize";
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    case SDLK_LEFT:
                        l = false;
                        break;
                    case SDLK_RIGHT:
                        r = false;
                        break;
                    case SDLK_w:
                        w = false;
                        break;
                    case SDLK_s:
                        s = false;
                        break;
                    case SDLK_a:
                        a = false;
                        break;
                    case SDLK_d:
                        d = false;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void Draw(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < walls.size(); i++){ //renders walls in top down view
        SDL_RenderDrawLine(renderer, walls[i][0], walls[i][1], walls[i][2], walls[i][3]);
    }
    if(mode == "fullvisualize"){    //top down view with rays in all directions
        for(double i = 0; i < 2*M_PI-.00001; i+=(2*M_PI)/numarrays){
            double x = cos(i) * 500 * 2 + originx;
            double y = sin(i) * 500 * 2 + originy;
            double mininterx = 1000;
            double minintery = 1000;
            double interx;
            double intery;
            for(int j = 0; j < walls.size(); j++){
                double denom = (originx - x)*(walls[j][1] - walls[j][3]) - (originy - y)*(walls[j][0] - walls[j][2]);
                if(denom != 0){
                    double t = ((originx - walls[j][0])*(walls[j][1] - walls[j][3]) - (originy - walls[j][1])*(walls[j][0] - walls[j][2])) / denom;
                    double u = -1*((originx - x)*(originy - walls[j][1]) - (originy - y)*(originx - walls[j][0])) / denom;
                    if((t >= 0.0 && t <= 1.0) && (u >= 0.0 && u <=1.0)){
                        interx = originx + t*(x - originx);
                        intery = originy + t*(y - originy);
                        if(sqrt(pow(interx - originx, 2) + pow(intery - originy, 2)) < sqrt(pow(mininterx - originx, 2) + pow(minintery - originy, 2))){
                            mininterx = interx;
                            minintery = intery;
                        }
                    }
                }
            }
            if(mininterx == 1000 && minintery == 1000){}
            else
                SDL_RenderDrawLine(renderer, originx, originy, static_cast<int>(mininterx), static_cast<int>(minintery));
        }
    }

    vector<vector<double>> dist;
    for(double i = 0; i < viewarrays/2; i++){ //calsc intersect of visions rays with walls
        double ang = viewdirection + (M_PI/(360/viewwidth))/(viewarrays/2) * i;
        if(ang > 2*M_PI)
            ang -= 2*M_PI;
        double x = cos(ang) * 500 * 2 + originx;
        double y = sin(ang) * 500 * 2 + originy;
        double mininterx = 1000;
        double minintery = 1000;
        double interx;
        double intery;
        for(int j = 0; j < walls.size(); j++){
            double denom = (originx - x)*(walls[j][1] - walls[j][3]) - (originy - y)*(walls[j][0] - walls[j][2]);
            if(denom != 0){
                double t = ((originx - walls[j][0])*(walls[j][1] - walls[j][3]) - (originy - walls[j][1])*(walls[j][0] - walls[j][2])) / denom;
                double u = -1*((originx - x)*(originy - walls[j][1]) - (originy - y)*(originx - walls[j][0])) / denom;
                if((t >= 0.0 && t <= 1.0) && (u >= 0.0 && u <=1.0)){
                    interx = originx + t*(x - originx);
                    intery = originy + t*(y - originy);
                    if(sqrt(pow(interx - originx, 2) + pow(intery - originy, 2)) < sqrt(pow(mininterx - originx, 2) + pow(minintery - originy, 2))){
                        mininterx = interx;
                        minintery = intery;
                    }
                }
            }
        }
        if(mininterx == 1000 && minintery == 1000){}
        else
            SDL_RenderDrawLine(renderer, originx, originy, static_cast<int>(mininterx), static_cast<int>(minintery));
        dist.push_back({250+i, sqrt(pow(mininterx - originx, 2) + pow(minintery - originy, 2))});
    }
    for(double i = 1; i < viewarrays/2 + 1; i++){
        double ang = viewdirection - (M_PI/(360/viewwidth))/(viewarrays/2) * i;
        if(ang < 0)
            ang += 2*M_PI;
        double x = cos(ang) * 500 * 2 + originx;
        double y = sin(ang) * 500 * 2 + originy;
        double mininterx = 1000;
        double minintery = 1000;
        double interx;
        double intery;
        for(int j = 0; j < walls.size(); j++){
            double denom = (originx - x)*(walls[j][1] - walls[j][3]) - (originy - y)*(walls[j][0] - walls[j][2]);
            if(denom != 0){
                double t = ((originx - walls[j][0])*(walls[j][1] - walls[j][3]) - (originy - walls[j][1])*(walls[j][0] - walls[j][2])) / denom;
                double u = -1*((originx - x)*(originy - walls[j][1]) - (originy - y)*(originx - walls[j][0])) / denom;
                if((t >= 0.0 && t <= 1.0) && (u >= 0.0 && u <=1.0)){
                    interx = originx + t*(x - originx);
                    intery = originy + t*(y - originy);
                    if(sqrt(pow(interx - originx, 2) + pow(intery - originy, 2)) < sqrt(pow(mininterx - originx, 2) + pow(minintery - originy, 2))){
                        mininterx = interx;
                        minintery = intery;
                    }
                }
            }
        }
        if(mininterx == 1000 && minintery == 1000){}
        else
            SDL_RenderDrawLine(renderer, originx, originy, static_cast<int>(mininterx), static_cast<int>(minintery));
        dist.push_back({250-i, sqrt(pow(mininterx - originx, 2) + pow(minintery - originy, 2))});
    }
    for(int i = 0; i < dist.size(); i++){ //renders walls in right side of screen
        double color = 255*1/((dist[i][1] * cos(abs(dist[i][0] - 250) * (M_PI/(360/viewwidth))/(viewarrays/2)))/85); //screenHeight * 90/dist[i][1];
        if(color > 240)
            color = 240;
        SDL_SetRenderDrawColor(renderer, color, color, color, 255);
        double distance = dist[i][1] * cos(abs(dist[i][0] - 250) * (M_PI/(360/viewwidth))/(viewarrays/2));
        double height = 500*1/(distance/wallHeight);
        if(height > 500)
            height = 500;

        pos.x = dist[i][0] + 500;
        pos.y = 250 - height/2;
        pos.w = 1;
        pos.h = height;
        SDL_RenderFillRect(renderer, &pos);
    }
}

void Generate(int x, int y){ //recursive algorithum
    grid[x][y][4] = 1;
    while(VisitedNeighbors(x, y) == false){
        vector<char> UVN = UnvisitedNeighbors(x, y);
        int index = rand() % UVN.size(); //chooses cell to do next
        char cell = UVN[index];

        if(cell == 'U'){
            grid[x][y][0] = 0;
            grid[x][y-1][1] = 0;
            Generate(x, y-1);
        }
        else if(cell == 'D'){
            grid[x][y][1] = 0;
            grid[x][y+1][0] = 0;
            Generate(x, y+1);
        }
        else if(cell == 'L'){
            grid[x][y][2] = 0;
            grid[x-1][y][3] = 0;
            Generate(x-1, y);
        }
        else if(cell == 'R'){
            grid[x][y][3] = 0;
            grid[x+1][y][2] = 0;
            Generate(x+1, y);
        }
    }
}

bool VisitedNeighbors(int x, int y){ //checks adjacent cells, returns false if a cell is unvisited
    int offset = y-1;
    if(y-1 >= 0){
        if(grid[x][y-1][4] == 0)
            return false;
    }
    offset = y+1;
    if(offset < grid[x].size()){
        if(grid[x][y+1][4] == 0)
            return false;
    }

    offset = x-1;
    if(offset >= 0){
        if(grid[x-1][y][4] == 0)
            return false;
    }
    offset = x+1;
    if(x+1 < grid.size()){
        if(grid[x+1][y][4] == 0)
            return false;
    }

    return true;
}

vector<char> UnvisitedNeighbors(int x, int y){ //returns vector holding direction of unvisited cells
    vector<char> unvisited; //holds the direction of the unvisited cells, U D L R
    int offset = y-1;
    
    if(y-1 >= 0){
        if(grid[x][y-1][4] == 0)
            unvisited.push_back('U');
    }
    offset = y+1;
    if(offset < grid[x].size()){
        if(grid[x][y+1][4] == 0)
            unvisited.push_back('D');
    }

    offset = x-1;
    if(offset >= 0){
        if(grid[x-1][y][4] == 0)
            unvisited.push_back('L');
    }
    offset = x+1;
    if(x+1 < grid.size()){
        if(grid[x+1][y][4] == 0)
            unvisited.push_back('R');
    }

    return unvisited;
}

double ScaleNum(double n, double minN, double maxN, double min, double max){
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}