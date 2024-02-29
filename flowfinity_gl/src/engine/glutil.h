#pragma once

namespace GLUtil {
void printGLErrorLog();
void printLinkInfoLog(int prog);
void printShaderCompileInfoLog(int shader);
}; // namespace GLUtil