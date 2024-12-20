# (3) Star Shooter -- (19/12/2024) - Grupo 3

## Nome dos integrantes

   - Pedro de Luca Occulate Serra
   - Gabriela Sampaio Magiri
   - Gustavo da Silva Bocato

## Introdução

**Star Shooter** é um jogo interativo desenvolvido em C++ utilizando a biblioteca ABCg. Neste jogo, cubos e esferas são gerados aleatoriamente na tela, e o jogador deve clicar nas formas corretas para acumular pontos. O cenário de fundo apresenta um efeito de starfield com esferas que se movem em direção à câmera, criando uma experiência imersiva e dinâmica. A proposta desse jogo é ser um jogo de atenção, onde o jogador precisa clicar nas formas corretas. A ideia é exercitar a atenção, uso do mouse e coordenação.

# Guia de utilização

## Instalação

### Pré-requisitos

- **C++ Compiler:** Compatível com C++17 ou superior.
- **CMake:** Versão 3.10 ou superior.
- **Biblioteca ABCg:** Certifique-se de ter a biblioteca ABCg instalada.

### Passos de Instalação

1. **Clone o Repositório:**
    Clone o repositorio do projeto dentro da pasta "examples" da abcg
   ```bash
   git clone https://github.com/peserra/starshooter_v2
   cd starshooter
   ```

2. **Altere o arquivo CmakeList do diretorio examples:**
   Altere o arquivo CmakeList.txt para compilar o projeto incluindo a linha
   ```bash
   add_subdirectory(starshooter_v2)
   ```

3. **Compile o Projeto:**
   No diretorio abcg, execute
   ```bash
   ./build.sh
   ```

4. **Execute o Jogo:**
   No diretorio abcg, execute
   ```bash
    ./build/bin/starshooter/starshooter
   ```

## Como jogar

 Observe o visor no lado inferior direito: Ele indica qual forma você precisa clicar. As formas são apresentadas no visor por meio de ícones:
     -  Quadrado verde: Clique nos cubos;
     -  Círculo verde: Clique nas esferas;
     -  Quadrado vermelho: Clique nas esferas;
     -  Círculo vermelho: Clique nos cubos.

# Projeto e desenvolvimento

## Funcionalidades

- **Geração Aleatória de Formas:** Cubos e esferas aparecem aleatoriamente na tela em diferentes posições e tamanhos, em cada fase.
- **Interação do Jogador:** Clique nas formas corretas para ganhar pontos.
- **Efeito de Starfield:** Fundo animado com esferas que se movem em direção à câmera, simulando um campo de estrelas.
- **Pontuação:** Sistema de pontuação que recompensa acertos e penaliza erros.
- **Gráficos 3D:** Renderização de formas em 3D para uma melhor experiência visual.
- **Iluminação:** As formas renderizadas são iluminadas.

## Detalhes do código

Para a janela vamos analisar os seguintes métodos:

   - onEvent()
   - randomizeStar()
   - setupPhases()
   - onUpdate()
   - onPaint()
   - onPaintUI()
   - computePoints()

O onEvent() é chamado para lidar com eventos do SDL, no nosso caso foi sobrescrito para lidar com movimento do mouse e clicks do mouse. Atribuindo a uma variável global m_mousePosition os valores das coordenadas do mouse, sempre que o mesmo for movido. Caso o evento seja um click do mouse então se checa o mouse está dentro do hitbox dos alvos (as esferas ou os cubos) para se setar o valor de alvo.m_hit como true, marcando que o alvo foi acertado.

```cpp
void Window::onEvent(SDL_Event const &event) {

  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      for (auto &alvo : m_alvos) {
        if (alvo.m_mouseInside) {
          alvo.m_hit = true;
        }
      }
    }
  }
  if (event.type == SDL_MOUSEMOTION) {
    SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
  }
}
```

O método randomizeStar() gera coordenadas centrais e eixos de rotação aleatórios paras as esferas do background. Já o método setupPhases() faz o mesmo para os 4 alvos em tela e o alvo de referência no canto inferior direito.

