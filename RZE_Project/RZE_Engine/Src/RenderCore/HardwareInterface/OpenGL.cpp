#include "StdAfx.h"

#include "OpenGL.h"

OpenGLRHI::OpenGLRHI()
{
}

OpenGLRHI::~OpenGLRHI()
{
}

void OpenGLRHI::Init()
{
	LOG_CONSOLE_ARGS("OpenGL version is %s", glGetString(GL_VERSION));

	// @implementation these need to be read in via window attributes
	glViewport(0, 0, 1280, 720);

	InitGLEW();
}

void OpenGLRHI::InitGLEW()
{
	glewExperimental = true;

	GLenum err = glewInit();
	if (err == GLEW_OK)
	{
		LOG_CONSOLE_ANNOUNCE("GLEW initialized successfully.");
	}
	else
	{
		// @todo do something meaningful here
		LOG_CONSOLE_ANNOUNCE("GLEW did not initialize.");
		AssertFalse();
	}
}

void OpenGLRHI::ClearColor(const GLfloat red, const GLfloat green, const GLfloat blue, const GLfloat alpha) const
{
	glClearColor(red, green, blue, alpha);
}

void OpenGLRHI::Clear(const GLuint mask) const
{
	glClear(mask);
}

void OpenGLRHI::GenVertexArrays(const GLuint arrayCount, GLuint* outBufferHandle) const
{
	AssertExpr(arrayCount > 0);
	glGenVertexArrays(arrayCount, outBufferHandle);
}

void OpenGLRHI::BindVertexArray(const GLuint arrayObjectHandle) const
{
	glBindVertexArray(arrayObjectHandle);
}

void OpenGLRHI::GenerateBuffer(GLuint bufferCount, GLuint* outBufferHandle) const
{
	AssertExpr(bufferCount > 0);
	glGenBuffers(bufferCount, outBufferHandle);
}

void OpenGLRHI::BindBuffer(const GLuint target, const GLuint bufferObjectHandle) const
{
	glBindBuffer(target, bufferObjectHandle);
}

void OpenGLRHI::DeleteBuffer(GLuint bufferCount, GLuint* bufferHandle)
{
	AssertExpr(bufferCount > 0 && bufferHandle);
	glDeleteBuffers(bufferCount, bufferHandle);
}

void OpenGLRHI::SetBufferData(const GLuint target, const GLuint size, const void* const data, const GLuint bufferUsage) const
{
	AssertNotNull(data);
	AssertExpr(size > 0);
	// @note if this doesnt work check the const void* const signature because its possible under the hood it no likey
	glBufferData(target, size, data, bufferUsage);
}

void OpenGLRHI::EnableVertexAttributeArray(const GLuint index) const
{
	glEnableVertexAttribArray(index);
}

