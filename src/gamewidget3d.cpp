#include "gamewidget3d.h"
#include <cmath>
#include <QPainter>

//Vertex Shader
static const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

//Fragment Shader (Phong)
static const char *fragmentShaderSource = R"(
#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

// ─── Konstruktor ────────────────────────────────────────────────────────
GameWidget3D::GameWidget3D(GameLogic *logic, QWidget *parent)
    : QOpenGLWidget(parent), logic(logic),
    boardVBO(QOpenGLBuffer::VertexBuffer),
    wallsVBO(QOpenGLBuffer::VertexBuffer),
    sphereVBO(QOpenGLBuffer::VertexBuffer)
{
    // MSAA 4x, OpenGL 3.3 Core
    QSurfaceFormat format;
    format.setSamples(4);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    setFormat(format);

    setFocusPolicy(Qt::StrongFocus);
    connect(logic, &GameLogic::stateUpdated, this, &GameWidget3D::onStateUpdated);
    connect(logic, &GameLogic::mazeChanged, this, [this]() {
        if (!glInitialized) return;
        makeCurrent();
        buildWallsGeometry();
        doneCurrent();
        update();
    });
}

GameWidget3D::~GameWidget3D() {
    makeCurrent();
    boardVBO.destroy();
    boardVAO.destroy();
    wallsVBO.destroy();
    wallsVAO.destroy();
    sphereVBO.destroy();
    sphereVAO.destroy();
    delete shaderProgram;
    doneCurrent();
}

void GameWidget3D::retranslateUI() {
    winLabel->setText(tr("Wygrana!"));
}

void GameWidget3D::forceRefresh() {
    if (!glInitialized) return;
    makeCurrent();
    resizeGL(width(), height());
    doneCurrent();
    update();
}

void GameWidget3D::onStateUpdated() {
    if (!glInitialized) return;
    update();
}

// ─── Kamera orbitalna ───────────────────────────────────────────────────
void GameWidget3D::updateViewMatrix() {
    float centerX = 0.5f;
    float centerY = 0.5f;

    float camX = centerX + cameraDistance * sin(cameraElevation) * cos(cameraAzimuth);
    float camY = centerY + cameraDistance * sin(cameraElevation) * sin(cameraAzimuth);
    float camZ = cameraDistance * cos(cameraElevation);

    viewMatrix.setToIdentity();
    viewMatrix.lookAt(
        QVector3D(camX, camY, camZ),
        QVector3D(centerX, centerY, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f)
        );
}

void GameWidget3D::setAzimuth(int value) {
    cameraAzimuth = value * M_PI / 180.0f;
    updateViewMatrix();
    update();
}

void GameWidget3D::setElevation(int value) {
    cameraElevation = value * M_PI / 180.0f;
    updateViewMatrix();
    update();
}

// ─── Inicjalizacja OpenGL ───────────────────────────────────────────────
void GameWidget3D::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Kompilacja shaderów
    shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram->link();

    uModelLoc = shaderProgram->uniformLocation("model");
    uViewLoc = shaderProgram->uniformLocation("view");
    uProjLoc = shaderProgram->uniformLocation("projection");
    uColorLoc = shaderProgram->uniformLocation("objectColor");
    uLightPosLoc = shaderProgram->uniformLocation("lightPos");
    uViewPosLoc = shaderProgram->uniformLocation("viewPos");

    // Buduj geometrię
    buildBoardGeometry();
    buildWallsGeometry();
    buildSphereGeometry(20, 20);

    // Macierz widoku — kamera orbitalna
    updateViewMatrix();

    glInitialized = true;
}

void GameWidget3D::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, float(w) / float(h ? h : 1), 0.1f, 100.0f);
}