```cpp
void Window::randomizeStar(Star &star) {
  // Random position: x and y in [-20, 20), z in [-100, 0)
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);
  star.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

  // Random rotation axis
  star.m_rotationAxis = glm::sphericalRand(1.0f);
}

void Window::setupPhases(std::uniform_int_distribution<int> distribuicao) {
  // para cada fase
  for (auto i = 0; i < (int)m_fases.size(); i++) {
    auto &fase = m_fases[i];
    fase.m_points = 0; // pontos iniciais adquiridos naquela fase

    // preenche quais formas terao naquela fase
    for (auto j = 0; j < (int)fase.m_targetForms.size(); j++) {
      int numero = distribuicao(m_randomEngine) % 2;
      if (numero == 0) {
        fase.m_targetForms[j] = Forms::SQUARE;
      } else {
        fase.m_targetForms[j] = Forms::SPHERE;
      }
    }

    // alvo é uma forma aleatoria dos alvos criados na fase
    // garante que sempre vai ter um alvo valido
    int selecAlvo =
        distribuicao(m_randomEngine) % (int)fase.m_targetForms.size();
    fase.m_targetForm = fase.m_targetForms[selecAlvo];

    // seleciona cor aleatoria entre verde e vermelho
    int selecCor = distribuicao(m_randomEngine) % 2;
    fase.m_targetColor = m_colors[selecCor];
  }
}
```

