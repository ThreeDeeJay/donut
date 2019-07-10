#include <MemoryStream.h>
#include <cstring>

namespace Donut {

MemoryStream::MemoryStream(const std::vector<std::uint8_t>& data)
{
	_data = data;
	_position = _data.begin();
}

void MemoryStream::ReadBytes(std::uint8_t* dest, std::size_t length)
{
	// check cur + length doesn't exceed size

	// copy the data
	std::memcpy(dest, &(*_position), length);

	// advance our current position iterator
	std::advance(_position, length);
}

std::string MemoryStream::ReadLPString()
{
	std::uint8_t length = Read<std::uint8_t>();

	// read into a char* first, reading directly into a std::string fucks length up
	char* str = new char[length];
    ReadBytes(reinterpret_cast<uint8_t*>(str), length);
	std::string ret(str);
	delete[] str;

	return ret;
}

void MemoryStream::Seek(std::size_t position, SeekMode mode)
{
	switch (mode) {
		case SeekMode::Begin:
		_position = std::next(_data.begin(), position);
		break;
		case SeekMode::Current:
		std::advance(_position, position);
		break;
		case SeekMode::End:
		_position = std::next(_data.end(), -position);
		break;
	}
}


}