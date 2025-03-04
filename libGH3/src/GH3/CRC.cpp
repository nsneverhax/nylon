#include <GH3/CRC.hpp>
#include <string>

GH3::CRC::ChecksumName* GH3::CRC::ChecksumNameHashTable = reinterpret_cast<GH3::CRC::ChecksumName*>(0x00c0775c);
char* GH3::CRC::ChecksumNames = reinterpret_cast<char*>(0x00c07760);

uint32_t GH3::CRC::CRC32TableLength = 256;
uint32_t* GH3::CRC::CRC32Table = reinterpret_cast<uint32_t*>(0x0095CD28);

GH3::CRCKey GH3::CRC::CreateKeyFromStringOut(CRCKey* outKey, const char* srcString)
{
	if (srcString == nullptr)
	{
		if (outKey != nullptr)
			*outKey = 0;
		return 0;
	}

	CRCKey finalKey = 0xFFFFFFFF;
	char currChar = *srcString;

	auto string = std::string_view(srcString);

	uint32_t lastChar = 0;

	for (uint32_t i = 0; i < string.length(); i++)
	{
		char c = string[i];

		if ((c + 0xBFU) < 0x1A)
			c += 32;

		lastChar = c;

		if (c == '/')
			lastChar = '\\';

		finalKey = finalKey >> 8 ^ CRC32Table[(lastChar ^ finalKey) & 0xFF];
	}

	if (outKey != nullptr)
		*outKey = finalKey;

	return finalKey;
}

GH3::CRCKey GH3::CRC::CreateKeyFromString(const char* srcString)
{
	return CreateKeyFromStringOut(nullptr, srcString);
}


void GH3::CRC::CreateKeyNameAssociation(CRCKey key, char* name)
{
	reinterpret_cast<void(__cdecl*)(CRCKey, char*)>(key, name);
}

char* GH3::CRC::FindChecksumName(CRCKey key)
{
	return reinterpret_cast<char*(*)(CRCKey)>(0x004a69a0)(key);
}