O onUpdate() é responsável por atualizar o estado da janela e seus elementos a cada quadro. Ele incrementa o tempo acumulado para alternar a fase do jogo a cada 5 segundos, realiza as transições de fov da câmera entre 170 para 70 (na animação de início de uma fase). As estrelas na cena têm suas posições incrementadas ao longo do eixo Z, retornando a uma posição aleatória com coordenada z = -100 quando passam pela câmera, criando um efeito de movimento contínuo. Esse método garante a atualização contínua da lógica e dos elementos gráficos do jogo. Além disso, ele chama o método detectTargetPosition e o computePoints(). O primeiro calcula as coordenadas dos alvos na tela com base nas transformações da câmera, determina o raio aparente dos alvos, e verifica se o cursor do mouse está dentro dos limites de cada alvo, atualizando o estado correspondente (m_mouseInside). O segundo atualiza a pontuação do momento. A cada mudança de fase, um novo Ka e Kd são chamados
aleatoriamente, para colorir de maneira aleatoria as formas alvo.

 ```cpp
void Window::onUpdate() {
  // Increase angle by 90 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  if (m_reduceFOV) {
    m_timeAccFOV += deltaTime;   // Acumula tempo
    float const duration = 0.5f; // Duração da transição em segundos
    if (m_timeAccFOV <= duration) {
      // Interpola o FOV de 170 para 70
      float const t = m_timeAccFOV / duration; // Normaliza entre 0 e 1
      m_camera.m_FOV = glm::mix(170.0f, 70.0f, t);
    } else {
      // Garante que o FOV finalize em 70 e termina a transição
      m_camera.m_FOV = 70.0f;
      m_reduceFOV = false; // Finaliza a transição
    }
  }

  if (m_gameStatus == GameStatus::PLAYING) {
    // jogo acabou
    if (m_faseAtual == (int)m_fases.size()) {
      m_faseAtual = 0;
      m_camera.m_FOV = 170.0f;
      m_timeAccFOV = 0;
      m_reduceFOV = false;
      m_gameStatus = GameStatus::ON_MENU;
        std::uniform_int_distribution<int> distribuicao(0, 1000);

      setupPhases(distribuicao);
    }
    
    // a cada 5s vai trocar de alvos na tela
    m_timeAcc += deltaTime;
    if (m_timeAcc >= 5.0f) {
      for (auto &a : m_alvos) {
        // reseta os alvos quando muda de fase
        a.m_hit = false;
        a.m_alreadyComputePoint = false;
      }

      std::uniform_int_distribution<int> distr(0,3000);

      int idxMat = distr(m_randomEngine) % (int)m_targetsMaterials.size();
      m_Ka = m_targetsMaterials[idxMat].Ka;
      m_Kd = m_targetsMaterials[idxMat].Kd;

      m_faseAtual++;
      m_timeAcc = 0.0f; // Reinicia o acumulador
    }
  }

  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.computeProjectionMatrix(m_viewportSize);

  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // Update stars
  for (auto &star : m_stars) {
    // Increase z by 10 units per second
    star.m_position.z += deltaTime * 10.0f;

    // If this star is behind the camera, select a new random position &
    // orientation and move it back to -100
    if (star.m_position.z > 0.1f) {
      randomizeStar(star);
      star.m_position.z = -100.0f; // Back to -00
    }
  }
  if (m_gameStatus == GameStatus::PLAYING) {
    detectTargetPosition();
    computePoints();
  }
}

void Window::detectTargetPosition() {

  for (auto &alvo : m_alvos) {
    // Transformar a posição do alvo para o espaço da câmera
    glm::vec4 worldPos = glm::vec4(alvo.m_positionTarget, 1.0f);
    glm::vec4 clipSpacePos =
        m_camera.getProjMatrix() * m_camera.getViewMatrix() * worldPos;

    if (clipSpacePos.w != 0.0f) {
      // Normalizar para NDC
      glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

      // Converter NDC para coordenadas de tela
      glm::vec2 screenPos;
      screenPos.x = (ndcPos.x * 0.5f + 0.5f) * m_viewportSize.x;
      screenPos.y = (1.0f - (ndcPos.y * 0.5f + 0.5f)) * m_viewportSize.y;

      // Calcular o raio aparente com base na escala do alvo
      glm::vec3 scaleOffset =
          glm::vec3(alvo.m_scaleTarget.x * 0.5f); // Raio no espaço do mundo
      glm::vec4 scaleWorldPos = worldPos + glm::vec4(scaleOffset, 0.0f);
      glm::vec4 scaleClipSpacePos =
          m_camera.getProjMatrix() * m_camera.getViewMatrix() * scaleWorldPos;

      float apparentRadius = 0.0f;
      if (scaleClipSpacePos.w != 0.0f) {
        glm::vec3 scaleNDC = glm::vec3(scaleClipSpacePos) / scaleClipSpacePos.w;
        glm::vec2 scaleScreenPos;
        scaleScreenPos.x = (scaleNDC.x * 0.5f + 0.5f) * m_viewportSize.x;
        scaleScreenPos.y =
            (1.0f - (scaleNDC.y * 0.5f + 0.5f)) * m_viewportSize.y;

        apparentRadius = glm::distance(screenPos, scaleScreenPos);
      }

      // Verificar a distância do mouse até o centro do alvo
      glm::vec2 mousePos = glm::vec2(m_mousePosition);
      float dist = glm::distance(mousePos, screenPos);

      // Determinar se o mouse está dentro do alvo
      if (dist <= apparentRadius * 0.85f) {
        alvo.m_mouseInside = true;
      } else {
        alvo.m_mouseInside = false;
      }
    }
  }
}

void Window::computePoints() {
  auto faseAtual = m_fases[m_faseAtual];
  auto corAtual = faseAtual.m_targetColor;
  auto formaAtual = faseAtual.m_targetForm;

  for (auto i = 0; i < (int)m_alvos.size(); i++) {
    auto &a = m_alvos[i];
    if (!a.m_hit)
      continue;

    if (a.m_alreadyComputePoint)
      continue;

    // cor atual eh verde ?
    if (corAtual == m_colors[0]) {
      if (formaAtual == faseAtual.m_targetForms[i]) {
        m_totalPoints += 100;
      } else {
        m_totalPoints -= 50;
      }
    } else {
      if (formaAtual != faseAtual.m_targetForms[i]) {
        m_totalPoints += 100;
      } else {
        m_totalPoints -= 50;
      }
    }
    a.m_alreadyComputePoint = true;
  }
}
```

