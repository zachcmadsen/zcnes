namespace ZcNes.Core.Tests;

internal class ProcessorTestCpuBus : ICpuBus
{
    public byte[] Ram { get; } = new byte[0x10000];

    public List<BusState> Cycles { get; } = [];

    public byte Read(ushort addr)
    {
        var data = Ram[addr];

        var busState = new BusState
        {
            Addr = addr,
            Data = data,
            Kind = CycleKind.Read,
        };
        Cycles.Add(busState);

        return data;
    }

    public void Write(ushort addr, byte data)
    {
        var busState = new BusState
        {
            Addr = addr,
            Data = data,
            Kind = CycleKind.Write,
        };
        Cycles.Add(busState);

        Ram[addr] = data;
    }
}