void OpenGLRHI::VertexAttribPointer(const GLuint index, const GLint size, const GLuint type, const GLboolean normalized, const GLuint stride, const void* const pointer) const
{
	AssertExpr(size > 0);
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void OpenGLRHI::DrawArrays(const GLuint mode, const GLint first, const GLuint count) const
{
	AssertExpr(count > 0);
	glDrawArrays(mode, first, count);
}

void OpenGLRHI::CreateShaderProgram(GLuint& outProgramID) const
{
	outProgramID = glCreateProgram();
}

void OpenGLRHI::CreateShader(const GLuint shaderType, GLuint& outShaderID) const
{
	outShaderID = glCreateShader(shaderType);
}

void OpenGLRHI::DeleteShader(const GLuint shaderID) const
{
	glDeleteShader(shaderID);
}

void OpenGLRHI::CompileShader(const GLuint shaderID) const
{
	glCompileShader(shaderID);
}

void OpenGLRHI::AttachShader(const GLuint shaderProgramID, const GLuint shaderID) const
{
	glAttachShader(shaderProgramID, shaderID);
}

void OpenGLRHI::UseShaderProgram(const GLuint shaderProgramID) const
{
	glUseProgram(shaderProgramID);
}

void OpenGLRHI::SetShaderSource(const GLuint shaderID, const GLuint count, const GLchar* sourceCode, const GLint* length) const
{
	GLboolean bIsValidShader = false;
	IsShader(shaderID, bIsValidShader);

	AssertExpr(count > 0);
	AssertNotNull(sourceCode);

	glShaderSource(shaderID, count, &sourceCode, length);
}

void OpenGLRHI::IsShader(const GLuint shaderID, GLboolean& outResult) const
{
	outResult = glIsShader(shaderID);
}

void OpenGLRHI::IsShaderProgram(const GLuint shaderProgramID, GLboolean& outResult) const
{
	outResult = glIsProgram(shaderProgramID);
}

void OpenGLRHI::LinkShaderProgram(const GLuint mShaderProgramID) const
{
	glLinkProgram(mShaderProgramID);
}

void OpenGLRHI::GetProgramiv(const GLuint mShaderProgramID, const EGLShaderProgramStatusParam::Value programStatusParam, GLint* programLinkStatus) const
{
	glGetProgramiv(mShaderProgramID, programStatusParam, programLinkStatus);
}

void OpenGLRHI::GetShaderiv(const GLuint shaderID, const GLenum shaderStatusParam, GLint* const params) const
{
	glGetShaderiv(shaderID, shaderStatusParam, params);
}

void OpenGLRHI::UniformLocation(const GLuint shaderProgramID, const GLchar* uniformName, GLint& outUniformLocation) const
{
	outUniformLocation = glGetUniformLocation(shaderProgramID, uniformName);
}

void OpenGLRHI::GetProgramInfoLog(const GLuint programID, const GLsizei maxLength, GLsizei* length, GLchar* infoLog) const
{
	glGetProgramInfoLog(programID, maxLength, length, infoLog);
}

void OpenGLRHI::GetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog) const
{
	glGetShaderInfoLog(shader, maxLength, length, infoLog);
}

void OpenGLRHI::SetUniformInt(const GLint uniformLocation, const int value) const
{
	glUniform1i(uniformLocation, value);
}

void OpenGLRHI::SetUniformMat4x4(const GLint uniformLocation, const GLsizei count, const GLboolean transpose, const GLfloat* valuePtr) const
{
	glUniformMatrix4fv(uniformLocation, count, transpose, valuePtr);
}

void OpenGLRHI::SetUniformVec4D(const GLint uniformLocation, const float x, const float y, const float z, const float a) const
{
	glUniform4f(uniformLocation, x, y, z, a);
}

//
//	OPENGLVBO
//

OpenGLVBO::OpenGLVBO(OpenGLVAO* parentBuf)
	: mBufferHandle(0)
	, mBufferTarget(EGLBufferTarget::ArrayBuffer)
	, mBufferUsageMode(EGLBufferUsage::StaticDraw)
{
	parentBuf->Bind();

	Generate();
	Bind();
	OpenGLRHI::Get().EnableVertexAttributeArray(0);
	OpenGLRHI::Get().VertexAttribPointer(0, 3, EGLDataType::Float, EGLBooleanValue::False, 0, 0);

	parentBuf->Unbind();
}

OpenGLVBO::~OpenGLVBO()
{
}

void OpenGLVBO::SetBufferTarget(const U32 newBufferTarget)
{
	mBufferTarget = newBufferTarget;
}

void OpenGLVBO::SetBufferUsageMode(const U32 newBufferUsageMode)
{
	mBufferUsageMode = newBufferUsageMode;
}

void OpenGLVBO::SetBufferData(const void* const data, const U32 size)
{
	AssertNotNull(data);
	OpenGLRHI::Get().SetBufferData(mBufferTarget, size, data, mBufferUsageMode);
}

void OpenGLVBO::Generate()
{
	OpenGLRHI::Get().GenerateBuffer(1, &mBufferHandle);
}

void OpenGLVBO::Bind()
{
	OpenGLRHI::Get().BindBuffer(mBufferTarget, mBufferHandle);
}

OpenGLVAO::OpenGLVAO()
{
	Generate();
}

//
//	OPENGLVAO
//
OpenGLVAO::~OpenGLVAO()
{

}

void OpenGLVAO::Bind()
{
	OpenGLRHI::Get().BindVertexArray(mBufferHandle);
}

void OpenGLVAO::Unbind()
{
	OpenGLRHI::Get().BindVertexArray(0);
}

void OpenGLVAO::Generate()
{
	OpenGLRHI::Get().GenVertexArrays(1, &mBufferHandle);
}