// ─── Budowanie geometrii ────────────────────────────────────────────────
void GameWidget3D::buildBoardGeometry() {
    float vertices[] = {
        // pos                  // normal
        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,     0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    boardVAO.create();
    boardVAO.bind();

    boardVBO.create();
    boardVBO.bind();
    boardVBO.allocate(vertices, sizeof(vertices));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    boardVAO.release();
}

void GameWidget3D::buildWallsGeometry() {
    const auto &walls = logic->getWalls();
    cachedWallCount = walls.size();
    float wh = 0.06f;

    std::vector<float> vertices;
    vertices.reserve(walls.size() * 36 * 6);

    auto addQuad = [&](QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, QVector3D n) {
        vertices.insert(vertices.end(), {p0.x(), p0.y(), p0.z(), n.x(), n.y(), n.z()});
        vertices.insert(vertices.end(), {p1.x(), p1.y(), p1.z(), n.x(), n.y(), n.z()});
        vertices.insert(vertices.end(), {p2.x(), p2.y(), p2.z(), n.x(), n.y(), n.z()});
        vertices.insert(vertices.end(), {p0.x(), p0.y(), p0.z(), n.x(), n.y(), n.z()});
        vertices.insert(vertices.end(), {p2.x(), p2.y(), p2.z(), n.x(), n.y(), n.z()});
        vertices.insert(vertices.end(), {p3.x(), p3.y(), p3.z(), n.x(), n.y(), n.z()});
    };

    for (const QRectF &wall : walls) {
        float x = wall.x();
        float y = wall.y();
        float w = wall.width();
        float h = wall.height();

        addQuad({x,y,wh}, {x+w,y,wh}, {x+w,y+h,wh}, {x,y+h,wh}, {0,0,1});
        addQuad({x,y,0}, {x,y+h,0}, {x+w,y+h,0}, {x+w,y,0}, {0,0,-1});
        addQuad({x,y,0}, {x+w,y,0}, {x+w,y,wh}, {x,y,wh}, {0,-1,0});
        addQuad({x,y+h,0}, {x,y+h,wh}, {x+w,y+h,wh}, {x+w,y+h,0}, {0,1,0});
        addQuad({x,y,0}, {x,y,wh}, {x,y+h,wh}, {x,y+h,0}, {-1,0,0});
        addQuad({x+w,y,0}, {x+w,y+h,0}, {x+w,y+h,wh}, {x+w,y,wh}, {1,0,0});
    }

    wallsVertexCount = vertices.size() / 6;

    if (!wallsVAO.isCreated()) wallsVAO.create();
    wallsVAO.bind();

    if (!wallsVBO.isCreated()) wallsVBO.create();
    wallsVBO.bind();
    wallsVBO.allocate(vertices.data(), vertices.size() * sizeof(float));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    wallsVAO.release();
}

void GameWidget3D::buildSphereGeometry(int stacks, int slices) {
    std::vector<float> vertices;

    for (int i = 0; i < stacks; ++i) {
        float phi0 = M_PI * float(i) / stacks;
        float phi1 = M_PI * float(i + 1) / stacks;

        for (int j = 0; j < slices; ++j) {
            float theta0 = 2.0f * M_PI * float(j) / slices;
            float theta1 = 2.0f * M_PI * float(j + 1) / slices;

            auto spherePoint = [](float phi, float theta) -> QVector3D {
                return QVector3D(
                    sin(phi) * cos(theta),
                    sin(phi) * sin(theta),
                    cos(phi)
                    );
            };

            QVector3D p0 = spherePoint(phi0, theta0);
            QVector3D p1 = spherePoint(phi0, theta1);
            QVector3D p2 = spherePoint(phi1, theta1);
            QVector3D p3 = spherePoint(phi1, theta0);

            auto addVert = [&](QVector3D p) {
                QVector3D n = p.normalized();
                vertices.insert(vertices.end(), {p.x(), p.y(), p.z(), n.x(), n.y(), n.z()});
            };

            addVert(p0); addVert(p1); addVert(p2);
            addVert(p0); addVert(p2); addVert(p3);
        }
    }

    sphereVertexCount = vertices.size() / 6;

    sphereVAO.create();
    sphereVAO.bind();

    sphereVBO.create();
    sphereVBO.bind();
    sphereVBO.allocate(vertices.data(), vertices.size() * sizeof(float));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          reinterpret_cast<void*>(3 * sizeof(float)));

    sphereVAO.release();
}

