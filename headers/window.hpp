#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <array>
#include <random>

#include "abcgOpenGL.hpp"
#include "bgmodel.hpp"
#include "model.hpp"
#include "camera.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onEvent(SDL_Event const &event) override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  std::default_random_engine m_randomEngine;

  inline static const glm::vec4 m_Green{0.00f, 0.85f, 0.22f, 1.00f};
  inline static const glm::vec4 m_Red{1.00f, 0.00f, 0.00f, 1.00f};


  ImFont *m_font{};
  glm::ivec2 m_viewportSize{};

  enum class GameStatus {PLAYING, ON_MENU};
  GameStatus m_gameStatus{GameStatus::ON_MENU};

  enum class Forms {SQUARE, SPHERE};
  std::array <glm::vec4, 2> m_colors{m_Green, m_Red};
  float m_timeAcc{1.0f};
  float m_timeAccFOV{0.0f};
  bool m_reduceFOV{false};

  BgModel m_model;
  Model m_modelSphere;
  Model m_modelSquare;
  Camera m_camera;

  glm::ivec2 m_mousePosition{};

  //Model m_modelTargets;
  int m_trianglesToDraw{};

  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};

  struct Materials {
    glm::vec4 Ka;
    glm::vec4 Kd;
  };
  std::array<Materials, 4> m_targetsMaterials = {{
      {{0.06f, 0.0f, 0.0f, 1.0f}, {0.8f, 0.1f, 0.1f, 0.5f}}, // vermelho
      {{0.56f, 0.08f, 0.6f, 1.0f}, {0.11f, 0.2f, 0.2f, 0.5f}}, // roxo
      {{0.11f, 0.05f, 0.0f, 1.0f}, {0.71f, 0.6f, 0.2f, 0.15f}}, // amarelo
      {{0.2f, 0.002f, 0.2f, 1.0f}, {0.00f, 0.85f, 0.22f, 0.5f}}  // Quarto material
  }};


  struct Star {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };
  // cena 3D formada por 500 estrelas
  std::array<Star, 500> m_stars;
  
  struct Alvos {
    glm::vec3 m_positionTarget{};
    glm::vec3 m_scaleTarget{0.25f};
    glm::vec3 m_rotationAxis{};
    bool m_mouseInside{false};
    bool m_hit{false};
    bool m_alreadyComputePoint{false};
  };

  int m_faseAtual{0};


  std::array<glm::vec3, 4> m_targetScreenPos{
      glm::vec3{0.25f, 0.25f, -1.0f},
      glm::vec3{-0.25f, 0.25f, -1.0f},
      glm::vec3{-0.25f, -0.25f, -1.0f},
      glm::vec3{0.25f, -0.25f, -1.0f}
  };

  std::array<Alvos, 4> m_alvos;
  int m_totalPoints{0};

  struct Fases{
    Forms m_targetForm;
    glm::vec4  m_targetColor; // verde ou vermelho (atira nesse ou nao)
    std::array<Forms, 4> m_targetForms;
    int m_points;
  };

  std::array<Fases, 5> m_fases;


  // angulo em radianos que rotaciona cada cubo (cada cubo tem seu proprio eixo
  // de rotacao)
  float m_angle{};

  // iniciadas como identidade, mas alteradas em onCreate e onPaint
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
    
  // angulo de abertura vertical de visao, slider vai alterar
  float *m_FOV = &m_camera.m_FOV;

  GLuint m_program{};
  GLuint m_programForms{};


  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f};
  float m_shininess{25.0f};

  void randomizeStar(Star &star); // gera posicao da estrela aleatoriamente
  void detectTargetPosition(); // funcao que detecta se mouse esta acima dos alvos
  void computePoints(); // funcao que computa os pontos, dependendo do alvo da rodada
  void setupPhases(std::uniform_int_distribution<int> dist); // seta um novo conjunto de fases
};

#endif
