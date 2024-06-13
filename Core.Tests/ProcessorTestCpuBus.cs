namespace ZcNes.Core.Tests;

internal class ProcessorTestCpuBus : ICpuBus
{
    public byte[] Ram { get; } = new byte[0x10000];

    public byte Read(ushort addr)
    {
        return Ram[addr];
    }

    public void Write(ushort addr, byte data)
    {
        Ram[addr] = data;
    }
}
