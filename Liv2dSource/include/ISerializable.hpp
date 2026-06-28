#pragma once

namespace live2d {

class BinaryReader;

class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual void read(BinaryReader& br) = 0;
    int type;

    static void ISerializableRelease(void* ptr);
};

} // namespace live2d