O onPaint() é responsável por renderizar a cena na tela a cada quadro. Ele limpa o buffer de cor e profundidade, configura o viewport e ativa o programa de shaders usado para desenhar os objetos. Matrizes de visão e projeção da câmera são enviadas como variáveis uniformes para os shaders. Em seguida, o método renderiza os alvos da fase atual por meio de renderTargets, e cada estrela na cena é renderizada individualmente, com sua matriz de modelo calculada para aplicar transformações de posição, escala e rotação. Também se aplica iluminação aos objetos a serem renderizados. Finalmente, o programa de shaders é desativado, completando o ciclo de renderização.

```cpp
void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Utilizado para renderizar o fundo (o campo de estrelas)
  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};

  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White

  // Render each star
  for (auto &star : m_stars) {
    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, star.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, star.m_rotationAxis);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }

  // Outro shader é utilizado para renderizar os alvos, para poder aplicar iluminação

  abcg::glUseProgram(m_programForms);
  // get location of forms uniforms variables
  auto const viewFormsMatrixLoc{
      abcg::glGetUniformLocation(m_programForms, "viewMatrix")};
  auto const projFormsMatrixLoc{
      abcg::glGetUniformLocation(m_programForms, "projMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(m_programForms, "lightDirWorldSpace")};

  auto const modelFormsMatrixLoc{
      abcg::glGetUniformLocation(m_programForms, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(m_programForms, "normalMatrix")};
  auto const shininessLoc{
      abcg::glGetUniformLocation(m_programForms, "shininess")};

  // Iluminação e Materiais
  auto const IaLoc{abcg::glGetUniformLocation(m_programForms, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(m_programForms, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(m_programForms, "Is")};

  auto const KaLoc{abcg::glGetUniformLocation(m_programForms, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(m_programForms, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(m_programForms, "Ks")};

  abcg::glUniformMatrix4fv(viewFormsMatrixLoc, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(projFormsMatrixLoc, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glUniform4fv(lightDirLoc, 1, &m_lightDir.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);
  
  
   
  Fases fase = m_fases[m_faseAtual]; 
  if (m_gameStatus == GameStatus::PLAYING) {
    for (auto i = 0; i < (int)m_alvos.size(); i++) {
            auto &alvo = m_alvos[i];
      glm::mat4 model{1.0f};
      glm::vec3 targetPosition = m_targetScreenPos[i];

      alvo.m_positionTarget = targetPosition;
      model = glm::translate(model, alvo.m_positionTarget);
      model = glm::rotate(model, m_angle, alvo.m_rotationAxis);

      if (fase.m_targetForms[i] == Forms::SQUARE) {
        alvo.m_scaleTarget = {0.2f, 0.2f, 0.2f};
      }

      model = glm::scale(model, alvo.m_scaleTarget);

      abcg::glUniformMatrix4fv(modelFormsMatrixLoc, 1, GL_FALSE, &model[0][0]);

      abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
      abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
      abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
      abcg::glUniform1f(shininessLoc, m_shininess);

      auto const modelViewMatrix{glm::mat3(m_camera.getViewMatrix() * model)};
      auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
      abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE,
                               &normalMatrix[0][0]);
      if (!alvo.m_hit) {
        if (fase.m_targetForms[i] == Forms::SQUARE) {
          m_modelSquare.render(m_trianglesToDraw);
        } else {
          m_modelSphere.render(m_trianglesToDraw);
        }
      }
    }
  }

  abcg::glUseProgram(0);
}
```

O onPaintUI() é responsável por renderizar a interface gráfica do jogo utilizando a biblioteca ImGui. Ele exibe dois widgets: um no topo direito para mostrar a pontuação total do jogador e outro no canto inferior direito para indicar a forma e a cor do alvo atual. O método configura o tamanho e a posição de cada widget, utiliza uma fonte personalizada e, no caso do widget do alvo, desenha dinamicamente uma esfera ou um retângulo preenchido, dependendo do formato-alvo da fase atual, com a cor associada. Isso proporciona ao jogador informações visuais claras sobre o objetivo da fase e sua pontuação.

