#include <raylib.h>
#include <stddef.h>

#define DROPS_SIZE 10
#define SPAWN_TIME_MAX 5
#define SPAWN_TIME_MIN 1
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_ELIPSE_RADIUS 1600

typedef struct {
  int x;
  int y;
  float radius;
  float radiusRate;
  bool active;
} Drop;

Color WAVE_COLOR = BLUE;
Drop dropList[DROPS_SIZE];
int dropCount = 0;

// Shader de glow (pós-processamento, um único passe)
const char *glowFs = "#version 330\n"
                     "in vec2 fragTexCoord;\n"
                     "in vec4 fragColor;\n"
                     "out vec4 finalColor;\n"
                     "uniform sampler2D texture0;\n"
                     "uniform vec2 resolution;\n"
                     "uniform float intensity;\n"
                     "void main() {\n"
                     "    vec2 texel = 1.0 / resolution;\n"
                     "    vec3 color = texture(texture0, fragTexCoord).rgb;\n"
                     "    vec3 blur = vec3(0.0);\n"
                     "    float wsum = 0.0;\n"
                     "    for (int x = -3; x <= 3; x++) {\n"
                     "        for (int y = -3; y <= 3; y++) {\n"
                     "            float w = exp(-float(x*x + y*y) / 8.0);\n"
                     "            blur += texture(texture0, fragTexCoord + "
                     "vec2(float(x), float(y)) * texel * 2.0).rgb * w;\n"
                     "            wsum += w;\n"
                     "        }\n"
                     "    }\n"
                     "    blur /= wsum;\n"
                     "    vec3 glow = blur * intensity;\n"
                     "    finalColor = vec4(color + glow, 1.0);\n"
                     "}\n";

Drop NewDrop() {
  int x = GetRandomValue(-300, WINDOW_WIDTH);
  int y = GetRandomValue(-300, WINDOW_HEIGHT);
  int radius = GetRandomValue(10, 500);
  float radiusRate = (float)GetRandomValue(5, 140) / 10;
  return (Drop){x, y, radius, radiusRate, true};
}

void SpawnDrop(Drop *drops, int *dropCount) {
  for (int i = 0; i < DROPS_SIZE; i++) {
    if (!drops[i].active) {
      drops[i] = NewDrop();
      (*dropCount)++;
      return;
    }
  }
}

void UpdateDrop(Drop *d) {
  d->radius += d->radiusRate;
  if (d->radius > MAX_ELIPSE_RADIUS) {
    d->active = false;
  }
}

int CountActiveDrops(Drop *drops) {
  int count = 0;
  for (int i = 0; i < DROPS_SIZE; i++) {
    if (drops[i].active)
      count++;
  }
  return count;
}

void DrawEllipseLinesThick(int cx, int cy, float rh, float rv, Color color,
                           float thickness) {
  float half = thickness * 0.5f;
  for (float t = -half; t < half; t += 1.0f) {
    DrawEllipseLines(cx, cy, rh + t, rv + t, color);
  }
}

void DrawDrop(Drop d) {
  DrawEllipseLinesThick(d.x, d.y, d.radius, d.radius / 2, WAVE_COLOR, 10.0f);
}

void Setup() {
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Fossil Waves");
  SetTargetFPS(60);
}

int main(void) {
  Setup();

  double spawnTime = SPAWN_TIME_MIN;
  float dt;

  RenderTexture2D canvas = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

  // Carrega o shader de glow e configura os uniforms
  Shader glow = LoadShaderFromMemory(NULL, glowFs);
  if (glow.id == 0) {
    TraceLog(LOG_WARNING, "Glow shader nao carregou corretamente");
  }

  float resolution[2] = {(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT};
  float glowIntensity = 2.0f;
  int resLoc = GetShaderLocation(glow, "resolution");
  int intLoc = GetShaderLocation(glow, "intensity");
  SetShaderValue(glow, resLoc, resolution, SHADER_UNIFORM_VEC2);
  SetShaderValue(glow, intLoc, &glowIntensity, SHADER_UNIFORM_FLOAT);

  while (!WindowShouldClose()) {
    BeginDrawing();
    dt = GetFrameTime();
    spawnTime -= dt;

    if (spawnTime <= 0) {
      SpawnDrop(dropList, &dropCount);
      spawnTime = GetRandomValue(SPAWN_TIME_MIN, SPAWN_TIME_MAX);
    }

    // Desenha a elipse nova na folha (acumula, nunca é apagada)
    BeginTextureMode(canvas);

    for (int i = 0; i < DROPS_SIZE; i++) {
      Drop *d = &dropList[i];
      if (!d->active)
        continue;
      UpdateDrop(d);
      DrawDrop(*d);
    }

    dropCount = CountActiveDrops(dropList);

    DrawRectangleRec((Rectangle){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
                     (Color){0, 0, 0, 10});
    EndTextureMode();

    // Mostra a folha na tela com o efeito de glow (Y negativo corrige o flip)
    BeginShaderMode(glow);
    DrawTextureRec(
        canvas.texture,
        (Rectangle){0, 0, (float)WINDOW_WIDTH, (float)-WINDOW_HEIGHT},
        (Vector2){0, 0}, WHITE);
    EndShaderMode();

    EndDrawing();
  }

  UnloadShader(glow);
  UnloadRenderTexture(canvas);
  CloseWindow();
  return 0;
}
