#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLFunctions_3_3_Core>
#include <QObject>
#include <memory>
#include <vector>
#include <QScopedPointer>
#include <glm/glm.hpp>
#include <map>
#include <string>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int id;
	std::string name;
	std::string type;
	std::string fullpath;
	QOpenGLTexture* ptr;
	QImage image;
	bool enabled = false;
};

struct Material
{
	glm::vec3 ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	bool ambientEnabled = false;

	glm::vec3 diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	bool diffuseEnabled = false;

	glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);
	bool specularEnabled = false;

	glm::vec3 emission = glm::vec3(0.0f, 0.0f, 0.0f);
	bool emissionEnabled = false;

	float shininess = 0.0f;

	//float refractIndex = 1.0f;
	//float opacity = 1.0f;
	std::string name = "";

	bool diffuseMapEnabled = false;
	bool specularMapEnabled = false;
	bool normalMapEnabled = false;
	
};

struct Container
{
	QOpenGLVertexArrayObject* vao = nullptr;
	QOpenGLBuffer* vbo = nullptr;
	QOpenGLBuffer* ebo = nullptr;
};

struct UniformContainer
{
	std::map<std::string, QVector4D> uniformToVec4;
	std::map<std::string, QVector3D> uniformToVec3;
	std::map<std::string, QVector2D> uniformToVec2;
	std::map<std::string, GLfloat> uniformToFloat;
	std::map<std::string, bool> uniformToBool;
	std::map<std::string, int> uniformToInt;
};

enum class DrawType { Arrays, Elements };

using ShaderProgram = QOpenGLShaderProgram;
using VAO = QOpenGLVertexArrayObject;

class GLRenderEngine : QObject
{
public:
	explicit GLRenderEngine(QObject* parent = 0);
	~GLRenderEngine();

	void init();
	void render(VAO* vao, int vertexCount, ShaderProgram* shader, DrawType drawType, UniformContainer uniformContainer, std::vector<int> textures);

	Container createMesh(std::vector<Vertex> vertices, std::vector<int> indices);

	void clearDisplay(float r, float g, float b);
	void clearDepthBuffer();
	void setViewport(double x, double y, double width, double height);

	QOpenGLShaderProgram* createShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath);
	void setWorldMatrix(const QMatrix4x4& matrix);
	const QMatrix4x4& getWorldMatrix() const;

	void setViewMatrix(const QMatrix4x4& matrix);
	const QMatrix4x4& getViewMatrix() const;

	void setProjMatrix(const QMatrix4x4& matrix);
	const QMatrix4x4& getProjMatrix() const;

	int loadTexture(std::string path);
	Texture* getTexture(int id);
private:
	std::map<int, Texture> texturesStorage;
	QOpenGLFunctions* glFunctions;
	QMatrix4x4 worldMatrix;
	QMatrix4x4 viewMatrix;
	QMatrix4x4 projMatrix;

	void setUniformsToShader(UniformContainer uniformContainer, ShaderProgram* shaderProgram);
};