```cpp
void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  if (m_gameStatus == GameStatus::PLAYING) {
    { // Tamanho e posição do widget
      auto const pointsWidget{ImVec2(210, 40)};
      ImGui::PushFont(m_font);
      ImGui::SetNextWindowPos(
          ImVec2(m_viewportSize.x - pointsWidget.x - 10, 10));
      ImGui::SetNextWindowSize(pointsWidget);
      ImGui::Begin("Points", nullptr, ImGuiWindowFlags_NoDecoration);

      // Mostrando os pontos
      ImGui::Text("Pontos: %d", m_totalPoints);
      ImGui::End();
    }
    {
      // Tamanho e posição do widget
      auto const widgetSize{ImVec2(210, 150)};
      ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 10,
                                     m_viewportSize.y - widgetSize.y - 10));
      ImGui::SetNextWindowSize(widgetSize);
      ImGui::Begin("Forms", nullptr, ImGuiWindowFlags_NoDecoration);
      ImGui::PushFont(m_font);

      ImGui::Text("Alvo");
      // Obter o contexto de desenho
      ImDrawList *drawList = ImGui::GetWindowDrawList();

      // Posição inicial para desenhar dentro do widget
      ImVec2 widgetPos = ImGui::GetCursorScreenPos();

      Fases fase = m_fases[m_faseAtual];
      auto targetColor = fase.m_targetColor * 255.0f;
      if (fase.m_targetForm == Forms::SPHERE) {
        // Desenhar um círculo preenchido
        drawList->AddCircleFilled(
            ImVec2(widgetPos.x + 105, widgetPos.y + 55), // Centro
            50.0f,                                       // Raio
            IM_COL32(targetColor.x, targetColor.y, targetColor.z,
                     targetColor.w));
      } else {
        drawList->AddRectFilled(
            ImVec2(widgetPos.x + 55,
                   widgetPos.y + 10), // Canto superior esquerdo
            ImVec2(widgetPos.x + 160,
                   widgetPos.y + 105), // Canto inferior direito
            IM_COL32(targetColor.x, targetColor.y, targetColor.z,
                     targetColor.w));
      }

      // Desenhar um quadrado preenchido

      ImGui::End();
    }
  } else {
    {
      // Tamanho e posição do widget
      auto const widgetSize{ImVec2(500, 170)};
      ImGui::PushFont(m_font); // Fonte padrão
      ImGui::SetNextWindowPos(
          ImVec2((m_viewportSize.x / 2) - widgetSize.x / 2,
                 (m_viewportSize.y / 2) - widgetSize.y / 2));
      ImGui::SetNextWindowSize(widgetSize);
      ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration);

      // Centralizando o título
      auto const titleText = "Starshooter";
      auto const textSize = ImGui::CalcTextSize(titleText);
      ImGui::SetCursorPosX((widgetSize.x - textSize.x) / 2);

      // Fonte maior para o título
      ImGui::PushFont(m_font); // Fonte padrão
      ImGui::Text("%s", titleText);
      ImGui::PopFont();

      // Espaço entre o título e o botão
      ImGui::Dummy(ImVec2(0.0f, 20.0f));

      // Centralizando o botão
      auto const buttonSize = ImVec2(200, 50); // Botão maior
      ImGui::SetCursorPosX((widgetSize.x - buttonSize.x) / 2);
      if (ImGui::Button("Start", buttonSize)) {
        // Ação ao clicar no botão
        m_gameStatus = GameStatus::PLAYING;
        m_totalPoints = 0;
        m_reduceFOV = true;
      }
      ImGui::Dummy(ImVec2(0.0f, 20.0f));
      ImGui::PushFont(m_font); // Fonte padrão
      ImGui::Text("Total points: %d", m_totalPoints);
      ImGui::PopFont();

      ImGui::End();
      ImGui::PopFont();
    }
  }
}
```

Além da classe da janela, temos a classe do modelo. Que ficou responsável por encapsular todas as lógicas de VAO, VBO, EBO e renderização dos modelos de objetos (dos polígonos usados). Para essa classe temos os seguintes métodos:

   - loadObj()
   - render()
   - setupVAO()
   - destroy()
   - createBuffers()
   - standardize()

