#include "glrenderengine.h"
#include <QDebug>
#include <QImage>

GLRenderEngine::GLRenderEngine(QObject* parent) : QObject(parent), glFunctions(nullptr)
{
	worldMatrix.setToIdentity();
	viewMatrix.setToIdentity();
	projMatrix.setToIdentity();
}

GLRenderEngine::~GLRenderEngine()
{
	delete glFunctions;
}

void GLRenderEngine::init()
{
	QOpenGLContext* currContext = QOpenGLContext::currentContext();

	if (!currContext)
	{
		qCritical() << "Can't get OpenGL context";
		return;
	}

	glFunctions = currContext->functions();
	glFunctions->initializeOpenGLFunctions();
	glFunctions->glEnable(GL_DEPTH_TEST);
	glFunctions->glDepthMask(GL_TRUE);
}

void GLRenderEngine::render(VAO* vao, int vertexCount, ShaderProgram* shaderProgram, DrawType drawCall,
						    UniformContainer uniformContainer, std::vector<int> textures)
{
	shaderProgram->setUniformValue("model", worldMatrix);
	shaderProgram->setUniformValue("view", viewMatrix);
	shaderProgram->setUniformValue("projection", projMatrix);

    setUniformsToShader(uniformContainer, shaderProgram);

	for (int i = 0; i < textures.size(); i++)
	{
		//break;
		//shaderProgram->setUniformValue("map.diffuse", 0);
		glFunctions->glActiveTexture(GL_TEXTURE0);
		texturesStorage[textures[i]].ptr->bind();
		shaderProgram->setUniformValue("map.diffuse", 0);//textures[i].name.c_str()
		break;
	}

	vao->bind();
	if (drawCall == DrawType::Arrays)
	{
		glFunctions->glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	else if (drawCall == DrawType::Elements)
	{
		glFunctions->glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
	}
	vao->release();

	for (int i = 0; i < textures.size(); i++)
	{
		//break;
		glFunctions->glActiveTexture(GL_TEXTURE0);
		texturesStorage[textures[i]].ptr->release();
		break;
	}
}

Container GLRenderEngine::createMesh(std::vector<Vertex> vertices, std::vector<int> indices)
{
	Container container;

	container.vao = new QOpenGLVertexArrayObject;//std::make_unique<QOpenGLVertexArrayObject>();
	container.vao->create();
	container.vao->bind();

	container.vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);//std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
	container.vbo->create();
	container.vbo->bind();
	container.vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
	container.vbo->allocate(vertices.data(), vertices.size() * sizeof(Vertex));

	container.ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);//std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
	container.ebo->create();
	container.ebo->bind();
	container.ebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
	container.ebo->allocate(indices.data(), indices.size() * sizeof(int));

	// vertex positions
	glFunctions->glEnableVertexAttribArray(0);
	glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glFunctions->glEnableVertexAttribArray(1);
	glFunctions->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glFunctions->glEnableVertexAttribArray(2);
	glFunctions->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	container.vao->release();
	return container;
}

void GLRenderEngine::clearDisplay(float r, float g, float b)
{
	glFunctions->glClearColor(r, g, b, 1.0f);
	glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderEngine::clearDepthBuffer()
{
	glFunctions->glClear(GL_DEPTH_BUFFER_BIT);
}

void GLRenderEngine::setViewport(double x, double y, double width, double height)
{
	glFunctions->glViewport(x, y, width, height);
}

QOpenGLShaderProgram* GLRenderEngine::createShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	QOpenGLShaderProgram* shaderProgram = new QOpenGLShaderProgram();
	if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderPath.c_str()))
		qDebug() << "Vertex shader errors:\n" << shaderProgram->log();

	if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderPath.c_str()))
		qDebug() << "Fragment shader errors:\n" << shaderProgram->log();

	if (!shaderProgram->link())
		qDebug() << "Shader linker errors:\n" << shaderProgram->log();

	return shaderProgram;
}

void GLRenderEngine::setWorldMatrix(const QMatrix4x4& matrix)
{
	worldMatrix = matrix;
}

const QMatrix4x4& GLRenderEngine::getWorldMatrix() const
{
	return worldMatrix;
}

void GLRenderEngine::setViewMatrix(const QMatrix4x4& matrix)
{
	viewMatrix = matrix;
}

const QMatrix4x4& GLRenderEngine::getViewMatrix() const
{
	return viewMatrix;
}

void GLRenderEngine::setProjMatrix(const QMatrix4x4& matrix)
{
	projMatrix = matrix;
}

const QMatrix4x4& GLRenderEngine::getProjMatrix() const
{
	return projMatrix;
}

int GLRenderEngine::loadTexture(std::string path)
{
	Texture texture;
	QImage image(QString::fromStdString(path));
	texture.ptr = new QOpenGLTexture(image.mirrored());
	texturesStorage.emplace(texturesStorage.size(), texture);
	return texturesStorage.size() - 1;
}

void GLRenderEngine::setUniformsToShader(UniformContainer uniformContainer, ShaderProgram* shaderProgram)
{
	for (const auto& param : uniformContainer.uniformToFloat)
	{
		shaderProgram->setUniformValue(param.first.c_str(), param.second);
	}

	for (const auto& param : uniformContainer.uniformToVec4)
	{
		shaderProgram->setUniformValue(param.first.c_str(), param.second);
	}

	for (const auto& param : uniformContainer.uniformToVec3)
	{
		shaderProgram->setUniformValue(param.first.c_str(), param.second);
	}

	for (const auto& param : uniformContainer.uniformToVec2)
	{
		shaderProgram->setUniformValue(param.first.c_str(), param.second);
	}

	for (const auto& param : uniformContainer.uniformToBool)
	{
		shaderProgram->setUniformValue(param.first.c_str(), param.second);
	}
}