// ─── Renderowanie ───────────────────────────────────────────────────────
void GameWidget3D::paintGL() {
    QPainter painter(this);
    painter.beginNativePainting();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->bind();

    shaderProgram->setUniformValue(uViewLoc, viewMatrix);
    shaderProgram->setUniformValue(uProjLoc, projectionMatrix);
    shaderProgram->setUniformValue(uLightPosLoc, QVector3D(0.5f, 0.5f, 2.0f));
    shaderProgram->setUniformValue(uViewPosLoc, QVector3D(0.5f, 0.3f, 1.5f));

    drawBoard();
    drawWalls();
    drawGoal();
    drawBall();

    shaderProgram->release();

    glDisable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    painter.endNativePainting();

    if (logic->isGameWon()) {
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(QColor(0, 0, 0, 150));
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, width(), height());

        painter.setPen(QColor(0, 220, 0));
        QFont font = painter.font();
        font.setPixelSize(height() * 0.08);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, tr("Wygrana!"));
    }
}

void GameWidget3D::drawBoard() {
    QMatrix4x4 model;
    model.scale(1.0f, -1.0f, 1.0f);
    model.translate(0.0f, -1.0f, 0.0f);

    shaderProgram->setUniformValue(uModelLoc, model);
    shaderProgram->setUniformValue(uColorLoc, QVector3D(0.82f, 0.78f, 0.68f));

    boardVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    boardVAO.release();
}

void GameWidget3D::drawWalls() {
    QMatrix4x4 model;
    model.scale(1.0f, -1.0f, 1.0f);
    model.translate(0.0f, -1.0f, 0.0f);

    shaderProgram->setUniformValue(uModelLoc, model);
    shaderProgram->setUniformValue(uColorLoc, QVector3D(0.35f, 0.22f, 0.10f));

    wallsVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, wallsVertexCount);
    wallsVAO.release();
}

void GameWidget3D::drawBall() {
    float bx = logic->getBallX();
    float by = logic->getBallY();
    float br = logic->getBallRadius();

    QMatrix4x4 model;
    model.scale(1.0f, -1.0f, 1.0f);
    model.translate(0.0f, -1.0f, 0.0f);
    model.translate(bx, by, br);
    model.scale(br);

    shaderProgram->setUniformValue(uModelLoc, model);
    shaderProgram->setUniformValue(uColorLoc, QVector3D(0.15f, 0.15f, 0.15f));

    sphereVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
    sphereVAO.release();
}

void GameWidget3D::drawGoal() {
    float pulse = 0.7f + 0.3f * std::sin(logic->getPulsePhase());
    float gr = logic->getBallRadius() * pulse;

    QMatrix4x4 model;
    model.scale(1.0f, -1.0f, 1.0f);
    model.translate(0.0f, -1.0f, 0.0f);
    model.translate(logic->getGoalX(), logic->getGoalY(), gr);
    model.scale(gr);

    shaderProgram->setUniformValue(uModelLoc, model);
    shaderProgram->setUniformValue(uColorLoc, QVector3D(0.0f, 0.8f, 0.0f));

    sphereVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
    sphereVAO.release();
}

// ─── Klawiatura ─────────────────────────────────────────────────────────
void GameWidget3D::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) { event->ignore(); return; }

    switch (event->key()) {
    case Qt::Key_Up:    goUp = true;    break;
    case Qt::Key_Down:  goDown = true;  break;
    case Qt::Key_Left:  goLeft = true;  break;
    case Qt::Key_Right: goRight = true; break;
    default: QWidget::keyPressEvent(event); return;
    }

    double dx = (goRight ? 1.0 : 0.0) - (goLeft ? 1.0 : 0.0);
    double dy = (goDown ? 1.0 : 0.0) - (goUp ? 1.0 : 0.0);
    logic->setKeyboardAcceleration(dx, dy);
    event->accept();
}

void GameWidget3D::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) { event->ignore(); return; }

    switch (event->key()) {
    case Qt::Key_Up:    goUp = false;    break;
    case Qt::Key_Down:  goDown = false;  break;
    case Qt::Key_Left:  goLeft = false;  break;
    case Qt::Key_Right: goRight = false; break;
    default: QWidget::keyReleaseEvent(event); return;
    }

    double dx = (goRight ? 1.0 : 0.0) - (goLeft ? 1.0 : 0.0);
    double dy = (goDown ? 1.0 : 0.0) - (goUp ? 1.0 : 0.0);
    logic->setKeyboardAcceleration(dx, dy);
    event->accept();
}