#include "argon2-gpu-common/argon2-common.h"
#include "argon2-gpu-common/argon2params.h"
#include "argon2-opencl/device.h"
#include "argon2-opencl/globalcontext.h"
#include "argon2-opencl/processingunit.h"
#include "argon2-opencl/programcontext.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];
	}

	return ret;
}


int main() {
  std::string salt = "A998c130E8d2d7447672e0cf4d956b3381BC66Ef";
  std::string key = "deadbeaf";

  auto params = argon2::Argon2Params(64, &salt, salt.size(), nullptr, 0,
                                     nullptr, 0, 1, 65000, 1);

  auto global = argon2::opencl::GlobalContext();

  auto &devices = global.getAllDevices();

  auto &device = devices[0];

  std::cout << device.getName() << std::endl;

  auto context = argon2::opencl::ProgramContext(
      &global, {device}, argon2::Type::ARGON2_ID,
      argon2::Version::ARGON2_VERSION_13, "../data/kernels");

  auto unit =
      argon2::opencl::ProcessingUnit(&context, &params, &device, 1, true, false);
  std::string pw = "d00dd1afe7a55d0e1ae588e867c1addaa96bebc5fca0aff667a8293e1853c02a";
  unit.setPassword(0, &pw, pw.size());

  unit.beginProcessing();
  unit.endProcessing();

  uint8_t hashBuf[64];
  unit.getHash(0, hashBuf);

  std::cout << base64_encode(hashBuf, 64) << std::endl;
}