O loadObj() carrega um modelo 3D no formato OBJ de um arquivo especificado pelo caminho `path` e converte seus dados em uma estrutura utilizável para renderização. Ele utiliza a biblioteca `tinyobjloader` para processar o arquivo OBJ, extrair atributos como posições dos vértices e organizá-los em listas de vértices e índices (`m_vertices` e `m_indices`). Para evitar duplicatas, um mapa de hash associa vértices únicos a índices, otimizando o uso de dados. Caso a opção `standardize` seja ativada, o modelo é padronizado para ajustar sua escala e posição. Finalmente, o método chama `createBuffers` para configurar os buffers necessários para renderização. Ele também trata erros e avisos relacionados ao carregamento do arquivo.

```cpp
void Model::loadObj(std::string_view path, bool standardize) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  m_hasNormals = false;

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      // Vertex normal
      glm::vec3 normal{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        auto const normalStartIndex{3 * index.normal_index};
        normal = {attrib.normals.at(normalStartIndex + 0),
                  attrib.normals.at(normalStartIndex + 1),
                  attrib.normals.at(normalStartIndex + 2)};
      }

      Vertex const vertex{.position = position, .normal = normal};

      // If hash doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  if (standardize) {
    Model::standardize();
  }

  if (!m_hasNormals) {
    computeNormals();
  }

  createBuffers();
}
```

O render() é responsável por renderizar o modelo 3D utilizando os dados de vértices e índices armazenados no VAO (Vertex Array Object). Ele vincula o VAO correspondente ao modelo, calcula o número de índices a ser desenhado (com base no número de triângulos fornecido ou no total de índices do modelo), e em seguida, realiza a renderização com a função glDrawElements para desenhar os triângulos. Após a renderização, o VAO é desvinculado, finalizando o processo de renderização.

```cpp
void Model::render(int numTriangles) const {
  abcg::glBindVertexArray(m_VAO);

  auto const numIndices{(numTriangles < 0) ? m_indices.size()
                                           : numTriangles * 3};

  abcg::glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);
}
```

O setupVAO() configura o Vertex Array Object (VAO) do modelo para preparar os dados de vértices e índices para renderização. Primeiro, ele remove qualquer VAO anterior e cria um novo. Em seguida, vincula os buffers de índices (EBO) e vértices (VBO) ao VAO. O método também vincula o atributo de posição dos vértices ao shader program usando a localização do atributo no programa de shaders. Após configurar o atributo de posição, ele termina o processo de vinculação dos buffers e do VAO, preparando o modelo para ser renderizado.

```cpp
void Model::setupVAO(GLuint program) {
  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Bind vertex attributes
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  auto const normalAttribute{abcg::glGetAttribLocation(program, "inNormal")};
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    auto const offset{offsetof(Vertex, normal)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Model::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}
```

A lógica de câmera look at é feita pala classe câmera. Que contêm os seguintes métodos:

   - setEye()
   - setAt()
   - setUp()
   - setFov()

Essa implementação de uma câmera "LookAt" utiliza a biblioteca GLM para calcular e manipular as matrizes de projeção e visão de uma câmera 3D. A câmera é configurada com uma posição (`eye`), um ponto de interesse (`at`) e um vetor para cima (`up`). O método `computeProjectionMatrix` calcula a matriz de projeção perspectiva com base no campo de visão (FOV) e a proporção da janela, enquanto o método `computeViewMatrix` cria a matriz de visão com a função `glm::lookAt`, que orienta a câmera para o ponto de interesse.

## Relação de onde os requisitos foram implementados

   - uso de gráficos 3D com primitivas do OpenGL: na classe model.
   - interação do usuário: no método OnEvent() da classe window.
   - animações: em diversas partes do código. A movimentação das esferas do background se dá no método OnUpdate() da classe window. A mudança de fov da câmera para simular aceleração desta entre as fases está no método OnUpdate() e OnCreate() da classe window. As rotações dos alvos e das esferas do background estão no método OnPaint(), sendo aplicadas antes de renderizar cada uma dessas formas.
   - iluminação: no método OnPaint() da classe window.

