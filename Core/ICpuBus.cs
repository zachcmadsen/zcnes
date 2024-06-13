namespace ZcNes.Core;

internal interface ICpuBus
{
    byte Read(ushort addr);

    void Write(ushort addr, byte data);
}
