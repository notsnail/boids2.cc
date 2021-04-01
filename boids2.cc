// (c) i am bad at usernames
// 3d port of 2d boids :)
// 2021 i am bad at usernames
#include <iostream>
#include <vector>

#include <random>

// math!
#include <cmath>

// vector math
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

// graphics
#include <raylib.h>

class Boid
{
public:
    Boid(glm::vec3 n_position, glm::vec3 n_velocity) : position(n_position), velocity(n_velocity)
    {
        id = BoidCount;
        BoidCount++;
    }

    ~Boid()
    {
        BoidCount--;
    }

    void Update(std::vector<Boid>& boids)
    {
        glm::vec3 acceleration = Avoid(boids) + Align(boids) + Cohere(boids) + Confine() * 5.0f;

        velocity += acceleration;
        velocity = glm::normalize(velocity) * MaxSpeed;
        position += velocity;
    }

    void Draw()
    {
        DrawSphere({ position.x, position.y, position.z }, 0.05f, { 0xE6, 0xBC, 0xCD, 0xFF });
    }

    void Draw(Model m)
    {
        #define TO_DEGREES(x) x * (180 / PI)

        float angle_radians = acos(glm::dot(position, velocity) / (glm::length(position) * glm::length(velocity)));
        float angle = TO_DEGREES(angle_radians);

        DrawModelEx(m, 
            { position.x, position.y, position.z },
            { position.x, position.y, position.z },
            angle,
            { 1.0f, 1.0f, 1.0f },
            { 0xE6, 0xBC, 0xCD, 0xFF }
        );
    }
public: // physical-related functions
    glm::vec3 GetPosition()
    {
        return position;
    }

    glm::vec3 GetVelocity()
    {
        return velocity;
    }
public: // id stuff
    unsigned int GetID()
    {
        return id;
    }
public: // boid universal options
    // physics options
    inline static float MaxSpeed = 0.05f;
    inline static float MaxSteerForce = 0.001f;
    inline static float DesiredSeparation = 1.5f;
    inline static float NearbyValue = 5.0f;

    inline static glm::vec3 Bounds[] = {
        glm::vec3(-25.0f, -25.0f, -25.0f),
        glm::vec3(25.0f, 25.0f, 25.0f)
    };

    // visual help config
    // inline static bool DrawLineOfSight = false;
    // inline static bool DrawAveragePosition = false;
private: // rules of boids <3
    glm::vec3 Avoid(std::vector<Boid>& boids)
    {
        glm::vec3 steer;
        float count = 0;

        for (auto &boid : boids)
        {
            if (id != boid.GetID())
            {
                float distance = abs(glm::length(position - boid.GetPosition()));
                
                if (distance <= DesiredSeparation) // if within bounds then separate
                {
                    glm::vec3 difference = position - boid.GetPosition();
                    difference = glm::normalize(difference);
                    difference /= distance;
                    steer += difference;
                    count++;
                }
            }
        }
        
        if (count > 0) steer /= count;
       
        if (glm::length(steer) > 0)
        {
            steer = glm::normalize(steer);
            steer *= MaxSpeed;
            steer -= velocity;
            steer = glm::normalize(steer) * MaxSteerForce;
        }
        return steer;
    }

    glm::vec3 Align(std::vector<Boid>& boids)
    {
        glm::vec3 sum;
        float count = 0;
        
        for (auto &boid : boids)
        {
            if (id != boid.GetID())
            {
                float distance = abs(glm::length(position - boid.GetPosition()));
                
                if (distance < NearbyValue)
                {
                    sum += boid.GetVelocity();
                    count++;
                }
            }
        }

        if (count > 0)
        {
            sum /= count;

            sum = glm::normalize(sum);
            sum *= MaxSpeed;

            glm::vec3 steer = sum - velocity;
            steer = glm::normalize(steer) * MaxSteerForce;
            return steer;
        }
        else
        {
            return glm::vec3(0, 0, 0);
        }
    }

    glm::vec3 Cohere(std::vector<Boid>& boids)
    {
        glm::vec3 sum;
        float count = 0;

        for (auto &boid : boids)
        {
            if (id != boid.GetID())
            {
                float distance = abs(glm::length(position - boid.GetPosition()));
                if (distance < NearbyValue)
                {
                    glm::vec3 other_pos = boid.GetPosition();
                    DrawLine3D({ position.x, position.y, position.z }, { other_pos.x, other_pos.y, other_pos.z }, RED);
                    
                    sum += boid.GetPosition();
                    count++;
                }
            }
        }

        if (count > 0)
        {
            sum /= count;
            // if (DrawAveragePosition) DrawCircle(sum.x, sum.y, 2.0f, BLUE);
            return seek_to(sum);
        }
        else
        {
            return glm::vec3(0, 0, 0);
        }
    }

