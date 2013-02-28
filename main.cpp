
#include "boost-multipart-form.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

const char *chunks_1 =
    "--randomboundaryXYZ\n"
    "Content-Disposition: form-data; name=\"sha1-9b03f7aca1ac60d40b5e570c34f79a3e07c918e8\"; filename=\"blob1\"\n"
    "Content-Type: application/octet-stream\n\n"
    "(binary or text blob data)\n"
    "(binary or text blob data)\n"
    "--randomboundaryXYZ\n"
    "Content-Disposition: form-data; name=\"sha1-deadbeefdeadbeefdeadbeefdeadbeefdeadbeef\"; filename=\"blob2\"\n"
    "Content-Type: application/octet-stream\n\n"
    "(binary or text blob data)\n"
    "(binary or text blob data)\n"
    "--randomboundaryXYZ--\n";

const char *chunk_1 =
    "Content-Disposition: form-data; name=\"sha1-deadbeefdeadbeefdeadbeefdeadbeefdeadbeef\"; filename=\"blob2\"\n"
    "Content-Type: application/octet-stream\n\n"
    "(binary or text blob data)\n"
    "(binary or text blob data)\n"
    "(binary or text blob data)\n";


int main() {
	std::string chunks1(chunks_1, 0, strlen(chunks_1));
	MultiPartFormData mpfd("randomboundaryXYZ", chunks1);

	LOG_INFO(mpfd << std::endl);

	return 0;
}
