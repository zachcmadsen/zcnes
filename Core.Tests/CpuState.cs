namespace ZcNes.Core.Tests;

internal class CpuState
{
    public ushort Pc { get; set; }
    public byte S { get; set; }
    public byte A { get; set; }
    public byte X { get; set; }
    public byte Y { get; set; }
    public byte P { get; set; }
    public required List<RamState> Ram { get; set; }
}