    glm::vec3 Confine()
    {
        glm::vec3 steer;
        // check lower
        if (position.x < Bounds[0].x)
        {
            steer.x += 10.0f;
        }
        if (position.y < Bounds[0].y)
        {
            steer.y += 10.0f;
        }
        if (position.z < Bounds[0].z)
        {
            steer.y += 10.0f;
        }

        // check upper
        if (position.x > Bounds[1].x)
        {
            steer.x -= 10.0f;
        }
        if (position.y > Bounds[1].y)
        {
            steer.y -= 10.0f;
        }
        if (position.z > Bounds[1].z)
        {
            steer.y -= 10.0f;
        }

        steer = glm::normalize(steer);
        steer *= MaxSpeed;
        steer -= velocity;
        steer = glm::normalize(steer) * MaxSteerForce;
    
        return steer;
    }
private: // utilities
    glm::vec3 seek_to(glm::vec3 target)
    {
        glm::vec3 desired = target - position;

        desired = glm::normalize(desired);
        desired *= MaxSpeed;

        glm::vec3 steer = desired - velocity;
        steer = glm::normalize(steer) * MaxSteerForce;
      
        return steer;
    }
private: // physical characteristics
    glm::vec3 velocity;
    glm::vec3 position;
private: // id Stuff
    unsigned int id;
public:
    inline static unsigned int BoidCount = 0;
};

class BoidFlock
{
public:
    BoidFlock(unsigned int count)
    {
        std::random_device rnd;
        std::mt19937 gen(rnd());

        std::uniform_real_distribution<> v_rand(-Boid::MaxSpeed, Boid::MaxSpeed);

        boids.push_back(Boid(glm::vec3(0, 3, 0), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(3, 0, 0), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(0, 0, 3), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(0, 3, 3), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(3, 3, 0), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(0, -3, 0), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(-3, 0, 0), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(0, 0, -3), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(0, -3, -3), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
        boids.push_back(Boid(glm::vec3(-3, -3, 0), glm::vec3(v_rand(gen), v_rand(gen) , v_rand(gen))));
    }

    ~BoidFlock()
    {
        boids.empty();
    }

    void Update()
    {
        for (auto &boid : boids)
        {
            boid.Update(boids);
        }
    }

    void Draw(Model m)
    {
        for (auto &boid : boids)
        {
            boid.Draw(m);
        }
    }

    int Size() 
    {
        return boids.size();
    }

    glm::vec3 LookAtCentre()
    {
        return boids[0].GetPosition();
    }
private:
    std::vector<Boid> boids;
};

int main(int argc, char* argv[])
{
    BoidFlock* flock = new BoidFlock(16);

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(640, 480, "boids2.cc");
    InitAudioDevice();

    // load assets
    Mesh cylinder_mesh = GenMeshCylinder(0.2f, 0.5f, 4);
    Model cylinder = LoadModelFromMesh(cylinder_mesh);

    Music music = LoadMusicStream("driftveil.mp3");
    SetMusicVolume(music, 0.25f);
    PlayMusicStream(music);

    SetTargetFPS(60);
    SetExitKey(0);

    Camera camera = { 
        .position = { 5.0f, 4.0f, 3.0f },
        .target = { 0.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fovy = 90.0f,
        .type = CAMERA_PERSPECTIVE
    };
    SetCameraMode(camera, CAMERA_CUSTOM);

    
    bool paused = false;
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_ESCAPE))
        {
            paused = !paused;
        }

        // camera handling code
        if (IsKeyDown(KEY_W))
        {
            
        }

        if (IsKeyDown(KEY_S))
        {
            
        }

        if (IsKeyDown(KEY_A))
        {

        }

        if (IsKeyDown(KEY_D))
        {
            
        }

        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            camera.position.y -= 0.1f;
        }

        if (IsKeyDown(KEY_SPACE))
        {
            camera.position.y += 0.1f;
        }

        UpdateCamera(&camera);
        
        BeginDrawing();
            ClearBackground({ 0x01, 0x2A, 0x36, 0xFF }); // #012A36
            BeginMode3D(camera);
                if (!paused)
                {
                    flock->Update();
                }

                DrawGrid(100, 0.2f);
                flock->Draw(cylinder);
            EndMode3D();
            
            if (paused)
            {
                DrawText("PAUSED", 5, 5, 10, RED);
            }
            DrawFPS(5, GetScreenHeight() - 15);
            DrawText(TextFormat("Active Boids: %i", flock->Size()), 5, GetScreenHeight() - 30, 10, RED);
        EndDrawing();
    }

    // unload assets
    UnloadModel(cylinder);
    UnloadMusicStream(music);

    CloseAudioDevice();
    CloseWindow();

    delete flock;
}