## Dificuldades e soluções

  - No desenvolvimento do primeiro projeto, Simon Says/Genius, enfrentamos uma dificuldade em criar turnos para a interação do usuário. O principal problema estava relacionado ao fato de que, a cada repetição de cores, o tempo de interação variava de maneira distinta. Para superar essa questão, nos apoiamos em bibliografias especializadas e, paralelamente, desenvolvemos um protótipo de jogo chamado Star Shooter.
  Embora Star Shooter tenha turnos iguais em todas as rodadas, o que simplificou a implementação, nós nos identificamos com a proposta e a lógica do jogo. Devido a isso, decidimos adotar esse modelo e usá-lo oficialmente no nosso projeto.

- Durante o desenvolvimento do projeto, enfrentamos desafios ao tentar utilizar dois shaders diferentes simultaneamente. A complexidade surgiu principalmente pela necessidade de gerenciar diferentes efeitos visuais e garantir que cada shader fosse aplicado corretamente aos objetos desejados. A integração de shaders distintos demandou um cuidado extra na coordenação de parâmetros e na aplicação de efeitos gráficos, o que exigiu muitos ajustes nas configurações e na lógica de renderização.

- Outro desafio técnico significativo foi a implementação da iluminação correta. Inicialmente, encontramos dificuldades ao tentar iluminar os objetos de maneira realista, pois a iluminação não estava sendo aplicada como esperado. Para resolver isso, foi necessário utilizar a matriz de visualização da câmera, em vez da matriz do mundo, para ajustar a forma como a luz interagia com os objetos. Essa abordagem permitiu um controle mais preciso sobre a direção e intensidade da luz, garantindo que os efeitos de iluminação fossem aplicados corretamente, conforme a perspectiva da câmera.

- Um desafio adicional foi a implementação de cores diferentes nas formas. Inicialmente, todos os objetos do jogo foram configurados com a mesma cor, o que limitou a diversidade visual e a distinção entre eles. Para superar esse obstáculo, precisávamos ajustar a lógica para permitir a aplicação de cores variadas em cada objeto. Isso exigiu modificações no código, garantindo que as propriedades de cor fossem corretamente atribuídas a cada forma.

# Resultados e análise

### Avaliação Computacional e de Desempenho

O projeto demonstrou um desempenho satisfatório em termos de processamento gráfico e eficiência computacional. Nos testes com o computadores pessoais nossos, obtivemos taxas constantemente maiores a 100 fps. 

### Análise Gráfica e Visual

O resultado gráfico foi satisfatório. Destacando o fundo dinâmico de esferas que proporciona um efeito visual contínuo e envolvente, criando a sensação de movimento constante em um campo de asteroides. As formas principais (cubos e esferas) foram renderizadas com texturas simples, mas eficazes, o que permitiu um contraste visual adequado entre os objetos clicáveis e o fundo.

### Interatividade da Aplicação

O jogo responde de forma precisa aos cliques, com feedback visual imediato indicando ganho ou perda de pontos, conforme as regras estabelecidas. A curva de aprendizado é amigável, com mecânicas intuitivas que incentivam a repetição e engajamento. No entanto, a adição de elementos sonoros poderia melhorar a experiência do usuário e aumentar a imersão.

# Conclusões

Entre os principais destaques deste projeto, podemos mencionar o background dinâmico com o efeito de starfield, que cria a sensação de aceleração da câmera ao passar por um campo de asteroides. Outro ponto notável foi a geração aleatória de formas, que são renderizadas pela pipeline gráfica do OpenGL, garantindo uma experiência visual única a cada execução do jogo. A interatividade com o usuário também foi um aspecto fundamental, sendo implementada por meio da detecção de eventos da SDL, como a movimentação e os cliques do mouse, além da criação de hitboxes para garantir uma resposta precisa aos inputs. A implementação de iluminação e variação de cores nas formas renderizadas também contribuiu para uma melhor experiência.
Como próximos passos para a evolução do jogo, destacamos a possibilidade de aumentar a quantidade de formas em cada fase, diversificar ainda mais as formas (incluindo objetos além de cubos e esferas) e aumentar a dificuldade, reduzindo o tempo disponível para concluir as interações e proporcionando um maior desafio aos jogadores.

# Referências

   - [notas de aula da disciplina](https://hbatagelo.github.io/cg/)

